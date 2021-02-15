#pragma once
#include <string>
#include <fstream>
#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <Manager/ResMgr.h>

#define  LOG_TAG    "SCENGINE"

#endif

namespace CSE {

	static std::string AssetsPath() {

		std::string path;
#if defined(_DEBUG) && defined(MSVC_CMAKE)
        path.append("../../../../Assets/");
#elif defined(_DEBUG) || defined(__linux__)
		path.append("../../../Assets/");
#endif
#ifdef __ANDROID__
        path = "";
#endif

		return path;
	}

	static std::string OpenAssetsTxtFile(std::string path) {

		std::string buf;

#if defined (WIN32) || defined(__linux__)

		std::ifstream fin(path, std::ios::binary);

		if (!fin.is_open()) return "";


		//while(!fin.eof()) {
		//	std::string line;
		//	std::getline(fin, line);
		//	buf += line + '\n';
		//}

		fin.seekg(0, std::ios::end);
		size_t size = fin.tellg();
		fin.seekg(0);
		buf = std::string(size, ' ');
		fin.read(&buf[0], size);
		fin.close();

#endif
#ifdef __ANDROID__
// get lists : https://stackoverflow.com/questions/13317387/how-to-get-file-in-assets-from-android-ndk
		AAssetManager* assetManager = RESMGR->GetAssetManager();
		AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_STREAMING);

		if(asset == nullptr){
			return "";
		}

		off_t fileSize = AAsset_getRemainingLength(asset);

		if(fileSize <= 0){

			return "";
		}

		char* strbuf = new char[fileSize + 1]();
		AAsset_read(asset, strbuf, fileSize);

		AAsset_close(asset);

		buf = strbuf;

		delete strbuf;


#endif


		return buf;
	}

}
