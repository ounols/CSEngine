#include "DAELoader.h"
#include "../../MoreString.h"
#include "DAEUtil/DAEConvertSGameObject.h"
#include <iostream>
#include "../../../Component/DrawableSkinnedMeshComponent.h"
#include "../../../Component/RenderComponent.h"
#include "../../Render/STexture.h"
#include "../../Render/SMaterial.h"
#include "../../../Manager/EngineCore.h"

using namespace CSE;

const mat4 CORRECTION = /*mat4::RotateX(90)*/ mat4::Identity();

DAELoader::DAELoader(const char* path, LOAD_TYPE type, bool isLoad) {
    if (type == NOTHING) return;

    if (isLoad) Load(path, type);
}

DAELoader::~DAELoader() {
    Exterminate();
}

void DAELoader::Load(const char* path, LOAD_TYPE type) {
    {
        std::string path_str = path;
        std::size_t index = path_str.rfind('/');
        if (index == std::string::npos) {
            index = path_str.rfind('\\');
        }
        std::size_t name_index = path_str.rfind('.');

        m_name = path_str.substr(index + 1, name_index - index - 1);
    }

    m_root = XFILE(path).getRoot();
    XNode collada = m_root->getChild("COLLADA");

    if (type == LOAD_TYPE::MESH || type == LOAD_TYPE::ALL || type == LOAD_TYPE::AUTO) {
        try {
            bool safe_result;
            safe_result = LoadSkin(collada.getChild("library_controllers"));
            safe_result &= LoadSkeleton(collada.getChild("library_visual_scenes"));
            m_isSkinning = safe_result;
        } catch (int error) {
            std::cout << "passing skinning...\n";
            m_isSkinning = false;
        }

        try {
            LoadTexturePath(collada.getChild("library_images"));
        } catch (int error) {
            std::cout << "passing texture...\n";
        }


        try {
            const auto& geometryData = collada.getChild("library_geometries").children;
            m_meshList.reserve(geometryData.size());

            for (const auto& geometry : geometryData) {
                auto meshData = new DAEMeshData();
                meshData->meshName = geometry.getAttribute("id").value;
                LoadGeometry(geometry, meshData);
                m_meshList.push_back(meshData);
            }
        } catch (int error) {
            std::cout << "passing geometry...\n";
        }
    }
    //
    if (type == ANIMATION || type == ALL || type == AUTO) {
        try {
            bool safe_exception;
            m_animationLoader = new DAEAnimationLoader();
            safe_exception = m_animationLoader->Load(path, m_name);
#ifdef __EMSCRIPTEN__
            if(!safe_exception) {
                SAFE_DELETE(m_animationLoader);
            }
#endif
        } catch (int error) {
            SAFE_DELETE(m_animationLoader);
            std::cout << "passing Animation...\n";
        }
    }
//    LoadTexture(texture_path);
}


void DAELoader::LoadTexture(const AssetMgr::AssetReference* asset) {
    if (asset == nullptr) return;

    auto* texture = SResource::Create<STexture>(asset);
    m_texture_name = texture->GetName();
}

void DAELoader::Exterminate() {
    SAFE_DELETE(m_root);
    for (auto mesh : m_meshList) {
        SAFE_DELETE(mesh);
    }
    m_meshList.clear();

    SAFE_DELETE(m_skinningData);
    SAFE_DELETE(m_animationLoader);
//    m_skeletonData->Destroy();
}

bool DAELoader::LoadSkin(const XNode& root_s) {
#ifdef __EMSCRIPTEN__
    if(!root_s.hasChild("controller")) return false;
#endif
    XNode skinningData = root_s.getChild("controller").getChild("skin");

    std::vector<std::string> jointsList = loadJointsList(skinningData);
    std::vector<float> weights = loadWeights(skinningData);
    const XNode& weightsDataNode = skinningData.getChild("vertex_weights");
    std::vector<int> effectorJointCounts = getEffectiveJointsCounts(weightsDataNode);
    std::vector<VertexSkinData*> vertexWeights = getSkinData(weightsDataNode, effectorJointCounts, weights);

    SAFE_DELETE(m_skinningData);
    m_skinningData = new SkinningData();
    m_skinningData->set_jointOrder(jointsList);
    m_skinningData->set_verticesSkinData(vertexWeights);
    return true;
}

