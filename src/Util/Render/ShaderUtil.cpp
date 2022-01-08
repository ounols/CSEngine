//
// Created by ounols on 19. 4. 30.
//

#include "ShaderUtil.h"
#include "../MoreString.h"
#include "SEnvironmentMgr.h"

#ifdef WIN32
#include <Windows.h>
#endif

using namespace CSE;

ShaderUtil::ShaderUtil() = default;

ShaderUtil::~ShaderUtil() = default;

GLProgramHandle*
ShaderUtil::CreateProgramHandle(const GLchar* vertexSource, const GLchar* fragmentSource, GLProgramHandle* handle) {
	if (vertexSource == nullptr || fragmentSource == nullptr) return nullptr;
	if (handle != nullptr && handle->Program != HANDLE_NULL) return nullptr;

    GLProgramHandle* newHandle = handle;
	auto program = createProgram(vertexSource, fragmentSource, *newHandle);
	if (!program) {
		return nullptr;
	}

	//Find important variables from shader.
	auto variables_vert = GetImportantVariables(vertexSource);
	auto variables_frag = GetImportantVariables(fragmentSource);

    if (newHandle == nullptr)
        newHandle = new GLProgramHandle();
    newHandle->SetProgram(program);
	//Get all variables from shader.
    newHandle->SetAttributesList(variables_vert, variables_frag);
    newHandle->SetUniformsList(variables_vert, variables_frag);

	//Binding important variables to engine.
	BindVariables(newHandle);

	return newHandle;
}

GLuint
ShaderUtil::createProgram(const GLchar* vertexSource, const GLchar* fragmentSource, const GLProgramHandle& handle) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource, handle);
	if (!vertexShader) {
		return 0;
	}

	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource, handle);
	if (!fragmentShader) {
		return 0;
	}

	return createProgram(vertexShader, fragmentShader, handle);
}

GLuint ShaderUtil::createProgram(GLuint vertexShader, GLuint fragmentShader, const GLProgramHandle& handle) {
	GLuint program = glCreateProgram();

	if (program) {

		//쉐이더를 attach 합니다.
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {

			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

			if (bufLength) {

				auto buf = static_cast<char*>(malloc(bufLength));

				if (buf) {

					glGetProgramInfoLog(program, bufLength, nullptr, buf);
                    auto errorLog = std::string("[") + handle.GetName() + "] Could not link program:" + buf;
                    SafeLog::Log(errorLog.c_str());
					free(buf);

				}
			}

			glDeleteProgram(program);
			program = 0;

		}

		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);

	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLuint ShaderUtil::loadShader(GLenum shaderType, const char *pSource, const GLProgramHandle& handle) {
	GLuint shader = glCreateShader(shaderType);

	if (shader) {

		glShaderSource(shader, 1, &pSource, nullptr);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {

			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen) {

				auto buf = static_cast<char*>(malloc(infoLen));

				if (buf) {
					glGetShaderInfoLog(shader, infoLen, nullptr, buf);
					//LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    auto errorLog = std::string("[") + handle.GetName() + "] Could not compile shader:" + buf;
                    SafeLog::Log(errorLog.c_str());
					free(buf);
				}

				glDeleteShader(shader);
				shader = 0;
			}
		}
	}

	return shader;
}

std::map<std::string, std::string> ShaderUtil::GetImportantVariables(const GLchar* source) {
	std::map<std::string, std::string> variables;

	std::vector<std::string> str_line = split(source, ';');

	std::string type_str;

	for (auto line : str_line) {
		line += ';';
		auto result = split(line, '\n');

		for (const auto& str : result) {
			int start_index;
			int end_index;
			if ((start_index = str.find("//[")) != std::string::npos) {
				start_index += 3;
				end_index = str.find("]//");
				type_str = str.substr(start_index, end_index - 3);
				continue;
			}

			int eoc_index;
			if (!type_str.empty() && (eoc_index = str.find(';')) != std::string::npos) {
				int startIndex = str.substr(0, eoc_index).rfind(' ');
				int endIndex = str.rfind('[');
				endIndex = endIndex == std::string::npos ? eoc_index : endIndex;
				auto detail = str.substr(startIndex, endIndex - startIndex);
				detail = trim(detail);
				variables[type_str] = detail;
				type_str.clear();
			}
		}

	}

	return variables;
}

