//#define SERVER_USE_TLS
//#define PATHNAME_TO_CERTIFICATE_FILE "localhost-100y.cert"
//#define PATHNAME_TO_PRIVATE_KEY_FILE "localhost-100y.key"

#include "liveMedia.hh"

#include "BasicUsageEnvironment.hh"
#include "announceURL.hh"
// ⬇ 추가: Digest SHA-256 지원을 위해 (클라이언트/서버 공용 헤더)
#include "DigestAuthentication.hh"

// Windows SHA-256 구현 (CNG/BCrypt). 다른 OS에선 OpenSSL 등으로 대체하세요.
#ifdef _WIN32
#include <bcrypt.h>
#pragma comment(lib, "Bcrypt.lib")
#endif

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
namespace fs = std::filesystem;

UsageEnvironment* env;

// 각 스트림에 대한 두 번째 및 이후 클라이언트가 첫 번째 클라이언트와 동일한
// 입력 스트림을 재사용하도록 하려면(각 클라이언트에서 처음부터 파일을 재생하는 대신)
// 다음 "False"를 "True"로 변경합니다.
Boolean reuseFirstSource = False;

// MPEG-1 또는 2개의 비디오 "I" 프레임만 스트리밍하려면
// (예: 네트워크 대역폭을 줄이기 위해)
// 다음 "False"를 "True"로 변경합니다.
Boolean iFramesOnly = False;

#pragma region Digest인증

// ====== Digest SHA-256: 서버 설정 ======
// 서버 Realm
static const char* kRealm = "ivx-realm";

// 사용자/비밀번호 예시
static const char* kUser  = "user1";
static const char* kPass  = "pass1";

// (옵션) 라이브러리 패치가 'algorithm=SHA-256' 광고를 지원한다면
// 이 매크로를 켜고 'RTSPServer' 에서 사용하세요.
#ifndef USE_RTSP_DIGEST_SHA256_ADVERTISEMENT
#define USE_RTSP_DIGEST_SHA256_ADVERTISEMENT 1
#endif
#define ACCESS_CONTROL

// SHA-256(hex) 유틸
static void toHex(const unsigned char* data, size_t len, char* out /* len*2+1 */)
{
   static const char* k = "0123456789abcdef";
   for (size_t i = 0; i < len; ++i)
   {
      out[2*i]   = k[(data[i] >> 4) & 0xF];
      out[2*i+1] = k[data[i] & 0xF];
   }
   out[len*2] = '\0';
}

// HA1 = H(username : realm : password)  (여기서는 H=SHA-256)
static std::string ComputeHA1_SHA256(const std::string& user, const std::string& realm, const std::string& pass)
{
#ifdef _WIN32
   BCRYPT_ALG_HANDLE hAlg = NULL;
   BCRYPT_HASH_HANDLE hHash = NULL;
   NTSTATUS st = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
   if (st != 0) return std::string();

   DWORD objLen = 0, cb = 0;
   st = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen, sizeof(DWORD), &cb, 0);
   if (st != 0) { BCryptCloseAlgorithmProvider(hAlg, 0); return std::string(); }

   std::vector<UCHAR> obj(objLen);
   std::vector<UCHAR> out(32);

   std::string msg = user + ":" + realm + ":" + pass;

   st = BCryptCreateHash(hAlg, &hHash, obj.data(), objLen, NULL, 0, 0);
   if (st == 0) st = BCryptHashData(hHash, (PUCHAR)msg.data(), (ULONG)msg.size(), 0);
   if (st == 0) st = BCryptFinishHash(hHash, out.data(), (ULONG)out.size(), 0);

   if (hHash) BCryptDestroyHash(hHash);
   if (hAlg)  BCryptCloseAlgorithmProvider(hAlg, 0);
   if (st != 0) return std::string();

   char hex[65]; toHex(out.data(), out.size(), hex);
   return std::string(hex);
#else
   // TODO: OpenSSL 등으로 대체
   return std::string();
#endif
}

#pragma endregion