bool DAELoader::LoadSkeleton(const XNode& root_s) {
#ifdef __EMSCRIPTEN__
    if(!root_s.hasChild("visual_scene")) return false;
    if(!root_s.getChild("visual_scene").hasNodeByAttribute("node", "id", "Armature")) return false;
#endif
    XNode armatureData = root_s.getChild("visual_scene").getNodeByAttribute("node", "id", "Armature");

    const XNode& headNode = armatureData.getChild("node");
    Joint* headJoint = loadJointData(headNode, true);


    //SAFE_DELETE(m_skeletonData);
    if (m_skeletonData == nullptr)
        m_skeletonData = new Skeleton(m_jointSize, headJoint);
    else
        m_skeletonData->SetJoint(m_jointSize, headJoint);

    return true;
}

bool DAELoader::LoadGeometry(const XNode& root_g, DAEMeshData* meshData) {
#ifdef __EMSCRIPTEN__
    if(!root_g.hasChild("mesh")) return false;
#endif
    const XNode& root_m = root_g.getChild("mesh");
    std::vector<VertexSkinData*> vertexSkinData;
    if (m_isSkinning) {
        vertexSkinData =
                m_skeletonData == nullptr ? std::vector<VertexSkinData*>() : m_skinningData->get_verticesSkinData();
    }

    //RAW data
    ReadPositions(root_m, vertexSkinData, meshData);


    std::string normalsId;
    std::string texCoordsId;

    try {
#ifdef __EMSCRIPTEN__
        if(!root_m.hasChild("polylist")) return false;
#endif
        const XNode& polylist = root_m.getChild("polylist");

        normalsId = polylist.getNodeByAttribute("input", "semantic", "NORMAL")
                .getAttribute("source").value.substr(1);
        const XNode& childWithAttribute = polylist.getNodeByAttribute("input", "semantic", "TEXCOORD");
        texCoordsId = childWithAttribute.getAttribute("source").value.substr(1);
        m_polygonType = POLYGON_TYPE::POLYLIST;
    }
    catch (int error) {
        try {
            const XNode& triangles = root_m.getChild("triangles");

            const XNode& normals = triangles.getNodeByAttribute("input", "semantic", "NORMAL");
            normalsId = normals.getAttribute("source").value.substr(1);
            const XNode& childWithAttribute = triangles.getNodeByAttribute("input", "semantic", "TEXCOORD");
            texCoordsId = childWithAttribute.getAttribute("source").value.substr(1);
            m_polygonType = POLYGON_TYPE::TRIANGLES;
        }
        catch (int error) {}
    }

    ReadNormals(root_m, normalsId, meshData);
    ReadUVs(root_m, texCoordsId, meshData);
    AssembleVertices(root_m, meshData);
    removeUnusedVertices(meshData);
    ConvertDataToVectors(meshData);

    return true;
}

//===================================================================
// GeometryLoader Functions
//===================================================================

void DAELoader::ReadPositions(const XNode& data, std::vector<VertexSkinData*> vertexWeight, DAEMeshData* meshData) const {
    std::string positionID = data.getChild("vertices").getChild("input").getAttribute("source").value.substr(1);
    XNode positionData = data.getNodeByAttribute("source", "id", positionID.c_str()).getChild("float_array");
    int vertsSize = std::stoi(positionData.getAttribute("count").value);
    std::vector<float> vertices = positionData.value.toFloatVector();

    for (int i = 0; i < vertsSize / 3; ++i) {
        float x = vertices[i * 3];
        float y = vertices[i * 3 + 1];
        float z = vertices[i * 3 + 2];

        // vec4 position = vec4{ x, y, z, 1 };
        mat4 transform = mat4::Translate(x, y, z) * CORRECTION;
        meshData->vertices.push_back(
                new Vertex(meshData->vertices.size(), vec3{ transform.w.x, transform.w.y, transform.w.z },
                           m_isSkinning ? vertexWeight.at(meshData->vertices.size()) : nullptr));
    }
}

void DAELoader::ReadNormals(const XNode& data, const std::string& normalsId, DAEMeshData* meshData) {
    XNode normalsData = data.getNodeByAttribute("source", "id", normalsId.c_str()).getChild("float_array");
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
        meshData->normals.emplace_back( transform.w.x, transform.w.y, transform.w.z );
    }
    // std::cout << "normal size : " << normalsSize << "\nnormal real size : " << m_normals.size() << '\n';
}

