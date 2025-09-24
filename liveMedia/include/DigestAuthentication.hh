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
// C++ header

#ifndef _DIGEST_AUTHENTICATION_HH
#define _DIGEST_AUTHENTICATION_HH

#ifndef _BOOLEAN_HH
#include <Boolean.hh>
#endif

// ��������Ʈ ������ ���Ǵ� Ŭ�����Դϴ�.
// "realm"�� "nonce" �ʵ�� �������� �����մϴ�.
// ("401 Unauthorized" ���信��)
// "username"�� "password" �ʵ�� Ŭ���̾�Ʈ���� �����մϴ�.
class LIVEMEDIA_API Authenticator 
{
public:
  Authenticator();
  Authenticator(char const* username, char const* password, Boolean passwordIsMD5 = False);
  // NOTE: SHA-256 ��� �ÿ��� passwordIsMD5=True��
  // "username:realm:password"�� ���� �ؽ�(H=MD5 �Ǵ� SHA-256)�� ������ HA1��
  // ���� �־��ٴ� �ǹ̷� �ؼ��˴ϴ�.
  Authenticator(const Authenticator& orig);
  Authenticator& operator=(const Authenticator& rightSide);
  Boolean operator<(const Authenticator* rightSide);
  virtual ~Authenticator();

  void reset();
  void setRealmAndNonce(char const* realm, char const* nonce);
  void setRealmAndRandomNonce(char const* realm);
  // ���� ������, nonce�� �������� �����˴ϴ�.
  // (�������� ���˴ϴ�.)
  void setUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5 = False);

  // "passwordIsMD5"�� True�̸� "password"�� ������ md5(<username>:<realm>:<actual-password>)�� ���� ���Դϴ�.
  // --- ���� �߰�: SHA-256 ��� ���� ���� ---
  void setUseSHA256(Boolean b) { fUseSHA256 = b; }
  Boolean useSHA256() const { return fUseSHA256; }

  char const* realm() const { return fRealm; }
  char const* nonce() const { return fNonce; }
  char const* username() const { return fUsername; }
  char const* password() const { return fPassword; }

  char const* computeDigestResponse(char const* cmd, char const* url) const;
  // �� �Լ����� ��ȯ�� ���ڿ��� ���߿� ������ ȣ���Ͽ� �����ؾ� �մϴ�.
  void reclaimDigestResponse(char const* responseStr) const;

private:
  void resetRealmAndNonce();
  void resetUsernameAndPassword();
  void assignRealmAndNonce(char const* realm, char const* nonce);
  void assignUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5);
  void assign(char const* realm, char const* nonce,
	      char const* username, char const* password, Boolean passwordIsMD5);

private:
  char* fRealm; char* fNonce;
  char* fUsername; char* fPassword;
  Boolean fPasswordIsMD5;
  Boolean fUseSHA256; // False: MD5(�⺻), True: SHA-256
};

#endif
