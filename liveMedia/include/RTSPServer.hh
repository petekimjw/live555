#ifndef _RTSP_SERVER_HH
#define _RTSP_SERVER_HH

#ifndef _GENERIC_MEDIA_SERVER_HH
#include "GenericMediaServer.hh"
#endif
#ifndef _DIGEST_AUTHENTICATION_HH
#include "DigestAuthentication.hh"
#endif

class LIVEMEDIA_API RTSPServer : public GenericMediaServer
{
public:
   Boolean UseSHA256 = false; // False: MD5(기본), True: SHA-256

   // ourPort.num() == 0이면 포트 번호를 선택합니다.
   // 참고: 호출자는 "authDatabase"를 회수해야 합니다.
   // "reclamationSeconds" > 0이면, 각 클라이언트의 "RTSPClientSession" 상태가
   // 회수됩니다(해당 RTP 스트림은
   // 해제됩니다). 이는 클라이언트로부터 최소 "reclamationSeconds"초 동안 RTSP 명령이나 RTCP "RR" 패킷을 수신하지 않으면
   // 회수됩니다.
   static RTSPServer* createNew(UsageEnvironment& env, Port ourPort = 554,
      UserAuthenticationDatabase* authDatabase = NULL,
      unsigned reclamationSeconds = 65);

   static Boolean lookupByName(UsageEnvironment& env, char const* name,
      RTSPServer*& resultServer);

   typedef void (responseHandlerForREGISTER)(RTSPServer* rtspServer, unsigned requestId, int resultCode, char* resultString);

   // "serverMediaSession"으로 표현되는 스트림을 지정된 원격 클라이언트(이름과 포트 번호로 지정)에 '등록'합니다.
   // 이 작업은 사용자 지정 "REGISTER" RTSP 명령을 사용하여 수행됩니다.
   // 이 함수는 요청을 식별하는 데 사용할 수 있는 고유 번호를 반환합니다. 이 번호는 "responseHandler"에도 전달됩니다.
   // 원격 클라이언트로부터 응답을 수신하거나 "REGISTER" 요청이 실패하면 지정된 응답 핸들러가
   // (NULL이 아닌 경우) 호출됩니다. (핸들러에 전달된 "resultString"은 동적으로 할당되었으며
   // 사용 후 핸들러에서 delete[]되어야 합니다.)
   // "receiveOurStreamViaTCP"가 True이면 원격 클라이언트가 RTP/RTCP-over-TCP를 사용하여 스트림에 액세스하도록 요청하는 것입니다.
   // (그렇지 않으면 원격 클라이언트는 일반 RTP/RTCP-over-UDP 스트리밍을 선택할 수 있습니다.)
   // "proxyURLSuffix"(선택 사항)는 원격 클라이언트가 프록시 서버이기도 한 경우에만 사용됩니다.
   // 프런트엔드 클라이언트가 스트림에 액세스할 때 프록시 서버가 "rtsp://" URL에 사용해야 하는 접미사를 지정합니다.
   unsigned registerStream(ServerMediaSession* serverMediaSession,
      char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum,
      responseHandlerForREGISTER* responseHandler,
      char const* username = NULL, char const* password = NULL,
      Boolean receiveOurStreamViaTCP = False,
      char const* proxyURLSuffix = NULL);

   typedef void (responseHandlerForDEREGISTER)(RTSPServer* rtspServer, unsigned requestId, int resultCode, char* resultString);
   unsigned deregisterStream(ServerMediaSession* serverMediaSession,
      char const* remoteClientNameOrAddress, portNumBits remoteClientPortNum,
      responseHandlerForDEREGISTER* responseHandler,
      char const* username = NULL, char const* password = NULL,
      char const* proxyURLSuffix = NULL);
   // Used to turn off a previous "registerStream()" - using our custom "DEREGISTER" RTSP command.

   char* rtspURL(ServerMediaSession const* serverMediaSession,
      int clientSocket = -1, Boolean useIPv6 = False) const;
   // returns a "rtsp://" URL that could be used to access the
   // specified session (which must already have been added to
   // us using "addServerMediaSession()".
   // This string is dynamically allocated; caller should delete[]
   // (If "clientSocket" is non-negative, then it is used (by calling "getsockname()") to determine
   //  the IP address to be used in the URL.)
// Shortcuts:
   char* ipv4rtspURL(ServerMediaSession const* serverMediaSession, int clientSocket = -1) {
      return rtspURL(serverMediaSession, clientSocket, False);
   }
   char* ipv6rtspURL(ServerMediaSession const* serverMediaSession, int clientSocket = -1) {
      return rtspURL(serverMediaSession, clientSocket, True);
   }

