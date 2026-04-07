#pragma once

#include <string>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/sha.h>

#define SHA_PASSWORD_LENGTH	64+1

namespace util
{
   // SHA-256 해시를 생성하는 클래스입니다.
	class SHA256
	{
	public:
		SHA256()
		{
			sha256 = NULL;
			init();
		}

		~SHA256()
		{
			reset();
		}

		void init()
		{
			reset();
			sha256 = EVP_MD_CTX_new();
			EVP_DigestInit_ex(sha256, EVP_sha256(), NULL);
		}

		std::string hash(const std::string& in)
		{
			char out_str[SHA_PASSWORD_LENGTH] = {0, };

			update(in.c_str(), in.size());
			final();

			for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
			{
				sprintf(out_str + (i * 2), "%02x", hashBuff[i]);
			}

			out_str[64] = '\0';

			return out_str;
		}
		
		bool compare(const std::string& in, const std::string& enc_in)
		{
			init();
			std::string ret = hash(in);
			return ret == enc_in;
		}

	private:
		void update(const char* in_str, int strLen)
		{
			EVP_DigestUpdate(sha256, in_str, strLen);
		}

		void final()
		{
			unsigned int len;
			EVP_DigestFinal_ex(sha256, hashBuff, &len);
		}

		void reset()
		{
			if (sha256) {
				EVP_MD_CTX_free(sha256);
				sha256 = NULL;
			}
		}

	private:
		EVP_MD_CTX* sha256;
		unsigned char hashBuff[SHA256_DIGEST_LENGTH];
	};

}

static const unsigned char SHA256_KEY[SHA256_DIGEST_LENGTH] = 
{
	0x42, 0x71, 0xb5, 0xe9, 0x39, 0x59, 0x92, 0xab, 
	0xd8, 0x12, 0x24, 0x52, 0x72, 0x80, 0x9b, 0xc1,
	0xe4, 0xef, 0x0f, 0x24, 0x2d, 0x4a, 0x5c, 0x76,
	0x98, 0xa8, 0xb0, 0xbf, 0xc6, 0xd5, 0x06, 0x14
};

//입력받은 문자열과 key 값을 이용하여 변환된 문자열 생성
static void sha256_transform(const char *in, char * out)
{
	int i = 0;

	for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		out[i] = in[i] + SHA256_KEY[i];
	}
}

//입력받은 값과 key 값을 사용하여 hash 생성
inline int sha256_hash_using_key(const char* in, char* hash)
{
	if(NULL == in)
	{
		return 0;
	}

	util::SHA256 sha256;
	char transform_str[SHA_PASSWORD_LENGTH] = {0,};
	sha256_transform(in, transform_str);
	std::string out = sha256.hash(transform_str);
	if(out.size() == 0)
	{
		return 0;
	}

	int len = out.size() + 1;
#ifndef _WIN32
	snprintf(hash, len, "%s", out.c_str());
#endif

	return 1;
}

//첫번째 인자로 입력받은 값을 key 값을 이용하여 hash 생성하고 두번째 인자와 비교
inline int sha256_compare_using_key(const char* in, const char* enc_in)
{
	if(NULL == in)
	{
		return 0;
	}

	util::SHA256 sha256;
	char transform_str[SHA_PASSWORD_LENGTH] = {0,};
	sha256_transform(in, transform_str);
	return sha256.compare(transform_str, enc_in);
}

//입력받은 문자열을 hash 생성
inline char const* sha256_hash(const char* in)
{
	if(NULL == in)
	{
		return NULL;
	}

	util::SHA256 sha256;
	std::string out = sha256.hash(in);
	if(out.size() == 0)
	{
		return NULL;
	}

	int len = out.size() + 1;
	char* ret = (char*)malloc(len);
	if(NULL == ret)
	{
		return NULL;
	}
#ifndef _WIN32
	snprintf(ret, len, "%s", out.c_str());
#else
	_snprintf(ret, len, "%s", out.c_str());
#endif
	return ret;
}

//랜덤한 문자열 생성
inline int sha256_random(size_t len, char* out) {
	EVP_RAND *rand;
	EVP_RAND_CTX *ctx = NULL;
	OSSL_PARAM params[3], *p = params;
	unsigned int strength = 128;
	unsigned char* buf = (unsigned char*)malloc(len);
	if (buf == NULL) return 0;

	rand = EVP_RAND_fetch(NULL, "HMAC-DRBG", NULL);
	ctx = EVP_RAND_CTX_new(rand, NULL);
	EVP_RAND_free(rand);

	*p++ = OSSL_PARAM_construct_utf8_string(OSSL_ALG_PARAM_MAC, const_cast<char*>(SN_hmac), 0);
	*p++ = OSSL_PARAM_construct_utf8_string(OSSL_DRBG_PARAM_DIGEST, const_cast<char*>(SN_sha256), 0);
	*p = OSSL_PARAM_construct_end();

	EVP_RAND_instantiate(ctx, strength, 0, NULL, 0, params);
	EVP_RAND_generate(ctx, buf, len, strength, 0, NULL, 0);
	if (ctx)
		EVP_RAND_CTX_free(ctx);

	size_t i = 0;
	for(; i < len; i++) {
		sprintf(out + (i * 2), "%02x", buf[i]);
	}
	out[i * 2] = '\0';
	free(buf);

	return 1;
}

//입력받은 값과 key 값을 사용하여 hash 생성하고 두번째 인자와 비교
inline int sha256_compare(const char* in, const char* enc_in)
{
	if(NULL == in)
	{
		return 0;
	}

	util::SHA256 sha256;
	return sha256.compare(in, enc_in);
}

//세 개의 입력값을 이용하여 hash 생성
inline char* sha256_hash_digest(const char* input1, const char* input2, const char* input3)
{
	if(NULL == input1 || NULL == input2 || NULL == input3)
	{
		return NULL;
	}

	util::SHA256 sha256;
	std::string hash1 = sha256.hash(input1);
	if(hash1.size() == 0)
	{
		return NULL;
	}
	sha256.init();
	std::string hash2 = sha256.hash(input2);	
	if(hash2.size() == 0)
	{
		return NULL;
	}
	sha256.init();	
	std::string inputEnd = hash1+std::string(input3)+hash2;
	std::string Response = sha256.hash(inputEnd);
	if(Response.size() == 0)
	{
		return NULL;
	}	

	int len = Response.size() + 1;
	char* ret = (char*)malloc(len);
	if(NULL == ret)
	{
		return NULL;
	}
#ifndef _WIN32
	snprintf(ret, len, "%s", Response.c_str());
#endif
	return ret;	
}