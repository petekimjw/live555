
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <string>

//�⺻������ RTP/UDP ���. RTP-over-TCP ����Ϸ��� True����
#define REQUEST_STREAMING_OVER_TCP True

#pragma region ���漱��

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

#pragma region Digest ����

// ���� Authenticator (������ �� ���� ���).
// ���� ���񽺶�� URL��, ���Ǻ��� �����ϼ���.
static Authenticator g_auth;
static std::string g_user, g_pass;

// �ҹ��� ��ȯ
static std::string ToLower(std::string s)
{
   for (char& c : s) c = (char)std::tolower((unsigned char)c);
   return s;
}

// �յ� ���� ����
static std::string Trim(const std::string& s)
{
   size_t b = s.find_first_not_of(" \t\r\n");
   size_t e = s.find_last_not_of(" \t\r\n");
   if (b == std::string::npos) return std::string();
   return s.substr(b, e - b + 1);
}

// WWW-Authenticate �� ��(or ��ü ��)���� algorithm �Ķ���� �Ľ� �� g_auth.setUseSHA256()
static void ApplyAlgorithmFromWWWAuthenticateLine(const char* wwwAuthLine)
{
   if (!wwwAuthLine) return;

   std::string line = wwwAuthLine;
   // ���λ� ����
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

   // algorithm= ã��
   size_t p = lowerLine.find("algorithm");
   if (p == std::string::npos)
   {
      // ������ �� MD5
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
   if (algLower.rfind("sha-256", 0) == 0) // "sha-256" �Ǵ� "sha-256-sess"
   {
      g_auth.setUseSHA256(True);
   }
   else
   {
      g_auth.setUseSHA256(False); // MD5, MD5-sess, ������
   }
}

// ��� ��ü ���ڿ����� WWW-Authenticate: Digest ... �� ã�� algorithm ����
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

// realm, nonce ���� �Ľ� (����ǥ �� ����)
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
   // ���ο� ��
   size_t j = p;
   while (j < headers.size() && headers[j] != ',' && headers[j] != '\r' && headers[j] != '\n') ++j;
   return Trim(headers.substr(p, j - p));
}

// URL���� user:pass ���� (rtsp://user:pass@host/...)
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
   // Authenticator�� �ݿ�
   g_auth.setUsernameAndPassword(g_user.c_str(), g_pass.c_str(), False /*plain*/);
}

#pragma endregion


#pragma region ����

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
   //UsageEnvironment ���ȯ�� �¾�
   TaskScheduler* scheduler = BasicTaskScheduler::createNew();
   UsageEnvironment* env = BasicUsageEnvironment::createNew(*scheduler);

   // We need at least one "rtsp://" URL argument:
   if (argc < 2) {
      usage(*env, argv[0]);
      return 1;
   }

   //argc-1 URL�� argv[1]���� argv[argc-1]���� �ֽ��ϴ�. �� URL�� ���� ��Ʈ������ �����ϼ���.
   for (int i = 1; i <= argc - 1; ++i) {
      openURL(*env, argv[0], argv[i]);
   }

   // ������ ��� Ȱ���� �̺�Ʈ ���� ������ ����˴ϴ�.
   env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
   // �� �Լ� ȣ���� "eventLoopWatchVariable"�� 0�� �ƴ� ������ �����Ǵ� Ư�� ������ �����ϰ�� ��ȯ���� �ʽ��ϴ�.

   return 0;
}

#pragma endregion


#pragma region StreamClientState(��Ʈ���� ���º���)

//��Ʈ���� ���º���
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
   // 401 ��õ� �� ���ѷ��� ����
   bool retriedWithAuth = false;
};

StreamClientState::StreamClientState()
   : iter(NULL), session(NULL), subsession(NULL), streamTimerTask(NULL), duration(0.0) {
}

StreamClientState::~StreamClientState() {
   delete iter;
   if (session != NULL) {
      // ���� "session"�� �����ϰ� "streamTimerTask"(������ ���)�� ������ ����ؾ� �մϴ�.
      UsageEnvironment& env = session->envir(); // alias

      env.taskScheduler().unscheduleDelayedTask(streamTimerTask);
      Medium::close(session);
   }
}

#pragma endregion

#pragma region ourRTSPClient

//���� RTSPClient ���� ��Ʈ������(StreamClientState) �Ӽ� �߰�
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

// �� ���� ����(��, �� ����� �Ǵ� ���� '���꽺Ʈ��')�� �����͸� �����ϴ� ������ ��ũ("MediaSink"�� ���� Ŭ����)�� �����մϴ�.
// �����δ� ���� ����� �Ǵ� ������ ���ڵ��� ���� �������ϴ� Ŭ����(�Ǵ� Ŭ���� ü��)�� �� �ֽ��ϴ�.
// �Ǵ� ���ŵ� �����͸� ���Ϸ� ����ϴ� "FileSink"�� ���� �ֽ��ϴ�("openRTSP" ���ø����̼ǿ���ó��).
// ������ �� ���� �ڵ忡���� ���� �����͸� ���������� �ƹ��� �۾��� ���� �ʴ� ������ '����' ��ũ�� �����մϴ�.

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

