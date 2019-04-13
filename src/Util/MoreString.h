#pragma once

#include <string>


static std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0; //string 처음부터 검사
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
    }
    return str;
}

static std::string
ReplaceFunction(std::string& str, const std::string& from, const std::string& from2, const std::string& to,
                const std::string& to2) {
    size_t start_pos = 0; //string 처음부터 검사
    size_t start_pos2 = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서

        if ((start_pos2 = str.find(from2, start_pos)) != std::string::npos) {
            if (str.find(from, start_pos) < start_pos2) continue;

            str.replace(start_pos2, from2.length(), to2);
            start_pos = start_pos2 + to2.length();

        }
    }
    return str;
}

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

static std::vector<std::string> split(const std::string& s, char seperator) {
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != std::string::npos) {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

    return output;
}