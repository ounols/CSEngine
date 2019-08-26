//
// Created by ounols on 19. 4. 28.
//

#include "SSceneLoader.h"
#include "../../../Object/SScene.h"

#include <sstream>
#include <functional>
#ifndef __ANDROID__
#include <fstream>
#endif

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

    values += std::string("<gameobject name=\"") + obj->GetName() + "\" id=\"" + obj->GetID() + "\">\n";
    for(auto component : obj->GetComponents()) {
        values += component->PrintValue();
    }

    SGameObject* parent = obj->GetParent();
    values += std::string("<parent>") + ((parent == nullptr) ? "__ROOT_OF_SCENE__" : parent->GetID()) + "</parent>\n";

    values += "</gameobject>";

    for(auto child : obj->GetChildren()) {
        values += GetGameObjectValue(child);
    }

    return values;
}

bool SSceneLoader::SavePrefab(SGameObject* root, std::string path) {
    std::stringstream value;

    value << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    value << "<CSEPREFAB version = \"" << "1.0.0" << "\">\n";

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
