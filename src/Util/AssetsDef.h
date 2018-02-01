#pragma once
#include <string>
#include <fstream>

namespace CSE {

	static std::string AssetsPath() {

		std::string path;
#ifdef _DEBUG
		path.append("../../../Assets/");
#endif

		return path;
	}

	static std::string OpenAssetsTxtFile(std::string path) {

		std::string buf;

#ifdef WIN32

		std::ifstream fin(path);

		if (!fin.is_open()) return nullptr;


		while(!fin.eof()) {
			std::string line;
			std::getline(fin, line);
			buf += line + '\n';
		}

#endif


		return buf;
	}

}
