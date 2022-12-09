//
// Created by ounols on 19. 4. 28.
//

#include "SSceneLoader.h"
#include "../../../Object/SScene.h"
#include "../../AssetsDef.h"
#include "../../MoreComponentFunc.h"
#include "../../../MacroDef.h"
#include "../../../Object/SPrefab.h"
#include "../../SafeLog.h"
#include "../../../Component/TransformComponent.h"

#include <sstream>
#include <functional>

#ifndef __ANDROID__

#include <fstream>
#include <iostream>
#include <utility>

#endif

using namespace CSE;

void Exploring(SGameObject* obj, int level = 0) {
	std::string str;

	for (int i = 0; i < level; i++) {
		str += "  ";
	}

	str += "ㄴ " + obj->GetName() + '\n';
	SafeLog::Log(str.c_str());

	for (auto child : obj->GetChildren()) {
		Exploring(child, level + 1);
	}
}

SSceneLoader::SSceneLoader() = default;

SSceneLoader::~SSceneLoader() = default;

bool SSceneLoader::SaveScene(SScene* scene, std::string path) {

	std::stringstream value;
	value << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	value << "<CSESCENE version = \"" << "1.0.0" << "\">\n";

	value << GetGameObjectValue(scene->GetRoot());

	value << "</CSESCENE>";

	return Save(value.str(), std::move(path));
}

std::string SSceneLoader::GetGameObjectValue(SGameObject* obj, bool ignorePrefab) {
	std::string values;

	if (obj->GetName() != "__ROOT_OF_SCENE__") {
		std::string resID = obj->GetResourceID();
		if (!ignorePrefab && !resID.empty()) {
			if (resID.find('*') == std::string::npos) {
				values += ComparePrefab(obj);
				return values;
			}
			//루트가 아닌 오브젝트
			//차후 추가바람
			return values;
		}

		values += PrintGameObject(obj);
	}

	for (auto child : obj->GetChildren()) {
		values += GetGameObjectValue(child, ignorePrefab);
	}

	return values;
}

bool SSceneLoader::SavePrefab(SGameObject* root, std::string path) {
	std::stringstream value;

	value << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	value << "<CSEPREFAB version=\"" << "1.0.0" << "\">\n";

	value << GetGameObjectValue(root);

	value << "</CSEPREFAB>";

	return Save(value.str(), std::move(path));
}

bool SSceneLoader::Save(const std::string& buf, const std::string& path) {
#ifndef __ANDROID__
	std::ofstream file(path);
	if (!file.is_open()) return false;
	file << buf;
	file.close();

	return true;
#endif

	return false;
}

SScene* SSceneLoader::LoadScene(const std::string& path) {
	const XNode* m_root;

	try {
		m_root = XFILE(path.c_str()).getRoot();
	}
	catch (int e) {
		return nullptr;
	}

	auto scene = new SScene();

	XNode sce_scene = m_root->getChild("CSESCENE");

	auto node_gameobjects = sce_scene.children;
	std::vector<NodeKey*> gameobjects;
	std::vector<ComponentValue*> components;

	for (const auto& node_obj : node_gameobjects) {
		if (node_obj.name == "gameobject") {
			ExploringScene(node_obj, gameobjects, components);
		}
		else if (node_obj.name == "prefab") {
			ExploringPrefab(node_obj, gameobjects, components, scene);
		}
	}

	LinkingID(gameobjects, scene->GetRoot());
	LinkingReference(components);

	for (auto gameobj : gameobjects) {
		SAFE_DELETE(gameobj);
	}

	for (auto comp : components) {
		SAFE_DELETE(comp);
	}

	Exploring(scene->GetRoot());
	SAFE_DELETE(m_root);

	return scene;
}

void SSceneLoader::ExploringScene(const XNode& node, std::vector<NodeKey*>& objs, std::vector<ComponentValue*>& comps) {
	std::string name = ConvertSpaceStr(node.getAttribute("name").value, true);
	auto node_components = node.children;
	std::string id = ConvertSpaceStr(node.getAttribute("id").value);
    std::string hash = node.getAttribute("hash").value;
	auto obj_new = new SGameObject(name);
    obj_new->SetHash(hash);

	for (const auto& comp : node_components) {
		if (comp.name != "component") continue;

		std::string comp_type = comp.getAttribute("type").value;
		bool comp_enable = comp.getAttribute("enable").value == "1";
		SComponent* component = (comp_type == "TransformComponent")
			                        ? static_cast<TransformComponent*>(obj_new->GetTransform())
			                        : MoreComponentFunc::CreateComponent(obj_new, comp_type);

		auto comp_val = new ComponentValue();
		comp_val->id = id + "?" + comp_type;
		comp_val->node = comp;
		comp_val->comp = component;
        component->SetIsEnable(comp_enable);

		comps.push_back(comp_val);
	}

	auto key = new NodeKey();
	key->node = node;
	key->id = id;
	key->obj = obj_new;
	key->hash = hash;

	objs.push_back(key);
}