// ������ �����͸� ó������ �ʴ���, �����͸� �����ؾ� �մϴ�.
// ����� ���� ũ�⸦ �����մϴ�.
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

// ���ŵ� �� �����ӿ� ���� ����� ����� ���� ���� ������ ���� ���� �ּ����� ó���ϼ���.
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

   // �Է� �ҽ����� ���� ������ �������� ��û�մϴ�. "afterGettingFrame()"�� ���߿� �����Ͱ� �����ϸ� ȣ��˴ϴ�.
   fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
      afterGettingFrame, this,
      onSourceClosure, this);
   return True;
}

#pragma endregion


#pragma region openURL, describe, setup, play �ڵ鷯

#define RTSP_CLIENT_VERBOSITY_LEVEL 1 // �⺻������ �� "RTSPClient"���� �ڼ��� ����� �μ��մϴ�.

static unsigned rtspClientCount = 0; // ���� ��� ���� ��Ʈ��(��, "RTSPClient")�� ���� ����մϴ�.

//DESCRIBE ���
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL) 
{
  // �����Ϸ��� �� ��Ʈ������ ������ "RTSPClient" ��ü����
  RTSPClient* rtspClient = ourRTSPClient::createNew(env, rtspURL, RTSP_CLIENT_VERBOSITY_LEVEL, progName);
  if (rtspClient == NULL) {
    env << "Failed to create a RTSP client for URL \"" << rtspURL << "\": " << env.getResultMsg() << "\n";
    return;
  }

  ++rtspClientCount;

  //RTSP "DESCRIBE" ����� �����Ͽ� ��Ʈ���� ���� SDP ������ ������. RTSP ������ ���߿� �̺�Ʈ ���� ������ ó���˴ϴ�.
  rtspClient->sendDescribeCommand(continueAfterDESCRIBE); 
}

//DESCRIBE ��ɿ� ���� ������ ó��
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString) 
{
   do 
   {
      UsageEnvironment& env = rtspClient->envir();
      StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

      if (resultCode != 0) //describe ����
      {
         // 401 Unauthorized �� ���, ������� algorithm/realm/nonce�� �����Ͽ� ��õ�
         std::string msg = resultString ? resultString : "";
         env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";

         // �Ϻ� ����/���忡�� resultString�� ��ü ����� ���Ե��� ���� �� �ֽ��ϴ�.
         // ������ ��츦 ����� ���⼭ �Ľ��� �õ��մϴ�.
         if (!scs.retriedWithAuth && (msg.find("401") != std::string::npos || msg.find("WWW-Authenticate") != std::string::npos))
         {
            // algorithm �ڵ� ����
            ApplyAlgorithmFromAllHeaders(msg.c_str());

            // realm, nonce ����(����ǥ �� ����)
            std::string realm = ExtractParamQuoted(msg, "realm");
            std::string nonce = ExtractParamQuoted(msg, "nonce");
            if (!realm.empty() && !nonce.empty())
            {
               g_auth.setRealmAndNonce(realm.c_str(), nonce.c_str());
            }

            // user/pass �� �ִ� ��� ��õ�
            if (!g_user.empty())
            {
               env << *rtspClient << "Retry DESCRIBE with Digest (SHA-256=" << (g_auth.useSHA256() ? "on" : "off") << ")\n";
               scs.retriedWithAuth = true;
               rtspClient->sendDescribeCommand(continueAfterDESCRIBE, &g_auth);
               delete[] resultString;
               return; // ��õ�
            }
         }

         delete[] resultString;
         break;
      }

      char* const sdpDescription = resultString;
      env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";

      //SDP ���ڿ� -> MediaSession ��ü ����
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

      //��������(Subsession) ���� -> MediaSubsession::initiate() ȣ���Ͽ� �������Ǹ��� "SETUP" ��� ����
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
//      if (resultCode != 0) //describe ����
//      {
//         env << *rtspClient << "Failed to get a SDP description: " << resultString << "\n";
//         delete[] resultString;
//         break;
//      }
//
//      char* const sdpDescription = resultString;
//      env << *rtspClient << "Got a SDP description:\n" << sdpDescription << "\n";
//
//      //SDP ���ڿ� -> MediaSession ��ü ����
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
//      //��������(Subsession) ���� -> MediaSubsession::initiate() ȣ���Ͽ� �������Ǹ��� "SETUP" ��� ����
//      scs.iter = new MediaSubsessionIterator(*scs.session);
//      setupNextSubsession(rtspClient);
//      return;
//   } while (0);
//
//   // An unrecoverable error occurred with this stream.
//   shutdownStream(rtspClient);
//}

//��������(Subsession) ���� -> MediaSubsession::initiate() ȣ���Ͽ� �������Ǹ��� "SETUP" ��� ����
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

         //subsession->initiate ������ "SETUP" ��� ����
         rtspClient->sendSetupCommand(*scs.subsession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
      }
      return;
   }

   //RTSP "PLAY" ���
   if (scs.session->absStartTime() != NULL) 
   {
      //absStartTime() �� ������ ���� �ð�(UTC/�ð�ð�) ��� ����� �õ��ϰ�,
      //�ƴϸ� NPT ���(��� �ð�, ��now�� �Ǵ� 0����) ���� PLAY�� �����ϴ�. ��) 2025-09-09 09:30:00���� 09:35:00���� ���
      rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, scs.session->absStartTime(), scs.session->absEndTime());

      // ���̺긦 5�� ������ ���(Ÿ�ӽ���Ʈ ���� ����������)
      //rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, "-5.000", NULL);

      double scale = 1.5; // ��������
      //rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY, 0, 0, scale);

   }
   else 
   {
      scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
      rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
   }
}

