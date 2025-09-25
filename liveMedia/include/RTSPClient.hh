#ifndef _RTSP_CLIENT_HH
#define _RTSP_CLIENT_HH

#ifndef _MEDIA_SESSION_HH
#include "MediaSession.hh"
#endif
#ifndef _NET_ADDRESS_HH
#include "NetAddress.hh"
#endif
#ifndef _DIGEST_AUTHENTICATION_HH
#include "DigestAuthentication.hh"
#endif
#ifndef _TLS_STATE_HH
#include "TLSState.hh"
#endif
#ifndef OMIT_REGISTER_HANDLING
#ifndef _RTSP_SERVER_HH
#include "RTSPServer.hh" // For the optional "HandlerForREGISTERCommand" mini-server
#endif
#endif

class LIVEMEDIA_API RTSPClient : public Medium 
{
public:

   // "tunnelOverHTTPPortNum"�� 0�� �ƴϸ�, RTSP(�� RTP)��
   // ������ ��Ʈ ��ȣ�� ����ϴ� HTTP ������ ���� �ͳθ��մϴ�. �� �����
   // Apple ���� <http://developer.apple.com/documentation/QuickTime/QTSS/Concepts/chapter_2_section_14.html>�� ����Ǿ� �ֽ��ϴ�.
   // "socketNumToServer"�� 0 �̻��̸�, ������ ���� ���� TCP ������ ���� ��ȣ�Դϴ�.
   // (�� ���, "rtspURL"�� ������ ���� �׼����� �� �ֵ��� ������ ��������Ʈ�� �����Ѿ� �մϴ�.)
   static RTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
      int verbosityLevel = 0,
      char const* applicationName = NULL,
      portNumBits tunnelOverHTTPPortNum = 0,
      int socketNumToServer = -1);

   // RTSP ��ɿ� ���� �������� ȣ��Ǵ� �Լ��Դϴ�. �Ű������� ������ �����ϴ�.
   // "rtspClient": ���� ����� ����� "RTSPClient" ��ü�Դϴ�.
   // "resultCode": 0�̸� ����� ���������� �Ϸ�Ǿ����ϴ�. 0�� �ƴϸ� ����� �Ϸ���� �ʾҽ��ϴ�.
   // ���������� �Ϸ���� �ʾ�����, "resultCode"�� ������ ���� ������ ��Ÿ���ϴ�.
   // ��� "resultCode"�� RTSP ���� �ڵ��Դϴ�(��: 404�� "ã�� �� ����"�� �ǹ�).
   // ���� "resultCode"�� ����/��Ʈ��ũ ������ ��Ÿ���ϴ�. 0���� "resultCode"������ ǥ�� "errno" �ڵ��Դϴ�.
   // "resultString": ����� �Բ� ��ȯ�Ǵ� ('\0'���� ������) ���ڿ��̰ų�, �׷��� ������ NULL�Դϴ�.
   // Ư��:
   // "DESCRIBE" ����� ������ ��� "resultString"�� �̵�� ������ SDP ������ �˴ϴ�.
   // "OPTIONS" ����� �����ϸ� "resultString"�� ���Ǵ� ��� ����� �˴ϴ�.
   // "resultCode"�� 0�� �ƴϴ���(��, ���� �޽����� ���) �� ���ڿ��� NULL�� �ƴ� �� �ֽ��ϴ�.
   // ���� "resultCode"�� 0�̴��� "resultString"�� NULL�� �� �ֽ��ϴ�(��: RTSP ����� ���������� ������ ��� ����� �������� ���� ���).
   // �� ���ڿ��� �������� �Ҵ�Ǹ�, �ڵ鷯(�Ǵ� ȣ����)���� �����ؾ� �մϴ�.
   // - "delete[]"�� ����մϴ�.
   typedef void (responseHandler)(RTSPClient* rtspClient, int resultCode, char* resultString);

   // RTSP "DESCRIBE" ����� ������ ����, ��ɿ� ���� "CSeq" ������ ��ȣ�� ��ȯ�մϴ�.
   // (���α׷��Ӱ� ������) "responseHandler" �Լ��� ���߿� ������ ó���ϱ� ���� ȣ��˴ϴ�.
   // (�Ǵ� ����� ������ �� ���� ��� ���� �ڵ�� �Բ� ��� ȣ��˴ϴ�.)
   // "authenticator"(���� ����)�� �׼��� ��� ���˴ϴ�. ����� �̸��� ��й�ȣ ���ڿ��� �ִ� ���,
   // "Authenticator(username, password) ��ü"�� �����Ͽ� ������ ���� �Ű������� �����Ͽ� ����� �� �ֽ��ϴ�.
   // (NULL�� �ƴ� "authenticator" �Ű������� �����ϴ� ���, ó�� �����ϴ� ��ɿ� ���ؼ��� �� �۾��� �����ϸ� �˴ϴ�.)
   unsigned sendDescribeCommand(responseHandler* responseHandler, Authenticator* authenticator = NULL);
   
   // RTSP "OPTIONS" ����� ������ ����, ��ɿ� ���� "CSeq" ������ ��ȣ�� ��ȯ�մϴ�.
   // ("responseHandler" �� "authenticator" �Ű������� "sendDescribeCommand"���� ������ �Ͱ� �����ϴ�.)
   unsigned sendOptionsCommand(responseHandler* responseHandler, Authenticator* authenticator = NULL);
   
   // RTSP "ANNOUNCE" ����� �����մϴ�(�Ű������� "sdpDescription" ���).
   // �׷� ���� ��ɿ� ���� "CSeq" ������ ��ȣ�� ��ȯ�մϴ�.
   // ("responseHandler" �� "authenticator" �Ű������� "sendDescribeCommand"���� ������ �Ͱ� �����ϴ�.)
   unsigned sendAnnounceCommand(char const* sdpDescription, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   
   // RTSP "SETUP" ����� ������ ��, ��ɿ� ���� "CSeq" ������ ��ȣ�� ��ȯ�մϴ�.
   // ("responseHandler" �� "authenticator" �Ű������� "sendDescribeCommand"���� ������ �Ͱ� �����ϴ�.)
   unsigned sendSetupCommand(MediaSubsession& subsession, responseHandler* responseHandler,
      Boolean streamOutgoing = False,
      Boolean streamUsingTCP = False,
      Boolean forceMulticastOnUnspecified = False,
      Authenticator* authenticator = NULL);
   
   // "session"���� ���� RTSP "PLAY" ����� ������ ����, ��ɿ� ���� "CSeq" ������ ��ȣ�� ��ȯ�մϴ�.
   // (����: start=-1�� 'resume'�� �ǹ��ϰ�, end=-1�� 'play to end'�� �ǹ��մϴ�.)
   // ("responseHandler" �� "authenticator" �Ű������� "sendDescribeCommand"���� ������ �Ͱ� �����ϴ�.)
   unsigned sendPlayCommand(MediaSession& session, responseHandler* responseHandler,
      double start = 0.0f, double end = -1.0f, float scale = 1.0f,
      Authenticator* authenticator = NULL);
   
   // "subsession"���� RTSP "PLAY" ����� ������ ����, ��ɿ� ���� "CSeq" ������ ��ȣ�� ��ȯ�մϴ�.
   // (����: start=-1�� 'resume'�� �ǹ��ϰ�, end=-1�� 'play to end'�� �ǹ��մϴ�.)
   // ("responseHandler" �� "authenticator" �Ű������� "sendDescribeCommand"���� ������ �Ͱ� �����ϴ�.)
   unsigned sendPlayCommand(MediaSubsession& subsession, responseHandler* responseHandler, 
      double start = 0.0f, double end = -1.0f, float scale = 1.0f,
      Authenticator* authenticator = NULL);
   

   // "sendPlayCommand()"�� �ٸ� ���·�, '����' �ð� ������ �����ϴ� "PLAY" ����� �����ϴ� �� ���˴ϴ�.
   // ("absStartTime" ���ڿ�(�� "absEndTime" ���ڿ�(�ִ� ���))�� *�ݵ��*
   // "YYYYMMDDTHHMMSSZ" �Ǵ� "YYYYMMDDTHHMMSS.<frac>Z" �����̾�� �մϴ�.)
   unsigned sendPlayCommand(MediaSession& session, responseHandler* responseHandler,
      char const* absStartTime, char const* absEndTime = NULL, float scale = 1.0f,
      Authenticator* authenticator = NULL);
   unsigned sendPlayCommand(MediaSubsession& subsession, responseHandler* responseHandler,
      char const* absStartTime, char const* absEndTime = NULL, float scale = 1.0f,
      Authenticator* authenticator = NULL);

   unsigned sendPauseCommand(MediaSession& session, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   // Issues an aggregate RTSP "PAUSE" command on "session", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)
   unsigned sendPauseCommand(MediaSubsession& subsession, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   // Issues a RTSP "PAUSE" command on "subsession", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)

   unsigned sendRecordCommand(MediaSession& session, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   // Issues an aggregate RTSP "RECORD" command on "session", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)
   unsigned sendRecordCommand(MediaSubsession& subsession, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   // Issues a RTSP "RECORD" command on "subsession", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)

   unsigned sendTeardownCommand(MediaSession& session, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   // Issues an aggregate RTSP "TEARDOWN" command on "session", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)
   unsigned sendTeardownCommand(MediaSubsession& subsession, responseHandler* responseHandler, Authenticator* authenticator = NULL);
   // Issues a RTSP "TEARDOWN" command on "subsession", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)

   unsigned sendSetParameterCommand(MediaSession& session, responseHandler* responseHandler,
      char const* parameterName, char const* parameterValue,
      Authenticator* authenticator = NULL);
   // Issues an aggregate RTSP "SET_PARAMETER" command on "session", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)

   unsigned sendGetParameterCommand(MediaSession& session, responseHandler* responseHandler, char const* parameterName,
      Authenticator* authenticator = NULL);
   // Issues an aggregate RTSP "GET_PARAMETER" command on "session", then returns the "CSeq" sequence number that was used in the command.
   // (The "responseHandler" and "authenticator" parameters are as described for "sendDescribeCommand".)

   void setRequireValue(char const* requireValue = NULL);
   // Sets a string to be used as the value of a "Require:" header to be included in
   // subsequent RTSP commands.  Call "setRequireValue()" again (i.e., with no parameter)
   // to clear this (and so stop "Require:" headers from being included in subsequent cmds).

   void sendDummyUDPPackets(MediaSession& session, unsigned numDummyPackets = 2);
   void sendDummyUDPPackets(MediaSubsession& subsession, unsigned numDummyPackets = 2);
   // Sends short 'dummy' (i.e., non-RTP or RTCP) UDP packets towards the server, to increase
   // the likelihood of RTP/RTCP packets from the server reaching us if we're behind a NAT.
   // (If we requested RTP-over-TCP streaming, then these functions have no effect.)
   // Our implementation automatically does this just prior to sending each "PLAY" command;
   // You should not call these functions yourself unless you know what you're doing.

   void setSpeed(MediaSession& session, float speed = 1.0f);
   // Set (recorded) media download speed to given value to support faster download using 'Speed:'
   // option on 'PLAY' command.

   Boolean changeResponseHandler(unsigned cseq, responseHandler* newResponseHandler);
   // Changes the response handler for the previously-performed command (whose operation returned "cseq").
   // (To turn off any response handling for the command, use a "newResponseHandler" value of NULL.  This might be done as part
   //  of an implementation of a 'timeout handler' on the command, for example.)
   // This function returns True iff "cseq" was for a valid previously-performed command (whose response is still unhandled).

   int socketNum() const { return fInputSocketNum; }

   static Boolean lookupByName(UsageEnvironment& env,
      char const* sourceName,
      RTSPClient*& resultClient);

   // "url"�� "rtsp://[<username>[:<password>]@]<server-address-or-name>[:<port>][/<stream-name>]"�� ���� �м��մϴ�.
   // (��ȯ�� "username"�� "password"�� NULL�̰ų�, ȣ���ڰ� ���߿� �����ؾ� �ϴ� �� �Ҵ� ���ڿ��Դϴ�[].)
   Boolean parseRTSPURL(char const* url,
      char*& username, char*& password, NetAddress& address, portNumBits& portNum, char const** urlSuffix = NULL);

   void setUserAgentString(char const* userAgentName);
   // sets an alternative string to be used in RTSP "User-Agent:" headers

   void disallowBasicAuthentication() { fAllowBasicAuthentication = False; }
   // call this if you don't want the server to request 'Basic' authentication
   // (which would cause the client to send usernames and passwords over the net).

   unsigned sessionTimeoutParameter() const { return fSessionTimeoutParameter; }

   char const* url() const { return fBaseURL; }

   static unsigned responseBufferSize;

public: 
   #pragma region RequestRecord Ŭ����

   // Some compilers complain if this is "private:"
   // The state of a request-in-progress:
   class LIVEMEDIA_API RequestRecord 
   {
   public:
      RequestRecord(unsigned cseq, char const* commandName, responseHandler* handler,
         MediaSession* session = NULL, MediaSubsession* subsession = NULL, u_int32_t booleanFlags = 0,
         double start = 0.0f, double end = -1.0f, float scale = 1.0f, char const* contentStr = NULL);
      RequestRecord(unsigned cseq, responseHandler* handler,
         char const* absStartTime, char const* absEndTime = NULL, float scale = 1.0f,
         MediaSession* session = NULL, MediaSubsession* subsession = NULL);
      // alternative constructor for creating "PLAY" requests that include 'absolute' time values
      virtual ~RequestRecord();

      RequestRecord*& next() { return fNext; }
      unsigned& cseq() { return fCSeq; }
      char const* commandName() const { return fCommandName; }
      MediaSession* session() const { return fSession; }
      MediaSubsession* subsession() const { return fSubsession; }
      u_int32_t booleanFlags() const { return fBooleanFlags; }
      double start() const { return fStart; }
      double end() const { return fEnd; }
      char const* absStartTime() const { return fAbsStartTime; }
      char const* absEndTime() const { return fAbsEndTime; }
      float scale() const { return fScale; }
      char* contentStr() const { return fContentStr; }
      responseHandler*& handler() { return fHandler; }

   private:
      RequestRecord* fNext;
      unsigned fCSeq;
      char const* fCommandName;
      MediaSession* fSession;
      MediaSubsession* fSubsession;
      u_int32_t fBooleanFlags;
      double fStart, fEnd;
      char* fAbsStartTime, * fAbsEndTime; // used for optional 'absolute' (i.e., "time=") range specifications
      float fScale;
      char* fContentStr;
      responseHandler* fHandler;
   };

   #pragma endregion

protected:
   RTSPClient(UsageEnvironment& env, char const* rtspURL,
      int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum, int socketNumToServer);
   // called only by createNew();
   virtual ~RTSPClient();

   void reset();
   void setBaseURL(char const* url);
   int grabSocket(); // allows a subclass to reuse our input socket, so that it won't get closed when we're deleted
   virtual unsigned sendRequest(RequestRecord* request);
   virtual Boolean setRequestFields(RequestRecord* request,
      char*& cmdURL, Boolean& cmdURLWasAllocated,
      char const*& protocolStr,
      char*& extraHeaders, Boolean& extraHeadersWereAllocated);
   // used to implement "sendRequest()"; subclasses may reimplement this (e.g., when implementing a new command name)
   virtual int connectToServer(int socketNum, portNumBits remotePortNum); // used to implement "openConnection()"; result values: -1: failure; 0: pending; 1: success

private: // redefined virtual functions
   virtual Boolean isRTSPClient() const;

private:
   
   #pragma region RequestQueue Ŭ����
   
   class LIVEMEDIA_API RequestQueue 
   {
   public:
      RequestQueue();
      RequestQueue(RequestQueue& origQueue); // moves the queue contents to the new queue
      virtual ~RequestQueue();

      void enqueue(RequestRecord* request); // "request" must not be NULL
      RequestRecord* dequeue();
      void putAtHead(RequestRecord* request); // "request" must not be NULL
      RequestRecord* findByCSeq(unsigned cseq);
      Boolean isEmpty() const { return fHead == NULL; }
      void reset();

   private:
      RequestRecord* fHead;
      RequestRecord* fTail;
   };

   #pragma endregion

   void resetTCPSockets();
   void resetResponseBuffer();
   int openConnection(); // result values: -1: failure; 0: pending; 1: success
   char* createAuthenticatorString(char const* cmd, char const* url);
   char* createBlocksizeString(Boolean streamUsingTCP);
   char* createKeyMgmtString(char const* url, MediaSubsession const& subsession);
   void handleRequestError(RequestRecord* request);
   Boolean parseResponseCode(char const* line, unsigned& responseCode, char const*& responseString);
   void handleIncomingRequest();
   static Boolean checkForHeader(char const* line, char const* headerName, unsigned headerNameLength, char const*& headerParams);
   Boolean parseTransportParams(char const* paramsStr,
      char*& serverAddressStr, portNumBits& serverPortNum,
      unsigned char& rtpChannelId, unsigned char& rtcpChannelId);
   Boolean parseScaleParam(char const* paramStr, float& scale);
   Boolean parseSpeedParam(char const* paramStr, float& speed);
   Boolean parseRTPInfoParams(char const*& paramStr, u_int16_t& seqNum, u_int32_t& timestamp);
   Boolean handleSETUPResponse(MediaSubsession& subsession, char const* sessionParamsStr, char const* transportParamsStr,
      Boolean streamUsingTCP);
   Boolean handlePLAYResponse(MediaSession* session, MediaSubsession* subsession,
      char const* scaleParamsStr, const char* speedParamsStr,
      char const* rangeParamsStr, char const* rtpInfoParamsStr);
   Boolean handleTEARDOWNResponse(MediaSession& session, MediaSubsession& subsession);
   Boolean handleGET_PARAMETERResponse(char const* parameterName, char*& resultValueString, char* resultValueStringEnd);
   Boolean handleAuthenticationFailure(char const* wwwAuthenticateParamsStr);
   Boolean resendCommand(RequestRecord* request);
   char const* sessionURL(MediaSession const& session) const;
   static void handleAlternativeRequestByte(void*, u_int8_t requestByte);
   void handleAlternativeRequestByte1(u_int8_t requestByte);
   void constructSubsessionURL(MediaSubsession const& subsession,
      char const*& prefix,
      char const*& separator,
      char const*& suffix);

   // Support for tunneling RTSP-over-HTTP:
   Boolean setupHTTPTunneling1(); // send the HTTP "GET"
   static void responseHandlerForHTTP_GET(RTSPClient* rtspClient, int responseCode, char* responseString);
   void responseHandlerForHTTP_GET1(int responseCode, char* responseString);
   Boolean setupHTTPTunneling2(); // send the HTTP "POST"

   // Support for asynchronous connections to the server:
   static void connectionHandler(void*, int /*mask*/);
   void connectionHandler1();

   // Support for handling data sent back by a server:
   static void incomingDataHandler(void*, int /*mask*/);
   void incomingDataHandler1();
   void handleResponseBytes(int newBytesRead);

   // Writing/reading data over a (already set-up) connection:
   int write(const char* data, unsigned count);
   int read(u_int8_t* buffer, unsigned bufferSize);

public:
   u_int16_t desiredMaxIncomingPacketSize;
   // If set to a value >0, then a "Blocksize:" header with this value (minus an allowance for
   // IP, UDP, and RTP headers) will be sent with each "SETUP" request.

protected:
   int fVerbosityLevel;
   unsigned fCSeq; // sequence number, used in consecutive requests
   Authenticator fCurrentAuthenticator;
   Boolean fAllowBasicAuthentication;
   struct sockaddr_storage fServerAddress;

private:
   portNumBits fTunnelOverHTTPPortNum;
   char* fUserAgentHeaderStr;
   unsigned fUserAgentHeaderStrLen;
   int fInputSocketNum, fOutputSocketNum;
   char* fBaseURL;
   unsigned char fTCPStreamIdCount; // used for (optional) RTP/TCP
   char* fLastSessionId;
   unsigned fSessionTimeoutParameter; // optionally set in response "Session:" headers
   char* fResponseBuffer;
   unsigned fResponseBytesAlreadySeen, fResponseBufferBytesLeft;
   RequestQueue fRequestsAwaitingConnection, fRequestsAwaitingHTTPTunneling, fRequestsAwaitingResponse;
   char* fRequireStr;

   // Support for tunneling RTSP-over-HTTP:
   char fSessionCookie[33];
   unsigned fSessionCookieCounter;
   Boolean fHTTPTunnelingConnectionIsPending;

   // Optional support for TLS:
   ClientTLSState fTLS;
   ClientTLSState fPOSTSocketTLS; // used only for RTSP-over-HTTPS
   ClientTLSState* fInputTLS;
   ClientTLSState* fOutputTLS;
   friend class ClientTLSState;
};


#pragma region HandlerServerForREGISTERCommand

#ifndef OMIT_REGISTER_HANDLING
//////////  /////////

// A simple server that creates a new "RTSPClient" object whenever a "REGISTER" request arrives (specifying the "rtsp://" URL
// of a stream).  The new "RTSPClient" object will be created with the specified URL, and passed to the provided handler function.

typedef void onRTSPClientCreationFunc(RTSPClient* newRTSPClient, Boolean requestStreamingOverTCP);

class LIVEMEDIA_API HandlerServerForREGISTERCommand : public RTSPServer 
{
public:
   static HandlerServerForREGISTERCommand* createNew(UsageEnvironment& env, onRTSPClientCreationFunc* creationFunc,
      Port ourPort = 0, UserAuthenticationDatabase* authDatabase = NULL,
      int verbosityLevel = 0, char const* applicationName = NULL);
   // If ourPort.num() == 0, we'll choose the port number ourself.  (Use the following function to get it.)
   portNumBits serverPortNum() const { return ntohs(fServerPort.num()); }

protected:
   HandlerServerForREGISTERCommand(UsageEnvironment& env, onRTSPClientCreationFunc* creationFunc, int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
      UserAuthenticationDatabase* authDatabase, int verbosityLevel, char const* applicationName);
   // called only by createNew();
   virtual ~HandlerServerForREGISTERCommand();

   virtual RTSPClient* createNewRTSPClient(char const* rtspURL, int verbosityLevel, char const* applicationName,
      int socketNumToServer);
   // This function - by default - creates a (base) "RTSPClient" object.  If you want to create a subclass
   // of "RTSPClient" instead, then subclass this class, and redefine this virtual function.

protected: // redefined virtual functions
   virtual char const* allowedCommandNames(); // "OPTIONS", "REGISTER", and (perhaps) "DEREGISTER" only
   virtual Boolean weImplementREGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
      char const* proxyURLSuffix, char*& responseStr);
   // redefined to return True (for cmd=="REGISTER")
   virtual void implementCmd_REGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
      char const* url, char const* urlSuffix, int socketToRemoteServer,
      Boolean deliverViaTCP, char const* proxyURLSuffix);

private:
   onRTSPClientCreationFunc* fCreationFunc;
   int fVerbosityLevel;
   char* fApplicationName;
};
#endif

#endif

#pragma endregion
