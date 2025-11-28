//
// Created by ounols on 19. 10. 27.
//

#include "SScriptObject.h"


#include <iostream>
#include <regex>
#include "sqrat.h"
#include "../Manager/ScriptMgr.h"
#include "sqrat/sqratVM.h"
#include "../Util/AssetsDef.h"
#include "../Util/MoreString.h"
#include "../Util/SafeLog.h"

using namespace CSE;
using namespace Sqrat;

RESOURCE_CONSTRUCTOR(SScriptObject) {}

SScriptObject::~SScriptObject() = default;

void SScriptObject::Exterminate() {

}

void SScriptObject::Init(const AssetMgr::AssetReference* asset) {
    RemakeScript(asset->name_path);
}

void SScriptObject::RegisterScript(const std::string& script) {
    const auto& script_mgr = CORE->GetCore(ScriptMgr);
    HSQUIRRELVM vm = script_mgr->GetVM();

    //register script
    if (!script.empty()) {
        Script compiledScript;
        compiledScript.CompileString(script);
        if (Sqrat::Error::Occurred(vm)) {
            const auto& errorMsg = Error::Message(vm);
            SafeLog::LogErrf(512, "[SScriptObject] Compile Failed - Script: %s, Error: %s",
                         m_className.empty() ? "Unknown" : m_className.c_str(),
                         errorMsg.c_str());
#ifdef __ANDROID__
//            LOGE("Compile Failed : %s", Error::Message(vm).c_str());
#else
            std::cout << "Compile Failed : " << Error::Message(vm) << '\n';
#endif
            return; // Don't try to run if compilation failed
        }

        compiledScript.Run();
        if (Sqrat::Error::Occurred(vm)) {
            const auto& errorMsg = Error::Message(vm);
            SafeLog::LogErrf(512, "[SScriptObject] Run Failed - Script: %s, Error: %s",
                         m_className.empty() ? "Unknown" : m_className.c_str(),
                         errorMsg.c_str());
#ifdef __ANDROID__
//            LOGE("Run Failed : %s", Error::Message(vm).c_str());
#else
            std::cout << "Run Failed : " << Error::Message(vm) << '\n';
#endif
            return;
        }

        compiledScript.Release();
        SafeLog::LogInfof("[SScriptObject] Script registered successfully - Class: %s",
                     m_className.c_str());
    } else {
        SafeLog::LogWarn("[SScriptObject] RegisterScript called with empty script content");
    }
}

void SScriptObject::RemakeScript(const std::string& path) {
    SafeLog::LogInfof("[SScriptObject] Loading script from path: %s", path.c_str());
    
    std::string script_str = AssetMgr::LoadAssetFile(path);
    
    if (script_str.empty()) {
        SafeLog::LogErrf(512, "[SScriptObject] ERROR: Script file is empty or failed to load - Path: %s",
                     path.c_str());
        return;
    }
    
    SafeLog::LogInfof("[SScriptObject] Script loaded successfully - Size: %zu bytes",
                 script_str.size());

    GetVariables(script_str);
    
    if (!m_className.empty()) {
        SafeLog::LogInfof("[SScriptObject] Class name extracted: %s", m_className.c_str());
    } else {
        SafeLog::LogWarnf(512, "[SScriptObject] WARNING: Could not extract class name from script - Path: %s",
                     path.c_str());
    }

    //replace GetComponent function
    script_str = ReplaceFunction(script_str, "GetComponent<", ">()", "GetComponent_", "_()");

    //replace GetCustomComponent(GetClass) function
    script_str = ReplaceFunction(script_str, "GetClass<", ">()", "GetClass(\"", "\")");

    RegisterScript(script_str);
}

void SScriptObject::GetVariables(const std::string& str) {
    // Step 1: 주석 제거
    std::string code = str;

    // 블록 주석 제거 (/* ... */)
    std::regex blockCommentRegex(R"(/\*[\s\S]*?\*/)");
    code = std::regex_replace(code, blockCommentRegex, "");

    // 라인 주석 제거 (// ...)
    std::regex lineCommentRegex(R"(//[^\n]*)");
    code = std::regex_replace(code, lineCommentRegex, "");

    // Step 2: 클래스명 추출
    // "class ClassName extends Parent" 또는 "class ClassName {"
    std::regex classRegex(R"(class\s+(\w+)(?:\s+extends\s+\w+)?\s*\{)");
    std::smatch classMatch;
    if (std::regex_search(code, classMatch, classRegex)) {
        m_className = classMatch[1].str();
    }

    // Step 3: 클래스 본문 추출 (첫 번째 레벨의 중괄호 내부)
    size_t classBodyStart = code.find('{');
    if (classBodyStart == std::string::npos) return;

    int braceLevel = 1;
    size_t classBodyEnd = classBodyStart + 1;

    for (; classBodyEnd < code.size() && braceLevel > 0; ++classBodyEnd) {
        if (code[classBodyEnd] == '{') braceLevel++;
        else if (code[classBodyEnd] == '}') braceLevel--;
    }

    std::string classBody = code.substr(classBodyStart + 1, classBodyEnd - classBodyStart - 2);

    // Step 4: 멤버 변수 추출
    // 패턴: "변수명 = 값;" 또는 "변수명;" (function, constructor, static 제외)
    // 중첩 블록(함수 본문 등) 제거
    std::string cleanBody;
    braceLevel = 0;
    for (const char& c : classBody) {
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
        } else if (braceLevel == 0) {
            cleanBody += c;
        }
    }

    // 변수 선언 패턴 매칭: 식별자 = 값; 또는 식별자;
    // function, constructor, static 키워드로 시작하는 것은 제외
    std::regex varRegex(R"((?:^|;|\n)\s*(?!function\b|constructor\b|static\b)(\w+)\s*(?:=[^;]*)?;)");

    std::sregex_iterator it(cleanBody.begin(), cleanBody.end(), varRegex);

    for (std::sregex_iterator end; it != end; ++it) {
        std::string varName = trim((*it)[1].str());
        if (!varName.empty()) {
            m_variables.push_back(varName);
        }
    }
}

std::vector<std::string> SScriptObject::GetVariables() const {
    return m_variables;
}

void SScriptObject::SetValue(const std::string& name_str, const Arguments& value) {
}

string SScriptObject::PrintValue() const {
    return {};
}
