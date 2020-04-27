#include "configfile.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <vector>

/*
//加密
static unsigned char *my_encrypt(const unsigned char *str, const std::string& pubkeyPath = "./pubkey.pem")
{
	unsigned char *p_en = NULL;
	RSA  *p_rsa = NULL;
	FILE *file = NULL;

	int  rsa_len = 0;    //flen为源文件长度， rsa_len为秘钥长度

	//1.打开秘钥文件
	if((file = fopen(pubkeyPath.c_str(), "rb")) == NULL)
	{
		perror("fopen() error 111111111 ");
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
		perror("malloc() error 2222222222");
		goto End;
	}
	memset(p_en, 0, rsa_len + 1);

	//5.对内容进行加密
	if(RSA_public_encrypt(rsa_len, str, p_en, p_rsa, RSA_NO_PADDING) < 0)
	{
		perror("RSA_public_encrypt() error 2222222222");
		goto End;
	}

End:

	//6.释放秘钥空间， 关闭文件
	if(p_rsa)    RSA_free(p_rsa);
	if(file)     fclose(file);

	return p_en;
}

//解密
static unsigned char *my_decrypt(const unsigned char *str, char *path_key)
{
	unsigned char *p_de = NULL;
	RSA  *p_rsa = NULL;
	FILE *file = NULL;
	int   rsa_len = 0;
	//1.打开秘钥文件
	file = fopen(path_key, "rb");
	if(!file)
	{
		perror("fopen() error 22222222222");
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
		perror("malloc() error ");
		goto End;
	}
	memset(p_de, 0, rsa_len + 1);

	//5.对内容进行加密
	if(RSA_private_decrypt(rsa_len, str, p_de, p_rsa, RSA_NO_PADDING) < 0)
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

std::string des_encrypt(const std::string &clearText, const std::string &key)
{
	std::string cipherText; // 密文

	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);

	// 构造补齐后的密钥
	if (key.length() <= 8)
		memcpy(keyEncrypt, key.c_str(), key.length());
	else
		memcpy(keyEncrypt, key.c_str(), 8);

	// 密钥置换
	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	// 循环加密，每8字节一次
	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCiphertext;
	unsigned char tmp[8];

	for (int i = 0; i < clearText.length() / 8; i++)
	{
		memcpy(inputText, clearText.c_str() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	if (clearText.length() % 8 != 0)
	{
		int tmp1 = clearText.length() / 8 * 8;
		int tmp2 = clearText.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, clearText.c_str() + tmp1, tmp2);
		// 加密函数
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	cipherText.clear();
	cipherText.assign(vecCiphertext.begin(), vecCiphertext.end());

	return cipherText;
}

// 解密 ecb模式
std::string des_decrypt(const std::string &cipherText, const std::string &key)
{
	std::string clearText; // 明文

	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);

	if (key.length() <= 8)
		memcpy(keyEncrypt, key.c_str(), key.length());
	else
		memcpy(keyEncrypt, key.c_str(), 8);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	const_DES_cblock inputText;
	DES_cblock outputText;
	std::vector<unsigned char> vecCleartext;
	unsigned char tmp[8];

	for (int i = 0; i < cipherText.length() / 8; i++)
	{
		memcpy(inputText, cipherText.c_str() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			if( tmp[j] != '\0')
				vecCleartext.push_back(tmp[j]);
	}

	if (cipherText.length() % 8 != 0 )
	{
		int tmp1 = cipherText.length() / 8 * 8;
		int tmp2 = cipherText.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, cipherText.c_str() + tmp1, tmp2);
		// 解密函数
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < tmp2; j++)
			vecCleartext.push_back(tmp[j]);
	}

	clearText.clear();
	clearText.assign(vecCleartext.begin(), vecCleartext.end());

	return clearText;
}
*/


configFile::configFile(const std::string &configFP, const std::string &desK):
	configFilePath(configFP),desEn(desK)
{
	configKV[ACCOUNT]="";
	configKV[PASSWORD]="";
	configKV[PRIDISK]="";
	configKV[LOCALPROT]="";
}

int configFile::analyze_config_file()
{
	std::string line;
	size_t pos;
	std::ifstream fsConfig(configFilePath);
	if( fsConfig )
	{
		while( 1 )
		{
			std::getline(fsConfig,line);
			if( !fsConfig.good() )
				break;
			pos = line.find('=');
			const std::string &configKey(line.substr(0,pos));
			const std::string &configValue(line.substr( pos+1 ));
			if( pos == std::string::npos )
			{
				fsConfig.close();
				return FILE_ERROR;
			}
			if( configValue.empty() )
			{
				fsConfig.close();
				return CONF_NOCOMPLETE;
			}
			if( 0 == configKey.compare(PASSWORD) )
			{
				unsigned char temp;
				std::string cipherText;
				for( int i = 0; i < configValue.length();i += 2)
				{
					if( configValue[i] <= '9' && configValue[i] >= '0' )
						temp = configValue[i] - '0';
					else if ( configValue[i] <= 'f' && configValue[i] >= 'a')
						temp = configValue[i] - 87;

					temp = temp << 4;
					if( configValue[i+1] <= '9' && configValue[i+1] >= '0' )
						temp += configValue[i+1] - '0';
					else if ( configValue[i+1] <= 'f' && configValue[i+1] >= 'a')
						temp += configValue[i+1] - 87;
					cipherText.push_back(temp);
				}
//				configKV[ configKey ] = des_decrypt(cipherText,this->desKey);
				configKV[ configKey ] = this->desEn.decrypt(cipherText);
			}
			else
				configKV[configKey] = configValue;
		}
		fsConfig.close();
		/* TEST */
//		{
//			std::string &content = configKV["PassWord"];
//			std::string result;
//			unsigned char temp;
//			for( int i = 0; i < content.length();i += 2)
//			{
//				if( content[i] <= '9' && content[i] >= '0' )
//					temp = content[i] - '0';
//				else if ( content[i] <= 'f' && content[i] >= 'a')
//					temp = content[i] - 87;

//				temp = temp << 4;
//				if( content[i+1] <= '9' && content[i+1] >= '0' )
//					temp += content[i+1] - '0';
//				else if ( content[i+1] <= 'f' && content[i+1] >= 'a')
//					temp += content[i+1] - 87;
//				result.push_back(temp);
//			}
//			unsigned char *ptf_de = nullptr;
//			ptf_de = my_decrypt((unsigned char *)result.c_str(), "./prikey.pem");
//			std::cout << "PB :" << ptf_de<<std::endl;
//			if( ptf_de ) free(ptf_de);
//		}
		/* END */

	}
	else
		return FILE_NOEXIST;
	return FILE_EXIST;
}

int configFile::flush_file()
{
	/* 写入数据合法性检查 */
	for(auto &x : configKV)
	{
		if( x.second.empty() )
			return CONF_NOCOMPLETE;
	}
	std::ofstream fsConfig(configFilePath,std::fstream::out| std::fstream::binary | std::fstream::trunc);
	if( fsConfig )
	{
		for(auto &x : configKV )
		{
			if( x.first.compare(PASSWORD) == 0)
			{
				std::string enResult;
//				enResult = des_encrypt(x.second,this->desKey);
				enResult = this->desEn.encrypt(x.second);
				fsConfig << PASSWORD << "=";
				for(unsigned char x : enResult)
					fsConfig << std::setw(2)<<std::setfill('0') << std::hex << (int) x;
				fsConfig<< std::endl;
			}
			else
				fsConfig << x.first << "=" << x.second<< std::endl;
		}
		fsConfig.close();
	}
	else
	{
		return -1;
	}
	return 0;
}
