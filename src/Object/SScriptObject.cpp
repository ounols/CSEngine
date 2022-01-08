//
// Created by ounols on 19. 10. 27.
//

#include "SScriptObject.h"


#include <iostream>
#include "sqrat.h"
#include "sqrat/sqratVM.h"
#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"

using namespace CSE;
using namespace Sqrat;

SScriptObject::SScriptObject() = default;

SScriptObject::~SScriptObject() = default;

void SScriptObject::Exterminate() {

}

void SScriptObject::Init(const AssetMgr::AssetReference* asset) {
    RemakeScript(asset->path);
}

void SScriptObject::RegisterScript(const std::string& script) {
    HSQUIRRELVM vm = DefaultVM::Get();

    //register script
    if (!script.empty()) {
        Script compiledScript;
        compiledScript.CompileString(script);
        if (Sqrat::Error::Occurred(vm)) {
#ifdef __ANDROID__
//            LOGE("Compile Failed : %s", Error::Message(vm).c_str());
#else
            std::cout << "Compile Failed : " << Error::Message(vm) << '\n';
#endif
        }

        compiledScript.Run();
        if (Sqrat::Error::Occurred(vm)) {
#ifdef __ANDROID__
//            LOGE("Run Failed : %s", Error::Message(vm).c_str());
#else
            std::cout << "Run Failed : " << Error::Message(vm) << '\n';
#endif
        }

        compiledScript.Release();
    }
}

void SScriptObject::RemakeScript(const std::string& path) {
    std::string script_str = AssetMgr::LoadAssetFile(path);

    GetVariables(script_str);

    //replace GetComponent function
    script_str = ReplaceFunction(script_str, "GetComponent<", ">()", "GetComponent_", "_()");

    //replace GetCustomComponent(GetClass) function
    script_str = ReplaceFunction(script_str, "GetClass<", ">()", "GetClass(\"", "\")");

    RegisterScript(script_str);
}

void SScriptObject::GetVariables(const std::string& str) {

    auto split_line = split(str, '\n');
    int level = -1;
    bool isComment = false;

    //split line
    for(auto line : split_line) {
		line = trim(line);
        if(line.find("//") != std::string::npos) continue;
        if(isComment && line.find("*/") != std::string::npos) {
            isComment = false;
            line = line.substr(line.find("*/") + 2);
        }
        else if(isComment || line.find("/*") != std::string::npos) {
            if(isComment) continue;

            isComment = true;
            line = line.substr(0, line.find("/*"));
        }

        //split start of brace
        bool isBraceStart = line.find('{') != std::string::npos;
        auto split_bs = split(line, '{');


        for(const auto& bs : split_bs) {

            if(level == 0 && m_className.empty()) {
                if(line.find("class") != std::string::npos) {
                    int start_index = line.find("class") + 5;
                    int end_index = line.find("extends", start_index) - 5;

                    m_className = trim(line.substr(start_index, end_index));
                }
            }

            //split end of brace
            bool isBraceEnd = line.find('}') != std::string::npos;
            auto split_be = split(bs, '}');

            for(const auto& be : split_be) {

                if(level == 1) {
                    //split semicolon
                    auto split_end = split(be, ';');
                    for(auto result : split_end) {
                        if(result.empty()) continue;
                        result = trim(result);

                        if(result.find("function") != std::string::npos) continue;
                        int index = result.find('=');
                        m_variables.push_back(trim(result.substr(0, index)));
                    }
                }

                if(isBraceEnd) level--;
            }

            if(isBraceStart) level++;
        }

    }

}

std::vector<std::string> SScriptObject::GetVariables() const {
    return m_variables;
}