void ShaderUtil::BindVariables(GLProgramHandle* handle) {

	if (handle == nullptr) return;

	//Attributes
	auto position = handle->AttributeLocation("att.position");
	auto normal = handle->AttributeLocation("att.normal");
	auto jointId = handle->AttributeLocation("att.joint_indices");
	auto weight = handle->AttributeLocation("att.weight");
	auto textureCoord = handle->AttributeLocation("att.tex_uv");
	auto color = handle->AttributeLocation("att.color");
	//Uniforms
	auto modelView = handle->UniformLocation("matrix.modelview");
	auto modelViewNoCamera = handle->UniformLocation("matrix.modelview_nc");
	auto cameraPosition = handle->UniformLocation("CAMERA_POSITION");
	auto projection = handle->UniformLocation("matrix.projection");
	auto skinningMode = handle->UniformLocation("matrix.skinning_mode");
	auto jointMatrix = handle->UniformLocation("matrix.joint");
	auto lightPosition = handle->UniformLocation("light.position");
	auto lightType = handle->UniformLocation("light.type");
	auto lightRadius = handle->UniformLocation("light.radius");
	auto lightColor = handle->UniformLocation("light.color");
	auto lightShadowMap = handle->UniformLocation("light.shadow_map");
	auto lightMatrix = handle->UniformLocation("light.matrix");
	auto lightShadowMode = handle->UniformLocation("light.shadow_mode");
	auto lightSize = handle->UniformLocation("light.size");
	auto lightIrradiance = handle->UniformLocation("light.irradiance");
	auto lightPrefilter = handle->UniformLocation("light.prefilter");
	auto lightBrdf = handle->UniformLocation("light.brdf");

	handle->Attributes.Position = position != nullptr ? position->id : HANDLE_NULL;
	handle->Attributes.Normal = normal != nullptr ? normal->id : HANDLE_NULL;
	handle->Attributes.JointId = jointId != nullptr ? jointId->id : HANDLE_NULL;
	handle->Attributes.Weight = weight != nullptr ? weight->id : HANDLE_NULL;
	handle->Attributes.TextureCoord = textureCoord != nullptr ? textureCoord->id : HANDLE_NULL;
	handle->Attributes.Color = color != nullptr ? color->id : HANDLE_NULL;

	handle->Uniforms.ModelView = modelView != nullptr ? modelView->id : HANDLE_NULL;
	handle->Uniforms.ModelNoCameraMatrix = modelViewNoCamera != nullptr ? modelViewNoCamera->id : HANDLE_NULL;
	handle->Uniforms.CameraPosition = cameraPosition != nullptr ? cameraPosition->id : HANDLE_NULL;
	handle->Uniforms.Projection = projection != nullptr ? projection->id : HANDLE_NULL;
	handle->Uniforms.SkinningMode = skinningMode != nullptr ? skinningMode->id : HANDLE_NULL;
	handle->Uniforms.JointMatrix = jointMatrix != nullptr ? jointMatrix->id : HANDLE_NULL;
	handle->Uniforms.LightPosition = lightPosition != nullptr ? lightPosition->id : HANDLE_NULL;
	handle->Uniforms.LightType = lightType != nullptr ? lightType->id : HANDLE_NULL;
	handle->Uniforms.LightRadius = lightRadius != nullptr ? lightRadius->id : HANDLE_NULL;
	handle->Uniforms.LightColor = lightColor != nullptr ? lightColor->id : HANDLE_NULL;
	handle->Uniforms.LightShadowMap = lightShadowMap != nullptr ? lightShadowMap->id : HANDLE_NULL;
	handle->Uniforms.LightMatrix = lightMatrix != nullptr ? lightMatrix->id : HANDLE_NULL;
	handle->Uniforms.LightShadowMode = lightShadowMode != nullptr ? lightShadowMode->id : HANDLE_NULL;
	handle->Uniforms.LightSize = lightSize != nullptr ? lightSize->id : HANDLE_NULL;
	handle->Uniforms.LightIrradiance = lightIrradiance != nullptr ? lightIrradiance->id : HANDLE_NULL;
	handle->Uniforms.LightPrefilter = lightPrefilter != nullptr ? lightPrefilter->id : HANDLE_NULL;
	handle->Uniforms.LightBrdfLut = lightBrdf != nullptr ? lightBrdf->id : HANDLE_NULL;
}

