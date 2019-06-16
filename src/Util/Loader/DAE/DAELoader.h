#pragma once

#include "Object/SPrefab.h"
#include "DAEUtil/VertexSkinData.h"
#include "DAEUtil/SkinningData.h"
#include "../../Animation/Skeleton.h"
#include "../XML/XML.h"
#include "DAEUtil/Vertex.h"
#include "../../Render/MeshSurface.h"
#include "DAEAnimationLoader.h"

class DAELoader {
public:
    enum LOAD_TYPE {
        ALL, MESH, ANIMATION, NOTHING
    };
public:
    DAELoader(const char* path, MeshSurface* obj, LOAD_TYPE type = ALL);

    ~DAELoader();

    void Load(const char* path, LOAD_TYPE type);
    void LoadTexture(const AssetMgr::AssetReference* asset);

    MeshSurface* GetMesh() const {
        return m_obj;
    }

    Skeleton* getSkeleton() const {
        return m_skeletonData;
    }

    SPrefab* GeneratePrefab();

private:
    void LoadSkin(XNode root_s);

    void LoadSkeleton(XNode root_s);

    void LoadGeometry(XNode root_g);

//===================================================================
// GeometryLoader Functions
//===================================================================
    void ReadPositions(XNode data, std::vector<VertexSkinData*> vertexWeight);

    void ReadNormals(XNode data, std::string normalsId);

    void ReadUVs(XNode data, std::string texCoordsId);

    void AssembleVertices(XNode data);

    Vertex* processVertex(int posIndex, int normIndex, int texIndex);

    Vertex* dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex);

    void removeUnusedVertices();

    void ConvertDataToVectors();

//===================================================================
// SkinLoader Functions
//===================================================================
    std::vector<std::string> loadJointsList(XNode skinningData);

    std::vector<float> loadWeights(XNode skinningData);

    std::vector<int> getEffectiveJointsCounts(XNode node);

    std::vector<VertexSkinData*>
    getSkinData(XNode weightsDataNode, std::vector<int> counts, std::vector<float> weights);

//===================================================================
// SkeletonLoader Functions
//===================================================================

    Joint* loadJointData(XNode jointNode, bool isRoot);
    Joint* extractMainJointData(XNode jointNode, bool isRoot);

    void LoadTexturePath(XNode imageNode);


    void AttachDataToObjSurface();

    void Exterminate();

private:
    const XNode* m_root{};
    MeshSurface* m_obj;

    std::vector<Vertex*> m_vertices;
    std::vector<vec3> m_normals;
    std::vector<vec2> m_texUVs;
    std::vector<int> m_indices;

    std::vector<float> m_f_vertices;
    std::vector<float> m_f_normals;
    std::vector<float> m_f_texUVs;
    std::vector<int> m_f_jointIDs;
    std::vector<float> m_f_weights;

    SkinningData* m_skinningData = nullptr;
    Skeleton* m_skeletonData = nullptr;
    DAEAnimationLoader* m_animationLoader = nullptr;


    int m_maxWeight = 3;
    int m_jointSize = 0;

    bool m_isSkinning = false;

    std::string m_name;
    std::string m_texture_name;

};