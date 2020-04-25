#include "encrypt.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <openssl/md5.h>
#include <vector>
#include <cstring>
#include <stdexcept>

#include <sstream>
#include <iomanip>

static const int KEY_LENGTH = 2048;
//static const int MD5_LENGTH = 32;
rsa_encrypt::rsa_encrypt()
{
}

void rsa_encrypt::generate_RSAKey()
{
	// 公私密钥对
	size_t pri_len;
	size_t pub_len;
	char *pri_key = NULL;
	char *pub_key = NULL;

	// 生成密钥对
	RSA *keypair = RSA_generate_key(KEY_LENGTH, RSA_3, NULL, NULL);

	BIO *pri = BIO_new(BIO_s_mem());
	BIO *pub = BIO_new(BIO_s_mem());

	PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, keypair);

	// 获取长度
	pri_len = BIO_pending(pri);
	pub_len = BIO_pending(pub);

	// 密钥对读取到字符串
	pri_key = (char *)malloc(pri_len + 1);
	pub_key = (char *)malloc(pub_len + 1);

	BIO_read(pri, pri_key, pri_len);
	BIO_read(pub, pub_key, pub_len);

	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';

	// 存储密钥对
	//	strKey[0] = pub_key;
	this->selfPubKey.assign(pub_key);
	//	strKey[1] = pri_key;
	this->selfPriKey.assign(pri_key);

	// 内存释放
	RSA_free(keypair);
	BIO_free_all(pub);
	BIO_free_all(pri);

	free(pri_key);
	free(pub_key);
}

std::string rsa_encrypt::get_self_pubKey()
{
	return this->selfPubKey;
}