   char* rtspURLPrefix(int clientSocket = -1, Boolean useIPv6 = False) const;
   // like "rtspURL()", except that it returns just the common prefix used by
   // each session's "rtsp://" URL.
   // This string is dynamically allocated; caller should delete[]
// Shortcuts:
   char* ipv4rtspURLPrefix(int clientSocket = -1) { return rtspURLPrefix(clientSocket, False); }
   char* ipv6rtspURLPrefix(int clientSocket = -1) { return rtspURLPrefix(clientSocket, True); }

   UserAuthenticationDatabase* setAuthenticationDatabase(UserAuthenticationDatabase* newDB);
   // Changes the server's authentication database to "newDB", returning a pointer to the old database (if there was one).
   // "newDB" may be NULL (you can use this to disable authentication at runtime, if desired).

   void disableStreamingRTPOverTCP() {
      fAllowStreamingRTPOverTCP = False;
   }

   Boolean setUpTunnelingOverHTTP(Port httpPort);
   // (Attempts to) enable RTSP-over-HTTP tunneling on the specified port.
   // Returns True iff the specified port can be used in this way (i.e., it's not already being used for a separate HTTP server).
   // Note: RTSP-over-HTTP tunneling is described in
   //  http://mirror.informatimago.com/next/developer.apple.com/quicktime/icefloe/dispatch028.html
   //  and http://images.apple.com/br/quicktime/pdf/QTSS_Modules.pdf
   portNumBits httpServerPortNum() const; // in host byte order.  (Returns 0 if not present.)

   void setAdvertiseDigestAlgorithmSHA256(Boolean enable);

   void setTLSState(char const* certFileName, char const* privKeyFileName,
      Boolean weServeSRTP = True, Boolean weEncryptSRTP = True);

protected:
   RTSPServer(UsageEnvironment& env,
      int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
      UserAuthenticationDatabase* authDatabase,
      unsigned reclamationSeconds);
   // called only by createNew();
   virtual ~RTSPServer();

   virtual char const* allowedCommandNames(); // used to implement "RTSPClientConnection::handleCmd_OPTIONS()"


   // "RTSPClientConnection::handleCmd_REGISTER()" 구현에 사용
   // 참고: "responseStr"은 동적으로 할당(또는 NULL)되며, 호출 후 delete[]d되어야 합니다.
   virtual Boolean weImplementREGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
      char const* proxyURLSuffix, char*& responseStr);

   virtual void implementCmd_REGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
      char const* url, char const* urlSuffix, int socketToRemoteServer,
      Boolean deliverViaTCP, char const* proxyURLSuffix);
   // used to implement "RTSPClientConnection::handleCmd_REGISTER()"

   virtual UserAuthenticationDatabase* getAuthenticationDatabaseForCommand(char const* cmdName);
   virtual Boolean specialClientAccessCheck(int clientSocket,
      struct sockaddr_storage const& clientAddr,
      char const* urlSuffix);

   // a hook that allows subclassed servers to do server-specific access checking
   // on each client (e.g., based on client IP address), without using digest authentication.
   virtual Boolean specialClientUserAccessCheck(int clientSocket,
      struct sockaddr_storage const& clientAddr,
      char const* urlSuffix, char const* username);

   // another hook that allows subclassed servers to do server-specific access checking
   // - this time after normal digest authentication has already taken place (and would otherwise allow access).
   // (This test can only be used to further restrict access, not to grant additional access.)
   virtual void specialHandlingOfAuthenticationFailure(int clientSocket,
      struct sockaddr_storage const& clientAddr,
      char const* urlSuffix);
   // a hook that allows subclassed servers to take extra action whenevever an authentication failure occurs

public: // redefined virtual functions
   virtual Boolean isRTSPServer() const;
   virtual void addServerMediaSession(ServerMediaSession* serverMediaSession);

