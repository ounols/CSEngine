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
    for (Element* element : m_elements) {
        SAFE_DELETE(element);
    }
    m_elements.clear();
}

void SMaterial::SetHandle(GLProgramHandle* handle) {
    m_handle = handle;
    ReleaseElements();

    auto attr_list = m_handle->GetAttributesList();
    auto uni_list = m_handle->GetUniformsList();

    for (const auto pair : attr_list) {
        const auto handle_element = pair.second;
        Element* element = new Element;

        element->id = handle_element->id;
        element->type = GetType(handle_element->type);

        m_elements.push_back(element);
    }

}

void SMaterial::AttachElement(const char* element_name, void* element) {

    int id = m_handle->UniformLocation(element_name);
    if (id == HANDLE_NULL) id = m_handle->AttributeLocation(element_name);
    if (id == HANDLE_NULL) return;

    for (Element* element_vec : m_elements) {
        if (element_vec == nullptr) continue;
        if (element_vec->id == id) {
            element_vec->value = element;
            break;
        }
    }

}

SMaterial::TYPE SMaterial::GetType(GLenum type) {
    switch (type) {
        case GL_FLOAT:
            return FLOAT;

        case GL_INT:
            return INT;

        default:
            return NONE;
    }
}