void DAELoader::ReadUVs(const XNode& data, const std::string& texCoordsId, DAEMeshData* meshData) {
    XNode texData = data.getNodeByAttribute("source", "id", texCoordsId.c_str()).getChild("float_array");
    int uvSize = std::stoi(texData.getAttribute("count").value);
    auto uvs = texData.value.toFloatVector();

    for (int i = 0; i < uvSize / 2; i++) {
        float s = uvs[i * 2];
        float t = uvs[i * 2 + 1];

        meshData->texUVs.emplace_back( s, t );
    }
    // std::cout << "normal size : " << normalsSize << "\nnormal real size : " << m_normals.size() << '\n';
}

void DAELoader::AssembleVertices(const XNode& data, DAEMeshData* meshData) {
    XNode poly = data.getChild(m_polygonType == POLYLIST ? "polylist" : "triangles");
    int typeCount = 0;

    for (const auto& child : poly.children) {
        if (child.name == "input") {
            int offset = std::stoi(child.getAttribute("offset").value) + 1;
            if (offset > typeCount) {
                typeCount = offset;
            }
        }
    }

    std::vector<int> indexData = poly.getChild("p").value.toIntegerVector();
    int texcoordOffset = -1;
    try {
        const XNode& texSemantic = poly.getNodeByAttribute("input", "semantic", "TEXCOORD");
        texcoordOffset = std::stoi(texSemantic.getAttribute("offset").value);
    }
    catch (int error) {
        texcoordOffset = -1;
    }

    // std::cout << "\n\ntypeCount : " << typeCount << '\n';

    for (int i = 0; i < indexData.size() / typeCount; i++) {
        int positionIndex = indexData[i * typeCount];
        int normalIndex = indexData[i * typeCount + 1];
        int texCoordIndex = -1;
        if (texcoordOffset != -1) {
            texCoordIndex = indexData[i * typeCount + texcoordOffset];
        }
        processVertex(positionIndex, normalIndex, texCoordIndex, meshData);
    }
}

Vertex* DAELoader::processVertex(int posIndex, int normIndex, int texIndex, DAEMeshData* meshData) {
    Vertex* currentVertex = meshData->vertices.at(posIndex);
    if (!currentVertex->isSet()) {
        currentVertex->setTextureIndex(texIndex);
        currentVertex->setNormalIndex(normIndex);
        meshData->indices.push_back(posIndex);
        // std::cout << "index = " << m_indices[m_indices.size() - 1] << " (processVertex)\n";
        return currentVertex;
    } else {
        return dealWithAlreadyProcessedVertex(currentVertex, texIndex, normIndex, meshData);
    }
}

Vertex* DAELoader::dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex,
                                                  DAEMeshData* meshData) {
    if (previousVertex->hasSameTextureAndNormal(newTextureIndex, newNormalIndex)) {
        meshData->indices.push_back(previousVertex->getIndex());
        // std::cout << "index = " << m_indices[m_indices.size() - 1] << " (hasSameTextureAndNormal)\n";
        return previousVertex;
    } else {
        Vertex* anotherVertex = previousVertex->getDuplicateVertex();
        if (anotherVertex != nullptr) {
            return dealWithAlreadyProcessedVertex(anotherVertex, newTextureIndex, newNormalIndex, meshData);
        } else {
            auto* duplicateVertex = new Vertex(meshData->vertices.size(),
                                                 previousVertex->getPosition(), previousVertex->getWeightsData());
            duplicateVertex->setTextureIndex(newTextureIndex);
            duplicateVertex->setNormalIndex(newNormalIndex);
            previousVertex->setDuplicateVertex(duplicateVertex);
            meshData->vertices.push_back(duplicateVertex);
            meshData->indices.push_back(duplicateVertex->getIndex());
            // std::cout << "index = " << m_indices[m_indices.size() - 1] << " (else)\n";
            return duplicateVertex;
        }
    }
}

void DAELoader::removeUnusedVertices(DAEMeshData* meshData) {
    for (const auto& vertex : meshData->vertices) {
        vertex->averageTangents();
        if (!vertex->isSet()) {
            vertex->setTextureIndex(0);
            vertex->setNormalIndex(0);
        }
    }
}

//===================================================================
// SkinLoader Functions
//===================================================================

