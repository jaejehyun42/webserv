#include "utils_conf.hpp"

vector<string> splitString(string& line)
{
	std::vector<std::string> result;
	std::string token;
	bool insideQuotes = false;
	bool escapeNext = false;
	std::string currentToken;

	for (size_t i = 0; i < line.size(); ++i)
	{
		char c = line[i];

		if (escapeNext)
		{
			currentToken += c;
			escapeNext = false;
		}
		else if (c == '\\')
		{
			escapeNext = true;
		}
		else if (c == '\"')
		{
			if (insideQuotes)
			{
				insideQuotes = false;
				result.push_back(currentToken);
				currentToken.clear();
			}
			else
				insideQuotes = true;
		}
		else if (std::isspace(c) && !insideQuotes)
		{
			if (!currentToken.empty())
			{
				result.push_back(currentToken);
				currentToken.clear();
			}
		} 
		else
			currentToken += c;
	}

	if (!currentToken.empty())
	{
		if (insideQuotes)
			throw std::runtime_error("Error: 닫히지 않은 큰따옴표가 있습니다.");
		if (currentToken == ";")
			result.back() += currentToken;
		else
			result.push_back(currentToken);
	}
	
	return result;
}
