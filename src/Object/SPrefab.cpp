//
// Created by ounols on 19. 2. 10.
//

#include "../Manager/SCloneFactory.h"
#include "../Manager/SPrefabContainer.h"
#include "../Manager/ResMgr.h"
#include "SPrefab.h"

SPrefab::SPrefab() {
    ResMgr::getInstance()->Register<SPrefabContainer, SPrefab>(this);

}

SPrefab::~SPrefab() {

}

void SPrefab::Exterminate() {

}

SGameObject* SPrefab::Clone(vec3 position, SGameObject* parent) {

    SGameObject* clone = SCloneFactory::Clone(m_root, parent);

    clone->GetTransform()->m_position = vec3(position);
    clone->SetIsPrefab(false);
    clone->SetUndestroyable(false);

    return clone;

}


SGameObject* SPrefab::Clone(vec3 position, vec3 scale, Quaternion rotation, SGameObject* parent) {
    SGameObject* clone = SCloneFactory::Clone(m_root, parent);

    clone->GetTransform()->m_position = vec3(position);
    clone->GetTransform()->m_scale = vec3(scale);
    clone->GetTransform()->m_rotation = Quaternion(rotation);
    clone->SetIsPrefab(false);

    return clone;
}

bool SPrefab::SetGameObject(SGameObject* obj) {
    if(m_root != nullptr) return false;

    m_root = obj;
    m_root->SetUndestroyable(true);
    m_root->SetIsPrefab(true);

    return true;
}