//SETUP ��� ���� ó��
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

      //������ ��ũ�� �����ϰ� "startPlaying()"�� ȣ��
      scs.subsession->sink = DummySink::createNew(env, *scs.subsession, rtspClient->url());

      if (scs.subsession->sink == NULL) 
      {
         env << *rtspClient << "Failed to create a data sink for the \"" << *scs.subsession
            << "\" subsession: " << env.getResultMsg() << "\n";
         break;
      }
      env << *rtspClient << "Created a data sink for the \"" << *scs.subsession << "\" subsession\n";

      scs.subsession->miscPtr = rtspClient; //�ڵ鷯���� RTSPClient�� �����ϵ��� ����
      
      scs.subsession->sink->startPlaying(*(scs.subsession->readSource()), subsessionAfterPlaying, scs.subsession);

      //�۽� ��(ī�޶�/����)�� ���� ���Ḧ �˸� �� ���� RTCP BYE�� ó��
      if (scs.subsession->rtcpInstance() != NULL) 
      {
         scs.subsession->rtcpInstance()->setByeWithReasonHandler(subsessionByeHandler, scs.subsession);
      }
   } while (0);
   delete[] resultString;

   // Set up the next subsession, if any:
   setupNextSubsession(rtspClient);
}

//PLAY ��� ���� ó��. �̵�����(scs.duration) ������ ������ �ش� �ð� ���Ŀ� ����(BYE)
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

      // PLAY���� ��Ʈ������ó���ϰ� ������ Ÿ�̸Ӹ� ����(RTCP "BYE")
      // ��Ʈ���� Ȱ�� ���·� �����Ϸ��� ���(��: ���߿� ��Ʈ�� ������ 'Ž��'�Ͽ� RTSP "PLAY"�� �ٽ� ������ �� �ֵ���) �� �ڵ带 ��������
      // (�Ǵ� ��ü ��Ʈ���� �������� �������� �� Ÿ�̸Ӹ� �� ª�� ������ ����)
      if (scs.duration > 0) 
      {
         unsigned const delaySlop = 2; //2�� �����ð�
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

#pragma region �ڵ鷯(subsessionAfterPlaying, subsessionByeHandler, streamTimerHandler, shutdownStream)

//PLAY���� Ʈ������� ȣ���
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

//�۽� ��(ī�޶�/����)�� ���� ���Ḧ �˸� �� ���� RTCP BYE�� ó��
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

//PLAY���� ��Ʈ������ó���ϰ� ������ Ÿ�̸ӿ��� ȣ���(RTCP "BYE")
void streamTimerHandler(void* clientData) 
{
   ourRTSPClient* rtspClient = (ourRTSPClient*)clientData;
   StreamClientState& scs = rtspClient->scs; // alias

   scs.streamTimerTask = NULL;

   // Shut down the stream:
   shutdownStream(rtspClient);
}

//������ "TEARDOWN" ������, ��Ʈ�� ����
void shutdownStream(RTSPClient* rtspClient, int exitCode) 
{
   UsageEnvironment& env = rtspClient->envir();
   StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs;

   //����, �ݾƾ� �� ���� ������ �ִ��� Ȯ���ϼ���.
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
               subsession->rtcpInstance()->setByeHandler(NULL, NULL);//������ "TEARDOWN"�� ó���ϴ� ���� RTCP "BYE"�� ������ ���
            }

            someSubsessionsWereActive = True;
         }
      }

      if (someSubsessionsWereActive) {
         // RTSP "TEARDOWN" ����� �����Ͽ� ������ ��Ʈ���� �����ϵ��� �����մϴ�.
         // "TEARDOWN"�� ���� ������ ó������ ������.
         rtspClient->sendTeardownCommand(*scs.session, NULL);
      }
   }

   env << *rtspClient << "Closing the stream.\n";
   Medium::close(rtspClient);
   // Note that this will also cause this stream's "StreamClientState" structure to get reclaimed.

   if (--rtspClientCount == 0) {
      // ������ ��Ʈ���� ����Ǿ����Ƿ� ���� ���ø����̼��� �����մϴ�.
      // (����, �� �ڵ带 �ڽ��� ���ø����̼ǿ� �Ӻ����ϴ� ��� �� �κ��� �ּ� ó���ϰ�
      // "eventLoopWatchVariable = 1;"�� ��ü�Ͽ� LIVE555 �̺�Ʈ ������ �����ϰ� "main()"�� ��� ������ �� �ֽ��ϴ�.)
      exit(exitCode);
   }
}

#pragma endregion

