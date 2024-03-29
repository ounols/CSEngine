#pragma once

#include "../../Vector.h"
#include "../../Matrix.h"

#include <vector>
#include <string>

#include "../../../Component/SComponent.h"
#include "../../STypeDef.h"
#include "../../../Manager/EngineCore.h"
#include "../../../Manager/ResMgr.h"

namespace CSE {

    class STexture;
    class SFrameBuffer;
    class SMaterial;

	class XMLParser {
	public:
		static void parse(std::vector<std::string> values, void* dst, SType type);
		
		static int parseInt(const char* value);
		static float parseFloat(const char* value);
		static bool parseBool(const char* value);


		static vec2 parseVec2(std::vector<std::string> values);
		static vec3 parseVec3(std::vector<std::string> values);
		static vec4 parseVec4(std::vector<std::string> values);

//		static mat2 parseMat2(std::vector<std::string> values);
//		static mat3 parseMat3(std::vector<std::string> values);
//		static mat4 parseMat4(std::vector<std::string> values);

		//Resources
		template <class TYPE>
		static TYPE* parseResources(const char* value);
		static STexture* parseTexture(const char* value);
        static SFrameBuffer* parseFrameBuffer(const char* value);
		static SMaterial* parseMaterial(const char* value);

		//Engine Objects
		static SComponent* parseComponent(const char* value);
		static SGameObject* parseGameObject(const char* value);

		static SType GetType(std::string type);
		static SType GetType(unsigned int type);

        static std::string ToString(SType type);
	};

	template <class TYPE>
	TYPE* XMLParser::parseResources(const char* value) {
        TYPE* res = CORE->GetCore(ResMgr)->GetObjectByHash<TYPE>(value);
		return res;
	}

}
