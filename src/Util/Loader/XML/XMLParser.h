#pragma once

#include "../../Vector.h"
#include "../../Matrix.h"

#include <vector>
#include <string>

#include "../../../Component/SComponent.h"
#include "../../Render/SMaterial.h"
#include "../../Render/STexture.h"

namespace CSE {

	class XMLParser {
	public:
		static int parseInt(const char* value);
		static float parseFloat(const char* value);
		static bool parseBool(const char* value);


		static vec2 parseVec2(std::vector<std::string> values);
		static vec3 parseVec3(std::vector<std::string> values);
		static vec4 parseVec4(std::vector<std::string> values);

		static mat2 parseMat2(std::vector<std::string> values);
		static mat3 parseMat3(std::vector<std::string> values);
		static mat4 parseMat4(std::vector<std::string> values);

		//Resources
		template <class TYPE>
		static TYPE* parseResources(const char* value);
		static STexture* parseTexture(const char* value);
		static SMaterial* parseMaterial(const char* value);

		//Engine Objects
		static SComponent* parseComponent(const char* value);
		static SGameObject* parseGameObject(const char* value);
	};

	template <class TYPE>
	TYPE* XMLParser::parseResources(const char* value) {
		TYPE* res = ResMgr::getInstance()->GetObjectById<TYPE>(value);
		return res;
	}

}
