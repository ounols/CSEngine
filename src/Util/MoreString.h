#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <vector>


namespace CSE {

    static std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
        }
        return str;
    }

    static std::string
    ReplaceFunction(std::string& str, const std::string& from, const std::string& from2, const std::string& to,
                    const std::string& to2) {
        size_t start_pos = 0;
        size_t start_pos2 = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();

            if ((start_pos2 = str.find(from2, start_pos)) != std::string::npos) {
                if (str.find(from, start_pos) < start_pos2) continue;

                str.replace(start_pos2, from2.length(), to2);
                start_pos = start_pos2 + to2.length();

            }
        }
        return str;
    }

    static std::string trim(std::string str) {
        std::string r = str.erase(str.find_last_not_of(" \n\t\r") + 1);
        return r.erase(0, r.find_first_not_of(" \n\t\r"));
    }

    static std::vector<std::string> split(const std::string& s, char separator) {
        std::vector<std::string> output;

        size_t prev_pos = 0;
        size_t pos = 0;

        // 문자열을 일부분만 저장하고 나머지는 버퍼에 저장합니다.
        // 이렇게 하면 입력 문자열의 길이에 상관없이
        // 항상 정해진 크기의 버퍼를 사용하기 때문에 성능이 향상됩니다.
        constexpr size_t BUFFER_SIZE = 64;
        char buffer[BUFFER_SIZE];

        while ((pos = s.find(separator, pos)) != std::string::npos) {
            // 입력 문자열의 일부분을 출력 벡터에 저장합니다.
            output.push_back(s.substr(prev_pos, pos - prev_pos));
            prev_pos = ++pos;
        }

        // 나머지 문자열을 버퍼에 저장합니다.
        const size_t length = s.length() - prev_pos;
        s.copy(buffer, length, prev_pos);
        buffer[length] = '\0';

        // 버퍼에 저장된 나머지 문자열을 출력 벡터에 추가합니다.
        output.emplace_back(buffer);

        return output;
    }

    static void make_upper(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

    static void make_lower(std::string& str) {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }

    static std::string make_lower_copy(std::string _str) {
        std::string str(std::move(_str));
        std::transform(str.begin(), str.end(), str.begin(), tolower);
        return str;
    }

    template <typename T>
    std::string appandAll(std::stringstream& sstream, T param) {
        sstream << param;
        auto result = sstream.str();
        sstream.str(std::string());
        sstream.clear();

        return result;
    }

    template <typename T0, typename ... Tn>
    std::string appandAll(std::stringstream& sstream, T0 param0, Tn... paramN) {
        sstream << param0;
        return appandAll(sstream, paramN...);
    }

    static std::string ConvertSpaceStr(const std::string& str, bool SpaceNotChange = false) {
        std::string converted_str = str;

        if (SpaceNotChange) {
            // $nbsp: 문자열을 공백 문자로 대체
            size_t pos = 0;
            while ((pos = converted_str.find("$nbsp:", pos)) != std::string::npos) {
                converted_str.replace(pos, 6, " ");
                pos += 1;
            }
        } else {
            // 공백 문자를 $nbsp: 문자열로 대체
            size_t pos = 0;
            while ((pos = converted_str.find(' ', pos)) != std::string::npos) {
                converted_str.replace(pos, 1, "$nbsp:");
                pos += 6;
            }
        }

        return converted_str;
    }
}