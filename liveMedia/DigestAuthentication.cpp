#include "DigestAuthentication.hh"
#include "ourMD5.hh"
#include <strDup.hh>
#include <GroupsockHelper.hh> // for gettimeofday()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SHA256.hh"

// ---- SHA-256 구현(Windows CNG; VS2022 기본 제공) ----
#ifdef _WIN32
#include <vector>
#include <bcrypt.h>
#pragma comment(lib, "Bcrypt.lib")
#endif

#pragma region toHex, our_SHA256Data

// Hex 인코딩 유틸
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

// our_MD5Data와 동일한 인터페이스를 갖는 SHA-256 버전
// resultBuffer가 NULL이면 내부에서 새로 할당(new[])해 반환(호출자가 delete[]로 해제)
static char const* our_SHA256Data(unsigned char const* input, unsigned inputLen, char* resultBuffer /* 65 or NULL */)
{
#ifndef _WIN32
   // Windows 외 플랫폼은 OpenSSL 등으로 바꿔주세요.
   // (이번 요청 환경은 Windows 11 + VS2022)
   return strDup("0000000000000000000000000000000000000000000000000000000000000000"); // placeholder
#else
   BCRYPT_ALG_HANDLE hAlg = NULL;
   BCRYPT_HASH_HANDLE hHash = NULL;
   NTSTATUS st = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
   if (st != 0) return NULL;

   DWORD objLen = 0, cb = 0;
   st = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&objLen, sizeof(DWORD), &cb, 0);
   if (st != 0) { BCryptCloseAlgorithmProvider(hAlg, 0); return NULL; }

   std::vector<UCHAR> obj(objLen);
   std::vector<UCHAR> out(32);

   st = BCryptCreateHash(hAlg, &hHash, obj.data(), objLen, NULL, 0, 0);
   if (st == 0) st = BCryptHashData(hHash, (PUCHAR)input, (ULONG)inputLen, 0);
   if (st == 0) st = BCryptFinishHash(hHash, out.data(), (ULONG)out.size(), 0);

   if (hHash) BCryptDestroyHash(hHash);
   if (hAlg)  BCryptCloseAlgorithmProvider(hAlg, 0);
   if (st != 0) return NULL;

   bool needAlloc = (resultBuffer == NULL);
   char* hex = resultBuffer;
   if (needAlloc) hex = new char[65];
   toHex(out.data(), out.size(), hex);
   return hex;
#endif
}

#pragma endregion


Authenticator::Authenticator() : fRealm(NULL), fNonce(NULL), fUsername(NULL), fPassword(NULL),
   fOpaque(NULL), fAlgorithm(NULL), fQop(NULL), fCnonce(NULL), fNonceCount(NULL),
   fPasswordIsMD5(False), fHashType(-1), fCount(0) {
   assign(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, False, -1);
   fUseSHA256 = False; // 기본: MD5
}

Authenticator::Authenticator(char const* username, char const* password, Boolean passwordIsMD5) :
   fRealm(NULL), fNonce(NULL), fUsername(NULL), fPassword(NULL),
   fOpaque(NULL), fAlgorithm(NULL), fQop(NULL), fCnonce(NULL), fNonceCount(NULL),
   fPasswordIsMD5(False), fHashType(-1), fCount(0) {
   assign(NULL, NULL, username, password, NULL, NULL, NULL, NULL, NULL, 0, passwordIsMD5, -1);
   fUseSHA256 = False; // 기본: MD5
}

Authenticator::Authenticator(const Authenticator& orig) {
   assign(orig.realm(), orig.nonce(), orig.username(), orig.password(),
      orig.opaque(), orig.algorithm(), orig.qop(), orig.cnonce(),
      orig.nc(), orig.count(), orig.fPasswordIsMD5, -1);
   fUseSHA256 = orig.fUseSHA256;
}

Authenticator& Authenticator::operator=(const Authenticator& rightSide) {
   if (&rightSide != this) {
      reset();
      assign(rightSide.realm(), rightSide.nonce(),
         rightSide.username(), rightSide.password(),
         rightSide.opaque(), rightSide.algorithm(), rightSide.qop(),
         rightSide.cnonce(), rightSide.nc(), /*rightSide.count()*/count(), rightSide.fPasswordIsMD5,
         rightSide.typeOfHash());
      fUseSHA256 = rightSide.fUseSHA256;
   }

   return *this;
}

