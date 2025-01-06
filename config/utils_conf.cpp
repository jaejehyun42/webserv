#include "utils_conf.hpp"

vector<string> splitString(string& line)
{
	string	token;
	istringstream iss(line);
	vector<string> result;

	while (iss >> token)
		result.push_back(token);

	return (result);
}
