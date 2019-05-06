#include "DAELoader.h"
#include "../../MoreString.h"
#include "DAEUtil/DAEConvertSGameObject.h"
#include <iostream>
#include "../../../Manager/TextureContainer.h"
#include "../../../Component/DrawableSkinnedMeshComponent.h"
#include "../../../Component/RenderComponent.h"


const mat4 CORRECTION = /*mat4::RotateX(90)*/ mat4::Identity();


DAELoader::DAELoader(const char* path, MeshSurface* obj, LOAD_TYPE type) {
    if (type == NOTHING) return;

    m_obj = obj;
    if (m_obj == nullptr) {
        m_obj = new MeshSurface();
    }

    Load(path, type, nullptr);
}

DAELoader::~DAELoader() {
    Exterminate();
}

void DAELoader::Load(const char* path, LOAD_TYPE type, const char* texture_path) {

    {
        std::string path_str = path;
        std::size_t index = path_str.rfind("/");
        if (index == std::string::npos) {
            index = path_str.rfind("\\");
        }
        std::size_t name_index = path_str.rfind(".");

        m_name = path_str.substr(index + 1, name_index - index - 1);

    }

    m_root = XFILE(path).getRoot();
    XNode collada = m_root->getChild("COLLADA");

    if (type == LOAD_TYPE::MESH || type == LOAD_TYPE::ALL) {
        try {
            //스킨데이터를 불러옴
            LoadSkin(collada.getChild("library_controllers"));
            LoadSkeleton(collada.getChild("library_visual_scenes"));
            m_isSkinning = true;
        } catch (int error) {
            std::cout << "passing skinning...\n";
            m_isSkinning = false;
        }


        //지오메트리를 불러옴
        LoadGeometry(collada.getChild("library_geometries"));
    }

    //애니메이션 데이터를 불러옴
    if (type == ANIMATION || type == ALL) {
        m_animationLoader = new DAEAnimationLoader();
        m_animationLoader->Load(path, m_name);

    }

    LoadTexture(texture_path);
}


void DAELoader::LoadTexture(const char* filePath) {
    if(filePath == nullptr) return;

    STexture* texture = new STexture();
    texture->LoadFile(filePath);
    m_texture_id = ResMgr::getInstance()->GetID<TextureContainer, STexture>(texture);
}

void DAELoader::Exterminate() {
    SAFE_DELETE(m_root);
    std::cout << "\ndeleting " << m_vertices.size() << " DAE Vertexes...\n";
    for (auto vertices : m_vertices) {
        SAFE_DELETE(vertices);
    }

    m_vertices.clear();

    SAFE_DELETE(m_skinningData);
    SAFE_DELETE(m_animationLoader);
//    m_skeletonData->Destroy();
}

void DAELoader::LoadSkin(XNode root_s) {
    XNode skinningData = root_s.getChild("controller").getChild("skin");


    std::vector<std::string> jointsList = loadJointsList(skinningData);
    std::vector<float> weights = loadWeights(skinningData);
    XNode weightsDataNode = skinningData.getChild("vertex_weights");
    std::vector<int> effectorJointCounts = getEffectiveJointsCounts(weightsDataNode);
    std::vector<VertexSkinData*> vertexWeights = getSkinData(weightsDataNode, effectorJointCounts, weights);

    //쓰레기값 제거
    SAFE_DELETE(m_skinningData);
    m_skinningData = new SkinningData();
    m_skinningData->set_jointOrder(jointsList);
    m_skinningData->set_verticesSkinData(vertexWeights);

}

void DAELoader::LoadSkeleton(XNode root_s) {
    XNode armatureData = root_s.getChild("visual_scene").getNodeByAttribute("node", "id", "Armature");

    XNode headNode = armatureData.getChild("node");
    Joint* headJoint = loadJointData(headNode, true);


    SAFE_DELETE(m_skeletonData);
    m_skeletonData = new Skeleton(m_jointSize, headJoint);

}

void DAELoader::LoadGeometry(XNode root_g) {
    XNode meshData = root_g.getChild("geometry").getChild("mesh");

    //RAW data
    ReadPositions(meshData, m_skinningData->get_verticesSkinData());
    ReadNormals(meshData);
    ReadUVs(meshData);

    AssembleVertices(meshData);

    removeUnusedVertices();

    ConvertDataToVectors();

    AttachDataToObjSurface();

}

//===================================================================
// GeometryLoader Functions
//===================================================================

void DAELoader::ReadPositions(XNode data, std::vector<VertexSkinData*> vertexWeight) {
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
        m_vertices.push_back(new Vertex(m_vertices.size(), vec3{transform.w.x, transform.w.y, transform.w.z},
                                        m_isSkinning ? vertexWeight.at(m_vertices.size()) : nullptr));
    }

}

