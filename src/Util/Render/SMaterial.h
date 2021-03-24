//
// Created by ounols on 19. 6. 1.
//

#pragma once

#include <functional>
#include <unordered_map>

#include "../GLProgramHandle.h"
#include "../STypeDef.h"
#include "../Interface/TransformInterface.h"
#include "../Render/GLMeshID.h"

namespace CSE {

    class SMaterial : public SResource {
    private:
        struct Element {
            int id = HANDLE_NULL;
            SType type = SType::UNKNOWN;
            std::vector<std::string> value_str;
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

		void SetAttribute(const GLMeshID& meshId) const;

		void SetCameraUniform(mat4 camera, vec3 cameraPosition, mat4 projection, TransformInterface* transform) const;

		void SetSkinningUniform(const GLMeshID& mesh, const std::vector<mat4>& jointMatrix);

		void SetInt(std::string name, int value);

		void SetFloat(std::string name, float value);

		void SetVec3(std::string name, vec3 value);

		void SetTexture(std::string name, SResource* texture);

        short GetOrderLayer() const;

        void SetOrderLayer(int orderLayer);

        GLProgramHandle* GetHandle() const;

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        void ReleaseElements();

		void SetBindFuncByType(Element* element, bool isUniform);

        void SetIntFunc(Element* element, int value);
        void SetFloatFunc(Element* element, float value);
        void SetBoolFunc(Element* element, bool value);

        void SetMat4Func(Element* element, mat4 value);
        void SetMat3Func(Element* element, mat3 value);
        void SetMat2Func(Element* element, mat2 value);
        void SetVec4Func(Element* element, vec4 value);
        void SetVec3Func(Element* element, vec3 value);
        void SetVec2Func(Element* element, vec2 value);

        void SetTextureFunc(Element* element, SResource* texture);

    private:
        GLProgramHandle* m_handle = nullptr;
        short m_orderLayer = 5000;
        //std::vector<Element*> m_elements;
		std::unordered_map<std::string, Element*> m_elements;
		std::unordered_map<std::string, Element*> m_attributeElements;
		int m_textureLayout = 0;
    };
}