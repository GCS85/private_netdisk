#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <fstream>
#include <map>
#include "encrypt.h"

#define RSA_LEN 128


#define ACCOUNT		"Account"
#define PASSWORD	"PassWord"
#define PRIDISK		"priDisk"
#define	LOCALPROT	"localPort"

class configFile
{
public:
	enum{
		CONF_NOCOMPLETE = -3,
		FILE_ERROR = -2,
		FILE_NOEXIST = -1,
		FILE_EXIST = 0
	};
	configFile(const std::string &configFP = "/home/libo/priDisk.conf" , const std::string &desK = "priDiskDes");
	std::string configFilePath;
	std::map<std::string,std::string> configKV;

	int analyze_config_file();
	int flush_file();
private:
	des_encrypt desEn;
};

#endif // CONFIGFILE_H