void DAELoader::ReadNormals(XNode data) {
    std::string normalsID = data.getChild("polylist").getNodeByAttribute("input", "semantic", "NORMAL").getAttribute(
            "source").value.substr(1);
    XNode normalsData = data.getNodeByAttribute("source", "id", normalsID.c_str()).getChild("float_array");
    int normalsSize = std::stoi(normalsData.getAttribute("count").value);
    std::vector<float> normals = normalsData.value.toFloatVector();

    for (int i = 0; i < normalsSize / 3; i++) {
        float x = normals[i * 3];
        float y = normals[i * 3 + 1];
        float z = normals[i * 3 + 2];

        // vec4 normal = vec4{ x, y, z, 0 };
        mat4 transform = mat4::Translate(x, y, z) * CORRECTION;
        // std::cout << "{ " << x << ", " << y << ", " << z << " }, ";
        // Matrix4f.transform(CORRECTION, normal, normal);
        m_normals.push_back(vec3{transform.w.x, transform.w.y, transform.w.z});

    }

    // std::cout << "normal size : " << normalsSize << "\nnormal real size : " << m_normals.size() << '\n';

}

void DAELoader::ReadUVs(XNode data) {
    std::string texID = data.getChild("polylist").getNodeByAttribute("input", "semantic", "TEXCOORD").getAttribute(
            "source").value.substr(1);
    XNode texData = data.getNodeByAttribute("source", "id", texID.c_str()).getChild("float_array");
    int uvSize = std::stoi(texData.getAttribute("count").value);
    auto uvs = texData.value.toFloatVector();

    for(int i = 0; i < uvSize/2; i++) {
        float s = uvs[i * 2];
        float t = uvs[i * 2 + 1];

        m_texUVs.push_back(vec2{s, t});
    }



    // std::cout << "normal size : " << normalsSize << "\nnormal real size : " << m_normals.size() << '\n';

}

