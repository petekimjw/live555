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

	void setOpaqueAndAlgorithmAndQop(char const* opaque, char const* algorithm, char const* qop);
	void setHashType(int hashType);
	void setCnonceAndNonceCount(char const* cnonce, char const* nc);
	

	// "passwordIsMD5"가 True이면 "password"는 실제로 md5(<username>:<realm>:<actual-password>)로 계산된 값입니다.
	// --- 새로 추가: SHA-256 사용 여부 제어 ---
	void setUseSHA256(Boolean b) { fUseSHA256 = b; }
	Boolean useSHA256() const { return fUseSHA256; }

	char const* realm() const { return fRealm; }
	char const* nonce() const { return fNonce; }
	char const* username() const { return fUsername; }
	char const* password() const { return fPassword; }

	char const* opaque() const { return fOpaque; }
	char const* algorithm() const { return fAlgorithm; }
	char const* qop() const { return fQop; }
	char const* nc() const { return fNonceCount; }
	unsigned int const count() const { return fCount; }
	char const* cnonce() const { return fCnonce; }
	int const typeOfHash() const { return fHashType; }

	char const* computeDigestResponse(char const* cmd, char const* url) const;
	// 이 함수에서 반환된 문자열은 나중에 다음을 호출하여 해제해야 합니다.
	void reclaimDigestResponse(char const* responseStr) const;
	void updateCount();

private:
	void resetRealmAndNonce();
	void resetUsernameAndPassword();

	void resetOpaqueAndAlgorithmAndQop();
	void resetCnonceAndNonceCount();
	void resetHashType();

	void assignRealmAndNonce(char const* realm, char const* nonce);
	void assignUsernameAndPassword(char const* username, char const* password, Boolean passwordIsMD5);
	void assign(char const* realm, char const* nonce,
		char const* username, char const* password, Boolean passwordIsMD5);

	void assignOpaqueAndAlgorithmAndQop(char const* opaque, char const* algorithm, char const* qop);
	void assignCnonceAndNonceCount(char const* cnonce, char const* nc);
	void assignHashType(int passwordType);
	void assignCount(unsigned const count);
	void assign(char const* realm, char const* nonce,
		char const* username, char const* password, char const* opaque, char const* algorithm,
		char const* qop, char const* cnonce, char const* nc, unsigned const count,
		Boolean passwordIsMD5, int passwordType);

private:
	//렐름. 인증 영역(Protection Space)
	char* fRealm; 
	//넌스. "number used once" → 1회용 난수/토큰
	char* fNonce;
	char* fUsername; 
	char* fPassword;
	Boolean fPasswordIsMD5;
	Boolean fUseSHA256; // False: MD5(기본), True: SHA-256

   //서버가준 불투명한 토근. 클라이언트는 해석하지 않고 그대로 응답에 포함 반환해야함
	char* fOpaque; 
   //서버가준 알고리즘. 예) SHA-256, MD5등
	char* fAlgorithm; 
   //Quality of Protection. 서버에서 보안수준 제시. 예) "auth", "auth-int. 클라이언트에서는 qop 포함될 경우 cnonce, nc 꼭 포함해야함 !
	char* fQop; 
	//클라이언트가 생성한 난수값
	char* fCnonce; 
	//nonce count. 클라이언트가 같은서버 nonce대해 요청을 보낼때마다 증가
	char* fNonceCount;
	int fHashType;
	unsigned fCount;
};

#endif