Boolean Authenticator::operator<(const Authenticator* rightSide) {
   // Returns True if "rightSide" is 'newer' than us:
   if (rightSide != NULL && rightSide != this &&
      (rightSide->realm() != NULL || rightSide->nonce() != NULL ||
         username() == NULL || password() == NULL ||
         strcmp(rightSide->username(), username()) != 0 ||
         strcmp(rightSide->password(), password()) != 0)) {
      return True;
   }

   return False;
}

Authenticator::~Authenticator() {
   reset();
}

void Authenticator::reset() {
   resetRealmAndNonce();
   resetUsernameAndPassword();
   resetOpaqueAndAlgorithmAndQop();
   resetCnonceAndNonceCount();
   resetHashType();
}

void Authenticator::setRealmAndNonce(char const* realm, char const* nonce) {
   resetRealmAndNonce();
   assignRealmAndNonce(realm, nonce);
}

void Authenticator::setRealmAndRandomNonce(char const* realm) {
   resetRealmAndNonce();

   char nonceBuf[65];
   sha256_random(32, nonceBuf);

   assignRealmAndNonce(realm, nonceBuf);
}

void Authenticator::setUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5) 
{
   resetUsernameAndPassword();
   assignUsernameAndPassword(username, password, passwordIsMD5);
}

void Authenticator::setHashType(int hashType) {
   resetHashType();
   assignHashType(hashType);
}

void Authenticator::setOpaqueAndAlgorithmAndQop(char const* opaque, char const* algorithm, char const* qop) {
   resetOpaqueAndAlgorithmAndQop();
   assignOpaqueAndAlgorithmAndQop(opaque, algorithm, qop);

   // Use MD5 to compute a 'random' nonce from this seed data:
   char nonceBuf[17];
   sha256_random(8, nonceBuf);

   char nc[9];
   sprintf(nc, "%08x", fCount);

   assignCnonceAndNonceCount(nonceBuf, nc);
}



