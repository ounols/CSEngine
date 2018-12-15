#include "DAELoader.h"
#include <string>
#include <iostream>

const mat4 CORRECTION = /*mat4::RotateX(90)*/mat4::Identity();


DAELoader::DAELoader(const char* path, ObjSurface* obj) {
    m_obj = obj;
    if(m_obj == nullptr){
        m_obj = new ObjSurface();
    }

    Load(path);
}

DAELoader::~DAELoader() {
    Exterminate();
}

void DAELoader::Load(const char* path) {
    m_root = XFILE(path).getRoot();
    XNode collada = m_root->getChild("COLLADA");


    //지오메트리를 불러옴
    LoadGeometry(collada.getChild("library_geometries"));
}

void DAELoader::Exterminate() {
    SAFE_DELETE(m_root);
    std::cout << "\ndeleting " << m_vertices.size() << " DAE Vertexes...\n";
    for(auto vertices : m_vertices) {
        SAFE_DELETE(vertices);
    }

    m_vertices.clear();
}

void DAELoader::LoadGeometry(XNode root_g) {
    XNode meshData = root_g.getChild("geometry").getChild("mesh");

    //RAW data
    ReadPositions(meshData);
    ReadNormals(meshData);

    AssembleVertices(meshData);

    removeUnusedVertices();

    ConvertDataToVectors();

    AttachDataToObjSurface();

}

void DAELoader::ReadPositions(XNode data) {
    std::string positionID = data.getChild("vertices").getChild("input").getAttribute("source").value.substr(1);
    XNode positionData = data.getNodeByAttribute("source", "id", positionID.c_str()).getChild("float_array");
    int vertsSize = std::stoi(positionData.getAttribute("count").value);
    std::vector<float> vertices = positionData.value.toFloatVector();

    for (int i = 0; i < vertsSize / 3; i++) {
        float x = vertices[i * 3];
        float y = vertices[i * 3 + 1];
        float z = vertices[i * 3 + 2];

        // vec4 position = vec4{ x, y, z, 1 };
        mat4 transform = mat4::Translate(x, y, z) * CORRECTION;
        // Matrix4f.transform(CORRECTION, position, position);
        m_vertices.push_back(new Vertex(m_vertices.size(), vec3{ transform.w.x, transform.w.y, transform.w.z }));
    }

}

void DAELoader::ReadNormals(XNode data) {
    std::string normalsID = data.getChild("polylist").getNodeByAttribute("input", "semantic", "NORMAL").getAttribute("source").value.substr(1);
    XNode normalsData = data.getNodeByAttribute("source", "id", normalsID.c_str()).getChild("float_array");
    int normalsSize = std::stoi(normalsData.getAttribute("count").value);
    std::vector<float> normals = normalsData.value.toFloatVector();

    for(int i = 0; i < normalsSize / 3; i++) {
        float x = normals[i * 3];
        float y = normals[i * 3 + 1];
        float z = normals[i * 3 + 2];

        // vec4 normal = vec4{ x, y, z, 0 };
        mat4 transform = mat4::Translate(x, y, z) * CORRECTION;
        // std::cout << "{ " << x << ", " << y << ", " << z << " }, ";
        // Matrix4f.transform(CORRECTION, normal, normal);
        m_normals.push_back(vec3{ transform.w.x, transform.w.y, transform.w.z });

    }

    // std::cout << "normal size : " << normalsSize << "\nnormal real size : " << m_normals.size() << '\n';

}

void DAELoader::AssembleVertices(XNode data) {
    XNode poly = data.getChild("polylist");
    int typeCount = 0;

    for(auto child : poly.children) {
        if(child.name == "input") {
            typeCount++;
        }
    }

    std::vector<int> indexData = poly.getChild("p").value.toIntegerVector();

    // std::cout << "\n\ntypeCount : " << typeCount << '\n';

    for(int i = 0; i<indexData.size() / typeCount; i++) {
        int positionIndex = indexData[i * typeCount];
        int normalIndex = indexData[i * typeCount + 1];
        int texCoordIndex = indexData[i * typeCount + 2];
        processVertex(positionIndex, normalIndex, texCoordIndex);
    }
}

