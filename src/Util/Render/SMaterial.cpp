//
// Created by ounols on 19. 6. 1.
//

#include "SMaterial.h"

#include "../Loader/XML/XML.h"
#include "../Loader/XML/XMLParser.h"
#include "../../Component/TransformComponent.h"

using namespace CSE;

SMaterial::SMaterial() {

}


SMaterial::SMaterial(const SMaterial* material) : SResource(material, false) {

    if(material == nullptr) throw -1;

    for(const auto& element_pair : material->m_elements) {
        const auto& element_src = element_pair.second;
        Element* element_copy = new Element;
        element_copy->count = element_src->count;
        element_copy->type = element_src->type;
        element_copy->value_str = element_src->value_str;
        m_elements.insert(std::pair<std::string, Element*>(element_pair.first, element_copy));
    }
    SetHandle(material->m_handle);
}

SMaterial::~SMaterial() {
    ReleaseElements();
}

void SMaterial::Exterminate() {
    ReleaseElements();
}

void SMaterial::ReleaseElements() {
	for (const auto pair : m_elements) {
		auto* element = pair.second;
		SAFE_DELETE(element);
	}
    m_elements.clear();

    for (const auto pair : m_attributeElements) {
        auto* element = pair.second;
        SAFE_DELETE(element);
    }
    m_attributeElements.clear();
}

void SMaterial::SetHandle(GLProgramHandle* handle) {
    m_handle = handle;
	InitElements();
}

void SMaterial::AttachElement() const {

	for (const auto element_pair : m_elements) {
		const auto* element = element_pair.second;
		if(element->id < 0) continue;
		element->attachFunc();
	}
    for (const auto element_pair : m_attributeElements) {
        const auto* element = element_pair.second;
        if(element->id < 0) continue;
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
//		if (handleElement == nullptr) {
//			handleElement = m_handle->AttributeLocation(element_name);	isUniform = false;
//		}
		if (handleElement == nullptr) continue;

		element->id = handleElement->id;
		SetBindFuncByType(element, isUniform);
	}

//	const auto attributeList = m_handle->GetAttributesList();
//	for(const auto attr_pair : attributeList) {
//        Element* element = new Element;
//        element->type = SType::UNKNOWN;
//        element->count = std::stoi(element_count);
//        element->value_str = element_value;
//	    attr_pair.second->id
//	}
	
}

void SMaterial::SetAttribute(const GLMeshID& meshId) const {
    int stride = 4 * sizeof(vec3) + sizeof(vec2);    //normal + position + uv
    const GLvoid* offset = (const GLvoid*) sizeof(vec3);
    GLint position = m_handle->Attributes.Position;
    GLint normal = m_handle->Attributes.Normal;
    GLint tex = m_handle->Attributes.TextureCoord;
    GLint weight = m_handle->Attributes.Weight;
    GLint jointId = m_handle->Attributes.JointId;

    bool isTex = tex != HANDLE_NULL;

    if (meshId.m_indexSize < 0) {
        glBindBuffer(GL_ARRAY_BUFFER, meshId.m_vertexBuffer);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
        if (isTex) {
            offset = (GLvoid*) (sizeof(vec3) * 2);
            glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, stride, offset);
        }
        if (meshId.m_hasJoint) {
            offset = (GLvoid*) (sizeof(vec3) * 2 + sizeof(vec2));
            glVertexAttribPointer(weight, 3, GL_FLOAT, GL_FALSE, stride, offset);

            offset = (GLvoid*) (sizeof(vec3) * 3 + sizeof(vec2));
            glVertexAttribPointer(jointId, 3, GL_FLOAT, GL_FALSE, stride, offset);
        }
        glDrawArrays(GL_TRIANGLES, 0, meshId.m_vertexSize);

    } else {
        glBindBuffer(GL_ARRAY_BUFFER, meshId.m_vertexBuffer);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
        if (isTex) {
            offset = (GLvoid*) (sizeof(vec3) * 2);
            glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, stride, offset);
        }
        if (meshId.m_hasJoint) {
            offset = (GLvoid*) (sizeof(vec3) * 2 + sizeof(vec2));
            glVertexAttribPointer(weight, 3, GL_FLOAT, GL_FALSE, stride, offset);

            offset = (GLvoid*) (sizeof(vec3) * 3 + sizeof(vec2));
            glVertexAttribPointer(jointId, 3, GL_FLOAT, GL_FALSE, stride, offset);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshId.m_indexBuffer);
        glDrawElements(GL_TRIANGLES, meshId.m_indexSize * 3, GL_UNSIGNED_SHORT, 0);

    }
}

void SMaterial::SetCameraUniform(mat4 camera, vec3 cameraPosition, mat4 projection, TransformInterface* transform) const {

    mat4 modelNoCameraView = static_cast<TransformComponent*>(transform)->GetMatrix();
    mat4 modelView = modelNoCameraView * camera;
    glUniformMatrix4fv(m_handle->Uniforms.Modelview, 1, 0, modelView.Pointer());
    glUniformMatrix4fv(m_handle->Uniforms.ModelNoCameraMatrix, 1, 0, modelNoCameraView.Pointer());
    glUniform3fv(m_handle->Uniforms.CameraPosition, 1, cameraPosition.Pointer());

    //normal matrix
//    glUniformMatrix3fv(handler->Uniforms.NormalMatrix, 1, 0, modelView.ToMat3().Pointer());


    //projection transform
    glUniformMatrix4fv(m_handle->Uniforms.Projection, 1, 0, projection.Pointer());
}