// Digest 인증 계산 공식 (RFC 7616 기반, qop="auth" 또는 "auth-int" 지원):
// response = H( HA1 : nonce : HA2 ) 또는
// response = H( HA1 : nonce : nc : cnonce : qop : HA2 ) <- qop 있는 경우
//   HA1 = H( username : realm : password ) 또는 fPasswordIsMD5==True면 이미 HA1
//   HA2 = H( cmd : url ) 또는
//   HA2 = H( cmd : url : entity ) <- qop="auth-int" 인 경우
// 여기서 H는 MD5(기본) 또는 SHA-256(fUseSHA256=True) 해시 스트링입니다.
char const* Authenticator::computeDigestResponse(char const* cmd, char const* url) const 
{
   if (fAlgorithm != NULL) //Digest 인증
   {
      if (strcmp(fAlgorithm, "SHA-256") == 0) 
      {
         //HA1 = H( username : realm : password )  (또는 fPasswordIsMD5==True면 이미 HA1)
         char ha1Buf[65];
         char entityhex[65];
         if (fPasswordIsMD5) 
         {
            strncpy(ha1Buf, password(), 64); 
            ha1Buf[64] = '\0'; // just in case
         } 
         else 
         {
            unsigned const ha1DataLen = strlen(username()) + 1 + strlen(realm()) + 1 + strlen(password());
            unsigned char* ha1Data = new unsigned char[ha1DataLen + 1];
            sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());
            char const* ha1temp = sha256_hash((char*)ha1Data);
            sprintf((char*)ha1Buf, "%s", ha1temp);
            free((char*)ha1temp);
            delete[] ha1Data;
         }

         //qop="auth-int" 인 경우 H2 = H( cmd : url : entity )
         //아닌 경우 H2 = H( cmd : url )
         unsigned ha2DataLen = 0;
         unsigned char* ha2Data = NULL;
         if (fQop != NULL && strcmp(fQop, "auth-int") == 0)
         {
            size_t n = sizeof(entityhex) / sizeof(entityhex[0]);
            ha2DataLen = strlen(cmd) + 1 + strlen(url) + 1 + n;
            ha2Data = new unsigned char[ha2DataLen + 1];
            sprintf((char*)ha2Data, "%s:%s:%s", cmd, url, entityhex);
         }
         else 
         {
            ha2DataLen = strlen(cmd) + 1 + strlen(url);
            ha2Data = new unsigned char[ha2DataLen + 1];
            sprintf((char*)ha2Data, "%s:%s", cmd, url);
         }

         char ha2Buf[65];
         char const* ha2temp = sha256_hash((char*)ha2Data);
         sprintf((char*)ha2Buf, "%s", ha2temp);
         free((char*)ha2temp);
         delete[] ha2Data;

         //qop="auth" 인 경우 response = H( HA1 : nonce : nc : cnonce : qop : HA2 )
         //아닌 경우: response = H( HA1 : nonce : HA2 )
         unsigned digestDataLen = 0;
         unsigned char* digestData = NULL;
         if (fQop != NULL && strcmp(fQop, "auth") == 0 && fNonceCount != NULL && fCnonce != NULL) 
         {
            digestDataLen = 64 + 1 + strlen(nonce()) + 1 + strlen(nc()) + 1 + strlen(cnonce()) + 1 + strlen(qop()) + 1 + 64;
            digestData = new unsigned char[digestDataLen + 1];
            sprintf((char*)digestData, "%s:%s:%s:%s:%s:%s", ha1Buf, nonce(), nc(), cnonce(), qop(), ha2Buf);
         }
         else 
         {
            digestDataLen = 64 + 1 + strlen(nonce()) + 1 + 64;
            digestData = new unsigned char[digestDataLen + 1];
            sprintf((char*)digestData, "%s:%s:%s", ha1Buf, nonce(), ha2Buf);
         }

         char const* result = sha256_hash((char*)digestData);
         delete[] digestData;
         return result;
      }
      else if (strcmp(fAlgorithm, "MD5") == 0) 
      {
         //HA1 = H( username : realm : password )  (또는 fPasswordIsMD5==True면 이미 HA1)
         char ha1Buf[33];
         char entityhex[33];
         if (fPasswordIsMD5) {
            strncpy(ha1Buf, password(), 32);
            ha1Buf[32] = '\0'; // just in case
         }
         else 
         {
            unsigned const ha1DataLen = strlen(username()) + 1 + strlen(realm()) + 1 + strlen(password());
            unsigned char* ha1Data = new unsigned char[ha1DataLen + 1];
            sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());
            our_MD5Data(ha1Data, ha1DataLen, ha1Buf);
            delete[] ha1Data;
         }

         //H2 = H( cmd : url )
         unsigned ha2DataLen = 0;
         unsigned char* ha2Data = NULL;
         if (strcmp(fQop, "auth-int") == 0)
         {
            size_t n = sizeof(entityhex) / sizeof(entityhex[0]);
            ha2DataLen = strlen(cmd) + 1 + strlen(url) + 1 + n;
            ha2Data = new unsigned char[ha2DataLen + 1];
            sprintf((char*)ha2Data, "%s:%s:%s", cmd, url, entityhex);
         }
         else 
         {
            ha2DataLen = strlen(cmd) + 1 + strlen(url);
            ha2Data = new unsigned char[ha2DataLen + 1];
            sprintf((char*)ha2Data, "%s:%s", cmd, url);
         }

         char ha2Buf[33];
         our_MD5Data(ha2Data, ha2DataLen, ha2Buf);
         delete[] ha2Data;

         //qop="auth" 인 경우 response = H( HA1 : nonce : nc : cnonce : qop : HA2 )
         //아닌 경우: response = H( HA1 : nonce : HA2 )
         unsigned digestDataLen = 0;
         unsigned char* digestData = NULL;
         if (strcmp(fQop, "auth") == 0) 
         {
            digestDataLen = 32 + 1 + strlen(nonce()) + 1 + strlen(nc()) + 1 + strlen(cnonce()) + 1 + strlen(qop()) + 1 + 32;
            digestData = new unsigned char[digestDataLen + 1];
            sprintf((char*)digestData, "%s:%s:%s:%s:%s:%s", ha1Buf, nonce(), nc(), cnonce(), qop(), ha2Buf);
         }
         else 
         {
            digestDataLen = 32 + 1 + strlen(nonce()) + 1 + 32;
            digestData = new unsigned char[digestDataLen + 1];
            sprintf((char*)digestData, "%s:%s:%s", ha1Buf, nonce(), ha2Buf);
         }

         char const* result = our_MD5Data(digestData, digestDataLen, NULL);
         delete[] digestData;
         return result;
      }
   } 
   else //Basic 인증
   {
      //HA1 = H( username : realm : password )  (또는 fPasswordIsMD5==True면 이미 HA1)
      char ha1Buf[33];
      if (fPasswordIsMD5) 
      {
         strncpy(ha1Buf, password(), 32);
         ha1Buf[32] = '\0'; // just in case
      } 
      else 
      {
         unsigned const ha1DataLen = strlen(username()) + 1 + strlen(realm()) + 1 + strlen(password());
         unsigned char* ha1Data = new unsigned char[ha1DataLen+1];
         sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());
         our_MD5Data(ha1Data, ha1DataLen, ha1Buf);
         delete[] ha1Data;
      }

      //H2 = H( cmd : url )
      unsigned const ha2DataLen = strlen(cmd) + 1 + strlen(url);
      unsigned char* ha2Data = new unsigned char[ha2DataLen+1];
      sprintf((char*)ha2Data, "%s:%s", cmd, url);
      char ha2Buf[33];
      our_MD5Data(ha2Data, ha2DataLen, ha2Buf);
      delete[] ha2Data;

      //response = H( HA1 : nonce : HA2 )
      unsigned const digestDataLen = 32 + 1 + strlen(nonce()) + 1 + 32;
      unsigned char* digestData = new unsigned char[digestDataLen+1];
      sprintf((char*)digestData, "%s:%s:%s", ha1Buf, nonce(), ha2Buf);
      char const* result = our_MD5Data(digestData, digestDataLen, NULL);
      delete[] digestData;
      return result;
   }
   return NULL;
}