Vertex* DAELoader::processVertex(int posIndex, int normIndex, int texIndex) {
    Vertex* currentVertex = m_vertices.at(posIndex);
    if (!currentVertex->isSet()) {
        currentVertex->setTextureIndex(texIndex);
        currentVertex->setNormalIndex(normIndex);
        m_indices.push_back(posIndex);
        // std::cout << "index = " << m_indices[m_indices.size() - 1] << " (processVertex)\n";
        return currentVertex;
    } else {
        return dealWithAlreadyProcessedVertex(currentVertex, texIndex, normIndex);
    }
}

Vertex* DAELoader::dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex) {
    if (previousVertex->hasSameTextureAndNormal(newTextureIndex, newNormalIndex)) {
			m_indices.push_back(previousVertex->getIndex());
            // std::cout << "index = " << m_indices[m_indices.size() - 1] << " (hasSameTextureAndNormal)\n";
			return previousVertex;
		} else {
			Vertex* anotherVertex = previousVertex->getDuplicateVertex();
			if (anotherVertex != nullptr) {
				return dealWithAlreadyProcessedVertex(anotherVertex, newTextureIndex, newNormalIndex);
			} else {
				Vertex* duplicateVertex = new Vertex(m_vertices.size(), previousVertex->getPosition()/*, previousVertex->getWeightsData()*/);
				duplicateVertex->setTextureIndex(newTextureIndex);
				duplicateVertex->setNormalIndex(newNormalIndex);
				previousVertex->setDuplicateVertex(duplicateVertex);
				m_vertices.push_back(duplicateVertex);
				m_indices.push_back(duplicateVertex->getIndex());
                // std::cout << "index = " << m_indices[m_indices.size() - 1] << " (else)\n";
				return duplicateVertex;
			}

		}
}

void DAELoader::removeUnusedVertices() {
    for (auto vertex : m_vertices) {
			// vertex->averageTangents();
			if (!vertex->isSet()) {
				vertex->setTextureIndex(0);
				vertex->setNormalIndex(0);
			}
		}
}

void DAELoader::ConvertDataToVectors() {
    {
        int size = m_vertices.size();
        m_f_vertices.resize(size * 3);
        m_f_normals.resize(size * 3);
        m_f_texUVs.resize(size * 2);
    }

    float furthestPoint = 0;
    for (int i = 0; i < m_vertices.size(); i++) {
        Vertex& currentVertex = *m_vertices.at(i);
        if (currentVertex.getLength() > furthestPoint) {
            furthestPoint = currentVertex.getLength();
        }
        vec3 position = currentVertex.getPosition();
        // vec2 textureCoord = textures.get(currentVertex.getTextureIndex());
        vec2 textureCoord = vec2{ 0, 0 };
        vec3 normalVector = m_normals.at(currentVertex.getNormalIndex());
        m_f_vertices[i * 3] = position.x;
        m_f_vertices[i * 3 + 1] = position.y;
        m_f_vertices[i * 3 + 2] = position.z;
        m_f_texUVs[i * 2] = textureCoord.x;
        m_f_texUVs[i * 2 + 1] = 1 - textureCoord.y;
        m_f_normals[i * 3] = normalVector.x;
        m_f_normals[i * 3 + 1] = normalVector.y;
        m_f_normals[i * 3 + 2] = normalVector.z;
        // VertexSkinData weights = currentVertex.getWeightsData();
        // jointIdsArray[i * 3] = weights.jointIds.get(0);
        // jointIdsArray[i * 3 + 1] = weights.jointIds.get(1);
        // jointIdsArray[i * 3 + 2] = weights.jointIds.get(2);
        // weightsArray[i * 3] = weights.weights.get(0);
        // weightsArray[i * 3 + 1] = weights.weights.get(1);
        // weightsArray[i * 3 + 2] = weights.weights.get(2);

    }
}


void DAELoader::AttachDataToObjSurface() {
    int sizeVertex = m_vertices.size(); 
    int sizeIndices = m_indices.size() / 3;

    // std::cout << "size vertices : " << sizeVertex << "\nsize indices : " << sizeIndices << '\n';

    m_obj->MakeVertices(sizeVertex, &m_f_vertices[0], &m_f_normals[0]);
    m_obj->MakeIndices(sizeIndices, &m_indices[0]);
}