#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include "../../src/Manager/AssetMgr.h"

namespace CSE {
    class STexture;
}

namespace CSEditor {

    struct APIResponse;

    /**
     * @brief Backend logic for Asset operations
     *
     * Handles asset listing, folder navigation, and preview loading.
     * Can be used by both REST API and Editor GUI.
     */
    class AssetBackend {
    public:
        using AssetsVector = std::vector<CSE::AssetMgr::AssetReference*>;
        using AssetMap = std::unordered_map<std::string, AssetsVector>;

        static AssetBackend& GetInstance();

        AssetBackend(const AssetBackend&) = delete;
        AssetBackend& operator=(const AssetBackend&) = delete;

        // ============================================
        // Direct Operations (sync, for GUI use)
        // ============================================

        /**
         * @brief Refresh asset map from resource manager
         * @return Map of folder paths to asset references
         */
        AssetMap RefreshAssetsDirect();

        /**
         * @brief Get assets in a specific folder
         * @param folderPath Path to the folder
         * @param assetMap Asset map (from RefreshAssetsDirect)
         * @return Pointer to assets vector or nullptr if not found
         */
        const AssetsVector* GetFolderAssetsDirect(const std::string& folderPath, const AssetMap& assetMap);

        /**
         * @brief Parse folder path into breadcrumb components
         * @param currentPath Current folder path
         * @param basePath Base path (e.g., "Assets/")
         * @return Vector of path components
         */
        std::vector<std::string> ParsePathComponentsDirect(const std::string& currentPath, const std::string& basePath);

        /**
         * @brief Load texture preview for an asset
         * @param asset Asset reference to load preview for
         * @return Texture ID as void* or nullptr if not applicable
         */
        void* LoadAssetPreviewDirect(CSE::AssetMgr::AssetReference* asset);

        /**
         * @brief Release a loaded preview texture
         * @param asset Asset reference with loaded preview
         */
        void ReleaseAssetPreviewDirect(CSE::AssetMgr::AssetReference* asset);

        /**
         * @brief Get preview texture ID if already loaded
         * @param asset Asset reference
         * @return Texture ID as void* or nullptr if not loaded
         */
        void* GetLoadedPreviewDirect(CSE::AssetMgr::AssetReference* asset);

    private:
        AssetBackend() = default;
        ~AssetBackend() = default;
    };

}
