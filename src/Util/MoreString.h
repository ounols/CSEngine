#pragma once
#include <string>


std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0; //string 처음부터 검사
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
	}
	return str;
}

std::string ReplaceFunction(std::string& str, const std::string& from, const std::string& from2, const std::string& to, const std::string& to2) {
	size_t start_pos = 0; //string 처음부터 검사
	size_t start_pos2 = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서

		if((start_pos2 = str.find(from2, start_pos)) != std::string::npos) {
			if(str.find(from, start_pos) < start_pos2) continue;

			str.replace(start_pos2, from2.length(), to2);
			start_pos = start_pos2 + to2.length();

		}
	}
	return str;
}