void ShaderUtil::BindCameraToShader(const GLProgramHandle& handle, const mat4& view, const vec3& cameraPosition,
                                    const mat4& projection, const mat4& transform) {
	mat4 modelView = transform * view;
	glUniformMatrix4fv(handle.Uniforms.ModelView, 1, 0, modelView.Pointer());
	glUniformMatrix4fv(handle.Uniforms.ModelNoCameraMatrix, 1, 0, transform.Pointer());
	glUniform3fv(handle.Uniforms.CameraPosition, 1, cameraPosition.Pointer());

	//normal matrix
	//    glUniformMatrix3fv(handler->Uniforms.NormalMatrix, 1, 0, modelView.ToMat3().Pointer());

	//projection transform
	glUniformMatrix4fv(handle.Uniforms.Projection, 1, 0, projection.Pointer());
}

// position(x y z) + normal(x y z) + tex(u v) + weight(x y z) + jointID(id1, id2, id3)
void ShaderUtil::BindAttributeToShader(const GLProgramHandle& handle, const GLMeshID& meshId) {
	int stride = 4 * sizeof(vec3) + sizeof(vec2); //normal + position + uv = (4 * sizeof(vec3) + sizeof(vec2))
	auto offset = (GLvoid*)sizeof(vec3);
	GLint position = handle.Attributes.Position;
	GLint normal = handle.Attributes.Normal;
	GLint tex = handle.Attributes.TextureCoord;
	GLint weight = handle.Attributes.Weight;
	GLint jointId = handle.Attributes.JointId;

    if(position < 0) return;

    glBindBuffer(GL_ARRAY_BUFFER, meshId.m_vertexBuffer);
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    if(normal >= 0) {
        glEnableVertexAttribArray(normal);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, stride, offset);
    }

    offset = (GLvoid*)(sizeof(vec3) * 2);
    if(tex >= 0) {
        glEnableVertexAttribArray(tex);
        glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, stride, offset);
    }

    offset = (GLvoid*)(sizeof(vec3) * 2 + sizeof(vec2));
    if(weight >= 0) {
        glEnableVertexAttribArray(weight);
        glVertexAttribPointer(weight, 3, GL_FLOAT, GL_FALSE, stride, offset);
    }
    offset = (GLvoid*)(sizeof(vec3) * 3 + sizeof(vec2));
    if(jointId >= 0) {
        glEnableVertexAttribArray(jointId);
        glVertexAttribPointer(jointId, 3, GL_FLOAT, GL_FALSE, stride, offset);
    }

    if (meshId.m_indexSize < 0) {
		glDrawArrays(GL_TRIANGLES, 0, meshId.m_vertexSize);
	}
	else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshId.m_indexBuffer);
		glDrawElements(GL_TRIANGLES, meshId.m_indexSize * 3, GL_UNSIGNED_SHORT, nullptr);
	}

    glDisableVertexAttribArray(position);
    if(normal >= 0)     glDisableVertexAttribArray(normal);
    if(tex >= 0)        glDisableVertexAttribArray(tex);
    if(weight >= 0)     glDisableVertexAttribArray(weight);
    if(jointId >= 0)    glDisableVertexAttribArray(jointId);
}

void ShaderUtil::BindAttributeToPlane() {
	SEnvironmentMgr::RenderPlaneVAO();
}

void ShaderUtil::BindSkinningDataToShader(const GLProgramHandle& handle, const GLMeshID& meshId,
                                          const std::vector<mat4>& jointMatrix) {
	if (!meshId.m_hasJoint || jointMatrix.empty()) {
		glUniform1i(handle.Uniforms.SkinningMode, 0);
		return;
	}

	std::vector<float> result;
	//    result.reserve(jointMatrix.size() * 16 + 1);
	for (const mat4& matrix : jointMatrix) {
		const auto* pointer = matrix.Pointer();
		for (int i = 0; i < 16; ++i, ++pointer) {
			result.push_back(*(pointer));
		}
	}

	glUniformMatrix4fv(handle.Uniforms.JointMatrix, MAX_JOINTS, GL_FALSE, &result[0]);
	glUniform1i(handle.Uniforms.SkinningMode, 1);
}
