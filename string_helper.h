#include <string>
#include <algorithm>

std::string static to_lower(std::string data)
{
	std::string newData = data;

	std::transform(newData.begin(), newData.end(), newData.begin(),
		[](unsigned char c) { return std::tolower(c); });

	return newData;
}