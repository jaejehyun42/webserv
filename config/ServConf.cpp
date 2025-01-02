#include "ServConf.hpp"

ServConf::ServConf() {}

ServConf::ServConf(const ServConf& ref)
{
	*this = ref;
}

ServConf& ServConf::operator=(const ServConf& ref)
{
	if (this != &ref)
	{
		this->aliveTime = ref.aliveTime;
		this->mime = ref.mime;
		this->serv = ref.serv;
	}
	return (*this);
}

ServConf::~ServConf() {}

void ServConf::parse(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	if (!file)
		throw std::runtime_error("Error: Could not open file");

	std::string line;
	while (std::getline(file, line))
	{
		
	}
}