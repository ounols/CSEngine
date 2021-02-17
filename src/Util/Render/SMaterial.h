//
// Created by ounols on 19. 6. 1.
//

#pragma once

#include <functional>
#include <unordered_map>

#include "../GLProgramHandle.h"
#include "../STypeDef.h"

namespace CSE {

    class SMaterial : public SResource {
    private:
        struct Element {
            int id = HANDLE_NULL;
            SType type = SType::UNKNOWN;
            std::vector<std::string> value;
			int count = 1;
        	
			std::function<void()> attachFunc = nullptr;
        };
    public:
        SMaterial();

        SMaterial(const SMaterial* material);

        ~SMaterial() override;

        void Exterminate() override;

        void SetHandle(GLProgramHandle* handle);

        void AttachElement() const;

		void InitElements();

		void SetElements(std::string element_name, std::vector<std::string> value);

		std::vector<std::string> GetElements(std::string element_name) const;

    private:
        void ReleaseElements();

		static std::function<void()> SetBindFuncByType(Element* element, bool isUniform);

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;
    private:
        GLProgramHandle* m_handle = nullptr;
        //std::vector<Element*> m_elements;
		std::unordered_map<std::string, Element*> m_elements;
    };
}