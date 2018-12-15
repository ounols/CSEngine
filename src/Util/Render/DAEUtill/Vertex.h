#pragma once
#include "../../Matrix.h"
#include <vector>
// #include <iostream>

class Vertex {
public:
    Vertex(int index, vec3 position/*, VertexSkinData weightsData*/) {
        m_index = index;
        m_position = position;
        // std::cout << "index = " << index << '\n';
    }

    ~Vertex() {

    }

    int getIndex(){
		return m_index;
	}
	
	float getLength(){
		return m_length;
	}
	
	bool isSet(){
		return m_textureIndex != -1 && m_normalIndex != -1;
	}
	
	bool hasSameTextureAndNormal(int textureIndexOther,int normalIndexOther){
		return textureIndexOther == m_textureIndex && normalIndexOther == m_normalIndex;
	}
	
	void setTextureIndex(int textureIndex){
		m_textureIndex = textureIndex;
	}
	
	void setNormalIndex(int normalIndex){
		m_normalIndex = normalIndex;
	}

	vec3 getPosition() {
		return m_position;
	}

	int getTextureIndex() {
		return m_textureIndex;
	}

	int getNormalIndex() {
		return m_normalIndex;
	}

	Vertex* getDuplicateVertex() {
		return m_duplicateVertex;
	}

	void setDuplicateVertex(Vertex* duplicateVertex) {
		m_duplicateVertex = duplicateVertex;
	}

private:
    vec3 m_position;

    int m_textureIndex = -1;
    int m_normalIndex = -1;

    Vertex* m_duplicateVertex = nullptr;

    int m_index;
    float m_length;

    std::vector<vec3> m_tangents;
    vec3 m_avgTangent = vec3{ 0, 0, 0 };
};