
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <string>

//기본값으로 RTP/UDP 사용. RTP-over-TCP 사용하려면 True설정
#define REQUEST_STREAMING_OVER_TCP True

#pragma region 전방선언

// RTSP 'response handlers':
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData, char const* reason);
// called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
// called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// The main streaming routine (for each "rtsp://" URL):
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL);

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);

#pragma endregion

#pragma region Digest 인증

// 전역 Authenticator (간단히 한 개만 사용).
// 실제 서비스라면 URL별, 세션별로 보관하세요.
static Authenticator g_auth;
static std::string g_user, g_pass;

// 소문자 변환
static std::string ToLower(std::string s)
{
   for (char& c : s) c = (char)std::tolower((unsigned char)c);
   return s;
}

// 앞뒤 공백 제거
static std::string Trim(const std::string& s)
{
   size_t b = s.find_first_not_of(" \t\r\n");
   size_t e = s.find_last_not_of(" \t\r\n");
   if (b == std::string::npos) return std::string();
   return s.substr(b, e - b + 1);
}

// WWW-Authenticate 한 줄(or 전체 값)에서 algorithm 파라미터 파싱 → g_auth.setUseSHA256()
static void ApplyAlgorithmFromWWWAuthenticateLine(const char* wwwAuthLine)
{
   if (!wwwAuthLine) return;

   std::string line = wwwAuthLine;
   // 접두사 제거
   const std::string key = "www-authenticate:";
   std::string lowerLine = ToLower(line);
   size_t pos = lowerLine.find(key);
   if (pos != std::string::npos)
   {
      line = Trim(line.substr(pos + key.size()));
      lowerLine = ToLower(line);
   }

   if (lowerLine.find("digest") == std::string::npos)
   {
      return;
   }

   // algorithm= 찾기
   size_t p = lowerLine.find("algorithm");
   if (p == std::string::npos)
   {
      // 미지정 → MD5
      g_auth.setUseSHA256(False);
      return;
   }
   size_t eq = lowerLine.find('=', p);
   if (eq == std::string::npos)
   {
      g_auth.setUseSHA256(False);
      return;
   }

   size_t i = eq + 1;
   while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) ++i;

   std::string alg;
   if (i < line.size() && line[i] == '"')
   {
      size_t j = line.find('"', i + 1);
      if (j == std::string::npos) j = line.size();
      alg = line.substr(i + 1, j - (i + 1));
   }
   else
   {
      size_t j = i;
      while (j < line.size() && line[j] != ',' && line[j] != ' ' && line[j] != '\t' && line[j] != '\r' && line[j] != '\n') ++j;
      alg = line.substr(i, j - i);
   }

   std::string algLower = ToLower(Trim(alg));
   if (algLower.rfind("sha-256", 0) == 0) // "sha-256" 또는 "sha-256-sess"
   {
      g_auth.setUseSHA256(True);
   }
   else
   {
      g_auth.setUseSHA256(False); // MD5, MD5-sess, 미지정
   }
}

// 헤더 전체 문자열에서 WWW-Authenticate: Digest ... 줄 찾아 algorithm 적용
static void ApplyAlgorithmFromAllHeaders(const char* allHeaders)
{
   if (!allHeaders) return;
   const char* p = allHeaders;
   while (*p)
   {
      const char* e = strstr(p, "\r\n");
      std::string line = (e ? std::string(p, e) : std::string(p));
      std::string lwr = ToLower(line);
      if (lwr.find("www-authenticate:") != std::string::npos &&
         lwr.find("digest") != std::string::npos)
      {
         ApplyAlgorithmFromWWWAuthenticateLine(line.c_str());
         return;
      }
      if (!e) break;
      p = e + 2;
   }
}

// realm, nonce 간단 파싱 (따옴표 값 가정)
static std::string ExtractParamQuoted(const std::string& headers, const std::string& key)
{
   std::string L = ToLower(headers);
   std::string k = ToLower(key);
   size_t p = L.find(k);
   if (p == std::string::npos) return std::string();
   p = L.find('=', p);
   if (p == std::string::npos) return std::string();
   ++p;
   while (p < headers.size() && (headers[p] == ' ' || headers[p] == '\t')) ++p;
   if (p < headers.size() && headers[p] == '"')
   {
      size_t j = headers.find('"', p + 1);
      if (j == std::string::npos) return std::string();
      return headers.substr(p + 1, j - (p + 1));
   }
   // 비인용 값
   size_t j = p;
   while (j < headers.size() && headers[j] != ',' && headers[j] != '\r' && headers[j] != '\n') ++j;
   return Trim(headers.substr(p, j - p));
}

