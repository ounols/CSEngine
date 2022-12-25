
#include "SShaderGroup.h"
#include "../GLProgramHandle.h"
#include "../Loader/XML/XML.h"

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
        root = XFILE(asset->path.c_str()).getRoot();
    }
    catch (int e) {
        return;
    }

    XNode cse_mat = root->getChild("CSESHADER");
}
