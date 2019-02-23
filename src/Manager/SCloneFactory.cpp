//
// Created by ounols on 19. 2. 13.
//

#include "SCloneFactory.h"
#include "SceneMgr.h"
#include "../Object/SScene.h"
#include <map>
#include <sstream>

SCloneFactory::SCloneFactory() {

}

SCloneFactory::~SCloneFactory() {

}

SComponent* SCloneFactory::Clone(SComponent* component, SGameObject* parent) {

    return nullptr;
}

SGameObject* SCloneFactory::Clone(SGameObject* object, SGameObject* parent) {
    if(parent == nullptr) {
        Scene* currentScene = SceneMgr::getInstance()->GetCurrentScene();
        if(!dynamic_cast<SScene*>(currentScene)) return nullptr;

        parent = static_cast<SScene*>(currentScene)->GetRoot();
    }

    SGameObject* cloneObject = new SGameObject(object->GetName() + "_copy");

    std::vector<SComponent*> target_component = cloneObject->GetComponents();
    std::map<const char*, SComponent*> clone_component;

    for(auto component : target_component) {
        std::stringstream id;
        id << "CSG0COMP" << component;
        clone_component[id.str().c_str()] = component->Clone();
    }

    return nullptr;
}


