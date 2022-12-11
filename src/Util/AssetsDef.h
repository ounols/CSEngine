#pragma once
#include <string>
#include <fstream>

#include "Settings.h"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <Manager/EngineCore.h>
#include <Manager/ResMgr.h>

#define  LOG_TAG    "SCENGINE"

#endif

namespace CSE {
	static std::string NativeAssetsPath() {
		std::string path;
#if defined(MSVC_CMAKE)
        path.append("../../../../Assets/");
#elif defined(_WIN32) || defined(__linux__) || defined(__APPLE_CC__)
		path.append("../../../Assets/");
#endif
#ifdef __ANDROID__
        path = "";
#endif
		return path;
	}

	static std::string AssetsPath() {
        std::string path;

        if(Settings::IsAssetsPacked() == false)
            path = NativeAssetsPath();

        return path;
	}

	static std::string OpenNativeAssetsTxtFile(const std::string& path) {

		std::string buf;
#ifdef __ANDROID__
// get lists : https://stackoverflow.com/questions/13317387/how-to-get-file-in-assets-from-android-ndk
		AAssetManager* assetManager = CORE->GetCore(ResMgr)->GetAssetManager();
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
		buf = std::string(strbuf, fileSize);
		delete[] strbuf;
#elif defined (_WIN32) || defined(__linux__) || defined(__EMSCRIPTEN__) || defined(__APPLE_CC__)
		std::ifstream fin(path, std::ios::binary);
		if (!fin.is_open()) return "";

		fin.seekg(0, std::ios::end);
		size_t size = fin.tellg();
		fin.seekg(0);
		buf = std::string(size, ' ');
		fin.read(&buf[0], size);
		fin.close();
#endif
		return buf;
	}

    static bool SaveTxtFile(const std::string& path, std::string data) {
#ifdef __ANDROID__
        return true;
#endif
        std::ofstream fout(path, std::ios::binary | std::ios::trunc);
        fout << data;
        return fout.good();
    }
}