std::vector<std::string> DAELoader::loadJointsList(const XNode& skinningData) {
    const XNode& inputNode = skinningData.getChild("vertex_weights");
    std::string jointDataId = inputNode.getNodeByAttribute("input", "semantic", "JOINT").getAttribute(
            "source").value.substr(1);
    XNode jointsNode = skinningData.getNodeByAttribute("source", "id", jointDataId.c_str()).getChild("Name_array");

    std::vector<std::string> jointsList = jointsNode.value.toStringVector();

    return jointsList;
}

std::vector<float> DAELoader::loadWeights(const XNode& skinningData) {
    const XNode& inputNode = skinningData.getChild("vertex_weights");
    std::string weightsDataId = inputNode.getNodeByAttribute("input", "semantic", "WEIGHT").getAttribute(
            "source").value.substr(1);
    XNode weightsNode = skinningData.getNodeByAttribute("source", "id", weightsDataId.c_str()).getChild("float_array");
    std::vector<float> weights = weightsNode.value.toFloatVector();

    return weights;
}

std::vector<int> DAELoader::getEffectiveJointsCounts(const XNode& node) {
    if(m_polygonType == POLYLIST) {
        return node.getChild("vcount").value.toIntegerVector();
    }

    // TODO: Triangle 형식일 때의 Joint 갯수를 작성해야 함
    return {};
}