void SMaterial::SetSkinningUniform(const GLMeshID& mesh, const std::vector<mat4>& jointMatrix) {
    if (!mesh.m_hasJoint || jointMatrix.empty()) {
        glUniform1i(m_handle->Uniforms.SkinningMode, 0);
        return;
    }

    std::vector<float> result;
    for (mat4 matrix : jointMatrix) {
        for (int i = 0; i < 16; i++) {
            result.push_back(matrix.Pointer()[i]);

        }
    }

    glUniformMatrix4fv(m_handle->Uniforms.JointMatrix, MAX_JOINTS, 0, &result[0]);
    glUniform1i(m_handle->Uniforms.SkinningMode, 1);
}

void SMaterial::SetInt(std::string name, int value) {
    auto find_iter = m_elements.find(name);
    if(find_iter == m_elements.end()) return;
    SetIntFunc(find_iter->second, value);
}

void SMaterial::SetFloat(std::string name, float value) {
    auto find_iter = m_elements.find(name);
    if(find_iter == m_elements.end()) return;
    SetFloatFunc(find_iter->second, value);
}

void SMaterial::SetVec3(std::string name, vec3 value) {
    auto find_iter = m_elements.find(name);
    if(find_iter == m_elements.end()) return;
    SetVec3Func(find_iter->second, value);
}

void SMaterial::SetTexture(std::string name, SResource* texture) {
    auto find_iter = m_elements.find(name);
    if(find_iter == m_elements.end()) return;
    SetTextureFunc(find_iter->second, texture);
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
    auto shader_file_id = shader_node.getAttribute("id").value;
    auto shaderHandle = SResource::Create<GLProgramHandle>(shader_file_id);
    if(shaderHandle == nullptr) return;

    for (auto node : var_nodes) {

        auto element_value = node.value.toStringVector();
        auto element_type = node.getAttribute("type").value;
        SType type = XMLParser::GetType(element_type);
        auto element_name = node.getAttribute("name").value;
        auto element_count = node.getAttribute("count").value;

        Element* element = new Element;
        element->type = type;
        element->count = std::stoi(element_count);
        element->value_str = element_value;
        m_elements.insert(std::pair<std::string, Element*>(element_name, element));
    }
    SAFE_DELETE(m_root);
    m_handle = shaderHandle;
}


void SMaterial::SetBindFuncByType(Element* element, bool isUniform) {

	const GLenum type = element->type;

//	if(isUniform == false) {
//		switch (type) {
//		case SType::VEC2:
//			return [element]() {
//			    auto value = XMLParser::parseVec2(element->value_str);
//			    glVertexAttrib2fv(element->id, value.Pointer());
//			};
//		case SType::VEC3:
//			return [element]() {
//                auto value = XMLParser::parseVec3(element->value_str);
//                glVertexAttrib3fv(element->id, value.Pointer());
//			};
//		case SType::VEC4:
//			return [element]() {
//                auto value = XMLParser::parseVec4(element->value_str);
//                glVertexAttrib4fv(element->id, value.Pointer());
//			};
//		}
//		return nullptr;
//	}

	switch (type) {
	case SType::FLOAT:
        SetFloatFunc(element, XMLParser::parseFloat(element->value_str[0].c_str()));
	case SType::INT:
        SetIntFunc(element, XMLParser::parseInt(element->value_str[0].c_str()));
//	case SType::MAT4:
//		SetMat4Func(element, XMLParser::parseMat4(element->value_str));
//	case SType::MAT3:
//		SetMat3Func(element, XMLParser::parseMat3(element->value_str));
    case SType::VEC4:
        SetVec4Func(element, XMLParser::parseVec4(element->value_str));
	case SType::VEC3:
        SetVec3Func(element, XMLParser::parseVec3(element->value_str));
	case SType::TEXTURE:
        SetTextureFunc(element, XMLParser::parseTexture(element->value_str[0].c_str()));
	}
	
	return;
}


void SMaterial::SetIntFunc(Element* element, int value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniform1i(element->id, value);
    };
}

void SMaterial::SetFloatFunc(SMaterial::Element* element, float value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniform1f(element->id, value);
    };
}

void SMaterial::SetBoolFunc(SMaterial::Element* element, bool value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniform1i(element->id, value);
    };
}

void SMaterial::SetMat4Func(SMaterial::Element* element, mat4 value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniformMatrix4fv(element->id, element->count, 0, value.Pointer());
    };
}

void SMaterial::SetMat3Func(SMaterial::Element* element, mat3 value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniformMatrix3fv(element->id, element->count, 0, value.Pointer());
    };
}

void SMaterial::SetMat2Func(SMaterial::Element* element, mat2 value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniformMatrix2fv(element->id, element->count, 0, value.Pointer());
    };
}

void SMaterial::SetVec4Func(SMaterial::Element* element, vec4 value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniform4fv(element->id, element->count, value.Pointer());
    };
}

void SMaterial::SetVec3Func(SMaterial::Element* element, vec3 value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniform3fv(element->id, element->count, value.Pointer());
    };
}

void SMaterial::SetVec2Func(SMaterial::Element* element, vec2 value) {
    if(element == nullptr) return;
    element->attachFunc = [element, value]() {
        glUniform2fv(element->id, element->count, value.Pointer());
    };
}

void SMaterial::SetTextureFunc(SMaterial::Element* element, SResource* texture) {
    if(element == nullptr || texture == nullptr) return;
    STexture* value = static_cast<STexture*>(texture);
    element->attachFunc = [element, value]() {
        value->Bind(element->id, element->count);
    };
}

short SMaterial::GetOrderLayer() const {
    return m_orderLayer;
}

void SMaterial::SetOrderLayer(int orderLayer) {
    m_orderLayer = orderLayer;
}

GLProgramHandle* SMaterial::GetHandle() const {
    return m_handle;
}