// URL에서 user:pass 추출 (rtsp://user:pass@host/...)
static void ParseUserPassFromURL(const char* rtspURL)
{
   g_user.clear(); g_pass.clear();
   std::string url = rtspURL ? rtspURL : "";
   const std::string scheme = "rtsp://";
   if (ToLower(url).rfind(scheme, 0) != 0) return;
   size_t at = url.find('@');
   if (at == std::string::npos) return;
   size_t start = scheme.size();
   std::string userinfo = url.substr(start, at - start); // user:pass
   size_t colon = userinfo.find(':');
   if (colon == std::string::npos)
   {
      g_user = userinfo;
      g_pass.clear();
   }
   else
   {
      g_user = userinfo.substr(0, colon);
      g_pass = userinfo.substr(colon + 1);
   }
   // Authenticator에 반영
   g_auth.setUsernameAndPassword(g_user.c_str(), g_pass.c_str(), False /*plain*/);
}

#pragma endregion


#pragma region 메인

// A function that outputs a string that identifies each stream (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const RTSPClient& rtspClient) {
   return env << "[URL:\"" << rtspClient.url() << "\"]: ";
}

// A function that outputs a string that identifies each subsession (for debugging output).  Modify this if you wish:
UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession) {
   return env << subsession.mediumName() << "/" << subsession.codecName();
}

void usage(UsageEnvironment& env, char const* progName) {
   env << "Usage: " << progName << " <rtsp-url-1> ... <rtsp-url-N>\n";
   env << "\t(where each <rtsp-url-i> is a \"rtsp://\" URL)\n";
}

EventLoopWatchVariable eventLoopWatchVariable = 0;

int main(int argc, char** argv) 
{
   //UsageEnvironment 사용환경 셋업
   TaskScheduler* scheduler = BasicTaskScheduler::createNew();
   UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

   // We need at least one "rtsp://" URL argument:
   if (argc < 2) {
      usage(*env, argv[0]);
      return 1;
   }

   //argc-1 URL은 argv[1]부터 argv[argc-1]까지 있습니다. 각 URL을 열고 스트리밍을 시작하세요.
   for (int i = 1; i <= argc - 1; ++i) {
      openURL(*env, argv[0], argv[i]);
   }

   // 이후의 모든 활동은 이벤트 루프 내에서 진행됩니다.
   env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
   // 이 함수 호출은 "eventLoopWatchVariable"이 0이 아닌 값으로 설정되는 특정 시점을 제외하고는 반환되지 않습니다.

   return 0;
}

#pragma endregion


#pragma region StreamClientState(스트림별 상태보관)

//스트림별 상태보관
class StreamClientState {
public:
   StreamClientState();
   virtual ~StreamClientState();

public:
   MediaSubsessionIterator* iter;
   MediaSession* session;
   MediaSubsession* subsession;
   TaskToken streamTimerTask;
   double duration;
   // 401 재시도 시 무한루프 방지
   bool retriedWithAuth = false;
};

StreamClientState::StreamClientState()
   : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
   delete iter;
   if (session != NULL) {
      // 또한 "session"을 삭제하고 "streamTimerTask"(설정된 경우)의 일정을 취소해야 합니다.
      UsageEnvironment& env = session->envir(); // alias

      env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
      Medium::close(session);
   }
}

#pragma endregion

#pragma region ourRTSPClient

//개별 RTSPClient 마다 스트림상태(StreamClientState) 속성 추가
class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
				  int verbosityLevel = 0,
				  char const* applicationName = NULL,
				  portNumBits tunnelOverHTTPPortNum = 0);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};


ourRTSPClient* ourRTSPClient::createNew(UsageEnvironment& env, char const* rtspURL,
   int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum) {
   return new ourRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum);
}

ourRTSPClient::ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
   int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum)
   : RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, -1) {
}

