// RTSPServerTLS.cpp
#include "RTSPServerTLS.h"
#include <GroupsockHelper.hh>
#include <openssl/ssl.h>
#include <openssl/err.h>

static int createListenSocket(UsageEnvironment& env, Port ourPort) {
   int s = setupStreamSocket(env, ourPort);
   if (s < 0) return -1;
   makeSocketNonBlocking(s);
   increaseSendBufferTo(env, s, 50*1024);
   return s;
}

RTSPServerTLS* RTSPServerTLS::createNew(UsageEnvironment& env, Port ourPort,
   UserAuthenticationDatabase* authDB,
   unsigned reclamationTestSeconds,
   const char* certFile, const char* keyFile) {
   SSL_library_init();
   SSL_load_error_strings();
   OpenSSL_add_all_algorithms();

   SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
   if (!ctx) { env.setResultMsg("SSL_CTX_new failed"); return nullptr; }

   if (SSL_CTX_use_certificate_file(ctx, certFile, SSL_FILETYPE_PEM) != 1 ||
      SSL_CTX_use_PrivateKey_file(ctx, keyFile,  SSL_FILETYPE_PEM) != 1 ||
      !SSL_CTX_check_private_key(ctx)) {
      env.setResultMsg("Failed to load cert/key");
      SSL_CTX_free(ctx);
      return nullptr;
   }

   int listenSock = createListenSocket(env, ourPort);
   if (listenSock < 0) { SSL_CTX_free(ctx); return nullptr; }

   return new RTSPServerTLS(env, listenSock, ourPort, authDB, reclamationTestSeconds, ctx);
}

RTSPServerTLS::RTSPServerTLS(UsageEnvironment& env, int listenSocket, Port ourPort,
   UserAuthenticationDatabase* authDB,
   unsigned reclamationTestSeconds,
   SSL_CTX* sslCtx)
   : RTSPServer(env, listenSocket, ourPort, authDB, reclamationTestSeconds)
   , fSSLContext(sslCtx)
   , fListenSocket(listenSocket) {
   // �� �⺻ �ڵ鷯 ���� ��, �츮 TLS �ڵ鷯 ���
   envir().taskScheduler().turnOffBackgroundReadHandling(fListenSocket);
   envir().taskScheduler().turnOnBackgroundReadHandling(
      fListenSocket, &RTSPServerTLS::incomingConnectionHandlerTLS, this);
}

// ���� �ڵ鷯 �� �츮 ��� �ڵ鷯 ȣ��
void RTSPServerTLS::incomingConnectionHandlerTLS(void* clientData, int /*mask*/) {
   ((RTSPServerTLS*)clientData)->incomingConnectionHandler1TLS();
}

void RTSPServerTLS::incomingConnectionHandler1TLS() {
   struct sockaddr_in clientAddr; SOCKLEN_T addrLen = sizeof clientAddr;
   int clientSocket = accept(fListenSocket, (struct sockaddr*)&clientAddr, &addrLen);
   if (clientSocket < 0) return;

   makeSocketNonBlocking(clientSocket);

   SSL* ssl = SSL_new(fSSLContext);
   SSL_set_fd(ssl, clientSocket);
   if (SSL_accept(ssl) <= 0) {
      ERR_print_errors_fp(stderr);
      SSL_free(ssl);
#if defined(_WIN32)
      closesocket(clientSocket);
#else
      ::close(clientSocket);
#endif
      return;
   }

   // FD��SSL ���� ����
   fSSLByFD[clientSocket] = ssl;

   // ���� base �帧(���� ����)�� �����Ű����, base �� accept ���� ��ο� ����� �մϴ�.
   // ������ ���: �ٷ� createNewClientConnection ȣ��
   RTSPClientConnection* clientConn = createNewClientConnection(clientSocket, clientAddr);
   if (clientConn == nullptr) {
      // ���� �� ����
      SSL_shutdown(ssl);
      SSL_free(ssl);
#if defined(_WIN32)
      closesocket(clientSocket);
#else
      ::close(clientSocket);
#endif
   }
}

RTSPServer::RTSPClientConnection*
RTSPServerTLS::createNewClientConnection(int clientSocket, struct sockaddr_in clientAddr) {
   SSL* ssl = nullptr;
   auto it = fSSLByFD.find(clientSocket);
   if (it != fSSLByFD.end()) { ssl = it->second; fSSLByFD.erase(it); }
   return new RTSPServerTLS::RTSPClientConnectionTLS(*this, clientSocket, clientAddr, ssl);
}

/*********** TLS ClientConnection ***********/
RTSPServerTLS::RTSPClientConnectionTLS::RTSPClientConnectionTLS(
   RTSPServer& ourServer, int clientSocket, struct sockaddr_in clientAddr, SSL* ssl)
   : RTSPServer::RTSPClientConnection(ourServer, clientSocket, clientAddr),
   fSSL(ssl) {}

RTSPServerTLS::RTSPClientConnectionTLS::~RTSPClientConnectionTLS() {
   closeSockets();
}

void RTSPServerTLS::RTSPClientConnectionTLS::handleRequestBytes(int /*newBytesRead*/) {
   if (!fSSL) { RTSPServer::RTSPClientConnection::handleRequestBytes(-1); return; }

   unsigned char buf[20000];
   int n = SSL_read(fSSL, buf, sizeof buf);
   if (n <= 0) { RTSPServer::RTSPClientConnection::handleRequestBytes(-1); return; }

   // NOTE: �����δ� base Ŭ������ ���� ��û ����(fRequestBuffer)�� �������ְ�
   // �ļ��� �а� �ؾ� �մϴ�. ���⼱ �ٽ� �帧�� ǥ���մϴ�.
   RTSPServer::RTSPClientConnection::handleRequestBytes(n);
}

void RTSPServerTLS::RTSPClientConnectionTLS::closeSockets() {
   if (fSSL) {
      SSL_shutdown(fSSL);
      SSL_free(fSSL);
      fSSL = nullptr;
   }
   RTSPServer::RTSPClientConnection::closeSockets();
}
