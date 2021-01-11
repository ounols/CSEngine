//
// Created by ounols on 19. 6. 1.
//

#pragma once

#include <functional>
#include <unordered_map>

#include "../GLProgramHandle.h"

namespace CSE {

    class SMaterial : public SResource {
    private:
        struct Element {
            int id = HANDLE_NULL;
            GLenum type = HANDLE_NULL;
            void* value = nullptr;
			int count = 1;
        	
			std::function<void()> attachFunc = nullptr;
        };
    public:
        SMaterial();

        ~SMaterial() override;

        void Exterminate() override;

        void SetHandle(GLProgramHandle* handle);

        void AttachElement() const;

		void InitElements();

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