std::vector<VertexSkinData*>
DAELoader::getSkinData(const XNode& weightsDataNode, const std::vector<int>& counts, std::vector<float> weights) const {
    auto rawData = weightsDataNode.getChild("v").value.toIntegerVector();
    auto weightsCount = std::stoi(weightsDataNode.getAttribute("count").value);
    std::vector<VertexSkinData*> skinningData;
    int pointer = 0;
    for (int index = 0; index < weightsCount; ++index) {
        auto* skinData = new VertexSkinData();
        short count = (m_polygonType == POLYLIST) ? counts[index] : static_cast<short>(m_polygonType);
        for (short i = 0; i < count; ++i) {
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

Joint* DAELoader::extractMainJointData(const XNode& jointNode, bool isRoot) {
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

Joint* DAELoader::loadJointData(const XNode& jointNode, bool isRoot) {
    Joint* joint = extractMainJointData(jointNode, isRoot);

    try {
        for (const XNode& childNode : jointNode.children) {
            if (childNode.name == "node") {
                joint->addChild(loadJointData(childNode, false));
            }
        }
    } catch (int error) {}

    return joint;
}

void DAELoader::LoadTexturePath(const XNode& imageNode) {
#ifdef __EMSCRIPTEN__
    if(!imageNode.hasChild("image")) return;
#endif
    std::string path = imageNode.getChild("image").getChild("init_from").value;
    path = trim(path);
    auto asset = CORE->GetCore(ResMgr)->GetAssetReference(path);
    if (asset == nullptr) return;

    LoadTexture(asset);
}

void DAELoader::ConvertDataToVectors(DAEMeshData* meshData) const {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texUVs;
    std::vector<short> jointIDs;
    std::vector<float> weights;

    {
        int size = meshData->vertices.size();
        vertices.resize(size * 3);
        normals.resize(size * 3);
        texUVs.resize(size * 2);
        if (m_isSkinning) {
            jointIDs.resize(size * 3);
            weights.resize(size * 3);
        }
    }

    float furthestPoint = 0;
    int vertices_size = meshData->vertices.size();
    for (int i = 0; i < vertices_size; ++i) {
        Vertex& currentVertex = *meshData->vertices[i];
        if (currentVertex.getLength() > furthestPoint) {
            furthestPoint = currentVertex.getLength();
        }
        vec3 position = currentVertex.getPosition();
        // vec2 textureCoord = textures.get(currentVertex.getTextureIndex());
        vec2 textureCoord = meshData->texUVs.at(currentVertex.getTextureIndex());
        vec3 normalVector = meshData->normals.at(currentVertex.getNormalIndex());
        vertices[i * 3] = position.x;
        vertices[i * 3 + 1] = position.y;
        vertices[i * 3 + 2] = position.z;
        texUVs[i * 2] = textureCoord.x;
        texUVs[i * 2 + 1] = 1 - textureCoord.y;
        normals[i * 3] = normalVector.x;
        normals[i * 3 + 1] = normalVector.y;
        normals[i * 3 + 2] = normalVector.z;

        if (!m_isSkinning) continue;

        VertexSkinData* weightsData = currentVertex.getWeightsData();
        jointIDs[i * 3] = weightsData->getJointIDs()[0];
        jointIDs[i * 3 + 1] = weightsData->getJointIDs()[1];
        jointIDs[i * 3 + 2] = weightsData->getJointIDs()[2];
        weights[i * 3] = weightsData->getWeights()[0];
        weights[i * 3 + 1] = weightsData->getWeights()[1];
        weights[i * 3 + 2] = weightsData->getWeights()[2];
    }
    AttachDataToObjSurface(meshData->vertices.size(), vertices, normals, texUVs, meshData->indices, jointIDs, weights,
                           meshData);
}

void
DAELoader::AttachDataToObjSurface(int vertices_size, std::vector<float> vertices, std::vector<float> normals,
                                  std::vector<float> texUVs, std::vector<int> indices, std::vector<short> jointIDs,
                                  std::vector<float> weights, DAEMeshData* meshData) {
    int sizeIndices = indices.size() / 3;
    const auto meshSurface = meshData->meshSurface;
    meshSurface->MakeVertices(vertices_size, &vertices[0], &normals[0], (texUVs.empty() ? nullptr : &texUVs[0]),
                              (weights.empty() ? nullptr : &weights[0]), (jointIDs.empty() ? nullptr : &jointIDs[0]));
    meshSurface->MakeIndices(sizeIndices, &indices[0]);
}

SPrefab* DAELoader::GeneratePrefab(Animation* animation, SPrefab* prefab) {
    if (prefab == nullptr)
        prefab = new SPrefab();
    auto* root = new SGameObject(m_name);
    prefab->SetGameObject(root);

    SGameObject* joint_root = nullptr;

    if (m_isSkinning) {
        joint_root = new SGameObject("Armature");
        root->AddChild(joint_root);
        DAEConvertSGameObject::CreateJoints(joint_root, m_skeletonData->getHeadJoint());
    }

    JointComponent* joint_root_component = m_isSkinning ? joint_root->GetChildren().front()->GetComponent<JointComponent>() : nullptr;
    for (const auto& mesh : m_meshList) {
        auto* mesh_root = new SGameObject(mesh->meshName);
        root->AddChild(mesh_root);
        DrawableStaticMeshComponent* mesh_component = nullptr;

        if (m_isSkinning) {
            mesh_component = mesh_root->CreateComponent<DrawableSkinnedMeshComponent>();
        } else {
            mesh_component = mesh_root->CreateComponent<DrawableStaticMeshComponent>();
        }

        mesh_component->SetMesh(*mesh->meshSurface);

        if (m_isSkinning)
            dynamic_cast<DrawableSkinnedMeshComponent*>(mesh_component)->SetRootJoint(
                    joint_root_component->GetGameObject(),
                    m_skeletonData->getJointCount());

        auto renderComponent = mesh_root->CreateComponent<RenderComponent>();
//    mesh_root->GetComponent<RenderComponent>()->SetShaderHandle("PBR.shader");
    }

    if (m_isSkinning) {
        SGameObject* animationObj = DAEConvertSGameObject::CreateAnimation(root, joint_root_component,
                                                                           m_animationLoader->GetAnimation(),
                                                                           m_name, animation);
    }

    return prefab;
}

SPrefab* DAELoader::GeneratePrefab(const char* path, Skeleton* skeleton, MeshSurface* mesh, Animation* animation,
                                   SPrefab* prefab) {
    auto* loader = new DAELoader(path, AUTO, false);
    auto asset = CORE->GetCore(ResMgr)->GetAssetReference(path);
    if (asset == nullptr) {
        SAFE_DELETE(loader);
        return nullptr;
    }
    std::string prefab_id = asset->id;

    loader->m_resource_id = asset->id;
    loader->m_skeletonData = skeleton;

    if (loader->m_skeletonData == nullptr) {
        loader->m_skeletonData = new Skeleton();
    }

    const auto& meshData = loader->m_meshList;
    for (const auto& mesh : meshData) {
        mesh->meshSurface->LinkResource(prefab_id + "." + mesh->meshName +"?mesh");
    }
    loader->m_skeletonData->LinkResource(prefab_id + "?skeleton");

    loader->Load(path, AUTO);
    prefab = loader->GeneratePrefab(animation, prefab);

    SAFE_DELETE(loader);
    return prefab;
}