void SSceneLoader::LinkingID(std::vector<NodeKey*>& objs, SGameObject* root) {
	std::vector<NodeKey*> remain;

	for (auto node_obj : objs) {
		auto obj = node_obj->obj;
		std::string parent_hash = ConvertSpaceStr(node_obj->node.getChild("parent").value);
		obj->RemoveParent();
		//parent_hash = parent_hash.substr(1, parent_hash.size() - 2);

		if (parent_hash == "__ROOT_OF_SCENE__") {
			if (root == nullptr)
				obj->SetParent(SGameObject::FindByID(parent_hash));
			else
				obj->SetParent(root);
			continue;
		}

		SGameObject* target = nullptr;
		for (auto temp : objs) {
			if (parent_hash == temp->hash) {
				target = temp->obj;
				break;
			}
		}

		if (target == nullptr) {
			remain.push_back(node_obj);
			continue;
		}
		//        if(target == nullptr) {
		//            target = SGameObject::FindByID(parent_hash);
		//            if(target == nullptr) {
		//                obj->SetParent(SGameObject::FindByID("__ROOT_OF_SCENE__"));
		//                continue;
		//            }
		//        }

		obj->SetParent(target);
	}

	for (auto node_obj : remain) {
		auto obj = node_obj->obj;
		obj->RemoveParent();
		std::string parent_hash = ConvertSpaceStr(node_obj->node.getChild("parent").value);

		auto parent = SGameObject::FindByHash(parent_hash);
		obj->SetParent(parent);
	}
}

void SSceneLoader::LinkingReference(std::vector<ComponentValue*>& comps) {
	for (auto comp : comps) {
		auto node = comp->node;
		for (const auto& value : node.children) {
			if (value.name != "value") continue;

			std::string v_name = value.getAttribute("name").value;
			auto v_values = value.value.toStringVector();

			for (int i = 0; i < v_values.size(); ++i) {
				v_values[i] = ConvertSpaceStr(v_values[i], true);
			}

			comp->comp->SetValue(v_name, v_values);
		}
	}
}

void SSceneLoader::ExploringPrefab(const XNode& node, std::vector<NodeKey*>& objs, std::vector<ComponentValue*>& comps,
                                   SScene* scene) {
	std::string name = ConvertSpaceStr(node.getAttribute("name").value, true);
	auto node_values = node.children;
	std::string id = ConvertSpaceStr(node.getAttribute("id").value, true);
	std::string file_id = ConvertSpaceStr(node.getAttribute("fileid").value, true);
	std::string hash = ConvertSpaceStr(node.getAttribute("hash").value, true);

	//Create!
	auto prefab = SResource::Create<SPrefab>(file_id);
	SGameObject* root = prefab->Clone(vec3(), scene->GetRoot());
    root->SetHash(hash);

	//Change objects value
	for (const auto& child : node_values) {
		if (child.name == "gameobject") {
			ExploringScene(child, objs, comps);
		}
		else if (child.name == "changegameobject") {
			LinkingResourceID(child, root, objs, comps);
		}
	}

	//    NodeKey* key = new NodeKey();
	//    key->node = node;
	//    key->id = id;
	//    key->obj = root;
	//
	//    objs.push_back(key);
}

void SSceneLoader::LinkingResourceID(const XNode& node, SGameObject* root, std::vector<NodeKey*>& objs,
                                     std::vector<ComponentValue*>& comps) {
	std::string obj_fileid = ConvertSpaceStr(node.getAttribute("fileid").value, true);

	if (root->GetResourceID() == obj_fileid) {
		std::string obj_id = ConvertSpaceStr(node.getAttribute("id").value, true);
		std::string obj_name = ConvertSpaceStr(node.getAttribute("name").value, true);
		std::string obj_hash = ConvertSpaceStr(node.getAttribute("hash").value, true);

		auto key = new NodeKey();
		key->obj = root;
		key->node = node;
		key->id = obj_id;
		key->hash = obj_hash;
		root->SetName(obj_name);
		objs.push_back(key);

		for (const auto& comp : node.children) {
			if (comp.name != "component") continue;

			std::string comp_type = comp.getAttribute("type").value;
			SComponent* component = nullptr;

			for (auto comp_obj : root->GetComponents()) {
				if (comp_type == comp_obj->GetClassType()) {
					component = comp_obj;
					break;
				}
			}

			if (component == nullptr)
				component = MoreComponentFunc::CreateComponent(root, comp_type);

			auto comp_val = new ComponentValue();
			comp_val->id = obj_id + "?" + comp_type;
			comp_val->node = comp;
			comp_val->comp = component;

			comps.push_back(comp_val);
		}
	}

	auto children = root->GetChildren();
	for (const auto& child : children) {
		LinkingResourceID(node, child, objs, comps);
	}
}

