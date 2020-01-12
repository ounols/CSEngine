//
// Created by ounols on 19. 10. 27.
//

#pragma once

#include "SResource.h"

namespace CSE {

    class SScriptObject : public SResource {
    public:
        SScriptObject();

        ~SScriptObject();

        void Exterminate() override;

        std::vector<std::string> GetVariables() const;

        std::string GetClassName() const {
            return m_className;
        }

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        void RegisterScript(std::string script);
        void RemakeScript(std::string path);
        void GetVariables(std::string str);

    private:
        std::vector<std::string> m_variables;
        std::string m_className = "";
    };

}