public: // should be protected, but some old compilers complain otherwise
   
   // The state of a TCP connection used by a RTSP client:
   class RTSPClientSession; // forward
   
   #pragma region RTSPClientConnection 클래스

   class RTSPClientConnection : public GenericMediaServer::ClientConnection 
   {
   public:
      // A data structure that's used to implement the "REGISTER" command:
      class ParamsForREGISTER {
      public:
         ParamsForREGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
            RTSPClientConnection* ourConnection, char const* url, char const* urlSuffix,
            Boolean reuseConnection, Boolean deliverViaTCP, char const* proxyURLSuffix);
         virtual ~ParamsForREGISTER();
      private:
         friend class RTSPClientConnection;
         char const* fCmd;
         RTSPClientConnection* fOurConnection;
         char* fURL;
         char* fURLSuffix;
         Boolean fReuseConnection, fDeliverViaTCP;
         char* fProxyURLSuffix;
      };
   protected: // redefined virtual functions:
      virtual void handleRequestBytes(int newBytesRead);

   protected:
      RTSPClientConnection(RTSPServer& ourServer,
         int clientSocket, struct sockaddr_storage const& clientAddr,
         Boolean useTLS = False);
      virtual ~RTSPClientConnection();

      friend class RTSPServer;
      friend class RTSPClientSession;

      // Make the handler functions for each command virtual, to allow subclasses to reimplement them, if necessary:
      virtual void handleCmd_OPTIONS();
      // You probably won't need to subclass/reimplement this function; reimplement "RTSPServer::allowedCommandNames()" instead.
      virtual void handleCmd_GET_PARAMETER(char const* fullRequestStr); // when operating on the entire server
      virtual void handleCmd_SET_PARAMETER(char const* fullRequestStr); // when operating on the entire server
      virtual void handleCmd_DESCRIBE(char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);
      static void DESCRIBELookupCompletionFunction(void* clientData, ServerMediaSession* sessionLookedUp);
      virtual void handleCmd_DESCRIBE_afterLookup(ServerMediaSession* session);
      virtual void handleCmd_REGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
         char const* url, char const* urlSuffix, char const* fullRequestStr,
         Boolean reuseConnection, Boolean deliverViaTCP, char const* proxyURLSuffix);
      // You probably won't need to subclass/reimplement this function;
      //     reimplement "RTSPServer::weImplementREGISTER()" and "RTSPServer::implementCmd_REGISTER()" instead.
      virtual void handleCmd_bad();
      virtual void handleCmd_notSupported();
      virtual void handleCmd_redirect(char const* urlSuffix);
      virtual void handleCmd_notFound();
      virtual void handleCmd_sessionNotFound();
      virtual void handleCmd_unsupportedTransport();
      // Support for optional RTSP-over-HTTP tunneling:
      virtual Boolean parseHTTPRequestString(char* resultCmdName, unsigned resultCmdNameMaxSize,
         char* urlSuffix, unsigned urlSuffixMaxSize,
         char* sessionCookie, unsigned sessionCookieMaxSize,
         char* acceptStr, unsigned acceptStrMaxSize);
      virtual void handleHTTPCmd_notSupported();
      virtual void handleHTTPCmd_notFound();
      virtual void handleHTTPCmd_OPTIONS();
      virtual void handleHTTPCmd_TunnelingGET(char const* sessionCookie);
      virtual Boolean handleHTTPCmd_TunnelingPOST(char const* sessionCookie, unsigned char const* extraData, unsigned extraDataSize);
      virtual void handleHTTPCmd_StreamingGET(char const* urlSuffix, char const* fullRequestStr);
   protected:
      void resetRequestBuffer();
      void closeSocketsRTSP();
      static void handleAlternativeRequestByte(void*, u_int8_t requestByte);
      void handleAlternativeRequestByte1(u_int8_t requestByte);
      Boolean authenticationOK(char const* cmdName, char const* urlSuffix, char const* fullRequestStr);
      void changeClientInputSocket(int newSocketNum, ServerTLSState const* newTLSState,
         unsigned char const* extraData, unsigned extraDataSize);
      // used to implement RTSP-over-HTTP tunneling
      static void continueHandlingREGISTER(ParamsForREGISTER* params);
      virtual void continueHandlingREGISTER1(ParamsForREGISTER* params);

      // Shortcuts for setting up a RTSP response (prior to sending it):
      void setRTSPResponse(char const* responseStr);
      void setRTSPResponse(char const* responseStr, u_int32_t sessionId);
      void setRTSPResponse(char const* responseStr, char const* contentStr);
      void setRTSPResponse(char const* responseStr, u_int32_t sessionId, char const* contentStr);

      RTSPServer& fOurRTSPServer; // same as ::fOurServer
      int& fClientInputSocket; // aliased to ::fOurSocket
      int fClientOutputSocket;
      ServerTLSState fPOSTSocketTLS; // used only for RTSP-over-HTTPS
      int fAddressFamily;
      Boolean fIsActive;
      unsigned char* fLastCRLF;
      unsigned fRecursionCount;
      char const* fCurrentCSeq;
      Authenticator fCurrentAuthenticator; // used if access control is needed
      char* fOurSessionCookie; // used for optional RTSP-over-HTTP tunneling
      unsigned fBase64RemainderCount; // used for optional RTSP-over-HTTP tunneling (possible values: 0,1,2,3)
      unsigned fScheduledDelayedTask;
   };

   #pragma endregion

   #pragma region RTSPClientSession 클래스

   // RTSP 서버에서 처리하는 개별 클라이언트 세션(하나 이상의 순차적 TCP 연결 사용)의 상태:
   class LIVEMEDIA_API RTSPClientSession : public GenericMediaServer::ClientSession
   {
   protected:
      RTSPClientSession(RTSPServer& ourServer, u_int32_t sessionId);
      virtual ~RTSPClientSession();

      friend class RTSPServer;
      friend class RTSPClientConnection;
      // Make the handler functions for each command virtual, to allow subclasses to redefine them:
      virtual void handleCmd_SETUP(RTSPClientConnection* ourClientConnection,
         char const* urlPreSuffix, char const* urlSuffix, char const* fullRequestStr);
      static void SETUPLookupCompletionFunction1(void* clientData, ServerMediaSession* sessionLookedUp);
      virtual void handleCmd_SETUP_afterLookup1(ServerMediaSession* sms);
      static void SETUPLookupCompletionFunction2(void* clientData, ServerMediaSession* sessionLookedUp);
      virtual void handleCmd_SETUP_afterLookup2(ServerMediaSession* sms);
      virtual void handleCmd_withinSession(RTSPClientConnection* ourClientConnection,
         char const* cmdName,
         char const* urlPreSuffix, char const* urlSuffix,
         char const* fullRequestStr);
      virtual void handleCmd_TEARDOWN(RTSPClientConnection* ourClientConnection,
         ServerMediaSubsession* subsession);
      virtual void handleCmd_PLAY(RTSPClientConnection* ourClientConnection,
         ServerMediaSubsession* subsession, char const* fullRequestStr);
      virtual void handleCmd_PAUSE(RTSPClientConnection* ourClientConnection,
         ServerMediaSubsession* subsession);
      virtual void handleCmd_GET_PARAMETER(RTSPClientConnection* ourClientConnection,
         ServerMediaSubsession* subsession, char const* fullRequestStr);
      virtual void handleCmd_SET_PARAMETER(RTSPClientConnection* ourClientConnection,
         ServerMediaSubsession* subsession, char const* fullRequestStr);
   protected:
      void deleteStreamByTrack(unsigned trackNum);
      void reclaimStreamStates();
      Boolean isMulticast() const { return fIsMulticast; }

      // Shortcuts for setting up a RTSP response (prior to sending it):
      void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr) { ourClientConnection->setRTSPResponse(responseStr); }
      void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr, u_int32_t sessionId) { ourClientConnection->setRTSPResponse(responseStr, sessionId); }
      void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr, char const* contentStr) { ourClientConnection->setRTSPResponse(responseStr, contentStr); }
      void setRTSPResponse(RTSPClientConnection* ourClientConnection, char const* responseStr, u_int32_t sessionId, char const* contentStr) { ourClientConnection->setRTSPResponse(responseStr, sessionId, contentStr); }

   protected:
      RTSPServer& fOurRTSPServer; // same as ::fOurServer
      Boolean fIsMulticast, fStreamAfterSETUP;
      unsigned char fTCPStreamIdCount; // used for (optional) RTP/TCP
      Boolean usesTCPTransport() const { return fTCPStreamIdCount > 0; }
      unsigned fNumStreamStates;
      struct streamState {
         ServerMediaSubsession* subsession;
         int tcpSocketNum;
         void* streamToken;
      } *fStreamStates;

      // Member variables used to implement "handleCmd_SETUP()":
      RTSPServer::RTSPClientConnection* fOurClientConnection;
      char const* fURLPreSuffix; char const* fURLSuffix; char const* fFullRequestStr; char const* fTrackId;
   };

   #pragma endregion

