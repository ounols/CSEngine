#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <vector>


namespace CSE {

    static std::string ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = 0; //string ??????? ???
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from?? ??? ?? ???? ??????
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // ?????? ????? from.length() > to.length()?? ??? ?????
        }
        return str;
    }

    static std::string
    ReplaceFunction(std::string& str, const std::string& from, const std::string& from2, const std::string& to,
                    const std::string& to2) {
        size_t start_pos = 0; //string ??????? ???
        size_t start_pos2 = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from?? ??? ?? ???? ??????
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // ?????? ????? from.length() > to.length()?? ??? ?????

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

        std::string::size_type prev_pos = 0, pos = 0;

        while ((pos = s.find(seperator, pos)) != std::string::npos) {
            std::string substring(s.substr(prev_pos, pos - prev_pos));
            output.push_back(substring);
            prev_pos = ++pos;
        }

        output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

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

    static std::string ConvertSpaceStr(std::string str, bool SpaceNotChange = false) {
        auto n = str.find(' ');
        if (n == std::string::npos || SpaceNotChange) {
            n = str.find("$nbsp:");
            if (n != std::string::npos)
                return ReplaceAll(str, "$nbsp:", " ");
            return str;
        }
        return ReplaceAll(str, " ", "$nbsp:");
    }
}