#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <string>



class rsa_encrypt
{
public:
	rsa_encrypt();
	/* First  create  pri1 + pub1 */
	void generate_RSAKey();
	std::string get_self_pubKey();
	std::string encrypt_from_memory(const std::string &clearText , const std::string &pubKey);			//解密使用 selfPubKey  进行解密
	std::string decrypt_from_memory(const std::string &cipherText);		//加密使用 otherPubKey 进行加密

	unsigned char *encrypt_from_file(const unsigned char *clearText, const std::string& pubkeyPath = "./pubkey.pem");
	unsigned char *decrypt_from_file(const unsigned char *cipherText, char *path_key);

private:
	std::string selfPriKey;
	std::string selfPubKey;
};

/* ECB model */
class des_encrypt
{
public:
	des_encrypt(const std::string &key);
	std::string encrypt(const std::string &clearText);
	std::string decrypt(const std::string &cipherText);
private:
	enum{
		SECTION_LENGTH = 8
	};
	const std::string desKey;
};


std::string MD5_32(const std::string &clearText);

#endif // ENCRYPT_H
