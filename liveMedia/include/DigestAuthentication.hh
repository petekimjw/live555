#ifndef _DIGEST_AUTHENTICATION_HH
#define _DIGEST_AUTHENTICATION_HH

#ifndef _BOOLEAN_HH
#include <Boolean.hh>
#endif

// 다이제스트 인증에 사용되는 클래스입니다.
// "realm"과 "nonce" 필드는 서버에서 제공합니다.
// ("401 Unauthorized" 응답에서)
// "username"과 "password" 필드는 클라이언트에서 제공합니다.
class LIVEMEDIA_API Authenticator 
{
public:
  Authenticator();
  Authenticator(char const* username, char const* password, Boolean passwordIsMD5 = False);
  // NOTE: SHA-256 사용 시에도 passwordIsMD5=True는
  // "username:realm:password"에 선택 해시(H=MD5 또는 SHA-256)를 적용한 HA1을
  // 직접 넣었다는 의미로 해석됩니다.
  Authenticator(const Authenticator& orig);
  Authenticator& operator=(const Authenticator& rightSide);
  Boolean operator<(const Authenticator* rightSide);
  virtual ~Authenticator();

  void reset();
  void setRealmAndNonce(char const* realm, char const* nonce);
  void setRealmAndRandomNonce(char const* realm);
  // 위와 같지만, nonce는 무작위로 생성됩니다.
  // (서버에서 사용됩니다.)
  void setUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5 = False);


  // "passwordIsMD5"가 True이면 "password"는 실제로 md5(<username>:<realm>:<actual-password>)로 계산된 값입니다.
  // --- 새로 추가: SHA-256 사용 여부 제어 ---
  void setUseSHA256(Boolean b) { fUseSHA256 = b; }
  Boolean useSHA256() const { return fUseSHA256; }

  char const* realm() const { return fRealm; }
  char const* nonce() const { return fNonce; }
  char const* username() const { return fUsername; }
  char const* password() const { return fPassword; }

  char const* computeDigestResponse(char const* cmd, char const* url) const;
  // 이 함수에서 반환된 문자열은 나중에 다음을 호출하여 해제해야 합니다.
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
  Boolean fUseSHA256; // False: MD5(기본), True: SHA-256
};

#endif
