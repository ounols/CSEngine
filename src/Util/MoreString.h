#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <vector>
#include <random>


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

    static std::vector<std::string> split(const std::string& s, char seperator) {
        std::vector<std::string> output;

        std::string::size_type prev_pos = 0;
        std::string::size_type pos = s.find(seperator);

        // 문자열을 순회하면서 새로운 단어를 찾음
        while (pos != std::string::npos) {
            // 새로운 단어를 추가
            output.push_back(s.substr(prev_pos, pos - prev_pos));

            // 다음 단어를 찾기 위해 위치를 업데이트
            prev_pos = pos + 1;
            pos = s.find(seperator, prev_pos);
        }

        // 마지막 단어를 추가
        output.push_back(s.substr(prev_pos, pos - prev_pos));

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

    template<typename T>
    std::string appandAll(std::stringstream& sstream, T param) {
        sstream << param;
        auto result = sstream.str();
        sstream.str(std::string());
        sstream.clear();

        return result;
    }

    template<typename T0, typename ... Tn>
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

    static std::string GetRandomHash(int size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 61); // 0~9, A~Z, a~z까지 숫자 생성

        std::string result;
        result.reserve(size);

        for (int i = 0; i < size; i++) {
            int num = dis(gen);
            if (num < 10) {
                // 0~9인 경우, 숫자 문자로 추가
                result += std::to_string(num);
            } else if (num < 36) {
                // 10~35인 경우, A~Z인 문자로 추가
                result += static_cast<char>(num + 'A' - 10);
            } else {
                // 36~61인 경우, a~z인 문자로 추가
                result += static_cast<char>(num + 'a' - 36);
            }
        }

        return result;
    }
}