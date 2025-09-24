/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2025 Live Networks, Inc.  All rights reserved.
// A class used for digest authentication.
// Implementation

#include "DigestAuthentication.hh"
#include "ourMD5.hh"
#include <strDup.hh>
#include <GroupsockHelper.hh> // for gettimeofday()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---- SHA-256 ����(Windows CNG; VS2022 �⺻ ����) ----
#ifdef _WIN32
#include <vector>
#include <bcrypt.h>
#pragma comment(lib, "Bcrypt.lib")
#endif

#pragma region toHex, our_SHA256Data

// Hex ���ڵ� ��ƿ
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

// our_MD5Data�� ������ �������̽��� ���� SHA-256 ����
// resultBuffer�� NULL�̸� ���ο��� ���� �Ҵ�(new[])�� ��ȯ(ȣ���ڰ� delete[]�� ����)
static char const* our_SHA256Data(unsigned char const* input, unsigned inputLen, char* resultBuffer /* 65 or NULL */)
{
#ifndef _WIN32
   // Windows �� �÷����� OpenSSL ������ �ٲ��ּ���.
   // (�̹� ��û ȯ���� Windows 11 + VS2022)
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


Authenticator::Authenticator() {
  assign(NULL, NULL, NULL, NULL, False);
  fUseSHA256 = False; // �⺻: MD5
}

Authenticator::Authenticator(char const* username, char const* password, Boolean passwordIsMD5) {
  assign(NULL, NULL, username, password, passwordIsMD5);
  fUseSHA256 = False; // �⺻: MD5
}

Authenticator::Authenticator(const Authenticator& orig) {
  assign(orig.realm(), orig.nonce(), orig.username(), orig.password(), orig.fPasswordIsMD5);
  fUseSHA256 = orig.fUseSHA256;
}

Authenticator& Authenticator::operator=(const Authenticator& rightSide) {
  if (&rightSide != this) {
    reset();
    assign(rightSide.realm(), rightSide.nonce(),
	   rightSide.username(), rightSide.password(), rightSide.fPasswordIsMD5);
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
}

void Authenticator::setRealmAndNonce(char const* realm, char const* nonce) {
  resetRealmAndNonce();
  assignRealmAndNonce(realm, nonce);
}

void Authenticator::setRealmAndRandomNonce(char const* realm) {
  resetRealmAndNonce();

  // Construct data to seed the random nonce:
  struct {
    struct timeval timestamp;
    unsigned counter;
  } seedData;
  gettimeofday(&seedData.timestamp, NULL);
  static unsigned counter = 0;
  seedData.counter = ++counter;

  // Use MD5 to compute a 'random' nonce from this seed data:
  char nonceBuf[33];
  our_MD5Data((unsigned char*)(&seedData), sizeof seedData, nonceBuf);

  assignRealmAndNonce(realm, nonceBuf);
}

void Authenticator::setUsernameAndPassword(char const* username,
					   char const* password,
					   Boolean passwordIsMD5) {
  resetUsernameAndPassword();
  assignUsernameAndPassword(username, password, passwordIsMD5);
}

// ���� ����:
//   response = H( HA1 : nonce : HA2 )
//   HA1 = H( username : realm : password )  (�Ǵ� fPasswordIsMD5==True�� �̹� HA1)
//   HA2 = H( cmd : url )
// ���⼭ H�� MD5(�⺻) �Ǵ� SHA-256(fUseSHA256=True) �Դϴ�.
char const* Authenticator::computeDigestResponse(char const* cmd, char const* url) const 
{
   const bool useSHA256 = (fUseSHA256 == True);
   const unsigned hashHexLen = useSHA256 ? 64 : 32;

   // ---- HA1 ----
   // fPasswordIsMD5==True �� "�̹� HA1�� ���Դ�"�� �ؼ� (SHA-256���� ���� ����)
   char* ha1Dyn = NULL;
   char ha1Buf[65]; // �ִ� 64hex + NUL
   if (fPasswordIsMD5)
   {
      // �־��� password()�� HA1�̶�� ����
      strncpy(ha1Buf, password(), hashHexLen);
      ha1Buf[hashHexLen] = '\0';
   }
   else
   {
      const unsigned ha1DataLen = (unsigned)(strlen(username()) + 1 + strlen(realm()) + 1 + strlen(password()));
      unsigned char* ha1Data = new unsigned char[ha1DataLen + 1];
      sprintf((char*)ha1Data, "%s:%s:%s", username(), realm(), password());

      if (useSHA256)
         our_SHA256Data(ha1Data, ha1DataLen, ha1Buf);
      else
         our_MD5Data(ha1Data, ha1DataLen, ha1Buf);

      delete[] ha1Data;
   }

   // ---- HA2 ----
   const unsigned ha2DataLen = (unsigned)(strlen(cmd) + 1 + strlen(url));
   unsigned char* ha2Data = new unsigned char[ha2DataLen + 1];
   sprintf((char*)ha2Data, "%s:%s", cmd, url);
   char ha2Buf[65];
   if (useSHA256)
      our_SHA256Data(ha2Data, ha2DataLen, ha2Buf);
   else
      our_MD5Data(ha2Data, ha2DataLen, ha2Buf);
   delete[] ha2Data;

   // ---- response ----
   const unsigned digestDataLen = hashHexLen + 1 + (unsigned)strlen(nonce()) + 1 + hashHexLen;
   unsigned char* digestData = new unsigned char[digestDataLen + 1];
   sprintf((char*)digestData, "%s:%s:%s", ha1Buf, nonce(), ha2Buf);

   char const* result = NULL;
   if (useSHA256)
      result = our_SHA256Data(digestData, digestDataLen, NULL); // new[] ��ȯ
   else
      result = our_MD5Data(digestData, digestDataLen, NULL);    // new[] ��ȯ

   delete[] digestData;
   return result;
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