static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
   char const* streamName, char const* inputFileName); // forward

static EventLoopWatchVariable newDemuxWatchVariable;

static MatroskaFileServerDemux* matroskaDemux;

static void onMatroskaDemuxCreation(MatroskaFileServerDemux* newDemux, void* /*clientData*/) 
{
   matroskaDemux = newDemux;
   newDemuxWatchVariable = 1;
}

static OggFileServerDemux* oggDemux;
static void onOggDemuxCreation(OggFileServerDemux* newDemux, void* /*clientData*/) 
{
   oggDemux = newDemux;
   newDemuxWatchVariable = 1;
}

int main(int argc, char** argv) 
{
   fs::path cwd = fs::current_path();
   std::cout << "현재 디렉토리: " << cwd << std::endl;

   #pragma region BasicUsageEnvironment 스케줄러, 인증
 
   //TaskScheduler: 소켓/타이머 등록과 콜백 실행을 담당하는 이벤트 루프 엔진
   //UsageEnvironment: 그 스케줄러를 품고, 로그/에러 메시지·헬퍼를 제공하는 상위 컨텍스트

   TaskScheduler* scheduler = BasicTaskScheduler::createNew();
   env = BasicUsageEnvironment::createNew(*scheduler);

   UserAuthenticationDatabase* authDB = NULL;

   #ifdef ACCESS_CONTROL (기본 인증(Basic auth))
   // To implement client access control to the RTSP server, do the following:
   authDB = new UserAuthenticationDatabase;
   authDB->addUserRecord("user1", "pass1"); // replace these with real strings
   // Repeat the above with each <username>, <password> that you wish to allow
   // access to the server.
   #endif  
   #pragma endregion

   #pragma region Digest

   // === Digest 인증 활성화 ===
   // 1) UserAuthenticationDatabase 를 생성할 때 realm을 지정
   // 2) passwordIsMD5=True 로 설정하면 addUserRecord()에 'HA1'을 넣을 수 있습니다.
   //    (본 프로젝트에서는 'HA1'을 SHA-256로 계산 — 라이브러리 패치가 필요)
   {
      authDB = new UserAuthenticationDatabase(kRealm, True /*passwordsAreMD5 - 여기선 '이미 해시된 HA1' 의미*/);

      // SHA-256 HA1 계산 후 등록
      std::string ha1 = ComputeHA1_SHA256(kUser, kRealm, kPass);
      if (!ha1.empty())
      {
         // 패스워드 대신 HA1(SHA-256)을 기록
         authDB->addUserRecord(kUser, ha1.c_str());
      }
      else
      {
         // 폴백: 평문 패스워드 등록 (라이브러리가 MD5 기반인 경우 기본 동작)
         authDB->addUserRecord(kUser, kPass);
      }
   }

   #pragma endregion

   #pragma region RTSP server생성 (RTSP or RTSPs)

   #ifdef SERVER_USE_TLS
   // Serve RTSPS: RTSP over a TLS connection:
   RTSPServer* rtspServer = RTSPServer::createNew(*env, 322, authDB);
   #else
   // Serve regular RTSP (over a TCP connection):
   RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
   rtspServer->setAdvertiseDigestAlgorithmSHA256(True);
   #endif

   if (rtspServer == NULL) {
      *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
      exit(1);
   }

   #ifdef SERVER_USE_TLS
   #ifndef STREAM_USING_SRTP
   #define STREAM_USING_SRTP True
   #endif
   
   rtspServer->setTLSState(PATHNAME_TO_CERTIFICATE_FILE, PATHNAME_TO_PRIVATE_KEY_FILE, STREAM_USING_SRTP);
   #endif  
   
   #pragma endregion
   
   #pragma region ServerMediaSession

   //ServerMediaSession = 스트림 묶음(세션/URL 단위)
   //H264VideoFileServerMediaSubsession = 그 세션 안의 H.264 비디오 트랙(온디맨드 트랙).

   char const* descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

   // A H.264 video elementary stream:
   {
      char const* streamName = "h264ESVideoTest";
      char const* inputFileName = "test.264";
      ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);
      sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env, inputFileName, reuseFirstSource));
      rtspServer->addServerMediaSession(sms);

      announceStream(rtspServer, sms, streamName, inputFileName);
   }

   #pragma endregion

   #pragma region addServerMediaSession - 사용안함

   // RTSP 서버에서 제공할 수 있는 각 스트림을 설정합니다.
   // 이러한 각 스트림은
   // "ServerMediaSession" 객체와 각 오디오/비디오 하위 스트림에 대한 하나 이상의
   // "ServerMediaSubsession" 객체를 사용하여 구현됩니다.