protected: // redefined virtual functions
   // "RTSPClientConnection"을 하위 클래스로 만드는 경우, 하위 클래스의 새 객체를 생성하려면 이 가상 함수도 다시 정의해야 합니다.
   virtual ClientConnection* createNewClientConnection(int clientSocket, struct sockaddr_storage const& clientAddr);

protected:
   // "RTSPClientSession"을 하위 클래스로 만드는 경우, 하위 클래스의 새 객체를 생성하려면 이 가상 함수도 다시 정의해야 합니다.
   virtual ClientSession* createNewClientSession(u_int32_t sessionId);

private:
   static void incomingConnectionHandlerHTTPIPv4(void*, int /*mask*/);
   void incomingConnectionHandlerHTTPIPv4();
   static void incomingConnectionHandlerHTTPIPv6(void*, int /*mask*/);
   void incomingConnectionHandlerHTTPIPv6();

   void noteTCPStreamingOnSocket(int socketNum, RTSPClientSession* clientSession, unsigned trackNum);
   void unnoteTCPStreamingOnSocket(int socketNum, RTSPClientSession* clientSession, unsigned trackNum);
   void stopTCPStreamingOnSocket(int socketNum);

private:
   friend class RTSPClientConnection;
   friend class RTSPClientSession;
   friend class RegisterRequestRecord;
   friend class DeregisterRequestRecord;
   int fHTTPServerSocketIPv4, fHTTPServerSocketIPv6; // for optional RTSP-over-HTTP tunneling
   Port fHTTPServerPort; // ditto
   HashTable* fClientConnectionsForHTTPTunneling; // maps client-supplied 'session cookie' strings to "RTSPClientConnection"s
   // (used only for optional RTSP-over-HTTP tunneling)
   HashTable* fTCPStreamingDatabase;
   // maps TCP socket numbers to ids of sessions that are streaming over it (RTP/RTCP-over-TCP)
   HashTable* fPendingRegisterOrDeregisterRequests;
   unsigned fRegisterOrDeregisterRequestCounter;
   UserAuthenticationDatabase* fAuthDB;
   Boolean fAllowStreamingRTPOverTCP; // by default, True
   Boolean fOurConnectionsUseTLS; // by default, False
   Boolean fWeServeSRTP; // used only if "fOurConnectionsUseTLS" is True
   Boolean fWeEncryptSRTP; // used only if "fWeServeSRTP" is True
};


