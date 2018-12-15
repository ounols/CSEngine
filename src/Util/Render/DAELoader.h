#pragma once
#include "../XML/XML.h"
#include "DAEUtill/Vertex.h"
#include "ObjSurface.h"

class DAELoader{
public:
    DAELoader(const char* path, ObjSurface* obj);
    ~DAELoader();

    void Load(const char* path);
    ObjSurface* GetObject() const {
        return m_obj;
    }

private:
    void LoadGeometry(XNode root_g);
    void ReadPositions(XNode data);
    void ReadNormals(XNode data);

    void AssembleVertices(XNode data);
    Vertex* processVertex(int posIndex, int normIndex, int texIndex);
    Vertex* dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex);

    void removeUnusedVertices();

    void ConvertDataToVectors();

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
};