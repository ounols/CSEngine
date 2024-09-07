//
// Created by ounols on 19. 10. 27.
//

#pragma once

#include "SResource.h"
#include <vector>

namespace CSE {

    class SScriptObject : public SResource {
    public:
        RESOURCE_DEFINE_CONSTRUCTOR(SScriptObject);
        ~SScriptObject() override;

        void Exterminate() override;

        std::vector<std::string> GetVariables() const;

        std::string GetScriptClassName() const {
            return m_className;
        }

        void SetValue(std::string name_str, Arguments value) override;

        std::string PrintValue() const override;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        void RegisterScript(const std::string& script);
        void RemakeScript(const std::string& path);
        void GetVariables(const std::string& str);

    private:
        std::vector<std::string> m_variables;
        std::string m_className;
    };

}