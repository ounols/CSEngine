#pragma once

#include "../../Vector.h"
#include "../../Matrix.h"

#include <vector>
#include <string>

namespace CSE {

	class XMLParser
	{
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
	};

}