std::string SSceneLoader::ComparePrefab(SGameObject* obj) {
	const auto& prefab = SResource::Get<SPrefab>(obj->GetResourceID());
	if (prefab == nullptr) return "";

	std::string str_p = GetGameObjectValue(prefab->GetRoot(), true);
	std::string str_o = GetGameObjectValue(obj, true);
	std::string result;
	//    std::string addedObject_str;
	std::string absoluteID = obj->GetID();

	const XNode* n_p = XFILE().loadBuffer(str_p);
	const XNode* n_o = XFILE().loadBuffer(str_o);

	result += std::string("<prefab name=\"") + ConvertSpaceStr(obj->GetName()) + "\" id=\"" +
		ConvertSpaceStr(obj->GetID()) + "\" fileid=\"" + ConvertSpaceStr(obj->GetResourceID()) +
        "\" hash=\"" + obj->GetHash() +
		"\">\n";

	for (const auto& object_node : n_o->children) {
		bool isFind = false;
		XNode target_node;
		std::string obj_id;
		std::string obj_name = object_node.getAttribute("name").value;

		try {
			obj_id = object_node.getAttribute("fileid").value;
		}
		catch (int e) {
			//            std::string id = object_node.getAttribute("id").value;
			//            addedObject_str += PrintGameObject(SGameObject::FindByID(id));
			continue;
		}

		for (const auto& prefab_node : n_p->children) {
			std::string prefab_id = prefab_node.getAttribute("fileid").value;
			if (obj_id == prefab_id) {
				target_node = prefab_node;
				isFind = true;
				break;
			}
		}

		if (!isFind) continue;

		bool isChanged;

		//이름 확인
		isChanged = obj_name != target_node.getAttribute("name").value;
		//컴포넌트 갯수 확인
		isChanged = isChanged || object_node.children.size() != target_node.children.size();
		auto compare_o = str_o.substr(object_node.sub_index,
		                              object_node.getChild("parent").sub_index - object_node.sub_index);
		auto compare_p = str_p.substr(target_node.sub_index,
		                              target_node.getChild("parent").sub_index - target_node.sub_index);

		isChanged = isChanged || compare_o != compare_p;

		if (!isChanged) {
			std::string o_parent_resID;
			std::string p_parent_resID;
			std::string parent_o = object_node.getChild("parent").value;
			std::string parent_p = target_node.getChild("parent").value;

			for (const auto& obj_node : n_o->children) {
				if (obj_node.getAttribute("id").value == parent_o) {
					o_parent_resID = obj_node.getAttribute("fileid").value;
					break;
				}
			}

			for (const auto& prf_node : n_p->children) {
				if (prf_node.getAttribute("id").value == parent_p) {
					p_parent_resID = prf_node.getAttribute("fileid").value;
					break;
				}
			}

			if (o_parent_resID.empty()) isChanged = true;

			isChanged = isChanged || p_parent_resID != o_parent_resID;
		}

		if (isChanged) {
			std::string hash = object_node.getAttribute("hash").value;
			std::string str = PrintGameObject(SGameObject::FindByHash(hash));
			result += std::string("<change") + str.substr(1, str.size() - 14) + "</changegameobject>";
		}
	}

	//    result += addedObject_str;
	result += "</prefab>";

	SAFE_DELETE(n_p);
	SAFE_DELETE(n_o);
	return result;
}

std::string SSceneLoader::PrintGameObject(SGameObject* obj) {
    std::string values;

    // Pre-allocate space for the string to avoid costly re-allocations
    values.reserve(1024);

    const std::string& res_id = obj->GetResourceID();
    values += "<gameobject name=\"" + ConvertSpaceStr(obj->GetName()) + "\" id=\"" +
              ConvertSpaceStr(obj->GetID()) + "\" hash=\"" + obj->GetHash() + "\"";
    if (!res_id.empty()) {
        values += " fileid=\"" + res_id + "\"";
    }
    values += ">\n";

    const auto& components = obj->GetComponents();
    for (const auto& component : components) {
        values += component->PrintValue();
    }

    const SGameObject* parent = obj->GetParent();
    values += "<parent>" + ((parent == nullptr || parent->GetName() == "__ROOT_OF_SCENE__")
            ? "__ROOT_OF_SCENE__" : ConvertSpaceStr(parent->GetHash())) + "</parent>\n";

    values += "</gameobject>";

    return values;
}