ourRTSPClient::~ourRTSPClient() {
}

#pragma endregion

#pragma region DummySink

// 각 하위 세션(즉, 각 오디오 또는 비디오 '서브스트림')의 데이터를 수신하는 데이터 싱크("MediaSink"의 하위 클래스)를 정의합니다.
// 실제로는 수신 오디오 또는 비디오를 디코딩한 다음 렌더링하는 클래스(또는 클래스 체인)일 수 있습니다.
// 또는 수신된 데이터를 파일로 출력하는 "FileSink"일 수도 있습니다("openRTSP" 애플리케이션에서처럼).
// 하지만 이 예제 코드에서는 수신 데이터를 수신하지만 아무런 작업도 하지 않는 간단한 '더미' 싱크를 정의합니다.

class DummySink: public MediaSink {
public:
  static DummySink* createNew(UsageEnvironment& env,
			      MediaSubsession& subsession, // identifies the kind of data that's being received
			      char const* streamId = NULL); // identifies the stream itself (optional)

private:
  DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId);
    // called only by "createNew()"
  virtual ~DummySink();

  static void afterGettingFrame(void* clientData, unsigned frameSize,
                                unsigned numTruncatedBytes,
				struct timeval presentationTime,
                                unsigned durationInMicroseconds);
  void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
			 struct timeval presentationTime, unsigned durationInMicroseconds);

private:
  // redefined virtual functions:
  virtual Boolean continuePlaying();

private:
  u_int8_t* fReceiveBuffer;
  MediaSubsession& fSubsession;
  char* fStreamId;
};

// 들어오는 데이터를 처리하지 않더라도, 데이터를 수신해야 합니다.
// 사용할 버퍼 크기를 정의합니다.
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 100000

DummySink* DummySink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId) {
   return new DummySink(env, subsession, streamId);
}

DummySink::DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
   : MediaSink(env),
   fSubsession(subsession) {
   fStreamId = strDup(streamId);
   fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
}

DummySink::~DummySink() {
   delete[] fReceiveBuffer;
   delete[] fStreamId;
}

void DummySink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
   struct timeval presentationTime, unsigned durationInMicroseconds) {
   DummySink* sink = (DummySink*)clientData;
   sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

// 수신된 각 프레임에 대한 디버깅 출력을 보고 싶지 않으면 다음 줄을 주석으로 처리하세요.
#define DEBUG_PRINT_EACH_RECEIVED_FRAME 1

void DummySink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
   struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
   // We've just received a frame of data.  (Optionally) print out information about it:
#ifdef DEBUG_PRINT_EACH_RECEIVED_FRAME
   if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
   envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";
   if (numTruncatedBytes > 0) envir() << " (with " << numTruncatedBytes << " bytes truncated)";
   char uSecsStr[6 + 1]; // used to output the 'microseconds' part of the presentation time
   sprintf(uSecsStr, "%06u", (unsigned)presentationTime.tv_usec);
   envir() << ".\tPresentation time: " << (int)presentationTime.tv_sec << "." << uSecsStr;
   if (fSubsession.rtpSource() != NULL && !fSubsession.rtpSource()->hasBeenSynchronizedUsingRTCP()) {
      envir() << "!"; // mark the debugging output to indicate that this presentation time is not RTCP-synchronized
   }
#ifdef DEBUG_PRINT_NPT
   envir() << "\tNPT: " << fSubsession.getNormalPlayTime(presentationTime);
#endif
   envir() << "\n";
#endif

   // Then continue, to request the next frame of data:
   continuePlaying();
}

Boolean DummySink::continuePlaying() {
   if (fSource == NULL) return False; // sanity check (should not happen)

   // 입력 소스에서 다음 데이터 프레임을 요청합니다. "afterGettingFrame()"은 나중에 데이터가 도착하면 호출됩니다.
   fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
      afterGettingFrame, this,
      onSourceClosure, this);
   return True;
}

#pragma endregion


#pragma region openURL, describe, setup, play 핸들러

#define RTSP_CLIENT_VERBOSITY_LEVEL 1 // 기본적으로 각 "RTSPClient"에서 자세한 출력을 인쇄합니다.

static unsigned rtspClientCount = 0; // 현재 사용 중인 스트림(즉, "RTSPClient")의 수를 계산합니다.

