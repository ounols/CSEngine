
#include "SShaderGroup.h"
#include "../GLProgramHandle.h"
#include "../Loader/XML/XML.h"
#include "ShaderUtil.h"
#include "../../Manager/EngineCore.h"
#include "SMaterial.h"


using namespace CSE;

SShaderGroup::SShaderGroup() {

}

SShaderGroup::~SShaderGroup() {

}

void SShaderGroup::Exterminate() {

}

void SShaderGroup::Init(const AssetMgr::AssetReference* asset) {
    const XNode* root;

    try {
        root = XFILE(asset->name_path.c_str()).getRoot();
    }
    catch (int e) {
        SAFE_DELETE(root);
        return;
    }

    const auto& cse_mat = root->getChild("CSESHADER");
    const auto& cse_shaders = cse_mat.children;
    for (const auto& shader : cse_shaders) {
        const auto& pass = shader.getAttribute("pass").value;
        const auto& vert_path = shader.getAttribute("v").value;
        const auto& frag_path = shader.getAttribute("f").value;

        std::string&& vert = "";
        std::string&& frag = "";

        std::string hashes = "";

        if (shader.hasAttribute("localPath")
            && shader.getAttribute("localPath").value == "1") {
            const auto vert_asset_path = asset->path + vert_path;
            const auto frag_asset_path = asset->path + frag_path;
            vert = AssetMgr::LoadAssetFile(vert_asset_path);
            frag = AssetMgr::LoadAssetFile(frag_asset_path);
            hashes = GetShaderHash(vert_asset_path, frag_asset_path);
        } else {
            const auto& resMgr = CORE->GetCore(ResMgr);
            const auto& vert_asset = resMgr->GetAssetReference(vert_path);
            const auto& frag_asset = resMgr->GetAssetReference(frag_path);

            vert = AssetMgr::LoadAssetFile(vert_asset->name_path);
            frag = AssetMgr::LoadAssetFile(frag_asset->name_path);
            hashes = vert_asset->hash + frag_asset->hash;
        }

        if (vert.empty() || frag.empty() || hashes.empty()) continue;

        const auto& existHandle = SResource::Get<GLProgramHandle>(hashes);
        GLProgramHandle* handle = nullptr;
        if (existHandle == nullptr) {
            handle = ShaderUtil::CreateProgramHandle(vert.c_str(), frag.c_str());
            handle->SetName(hashes);
        } else {
            handle = existHandle;
        }
        m_handles.insert(std::pair<std::string, GLProgramHandle*>(pass, handle));
        if(pass == "geometry") m_geometryHandle = handle;
        if(pass == "forward") m_forwardHandle = handle;
    }

    SAFE_DELETE(root);
}

std::string SShaderGroup::GetShaderHash(const std::string& vert_path, const std::string& frag_path) {
    return AssetMgr::GetAssetHash(vert_path) + AssetMgr::GetAssetHash(frag_path);
}

const GLProgramHandle* SShaderGroup::GetHandle(const std::string& pass) const {
    return m_handles.at(pass);
}

const GLProgramHandle* SShaderGroup::GetHandleByMode(int mode) const {
    const std::unordered_map<SMaterial::SMaterialMode, std::string> pass_map = {
            {SMaterial::NORMAL,     "forward"},
            {SMaterial::DEFERRED,   "geometry"},
            {SMaterial::DEPTH_ONLY, "depthOnly"}
    };

    return m_handles.at(pass_map.at(static_cast<SMaterial::SMaterialMode>(mode)));
}