void DAELoader::AssembleVertices(XNode data) {
    XNode poly = data.getChild("polylist");
    int typeCount = 0;

    for (auto child : poly.children) {
        if (child.name == "input") {
            typeCount++;
        }
    }

    std::vector<int> indexData = poly.getChild("p").value.toIntegerVector();

    // std::cout << "\n\ntypeCount : " << typeCount << '\n';

    for (int i = 0; i < indexData.size() / typeCount; i++) {
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
            Vertex* duplicateVertex = new Vertex(m_vertices.size(),
                                                 previousVertex->getPosition(), previousVertex->getWeightsData());
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

//===================================================================
// SkinLoader Functions
//===================================================================

std::vector<std::string> DAELoader::loadJointsList(XNode skinningData) {
    XNode inputNode = skinningData.getChild("vertex_weights");
    std::string jointDataId = inputNode.getNodeByAttribute("input", "semantic", "JOINT").getAttribute(
            "source").value.substr(1);
    XNode jointsNode = skinningData.getNodeByAttribute("source", "id", jointDataId.c_str()).getChild("Name_array");

    std::vector<std::string> jointsList = jointsNode.value.toStringVector();

    return jointsList;
}

std::vector<float> DAELoader::loadWeights(XNode skinningData) {
    XNode inputNode = skinningData.getChild("vertex_weights");
    std::string weightsDataId = inputNode.getNodeByAttribute("input", "semantic", "WEIGHT").getAttribute(
            "source").value.substr(1);
    XNode weightsNode = skinningData.getNodeByAttribute("source", "id", weightsDataId.c_str()).getChild("float_array");
    std::vector<float> weights = weightsNode.value.toFloatVector();

    return weights;
}

std::vector<int> DAELoader::getEffectiveJointsCounts(XNode node) {
    return node.getChild("vcount").value.toIntegerVector();
}

std::vector<VertexSkinData*>
DAELoader::getSkinData(XNode weightsDataNode, std::vector<int> counts, std::vector<float> weights) {
    auto rawData = weightsDataNode.getChild("v").value.toIntegerVector();
    std::vector<VertexSkinData*> skinningData;
    int pointer = 0;
    for (int count : counts) {
        VertexSkinData* skinData = new VertexSkinData();
        for (int i = 0; i < count; i++) {
            int jointId = rawData[pointer++];
            int weightId = rawData[pointer++];
            skinData->addJointEffect(jointId, weights[weightId]);
        }
        skinData->limitJointNumber(m_maxWeight);
        skinningData.push_back(skinData);
    }
    return skinningData;
}


//===================================================================
// SkeletonLoader Functions
//===================================================================

Joint* DAELoader::extractMainJointData(XNode jointNode, bool isRoot) {
    std::string nameId = jointNode.getAttribute("id").value;
    int index = -1;
    auto jointOrders = m_skinningData->get_jointOrder();
    for (int i = 0; i < jointOrders.size(); ++i) {
        if (trim(jointOrders[i]) == nameId) {
            index = i;
            break;
        }
    }

    if (index < 0)
        return nullptr;

    std::vector<float> matrixData = jointNode.getChild("matrix").value.toFloatVector();
    mat4 matrix = mat4(&matrixData[0]);
    matrix = matrix.Transposed();
    if (isRoot) {
        //because in Blender z is up, but in our game y is up.
        matrix *= CORRECTION;
    }
    m_jointSize++;
    return new Joint(index, nameId, matrix);
}

Joint* DAELoader::loadJointData(XNode jointNode, bool isRoot) {
    Joint* joint = extractMainJointData(jointNode, isRoot);

    try {

        for (XNode childNode : jointNode.children) {
            if (childNode.name == "node") {
                joint->addChild(loadJointData(childNode, false));
            }
        }
    } catch (int error) {}


    return joint;
}


void DAELoader::ConvertDataToVectors() {
    {
        int size = m_vertices.size();
        m_f_vertices.resize(size * 3);
        m_f_normals.resize(size * 3);
        m_f_texUVs.resize(size * 2);
        if (m_isSkinning) {
            m_f_jointIDs.resize(size * 3);
            m_f_weights.resize(size * 3);
        }

    }

    float furthestPoint = 0;
    for (int i = 0; i < m_vertices.size(); i++) {
        Vertex& currentVertex = *m_vertices.at(i);
        if (currentVertex.getLength() > furthestPoint) {
            furthestPoint = currentVertex.getLength();
        }
        vec3 position = currentVertex.getPosition();
        // vec2 textureCoord = textures.get(currentVertex.getTextureIndex());
        vec2 textureCoord = m_texUVs.at(currentVertex.getTextureIndex());
        vec3 normalVector = m_normals.at(currentVertex.getNormalIndex());
        m_f_vertices[i * 3] = position.x;
        m_f_vertices[i * 3 + 1] = position.y;
        m_f_vertices[i * 3 + 2] = position.z;
        m_f_texUVs[i * 2] = textureCoord.x;
        m_f_texUVs[i * 2 + 1] = 1 - textureCoord.y;
        m_f_normals[i * 3] = normalVector.x;
        m_f_normals[i * 3 + 1] = normalVector.y;
        m_f_normals[i * 3 + 2] = normalVector.z;

        if (!m_isSkinning) continue;

        VertexSkinData* weights = currentVertex.getWeightsData();
        m_f_jointIDs[i * 3] = weights->getJointIDs()[0];
        m_f_jointIDs[i * 3 + 1] = weights->getJointIDs()[1];
        m_f_jointIDs[i * 3 + 2] = weights->getJointIDs()[2];
        m_f_weights[i * 3] = weights->getWeights()[0];
        m_f_weights[i * 3 + 1] = weights->getWeights()[1];
        m_f_weights[i * 3 + 2] = weights->getWeights()[2];

    }
}


void DAELoader::AttachDataToObjSurface() {
    int sizeVertex = m_vertices.size();
    int sizeIndices = m_indices.size() / 3;

    // std::cout << "size vertices : " << sizeVertex << "\nsize indices : " << sizeIndices << '\n';

    std::vector<float> jointIds(m_f_jointIDs.begin(), m_f_jointIDs.end());

    m_obj->MakeVertices(sizeVertex, &m_f_vertices[0], &m_f_normals[0], (m_f_texUVs.empty() ? nullptr : &m_f_texUVs[0]),
                        &m_f_weights[0], &jointIds[0]);
    m_obj->MakeIndices(sizeIndices, &m_indices[0]);
}

SPrefab* DAELoader::GeneratePrefab() {

    SPrefab* prefab = new SPrefab();
    SGameObject* root = new SGameObject(m_name);
    prefab->SetGameObject(root);

    SGameObject* joint_root = new SGameObject("Armature");
    root->AddChild(joint_root);
    DAEConvertSGameObject::CreateJoints(joint_root, m_skeletonData->getHeadJoint());

    SGameObject* mesh_root = new SGameObject("mesh");
    root->AddChild(mesh_root);
    auto mesh_component = mesh_root->CreateComponent<DrawableSkinnedMeshComponent>();
    mesh_component->SetMesh(*m_obj);

    mesh_component->SetRootJoint(joint_root->GetChildren()[0], m_skeletonData->getJointCount());

    mesh_root->CreateComponent<RenderComponent>();
    mesh_root->GetComponent<RenderComponent>()->SetShaderHandle(0);
//    mesh_root->GetComponent<RenderComponent>()->SetIsEnable(false);

    auto material = mesh_root->CreateComponent<MaterialComponent>();
//    material->SetDiffuseMaterial(vec4{0.7f, 0.6f, 1, 1});
    material->SetShininess(40);
    material->SetTexture(ResMgr::getInstance()->GetObject<TextureContainer, STexture>(m_texture_id));

    SGameObject* animationObj = DAEConvertSGameObject::CreateAnimation(root, mesh_root,
                                                                       m_animationLoader->GetAnimation());


    return prefab;
}