//DESCRIBE 명령
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL) 
{
  // 수신하려는 각 스트림마다 별도의 "RTSPClient" 객체생성
  RTSPClient* rtspClient = ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
  if (rtspClient == NULL) {
    env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
    return;
  }

  ++rtspClientCount;

  //RTSP "DESCRIBE" 명령을 전송하여 스트림에 대한 SDP 설명을 가져옴. RTSP 응답은 나중에 이벤트 루프 내에서 처리됩니다.
  rtspClient->sendDescribeCommand(continueAfterDESCRIBE); 
}

//DESCRIBE 명령에 대한 응답을 처리
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
   do 
   {
      UsageEnvironment& env = rtspClient->envir();
      StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

      if (resultCode != 0) //describe 실패
      {
         // 401 Unauthorized 인 경우, 헤더에서 algorithm/realm/nonce를 추출하여 재시도
         std::string msg = resultString ? resultString : "";
         env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";

         // 일부 서버/빌드에선 resultString에 전체 헤더가 포함되지 않을 수 있습니다.
         // 가능한 경우를 대비해 여기서 파싱을 시도합니다.
         if (!scs.retriedWithAuth && (msg.find("401") != std::string::npos || msg.find("WWW-Authenticate") != std::string::npos))
         {
            // algorithm 자동 결정
            ApplyAlgorithmFromAllHeaders(msg.c_str());

            // realm, nonce 추출(따옴표 값 가정)
            std::string realm = ExtractParamQuoted(msg, "realm");
            std::string nonce = ExtractParamQuoted(msg, "nonce");
            if (!realm.empty() && !nonce.empty())
            {
               g_auth.setRealmAndNonce(realm.c_str(), nonce.c_str());
            }

            // user/pass 가 있는 경우 재시도
            if (!g_user.empty())
            {
               env << *rtspClient << "Retry DESCRIBE with Digest (SHA-256=" << (g_auth.useSHA256() ? "on" : "off") << ")\n";
               scs.retriedWithAuth = true;
               rtspClient->sendDescribeCommand(continueAfterDESCRIBE, &g_auth);
               delete[] resultString;
               return; // 재시도
            }
         }

         delete[] resultString;
         break;
      }

      char* const sdpDescription = resultString;
      env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

      //SDP 문자열 -> MediaSession 객체 생성
      scs.session = MediaSession::createNew(env, sdpDescription);
      delete[] sdpDescription;
      if (scs.session == NULL) {
         env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
         break;
      }
      else if (!scs.session->hasSubsessions()) {
         env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
         break;
      }

      //하위세션(Subsession) 설정 -> MediaSubsession::initiate() 호출하여 하위세션마다 "SETUP" 명령 전송
      scs.iter = new MediaSubsessionIterator(*scs.session);
      setupNextSubsession(rtspClient);
      return;
   } while (0);

   // An unrecoverable error occurred with this stream.
   shutdownStream(rtspClient);
}

//void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) 
//{
//   do 
//   {
//      UsageEnvironment& env = rtspClient->envir();
//      StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;
//
//      if (resultCode != 0) //describe 실패
//      {
//         env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
//         delete[] resultString;
//         break;
//      }
//
//      char* const sdpDescription = resultString;
//      env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";
//
//      //SDP 문자열 -> MediaSession 객체 생성
//      scs.session = MediaSession::createNew(env, sdpDescription);
//      delete[] sdpDescription;
//      if (scs.session == NULL) {
//         env << *rtspClient << "Failed to create a MediaSession object from the SDP description: " << env.getResultMsg() << "\n";
//         break;
//      }
//      else if (!scs.session->hasSubsessions()) {
//         env << *rtspClient << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
//         break;
//      }
//
//      //하위세션(Subsession) 설정 -> MediaSubsession::initiate() 호출하여 하위세션마다 "SETUP" 명령 전송
//      scs.iter = new MediaSubsessionIterator(*scs.session);
//      setupNextSubsession(rtspClient);
//      return;
//   } while (0);
//
//   // An unrecoverable error occurred with this stream.
//   shutdownStream(rtspClient);
//}