//   // A MPEG-4 video elementary stream:
//   {
//      char const* streamName = "mpeg4ESVideoTest";
//      char const* inputFileName = "test.m4e";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(MPEG4VideoFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A H.265 video elementary stream:
//   {
//      char const* streamName = "h265ESVideoTest";
//      char const* inputFileName = "test.265";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(H265VideoFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A MPEG-1 or 2 audio+video program stream:
//   {
//      char const* streamName = "mpeg1or2AudioVideoTest";
//      char const* inputFileName = "test.mpg";
//      // NOTE: This *must* be a Program Stream; not an Elementary Stream
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      MPEG1or2FileServerDemux* demux
//         = MPEG1or2FileServerDemux::createNew(*env, inputFileName, reuseFirstSource);
//      sms->addSubsession(demux->newVideoServerMediaSubsession(iFramesOnly));
//      sms->addSubsession(demux->newAudioServerMediaSubsession());
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A MPEG-1 or 2 video elementary stream:
//   {
//      char const* streamName = "mpeg1or2ESVideoTest";
//      char const* inputFileName = "testv.mpg";
//      // NOTE: This *must* be a Video Elementary Stream; not a Program Stream
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(MPEG1or2VideoFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource, iFramesOnly));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // MP3 오디오 스트림(실제로는 모든 MPEG-1 또는 2 오디오 파일 사용 가능):
//   // 원시 MP3 프레임 대신 'ADU'를 사용하여 스트리밍하려면 다음 주석을 제거하세요.
//   //#define STREAM_USING_ADUS 1
//   // 스트리밍 전에 ADU 순서를 변경하려면 다음 주석을 제거하세요.
//   //#define INTERLEAVE_ADUS 1
//   // (ADU 및 인터리빙에 대한 자세한 내용은
//   // <http://www.live555.com/rtp-mp3/> 참조)
//   {
//      char const* streamName = "mp3AudioTest";
//      char const* inputFileName = "test.mp3";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      Boolean useADUs = False;
//      Interleaving* interleaving = NULL;
//
//#ifdef STREAM_USING_ADUS
//      useADUs = True;
//#ifdef INTERLEAVE_ADUS
//      unsigned char interleaveCycle[] = { 0,2,1,3 }; // or choose your own...
//      unsigned const interleaveCycleSize
//         = (sizeof interleaveCycle) / (sizeof(unsigned char));
//      interleaving = new Interleaving(interleaveCycleSize, interleaveCycle);
//#endif
//#endif
//
//      sms->addSubsession(MP3AudioFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource,
//            useADUs, interleaving));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A WAV audio stream:
//   {
//      char const* streamName = "wavAudioTest";
//      char const* inputFileName = "test.wav";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      // To convert 16-bit PCM data to 8-bit u-law, prior to streaming,
//      // change the following to True:
//      Boolean convertToULaw = False;
//      sms->addSubsession(WAVAudioFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource, convertToULaw));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // An AMR audio stream:
//   {
//      char const* streamName = "amrAudioTest";
//      char const* inputFileName = "test.amr";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(AMRAudioFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A 'VOB' file (e.g., from an unencrypted DVD):
//   {
//      char const* streamName = "vobTest";
//      char const* inputFileName = "test.vob";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      // Note: VOB files are MPEG-2 Program Stream files, but using AC-3 audio
//      MPEG1or2FileServerDemux* demux
//         = MPEG1or2FileServerDemux::createNew(*env, inputFileName, reuseFirstSource);
//      sms->addSubsession(demux->newVideoServerMediaSubsession(iFramesOnly));
//      sms->addSubsession(demux->newAC3AudioServerMediaSubsession());
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A MPEG-2 Transport Stream:
//   {
//      char const* streamName = "mpeg2TransportStreamTest";
//      char const* inputFileName = "test.ts";
//      char const* indexFileName = "test.tsx";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(MPEG2TransportFileServerMediaSubsession
//         ::createNew(*env, inputFileName, indexFileName, reuseFirstSource));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // An AAC audio stream (ADTS-format file):
//   {
//      char const* streamName = "aacAudioTest";
//      char const* inputFileName = "test.aac";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(ADTSAudioFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A DV video stream:
//   {
//      // First, make sure that the RTPSinks' buffers will be large enough to handle the huge size of DV frames (as big as 288000).
//      OutPacketBuffer::maxSize = 300000;
//
//      char const* streamName = "dvVideoTest";
//      char const* inputFileName = "test.dv";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(DVVideoFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource));
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A AC3 video elementary stream:
//   {
//      char const* streamName = "ac3AudioTest";
//      char const* inputFileName = "test.ac3";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//
//      sms->addSubsession(AC3AudioFileServerMediaSubsession
//         ::createNew(*env, inputFileName, reuseFirstSource));
//
//      rtspServer->addServerMediaSession(sms);
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A Matroska ('.mkv') file, with video+audio+subtitle streams:
//   {
//      char const* streamName = "matroskaFileTest";
//      char const* inputFileName = "test.mkv";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//
//      newDemuxWatchVariable = 0;
//      MatroskaFileServerDemux::createNew(*env, inputFileName, onMatroskaDemuxCreation, NULL);
//      env->taskScheduler().doEventLoop(&newDemuxWatchVariable);
//
//      Boolean sessionHasTracks = False;
//      ServerMediaSubsession* smss;
//      while ((smss = matroskaDemux->newServerMediaSubsession()) != NULL) {
//         sms->addSubsession(smss);
//         sessionHasTracks = True;
//      }
//      if (sessionHasTracks) {
//         rtspServer->addServerMediaSession(sms);
//      }
//      // otherwise, because the stream has no tracks, we don't add a ServerMediaSession to the server.
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A WebM ('.webm') file, with video(VP8)+audio(Vorbis) streams:
//   // (Note: ".webm' files are special types of Matroska files, so we use the same code as the Matroska ('.mkv') file code above.)
//   {
//      char const* streamName = "webmFileTest";
//      char const* inputFileName = "test.webm";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//
//      newDemuxWatchVariable = 0;
//      MatroskaFileServerDemux::createNew(*env, inputFileName, onMatroskaDemuxCreation, NULL);
//      env->taskScheduler().doEventLoop(&newDemuxWatchVariable);
//
//      Boolean sessionHasTracks = False;
//      ServerMediaSubsession* smss;
//      while ((smss = matroskaDemux->newServerMediaSubsession()) != NULL) {
//         sms->addSubsession(smss);
//         sessionHasTracks = True;
//      }
//      if (sessionHasTracks) {
//         rtspServer->addServerMediaSession(sms);
//      }
//      // otherwise, because the stream has no tracks, we don't add a ServerMediaSession to the server.
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // An Ogg ('.ogg') file, with video and/or audio streams:
//   {
//      char const* streamName = "oggFileTest";
//      char const* inputFileName = "test.ogg";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//
//      newDemuxWatchVariable = 0;
//      OggFileServerDemux::createNew(*env, inputFileName, onOggDemuxCreation, NULL);
//      env->taskScheduler().doEventLoop(&newDemuxWatchVariable);
//
//      Boolean sessionHasTracks = False;
//      ServerMediaSubsession* smss;
//      while ((smss = oggDemux->newServerMediaSubsession()) != NULL) {
//         sms->addSubsession(smss);
//         sessionHasTracks = True;
//      }
//      if (sessionHasTracks) {
//         rtspServer->addServerMediaSession(sms);
//      }
//      // otherwise, because the stream has no tracks, we don't add a ServerMediaSession to the server.
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // An Opus ('.opus') audio file:
//   // (Note: ".opus' files are special types of Ogg files, so we use the same code as the Ogg ('.ogg') file code above.)
//   {
//      char const* streamName = "opusFileTest";
//      char const* inputFileName = "test.opus";
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//
//      newDemuxWatchVariable = 0;
//      OggFileServerDemux::createNew(*env, inputFileName, onOggDemuxCreation, NULL);
//      env->taskScheduler().doEventLoop(&newDemuxWatchVariable);
//
//      Boolean sessionHasTracks = False;
//      ServerMediaSubsession* smss;
//      while ((smss = oggDemux->newServerMediaSubsession()) != NULL) {
//         sms->addSubsession(smss);
//         sessionHasTracks = True;
//      }
//      if (sessionHasTracks) {
//         rtspServer->addServerMediaSession(sms);
//      }
//      // otherwise, because the stream has no tracks, we don't add a ServerMediaSession to the server.
//
//      announceStream(rtspServer, sms, streamName, inputFileName);
//   }
//
//   // A MPEG-2 Transport Stream, coming from a live UDP (raw-UDP or RTP/UDP) source:
//   {
//      char const* streamName = "mpeg2TransportStreamFromUDPSourceTest";
//      char const* inputAddressStr = "239.255.42.42";
//      // This causes the server to take its input from the stream sent by the "testMPEG2TransportStreamer" demo application.
//      // (Note: If the input UDP source is unicast rather than multicast, then change this to NULL.)
//      portNumBits const inputPortNum = 1234;
//      // This causes the server to take its input from the stream sent by the "testMPEG2TransportStreamer" demo application.
//      Boolean const inputStreamIsRawUDP = False;
//      ServerMediaSession* sms
//         = ServerMediaSession::createNew(*env, streamName, streamName,
//            descriptionString);
//      sms->addSubsession(MPEG2TransportUDPServerMediaSubsession
//         ::createNew(*env, inputAddressStr, inputPortNum, inputStreamIsRawUDP));
//      rtspServer->addServerMediaSession(sms);
//
//      *env << "\n\"" << streamName << "\" stream, from a UDP Transport Stream input source \n\t(";
//      if (inputAddressStr != NULL) {
//         *env << "IP multicast address " << inputAddressStr << ",";
//      }
//      else {
//         *env << "unicast;";
//      }
//      *env << " port " << inputPortNum << ")\n";
//      announceURL(rtspServer, sms);
//   }

   #pragma endregion


   #pragma region  RTSP-over-HTTP 터널링 (HTTP 포트(80) -> 대체포트번호(8000 및 8080)로 시도)

   #ifdef SERVER_USE_TLS
   // (Attempt to) use the default HTTPS port (443) instead:
   char const* httpProtocolStr = "HTTPS";
   if (rtspServer->setUpTunnelingOverHTTP(443)) {

   #else
   char const* httpProtocolStr = "HTTP";
   if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
   #endif

      *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-" << httpProtocolStr << " tunneling.)\n";
   }
   else {
      *env << "\n(RTSP-over-" << httpProtocolStr << " tunneling is not available.)\n";
   }
   #pragma endregion


   env->taskScheduler().doEventLoop(); // does not return

   return 0; // only to prevent compiler warning
}

//파일이름과 매칭된 스트림이름을 로그출력(UsageEnvironment)
static void announceStream(RTSPServer * rtspServer, ServerMediaSession * sms, char const* streamName, char const* inputFileName) 
{
   UsageEnvironment& env = rtspServer->envir();

   env << "\n\"" << streamName << "\" stream, from the file \"" << inputFileName << "\"\n";
   announceURL(rtspServer, sms);
}
