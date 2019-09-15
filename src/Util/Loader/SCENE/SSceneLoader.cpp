//
// Created by ounols on 19. 4. 28.
//

#include "SSceneLoader.h"
#include "../../../Object/SScene.h"
#include "../../AssetsDef.h"
#include "../../MoreComponentFunc.h"
#include "../../../MacroDef.h"

#include <sstream>
#include <functional>
#ifndef __ANDROID__
#include <fstream>
#include <iostream>

#endif

void Exploring(SGameObject* obj, int level = 0) {

    for(int i = 0; i < level; i++) {
        std::cout << "  ";
    }

    std::cout << "ㄴ " << obj->GetName() << '\n';

    for(auto child : obj->GetChildren()) {
        Exploring(child, level+1);
    }

    return;

}

SSceneLoader::SSceneLoader() {

}

SSceneLoader::~SSceneLoader() {

}

bool SSceneLoader::SaveScene(SScene* scene, std::string path) {

    std::stringstream value;
    value << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    value << "<CSESCENE version = \"" << "1.0.0" << "\">\n";

    value << GetGameObjectValue(scene->GetRoot());

    value << "</CSESCENE>";

    return Save(value.str(), path);
}

std::string SSceneLoader::GetGameObjectValue(SGameObject* obj) {

    std::string values;

    if(obj->GetName() != "__ROOT_OF_SCENE__") {
        values += std::string("<gameobject name=\"") + ConvertSpaceStr(obj->GetName()) + "\" id=\"" + ConvertSpaceStr(obj->GetID()) + "\">\n";
        for(auto component : obj->GetComponents()) {
            values += component->PrintValue();
        }

        SGameObject* parent = obj->GetParent();
        values += std::string("<parent>") + ((parent == nullptr) ? "__ROOT_OF_SCENE__" : ConvertSpaceStr(parent->GetID())) + "</parent>\n";

        values += "</gameobject>";
    }

    for(auto child : obj->GetChildren()) {
        values += GetGameObjectValue(child);
    }

    return values;
}

bool SSceneLoader::SavePrefab(SGameObject* root, std::string path) {
    std::stringstream value;

    value << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    value << "<CSEPREFAB version=\"" << "1.0.0" << "\">\n";

    value << GetGameObjectValue(root);

    value << "</CSEPREFAB>";

    return Save(value.str(), path);
}



bool SSceneLoader::Save(std::string buf, std::string path) {
#ifndef __ANDROID__
    std::ofstream file(path);
    if(!file.is_open()) return false;
    file << buf;
    file.close();

    return true;
#endif

    return false;
}

SScene* SSceneLoader::LoadScene(std::string path) {
    const XNode* m_root;

    try {
        m_root = XFILE(path.c_str()).getRoot();
    }
    catch(int e) {
        return nullptr;
    }

    SScene* scene = new SScene();


    XNode sce_scene = m_root->getChild("CSESCENE");

    auto node_gameobjects = sce_scene.children;
    std::vector<NodeKey*> gameobjects;
    std::vector<ComponentValue*> components;


    for(auto node_obj : node_gameobjects) {
        if(node_obj.name == "gameobject") {
            ExploringScene(gameobjects, node_obj, components);
        }
    }

    LinkingID(gameobjects, scene->GetRoot());
    LinkingRefernece(components);

    for(auto gameobj : gameobjects) {
        SAFE_DELETE(gameobj);
    }

    for(auto comp : components) {
        SAFE_DELETE(comp);
    }

    Exploring(scene->GetRoot());
    return scene;

}

void SSceneLoader::ExploringScene(std::vector<NodeKey*>& objs, XNode node, std::vector<ComponentValue*>& comps) {

    std::string name = ConvertSpaceStr(node.getAttribute("name").value);
    std::cout << name << '\n';
    auto node_components = node.children;
    std::string id = ConvertSpaceStr(node.getAttribute("id").value);
    SGameObject* obj_new = new SGameObject(name);


    for(auto comp : node_components) {
        if(comp.name != "component") continue;

        std::string comp_type = comp.getAttribute("type").value;
        SComponent* component = MoreComponentFunc::CreateComponent(obj_new, comp_type);

        ComponentValue* comp_val = new ComponentValue();
        comp_val->id = id + "?" + comp_type;
        comp_val->node = comp;
        comp_val->comp = component;

        comps.push_back(comp_val);
    }


    NodeKey* key = new NodeKey();
    key->node = node;
    key->id = id;
    key->obj = obj_new;

    objs.push_back(key);

}

void SSceneLoader::LinkingID(std::vector<NodeKey*>& objs, SGameObject* root) {

    for(auto node_obj : objs) {

        auto obj = node_obj->obj;
        std::string parent_id = ConvertSpaceStr(node_obj->node.getChild("parent").value);
        //parent_id = parent_id.substr(1, parent_id.size() - 2);

        if(parent_id == "__ROOT_OF_SCENE__") {
            if(root == nullptr)
                obj->SetParent(SGameObject::FindByID(parent_id));
            else
                obj->SetParent(root);
            continue;
        }

        SGameObject* target = nullptr;
        for(auto temp : objs) {
            if(parent_id == temp->id) {
                target = temp->obj;
                break;
            }
        }

        obj->SetParent(target);

    }

}

void SSceneLoader::LinkingRefernece(std::vector<ComponentValue*>& comps) {

    for(auto comp : comps) {
        auto node = comp->node;
        for(auto value : node.children) {
            if(value.name != "value") continue;

            std::string v_name = value.getAttribute("name").value;
            auto v_values = value.value.toStringVector();

            comp->comp->SetValue(v_name, v_values);
        }
    }

    return;
}