//하위세션(Subsession) 설정 -> MediaSubsession::initiate() 호출하여 하위세션마다 "SETUP" 명령 전송
void setupNextSubsession(RTSPClient* rtspClient) 
{
   UsageEnvironment& env = rtspClient->envir();
   StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

   scs.subsession = scs.iter->next();
   if (scs.subsession != NULL) 
   {
      if (!scs.subsession->initiate()) 
      {
         env << *rtspClient << "Failed to initiate the \"" << *scs.subsession << "\" subsession: " << env.getResultMsg() << "\n";
         setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
      }
      else 
      {
         env << *rtspClient << "Initiated the \"" << *scs.subsession << "\" subsession (";
         if (scs.subsession->rtcpIsMuxed()) {
            env << "client port " << scs.subsession->clientPortNum();
         }
         else {
            env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1;
         }
         env << ")\n";

         //subsession->initiate 성공후 "SETUP" 명령 전송
         rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
      }
      return;
   }

   //RTSP "PLAY" 명령
   if (scs.session->absStartTime() != NULL) 
   {
      //absStartTime() 이 있으면 절대 시간(UTC/시계시간) 기반 재생을 시도하고,
      //아니면 NPT 기반(상대 시간, ‘now’ 또는 0부터) 으로 PLAY를 보냅니다. 예) 2025-09-09 09:30:00부터 09:35:00까지 재생
      rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());

      // 라이브를 5초 전부터 재생(타임시프트 지원 서버에서만)
      //rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, "-5.000", NULL);

      double scale = 1.5; // 빨리감기
      //rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, 0, 0, scale);

   }
   else 
   {
      scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
      rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
   }
}

//SETUP 명령 응답 처리
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
   do 
   {
      UsageEnvironment& env = rtspClient->envir();
      StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

      if (resultCode != 0) {
         env << *rtspClient << "Failed to set up the \"" << *scs.subsession << "\" subsession: " << resultString << "\n";
         break;
      }

      env << *rtspClient << "Set up the \"" << *scs.subsession << "\" subsession (";
      if (scs.subsession->rtcpIsMuxed()) {
         env << "client port " << scs.subsession->clientPortNum();
      }
      else {
         env << "client ports " << scs.subsession->clientPortNum() << "-" << scs.subsession->clientPortNum() + 1;
      }
      env << ")\n";

      //데이터 싱크를 생성하고 "startPlaying()"을 호출
      scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url());

      if (scs.subsession->sink == NULL) 
      {
         env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
            << "\" subsession: " << env.getResultMsg() << "\n";
         break;
      }
      env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";

      scs.subsession->miscPtr = rtspClient; //핸들러들이 RTSPClient에 접근하도록 저장
      
      scs.subsession->sink->startPlaying(*(scs.subsession->readSource()), subsessionAfterPlaying, scs.subsession);

      //송신 측(카메라/서버)이 세션 종료를 알릴 때 오는 RTCP BYE를 처리
      if (scs.subsession->rtcpInstance() != NULL) 
      {
         scs.subsession->rtcpInstance()->setByeWithReasonHandler(subsessionByeHandler, scs.subsession);
      }
   } while (0);
   delete[] resultString;

   // Set up the next subsession, if any:
   setupNextSubsession(rtspClient);
}

//PLAY 명령 응답 처리. 미디어길이(scs.duration) 정보가 있으면 해당 시간 이후에 종료(BYE)
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
   Boolean success = False;

   do 
   {
      UsageEnvironment& env = rtspClient->envir();
      StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

      if (resultCode != 0) {
         env << *rtspClient << "Failed to start playing session: " << resultString << "\n";
         break;
      }

      // PLAY이후 스트림종료처리하고 싶으면 타이머를 설정(RTCP "BYE")
      // 스트림을 활성 상태로 유지하려는 경우(예: 나중에 스트림 내에서 '탐색'하여 RTSP "PLAY"를 다시 수행할 수 있도록) 이 코드를 생략가능
      // (또는 전체 스트림을 수신하지 않으려면 이 타이머를 더 짧은 값으로 설정)
      if (scs.duration > 0) 
      {
         unsigned const delaySlop = 2; //2초 여유시간
         scs.duration += delaySlop;
         unsigned uSecsToDelay = (unsigned)(scs.duration * 1000000);
         scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
      }

      env << *rtspClient << "Started playing session";
      if (scs.duration > 0) {
         env << " (for up to " << scs.duration << " seconds)";
      }
      env << "...\n";

      success = True;
   } while (0);
   delete[] resultString;

   if (!success) {
      // An unrecoverable error occurred with this stream.
      shutdownStream(rtspClient);
   }
}