void Authenticator::reclaimDigestResponse(char const* responseStr) const {
   delete[](char*)responseStr;
}

void Authenticator::resetRealmAndNonce() {
   delete[] fRealm; fRealm = NULL;
   delete[] fNonce; fNonce = NULL;
}

void Authenticator::resetUsernameAndPassword() {
   delete[] fUsername; fUsername = NULL;
   delete[] fPassword; fPassword = NULL;
   fPasswordIsMD5 = False;
}

void Authenticator::resetOpaqueAndAlgorithmAndQop() {
   if (fOpaque) { delete[] fOpaque; fOpaque = NULL; }
   if (fAlgorithm) { delete[] fAlgorithm; fAlgorithm = NULL; }
   if (fQop) { delete[] fQop; fQop = NULL; }
}

void Authenticator::resetCnonceAndNonceCount() {
   if (fCnonce) {delete[] fCnonce; fCnonce = NULL; }
   if (fNonceCount) { delete[] fNonceCount; fNonceCount = NULL; }
   fCount = 0;
}

void Authenticator::resetHashType() {
   fHashType = -1;
}



void Authenticator::assignRealmAndNonce(char const* realm, char const* nonce) {
   fRealm = strDup(realm);
   fNonce = strDup(nonce);
}

void Authenticator::assignUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5) {
   if (username == NULL) username = "";
   if (password == NULL) password = "";

   fUsername = strDup(username);
   fPassword = strDup(password);
   fPasswordIsMD5 = passwordIsMD5;
}

void Authenticator::assign(char const* realm, char const* nonce,
   char const* username, char const* password, Boolean passwordIsMD5) {
   assignRealmAndNonce(realm, nonce);
   assignUsernameAndPassword(username, password, passwordIsMD5);
}



void Authenticator::assignOpaqueAndAlgorithmAndQop(char const* opaque, char const* algorithm, char const* qop) {
   resetOpaqueAndAlgorithmAndQop();
   fOpaque = strDup(opaque);
   fAlgorithm = strDup(algorithm);
   fQop = strDup(qop);
}

void Authenticator::assignCnonceAndNonceCount(char const* cnonce, char const* nc) {
   resetCnonceAndNonceCount();
   fCnonce = strDup(cnonce);
   fNonceCount = strDup(nc);
   if (nc)
      assignCount(strtol(nc, NULL, 16));
}

void Authenticator::assignCount(unsigned const count) {
   fCount = count;
}

void Authenticator::assignHashType(int hashType) {
   fHashType = hashType;
}

void Authenticator::assign(char const* realm, char const* nonce,
   char const* username, char const* password, char const* opaque, char const* algorithm,
   char const* qop, char const* cnonce, char const* nc, unsigned const count, Boolean passwordIsMD5, int hashType) {
   assignRealmAndNonce(realm, nonce);
   assignOpaqueAndAlgorithmAndQop(opaque, algorithm, qop);
   assignUsernameAndPassword(username, password, passwordIsMD5);
   assignCnonceAndNonceCount(NULL, NULL);
   assignCount(count);
   assignHashType(hashType);
}

void Authenticator::setCnonceAndNonceCount(char const* cnonce, char const* nc) {
   resetCnonceAndNonceCount();
   assignCnonceAndNonceCount(cnonce, nc);
}

void Authenticator::updateCount() {
   char nonceBuf[17];
   sha256_random(8, nonceBuf);

   char nc[9];
   sprintf(nc, "%08x", ++fCount);

   assignCnonceAndNonceCount(nonceBuf, nc);
}