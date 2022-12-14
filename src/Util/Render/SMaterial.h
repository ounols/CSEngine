#pragma once

#include <functional>
#include <unordered_map>

#include "../GLProgramHandle.h"
#include "../STypeDef.h"
#include "../Interface/TransformInterface.h"
#include "../Render/GLMeshID.h"

namespace CSE {

    class LightMgr;

    class SMaterial : public SResource {
    public:
        enum SMaterialMode { NORMAL = 0, DEFERRED = 1, DEPTH_ONLY = 2 };
    private:
        struct Element {
            int id = HANDLE_NULL;
            SType type = SType::UNKNOWN;
            std::vector<std::string> valueStr;
			int count = 1;
            std::string raw;
        	
			std::function<void()> attachFunc = nullptr;
        };

        typedef std::unordered_map<std::string, Element*> ElementsMap;
    public:
        SMaterial();

        explicit SMaterial(const SMaterial* material);

        ~SMaterial() override;

        void Exterminate() override;

        void SetHandle(GLProgramHandle* handle);

        void AttachElement() const;

		void InitElements(const ElementsMap& elements, GLProgramHandle* handle);

		void SetAttribute(const GLMeshID& meshId) const;

		void SetInt(const std::string& name, int value);

		void SetFloat(const std::string& name, float value);

		void SetVec3(const std::string& name, const vec3& value);

		void SetTexture(const std::string& name, SResource* texture);

        short GetOrderLayer() const;

        void SetOrderLayer(int orderLayer);

        SMaterialMode GetMode() const;

        void SetMode(SMaterialMode mode);

        GLProgramHandle* GetHandle() const;

        GLProgramHandle* GetLightPassHandle() const;

        int GetTextureCount() const;

        std::string PrintMaterial() const;

        static SMaterial* GenerateMaterial(GLProgramHandle* handle);

    protected:
        void Init(const AssetMgr::AssetReference* asset) override;

    private:
        void ReleaseElements();

		void SetBindFuncByType(Element* element);

        static void SetIntFunc(Element* element, int value);
        static void SetFloatFunc(Element* element, float value);
        static void SetBoolFunc(Element* element, bool value);

        static void SetMat4Func(Element* element, mat4 value);
        static void SetMat3Func(Element* element, mat3 value);
        static void SetMat2Func(Element* element, mat2 value);
        static void SetVec4Func(Element* element, vec4 value);
        static void SetVec3Func(Element* element, vec3 value);
        static void SetVec2Func(Element* element, vec2 value);

        void SetTextureFunc(Element* element, SResource* texture);

    private:
        GLProgramHandle* m_handle = nullptr;
        GLProgramHandle* m_lightPassHandle = nullptr;
        short m_orderLayer = 5000;
        //std::vector<Element*> m_elements;
		ElementsMap m_elements;
		mutable int m_textureLayout = 0;
        int m_textureCount = 0;
        SMaterialMode m_mode = NORMAL;

        LightMgr* m_lightMgr = nullptr;

        std::string m_refHash;
    };
}