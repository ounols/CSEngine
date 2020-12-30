//
// Created by ounols on 19. 4. 28.
//

#pragma once

#include <string>
#include "../../../Object/SScene.h"
#include "../XML/XML.h"

namespace CSE {

    class SSceneLoader {
    private:
        struct NodeKey {
            XNode node;
            std::string id;
            SGameObject* obj;
        };

        struct ComponentValue {
            XNode node;
            std::string id;
            SComponent* comp;
        };
    public:
        SSceneLoader();

        ~SSceneLoader();

        static SScene* LoadScene(std::string path);

        static bool SaveScene(SScene* scene, std::string path);

        static bool SavePrefab(SGameObject* root, std::string path);

    private:
        static void ExploringScene(XNode node, std::vector<NodeKey*>& objs, std::vector<ComponentValue*>& comps);

        static void LinkingID(std::vector<NodeKey*>& objs, SGameObject* root);

        static void LinkingReference(std::vector<ComponentValue*>& comps);

        static void ExploringPrefab(XNode node, std::vector<NodeKey*>& objs, std::vector<ComponentValue*>& comps,
                                    SScene* scene);

        static void LinkingResourceID(XNode node, SGameObject* root, std::vector<NodeKey*>& objs,
                                      std::vector<ComponentValue*>& comps);

        static std::string GetGameObjectValue(SGameObject* obj, bool ignorePrefab = false);

        static std::string ComparePrefab(SGameObject* obj);

        static std::string PrintGameObject(SGameObject* obj);

        static bool Save(std::string buf, std::string path);
    };
}