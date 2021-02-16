//
// Created by ounols on 19. 6. 1.
//

#include "SMaterial.h"

#include "../AssetsDef.h"
#include "../Loader/XML/XML.h"
#include "../Loader/XML/XMLParser.h"

using namespace CSE;

SMaterial::SMaterial() {

}


SMaterial::SMaterial(const SMaterial* material) {

    if(material == nullptr) throw -1;

    for(const auto element_pair : material->m_elements) {
        auto element_src = element_pair.second;
        Element* element_copy = new Element;
        element_copy->count = element_src->count;
        element_copy->type = element_src->type;
        element_copy->value = element_src->value;
        m_elements.insert(std::pair<std::string, Element*>(element_pair.first, element_copy));
    }
}

SMaterial::~SMaterial() {

}

void SMaterial::Exterminate() {
    ReleaseElements();
}

void SMaterial::ReleaseElements() {
    //for (Element* element : m_elements) {
    //    SAFE_DELETE(element);
    //}
	for (const auto pair : m_elements) {
		auto* element = pair.second;
		SAFE_DELETE(element);
	}
    m_elements.clear();
}

void SMaterial::SetHandle(GLProgramHandle* handle) {
    m_handle = handle;
    ReleaseElements();

	InitElements();
}

void SMaterial::AttachElement() const {

	for (const auto element_pair : m_elements) {
		const auto* element = element_pair.second;
		element->attachFunc();
	}

}

void SMaterial::InitElements() {

	for (auto element_pair : m_elements) {
		const auto element_name = element_pair.first.c_str();
		auto* element = element_pair.second;
		if (element->attachFunc != nullptr) continue;

		bool isUniform = true;
		auto* handleElement = m_handle->UniformLocation(element_name);
		if (handleElement == nullptr) {
			handleElement = m_handle->AttributeLocation(element_name);	isUniform = false;
		}
		if (handleElement == nullptr) continue;

		element->id = handleElement->id;
		element->attachFunc = SetBindFuncByType(element, isUniform);
	}
	
}

void SMaterial::SetElements(std::string element_name, void* value) {
    auto find_iter = m_elements.find(element_name);
    if(find_iter == m_elements.end()) return;

    find_iter->second->value = value;
}

void* SMaterial::GetElements(std::string element_name) const {
    auto find_iter = m_elements.find(element_name);
    if(find_iter == m_elements.end()) return nullptr;
    return find_iter->second->value;
}

std::function<void()> SMaterial::SetBindFuncByType(Element* element, bool isUniform) {

	const GLenum type = element->type;

	if(isUniform == false) {
		switch (type) {
		case SType::VEC2:
			return [element]() { glVertexAttrib2fv(element->id, ((vec2&)element->value).Pointer()); };
		case SType::VEC3:
			return [element]() { glVertexAttrib3fv(element->id, ((vec3&)element->value).Pointer()); };
		case SType::VEC4:
			return [element]() { glVertexAttrib4fv(element->id, ((vec4&)element->value).Pointer()); };
		}
		return nullptr;
	}

	switch (type) {
	case SType::FLOAT:
		return [element]() { glUniform1f(element->id, (float&)element->value); };
	case SType::INT:
		return [element]() { glUniform1i(element->id, (int&)element->value); };
	case SType::MAT4:
		return [element]() { glUniformMatrix4fv(element->id, element->count, 0, ((mat4&)element->value).Pointer()); };
	case SType::MAT3:
		return [element]() { glUniformMatrix3fv(element->id, element->count, 0, ((mat3&)element->value).Pointer()); };
	case SType::VEC3:
		return [element]() { glUniform3fv(element->id, element->count, ((vec3&)element->value).Pointer()); };
	case SType::VEC4:
		return [element]() { glUniform4fv(element->id, element->count, ((mat4&)element->value).Pointer()); };
	case SType::TEXTURE:
		return [element]() {
		    STexture* tex = (STexture*) element->value;
		    tex->Bind(element->id, element->count);
		};
	}
	
	return nullptr;
}

void SMaterial::Init(const AssetMgr::AssetReference* asset) {
	const XNode* m_root;

	try {
		m_root = XFILE(asset->path.c_str()).getRoot();
	}
	catch (int e) {
		return;
	}

	XNode sce_mat = m_root->getChild("CSEMAT");
	XNode shader_node = sce_mat.getChild("shader");

	auto var_nodes = shader_node.children;

	for (auto node : var_nodes) {

		auto element_value = node.value.toStringVector();
		auto element_type = node.getAttribute("type").value;
        SType type = XMLParser::GetType(element_type);
        auto element_name = node.getAttribute("name").value;
		auto element_count = node.getAttribute("count").value;

		Element* element = new Element;
        element->type = type;
        element->count = std::stoi(element_count);
		XMLParser::parse(element_value, element->value, type);
		m_elements.insert(std::pair<std::string, Element*>(element_name, element));
	}

	SAFE_DELETE(m_root);
}