std::string rsa_encrypt::encrypt_from_memory(const std::string &clearText, const std::string &pubKey)
{
	std::string strRet;
	RSA *rsa = NULL;
	BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);

	RSA* pRSAPublicKey = RSA_new();
	rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);

	int len = RSA_size(rsa);
	char *encryptedText = (char *)malloc(len + 1);
	memset(encryptedText, 0, len + 1);

	// 加密函数
	int ret = -1 ;

	//	ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(),
	//								 (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
	//	if (ret >= 0)
	//		strRet = std::string(encryptedText, ret);
	//	else
	//		throw std::runtime_error(ERR_error_string(ERR_get_error(),NULL));

	int clearTextLen = clearText.length();
	const int &maxEnLen(len-11);
	int i = 0 ;
	while( clearTextLen >= maxEnLen )
	{
		ret = RSA_public_encrypt( maxEnLen , (const unsigned char*)clearText.substr(i*maxEnLen , maxEnLen ).c_str(),
								  (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
		if (ret >= 0)
			strRet.append(std::string(encryptedText, ret));
		else
			throw std::runtime_error(ERR_error_string(ERR_get_error(),NULL));
		i++;
		clearTextLen -= maxEnLen;
	}
	if( clearTextLen > 0 )
	{
		ret = RSA_public_encrypt( clearTextLen  , (const unsigned char*)clearText.substr(i*maxEnLen).c_str(),
								  (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);
		if (ret >= 0)
			strRet.append(std::string(encryptedText, ret));
		else
			throw std::runtime_error(ERR_error_string(ERR_get_error(),NULL));
	}
	// 释放内存
	free(encryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}

std::string rsa_encrypt::decrypt_from_memory(const std::string &cipherText)
{
	std::string strRet;
	RSA *rsa = RSA_new();
	BIO *keybio;
	keybio = BIO_new_mem_buf((unsigned char *)this->selfPriKey.c_str(), -1);
	rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

	int len = RSA_size(rsa);
	char *decryptedText = (char *)malloc(len + 1);
	memset(decryptedText, 0, len + 1);

	// 解密函数
	//	int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);
	//	if (ret >= 0)
	//		strRet = std::string(decryptedText, ret);

	int ret = -1;
	int cipherTextLen( cipherText.length() );
	const int &maxDeLen(len);
	int i = 0 ;
	while( cipherTextLen >= maxDeLen)
	{
		ret = RSA_private_decrypt(maxDeLen , (const unsigned char*)cipherText.substr(i*maxDeLen, maxDeLen).c_str(),
								  (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING );
		if (ret >= 0)
			strRet.append(std::string(decryptedText, ret));
		else
			throw std::runtime_error(ERR_error_string(ERR_get_error(),NULL));
		cipherTextLen -= maxDeLen;
		i++;
	}
	if( cipherTextLen > 0 )
	{
		throw std::runtime_error("Unbeliveable : "+std::to_string(cipherTextLen));

	}
	// 释放内存
	free(decryptedText);
	BIO_free_all(keybio);
	RSA_free(rsa);

	return strRet;
}

unsigned char *rsa_encrypt::encrypt_from_file(const unsigned char *clearText, const std::string &pubkeyPath)
{

	unsigned char *p_en = NULL;
	RSA  *p_rsa = NULL;
	FILE *file = NULL;

	int  rsa_len = 0;    //flen为源文件长度， rsa_len为秘钥长度

	//1.打开秘钥文件
	if((file = fopen(pubkeyPath.c_str(), "rb")) == NULL)
	{
		perror("OPEN PUBLIC FILE FAILED");
		goto End;
	}

	//2.从公钥文件中获取 加密的公钥
	if((p_rsa = PEM_read_RSA_PUBKEY(file, NULL,NULL,NULL )) == NULL)
	{
		ERR_print_errors_fp(stdout);
		goto End;
	}

	//3.获取秘钥的长度
	rsa_len = RSA_size(p_rsa);

	//4.为加密后的内容 申请空间（根据秘钥的长度+1）
	p_en = (unsigned char *)malloc(rsa_len + 1);
	if(!p_en)
	{
		perror("MALLOC ERROR");
		goto End;
	}
	memset(p_en, 0, rsa_len + 1);

	//5.对内容进行加密
	if(RSA_public_encrypt(rsa_len, clearText, p_en, p_rsa, RSA_NO_PADDING) < 0)
	{
		perror("RSA_public_encrypt() error");
		goto End;
	}

End:

	//6.释放秘钥空间， 关闭文件
	if(p_rsa)    RSA_free(p_rsa);
	if(file)     fclose(file);

	return p_en;
}

unsigned char *rsa_encrypt::decrypt_from_file(const unsigned char *cipherText, char *path_key)
{
	unsigned char *p_de = NULL;
	RSA  *p_rsa = NULL;
	FILE *file = NULL;
	int   rsa_len = 0;
	//1.打开秘钥文件
	file = fopen(path_key, "rb");
	if(!file)
	{
		perror("OPEN Private File FAILED");
		goto End;
	}

	//2.从私钥中获取 解密的秘钥
	if((p_rsa = PEM_read_RSAPrivateKey(file, NULL,NULL,NULL )) == NULL)
	{
		ERR_print_errors_fp(stdout);
		goto End;
	}

	//3.获取秘钥的长度，
	rsa_len = RSA_size(p_rsa);

	//4.为加密后的内容 申请空间（根据秘钥的长度+1）
	p_de = (unsigned char *)malloc(rsa_len + 1);
	if(!p_de)
	{
		perror("MALLOC FAILED");
		goto End;
	}
	memset(p_de, 0, rsa_len + 1);

	//5.对内容进行加密
	if(RSA_private_decrypt(rsa_len, cipherText, p_de, p_rsa, RSA_NO_PADDING) < 0)
	{
		perror("RSA_public_encrypt() error ");
		goto End;
	}

End:
	//6.释放秘钥空间， 关闭文件
	if(p_rsa)    RSA_free(p_rsa);
	if(file)     fclose(file);

	return p_de;
}

des_encrypt::des_encrypt(const std::string &key)
	:desKey(key)
{

}

std::string des_encrypt::encrypt(const std::string &clearText)
{
	std::string cipherText; // 密文
	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, SECTION_LENGTH);

	// 构造补齐后的密钥
	if (desKey.length() <= SECTION_LENGTH)
		memcpy(keyEncrypt, desKey.c_str(), desKey.length());
	else
		memcpy(keyEncrypt, desKey.c_str(), SECTION_LENGTH);

	// 密钥置换
	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	// 循环加密，每8字节一次
	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCiphertext;
	unsigned char tmp[SECTION_LENGTH];

	for (int i = 0; i < clearText.length() / SECTION_LENGTH; i++)
	{
		memcpy(inputText, clearText.c_str() + i * SECTION_LENGTH, SECTION_LENGTH);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, SECTION_LENGTH);

		for (int j = 0; j < SECTION_LENGTH; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	if (clearText.length() % SECTION_LENGTH != 0)
	{
		int tmp1 = clearText.length() / SECTION_LENGTH * SECTION_LENGTH;
		int tmp2 = clearText.length() - tmp1;
		memset(inputText, 0, SECTION_LENGTH);
		memcpy(inputText, clearText.c_str() + tmp1, tmp2);
		// 加密函数
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, SECTION_LENGTH);

		for (int j = 0; j < SECTION_LENGTH; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	cipherText.clear();
	cipherText.assign(vecCiphertext.begin(), vecCiphertext.end());

	return cipherText;
}

std::string des_encrypt::decrypt(const std::string &cipherText)
{
	std::string clearText; // 明文
	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, SECTION_LENGTH);

	if (desKey.length() <= SECTION_LENGTH)
		memcpy(keyEncrypt, desKey.c_str(), desKey.length());
	else
		memcpy(keyEncrypt, desKey.c_str(), SECTION_LENGTH);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCleartext;
	unsigned char tmp[SECTION_LENGTH];

	for (int i = 0; i < cipherText.length() / SECTION_LENGTH; i++)
	{
		memcpy(inputText, cipherText.c_str() + i * SECTION_LENGTH, SECTION_LENGTH);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, SECTION_LENGTH);

		for (int j = 0; j < SECTION_LENGTH; j++)
			if( tmp[j] != '\0')
				vecCleartext.push_back(tmp[j]);
	}

	if (cipherText.length() % SECTION_LENGTH != 0 )
	{
		int tmp1 = cipherText.length() / SECTION_LENGTH * SECTION_LENGTH;
		int tmp2 = cipherText.length() - tmp1;
		memset(inputText, 0, SECTION_LENGTH);
		memcpy(inputText, cipherText.c_str() + tmp1, tmp2);
		// 解密函数
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, SECTION_LENGTH);

		for (int j = 0; j < tmp2; j++)
			vecCleartext.push_back(tmp[j]);
	}

	clearText.clear();
	clearText.assign(vecCleartext.begin(), vecCleartext.end());

	return clearText;
}

std::string MD5_32(const std::string &clearText)
{
	const int &md5Len = 16;
	char md5Result[md5Len];
	std::stringstream streamMD5;
	std::string MD5String;
	MD5((unsigned char *)clearText.c_str(),clearText.length(),(unsigned char *)md5Result);

	for( unsigned char x : md5Result )
		streamMD5 << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)x;

	streamMD5 >> MD5String;

	return MD5String;
}












