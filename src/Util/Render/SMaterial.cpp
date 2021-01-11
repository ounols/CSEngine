//
// Created by ounols on 19. 6. 1.
//

#include "SMaterial.h"

using namespace CSE;

SMaterial::SMaterial() {

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
		element->type = handleElement->type;
		element->attachFunc = SetBindFuncByType(element, isUniform);
	}
	
}

std::function<void()> SMaterial::SetBindFuncByType(Element* element, bool isUniform) {

	const GLenum type = element->type;
	const int id = element->id;
	const void* value = element->value;
	const int count = element->count;

	if(isUniform == false) {
		switch (type) {
		case GL_FLOAT_VEC3:
			return [id, value]() { glVertexAttrib3fv(id, ((vec3&)value).Pointer()); };
		case GL_FLOAT_VEC4:
			return [id, value]() { glVertexAttrib4fv(id, ((vec4&)value).Pointer()); };
		}
		return nullptr;
	}

	switch (type) {
	case GL_FLOAT:
		return [id, value]() { glUniform1f(id, (float&)value); };
	case GL_INT:
		return [id, value]() { glUniform1i(id, (int&)value); };
	case GL_FLOAT_MAT4:
		return [id, count, value]() { glUniformMatrix4fv(id, count, 0, ((mat4&)value).Pointer()); };
	case GL_FLOAT_MAT3:
		return [id, count, value]() { glUniformMatrix3fv(id, count, 0, ((mat3&)value).Pointer()); };
	case GL_FLOAT_VEC3:
		return [id, count, value]() { glUniform3fv(id, count, ((vec3&)value).Pointer()); };
	case GL_FLOAT_VEC4:
		return [id, count, value]() { glUniform4fv(id, count, ((mat4&)value).Pointer()); };
	}
	
	return nullptr;
}

void SMaterial::Init(const AssetMgr::AssetReference* asset) {
	
}
