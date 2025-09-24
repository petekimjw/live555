#pragma once
#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>  // setupStreamSocket, makeSocketNonBlocking
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unordered_map>

// RTSPServerTLS.hh
class RTSPServerTLS : public RTSPServer {
public:
   static RTSPServerTLS* createNew(UsageEnvironment& env, Port ourPort,
      UserAuthenticationDatabase* authDB = nullptr,
      unsigned reclamationTestSeconds = 65,
      const char* certFile = "server.crt",
      const char* keyFile  = "server.key");

protected:
   RTSPServerTLS(UsageEnvironment& env, int listenSocket, Port ourPort,
      UserAuthenticationDatabase* authDB,
      unsigned reclamationTestSeconds,
      SSL_CTX* sslCtx);
   virtual ~RTSPServerTLS();

   // �� �� �̻� override ������ ������ (base�� virtual�� �ƴ� �� ����)
   void incomingConnectionHandler1TLS();

   // �� ���� ��׶��� �ڵ鷯: �����ٷ��� �츮�� ���� ���
   static void incomingConnectionHandlerTLS(void* clientData, int mask);

   virtual RTSPClientConnection* createNewClientConnection(int clientSocket,
      struct sockaddr_in clientAddr) override;

   class RTSPClientConnectionTLS : public RTSPServer::RTSPClientConnection {
   public:
      RTSPClientConnectionTLS(RTSPServer& ourServer, int clientSocket,
         struct sockaddr_in clientAddr, SSL* ssl);
      virtual ~RTSPClientConnectionTLS();
   protected:
      void handleRequestBytes(int /*newBytesRead*/
