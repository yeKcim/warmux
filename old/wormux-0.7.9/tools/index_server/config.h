#ifndef CONFIG_H
#define CONFIG_H
#include <map>
#include <string>

const int VERSION = 1;

class Config
{
	std::map<std::string, std::string> str_value;
	std::map<std::string, int> int_value;
	std::map<std::string, bool> bool_value;

	void SetDefault(const std::string & name, const std::string & value);
	void SetDefault(const std::string & name, const int & value);
	void SetDefault(const std::string & name, const bool & value);

	void Load();
	void Display();
public:
	Config();

	bool Get( const std::string & name, 
                        std::string & value);
	bool Get( const std::string & name, 
                        int & value);
	bool Get( const std::string & name, 
                        bool & value);
};

extern Config config;

#endif
