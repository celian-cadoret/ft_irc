#include "main.hpp"

std::string toLowerStr( std::string s ) {
	std::string out;

	for (size_t i = 0; i < s.size(); i++) {
		out += s[i];
		if (out[i] >= 'A' && out[i] <= 'Z')
			out[i] += 32;
	}
	return out;
}

std::vector<std::string> splitStr( std::string s, char c ) {
	std::vector<std::string> out;
	size_t start = 0;

	while (start < s.size()&& s.find(c, start) != s.npos) {
		out.push_back(s.substr(start, s.find(c, start) - start));
		start += s.find(c, start) - start;
		while (start < s.size() && s[start] == c)
		    start++;
	}
	if (s[s.size() - 1] != c)
	    out.push_back(s.substr(start));
	return out;
}

bool isNickInVector( std::vector<std::string> v, std::string nick ) {
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++) {
		if (*it == nick)
			return true;
	}
	return false;
}