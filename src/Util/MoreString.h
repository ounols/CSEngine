#pragma once
#include <string>


std::string ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
	size_t start_pos = 0; //string ó������ �˻�
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from�� ã�� �� ���� ������
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // �ߺ��˻縦 ���ϰ� from.length() > to.length()�� ��츦 ���ؼ�
	}
	return str;
}