#pragma region 지정된 URL에 대한 프록싱을 설정하기 위해 "REGISTER" 명령을 구현하는 "RTSPServer"의 하위 클래스

class LIVEMEDIA_API RTSPServerWithREGISTERProxying : public RTSPServer 
{
public:
   static RTSPServerWithREGISTERProxying* createNew(UsageEnvironment& env, Port ourPort = 554,
      UserAuthenticationDatabase* authDatabase = NULL,
      UserAuthenticationDatabase* authDatabaseForREGISTER = NULL,
      unsigned reclamationSeconds = 65,
      Boolean streamRTPOverTCP = False,
      int verbosityLevelForProxying = 0,
      char const* backEndUsername = NULL,
      char const* backEndPassword = NULL);

protected:
   RTSPServerWithREGISTERProxying(UsageEnvironment& env, int ourSocketIPv4, int ourSocketIPv6, Port ourPort,
      UserAuthenticationDatabase* authDatabase, UserAuthenticationDatabase* authDatabaseForREGISTER,
      unsigned reclamationSeconds,
      Boolean streamRTPOverTCP, int verbosityLevelForProxying,
      char const* backEndUsername, char const* backEndPassword);
   // called only by createNew();
   virtual ~RTSPServerWithREGISTERProxying();

protected: // redefined virtual functions
   virtual char const* allowedCommandNames();
   virtual Boolean weImplementREGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
      char const* proxyURLSuffix, char*& responseStr);
   virtual void implementCmd_REGISTER(char const* cmd/*"REGISTER" or "DEREGISTER"*/,
      char const* url, char const* urlSuffix, int socketToRemoteServer,
      Boolean deliverViaTCP, char const* proxyURLSuffix);
   virtual UserAuthenticationDatabase* getAuthenticationDatabaseForCommand(char const* cmdName);

private:
   Boolean fStreamRTPOverTCP;
   int fVerbosityLevelForProxying;
   unsigned fRegisteredProxyCounter;
   char* fAllowedCommandNames;
   UserAuthenticationDatabase* fAuthDBForREGISTER;
   char* fBackEndUsername;
   char* fBackEndPassword;
};

#pragma endregion

// "parseTransportHeader()"의 특수 버전으로, 들어오는 "REGISTER" 명령에서 "Transport:" 헤더를 파싱하는 데에만 사용됩니다.
LIVEMEDIA_API void parseTransportHeaderForREGISTER(char const* buf, // in
   Boolean& reuseConnection, // out
   Boolean& deliverViaTCP, // out
   char*& proxyURLSuffix); // out

#endif
