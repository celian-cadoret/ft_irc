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
	size_t end = 0;
	bool new_start = false;

	for (end = 0; end < s.size() - 1; end++) {
		if (s[end] == c)
			continue;
		if (end < s.size() - 1 && s[end + 1] == c) {
			out.push_back(s.substr(start, end - start + 1));
			new_start = true;
		}
		if (end > 0 && s[end - 1] == c && new_start) {
			start = end;
			new_start = false;
		}
	}
	if (s[end] != c)
		out.push_back(s.substr(start, end - start + 1));
	return out;
}

bool isNickInVector( std::vector<std::string> v, std::string nick ) {
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); it++) {
		if (*it == nick)
			return true;
	}
	return false;
}