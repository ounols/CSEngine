//
// Created by ounols on 19. 2. 13.
//

#include "SCloneFactory.h"
#include "SceneMgr.h"
#include "../Object/SScene.h"
#include "../Component/TransformComponent.h"
#include "EngineCore.h"

#include <map>
#include <sstream>

using namespace CSE;

static void CloningObjects(SGameObject* object, std::map<SComponent*, SComponent*>& clone_comp,
                           std::map<SGameObject*, SGameObject*>& clone_obj, SGameObject* parent);


SCloneFactory::SCloneFactory() {

}

SCloneFactory::~SCloneFactory() {

}

SComponent* SCloneFactory::Clone(SComponent* component, SGameObject* parent) {
    if(parent == nullptr) {
        Scene* currentScene = CORE->GetCore(SceneMgr)->GetCurrentScene();
        if(!dynamic_cast<SScene*>(currentScene)) return nullptr;

        parent = static_cast<SScene*>(currentScene)->GetRoot();
    }

    SComponent* clone_component = component->Clone(parent);
    clone_component->CopyReference(component, std::map<SGameObject*, SGameObject*>(), std::map<SComponent*, SComponent*>());

    return clone_component;
}

SGameObject* SCloneFactory::Clone(SGameObject* object, SGameObject* parent) {
    if(parent == nullptr) {
        Scene* currentScene = CORE->GetCore(SceneMgr)->GetCurrentScene();
        if(!dynamic_cast<SScene*>(currentScene)) return nullptr;

        parent = static_cast<SScene*>(currentScene)->GetRoot();
    }


    std::map<SComponent*, SComponent*> clone_component;
    std::map<SGameObject*, SGameObject*> clone_object;

    CloningObjects(object, clone_component, clone_object, parent);

    SGameObject* cloneObject_root = clone_object[object];


    for(const auto& component_pair : clone_component) {
        SComponent* component = component_pair.second;
        component->CopyReference(component_pair.first, clone_object, clone_component);
    }

    return cloneObject_root;
}


void CloningObjects(SGameObject* object, std::map<SComponent*, SComponent*>& clone_comp,
                    std::map<SGameObject*, SGameObject*>& clone_obj, SGameObject* parent) {
    if(object == nullptr) return;

    //게임 오브젝트 복사
    SGameObject* cloneObject = new SGameObject(object->GetName() + "");
    clone_obj[object] = cloneObject;
    cloneObject->SetIsEnable(object->GetIsEnable());
    cloneObject->SetIsPrefab(object->isPrefab(true));
    cloneObject->SetResourceID(object->GetResourceID());

    //게임 컴포넌트 복사
    std::vector<SComponent*> target_component = object->GetComponents();
    for(const auto& component : target_component) {
        if(dynamic_cast<TransformComponent*>(component)) {
            TransformComponent* transform_copy = static_cast<TransformComponent*>(cloneObject->GetTransform());
            TransformInterface* transform_src = static_cast<TransformComponent*>(component);
            clone_comp[component] = transform_copy;

            transform_copy->m_position = vec3(transform_src->m_position);
            transform_copy->m_rotation = Quaternion(transform_src->m_rotation);
            transform_copy->m_scale = vec3(transform_src->m_scale);
            continue;
        }
        const auto& copy_comp = component->Clone(cloneObject);
        clone_comp[component] = copy_comp;
        copy_comp->SetIsEnable(component->GetIsEnable());
//        cloneObject->AddComponent(copy_comp);
    }

    auto children = object->GetChildren();
    for(const auto& child : children) {
        CloningObjects(child, clone_comp, clone_obj, cloneObject);
    }
    parent->AddChild(cloneObject);
}