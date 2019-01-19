#pragma once

#include "DAEUtil/VertexSkinData.h"
#include "DAEUtil/SkinningData.h"
#include "../XML/XML.h"
#include "DAEUtil/Vertex.h"
#include "ObjSurface.h"

class DAELoader {
public:
    DAELoader(const char* path, ObjSurface* obj);

    ~DAELoader();

    void Load(const char* path);

    ObjSurface* GetObject() const {
        return m_obj;
    }

private:
    void LoadSkin(XNode root_s);

    void LoadGeometry(XNode root_g);

    void ReadPositions(XNode data, std::vector<VertexSkinData*> vertexWeight);

    void ReadNormals(XNode data);

    void AssembleVertices(XNode data);

    Vertex* processVertex(int posIndex, int normIndex, int texIndex);

    Vertex* dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex);

    void removeUnusedVertices();

    void ConvertDataToVectors();

    std::vector<std::string> loadJointsList(XNode skinningData);

    std::vector<float> loadWeights(XNode skinningData);

    std::vector<int> getEffectiveJointsCounts(XNode node);

    std::vector<VertexSkinData*>
    getSkinData(XNode weightsDataNode, std::vector<int> counts, std::vector<float> weights);

    void AttachDataToObjSurface();

    void Exterminate();

private:
    const XNode* m_root;
    ObjSurface* m_obj;

    std::vector<Vertex*> m_vertices;
    std::vector<vec3> m_normals;
    std::vector<int> m_indices;

    std::vector<float> m_f_vertices;
    std::vector<float> m_f_normals;
    std::vector<float> m_f_texUVs;
    std::vector<int> m_f_jointIDs;
    std::vector<float> m_f_weights;

    SkinningData* m_skinningData;

    int m_maxWeight = 3;

    bool m_isSkinning = true;

};