#pragma endregion

#pragma region 핸들러(subsessionAfterPlaying, subsessionByeHandler, streamTimerHandler, shutdownStream)

//PLAY이후 트랙종료시 호출됨
void subsessionAfterPlaying(void* clientData) 
{
   MediaSubsession* subsession = (MediaSubsession*)clientData;
   RTSPClient* rtspClient = (RTSPClient*)(subsession->miscPtr);

   // Begin by closing this subsession's stream:
   Medium::close(subsession->sink);
   subsession->sink = NULL;

   // Next, check whether *all* subsessions' streams have now been closed:
   MediaSession& session = subsession->parentSession();
   MediaSubsessionIterator iter(session);
   while ((subsession = iter.next()) != NULL) {
      if (subsession->sink != NULL) return; // this subsession is still active
   }

   // All subsessions' streams have now been closed, so shutdown the client:
   shutdownStream(rtspClient);
}

//송신 측(카메라/서버)이 세션 종료를 알릴 때 오는 RTCP BYE를 처리
void subsessionByeHandler(void* clientData, char const* reason) 
{
   MediaSubsession* subsession = (MediaSubsession*)clientData;
   RTSPClient* rtspClient = (RTSPClient*)subsession->miscPtr;
   UsageEnvironment& env = rtspClient->envir(); // alias

   env << *rtspClient << "Received RTCP \"BYE\"";
   if (reason != NULL) {
      env << " (reason:\"" << reason << "\")";
      delete[](char*)reason;
   }
   env << " on \"" << *subsession << "\" subsession\n";

   // Now act as if the subsession had closed:
   subsessionAfterPlaying(subsession);
}

//PLAY이후 스트림종료처리하고 싶으면 타이머에서 호출됨(RTCP "BYE")
void streamTimerHandler(void* clientData) 
{
   ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
   StreamClientState& scs = rtspClient->scs; // alias

   scs.streamTimerTask = NULL;

   // Shut down the stream:
   shutdownStream(rtspClient);
}

//서버가 "TEARDOWN" 보내고, 스트림 중지
void shutdownStream(RTSPClient* rtspClient, int exitCode) 
{
   UsageEnvironment& env = rtspClient->envir();
   StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

   //먼저, 닫아야 할 하위 세션이 있는지 확인하세요.
   if (scs.session != NULL) 
   {
      Boolean someSubsessionsWereActive = False;
      MediaSubsessionIterator iter(*scs.session);
      MediaSubsession* subsession;

      while ((subsession = iter.next()) != NULL) 
      {
         if (subsession->sink != NULL) 
         {
            Medium::close(subsession->sink);
            subsession->sink = NULL;

            if (subsession->rtcpInstance() != NULL) {
               subsession->rtcpInstance()->setByeHandler(NULL, NULL);//서버가 "TEARDOWN"을 처리하는 동안 RTCP "BYE"를 보내는 경우
            }

            someSubsessionsWereActive = True;
         }
      }

      if (someSubsessionsWereActive) {
         // RTSP "TEARDOWN" 명령을 전송하여 서버에 스트림을 종료하도록 지시합니다.
         // "TEARDOWN"에 대한 응답은 처리하지 마세요.
         rtspClient->sendTeardownCommand(*scs.session, NULL);
      }
   }

   env << *rtspClient << "Closing the stream.\n";
   Medium::close(rtspClient);
   // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

   if (--rtspClientCount == 0) {
      // 마지막 스트림이 종료되었으므로 지금 애플리케이션을 종료합니다.
      // (물론, 이 코드를 자신의 애플리케이션에 임베드하는 경우 이 부분을 주석 처리하고
      // "eventLoopWatchVariable = 1;"로 대체하여 LIVE555 이벤트 루프를 종료하고 "main()"을 계속 실행할 수 있습니다.)
      exit(exitCode);
   }
}

#pragma endregion

