#pragma once

#include <iostream>
#include "../../../Object/SPrefab.h"
#include "DAEUtil/VertexSkinData.h"
#include "DAEUtil/SkinningData.h"
#include "../../Animation/Skeleton.h"
#include "../XML/XML.h"
#include "DAEUtil/Vertex.h"
#include "../../Render/MeshSurface.h"
#include "DAEAnimationLoader.h"
#include "../../Animation/Animation.h"

namespace CSE {

    class Animation;

    class DAELoader {
    public:
        enum LOAD_TYPE {
            ALL, MESH, ANIMATION, AUTO, NOTHING
        };
    private:
        class DAEMeshData {
        public:
            DAEMeshData() {
                meshSurface = new MeshSurface();
            };
            ~DAEMeshData() {
                std::cout << "\ndeleting " << vertices.size() << " DAE Vertexes...\n";
                for (auto vertex : vertices) {
                    SAFE_DELETE(vertex);
                }
                vertices.clear();
            }

            MeshSurface* meshSurface;
            std::vector<Vertex*> vertices;
            std::vector<vec3> normals;
            std::vector<vec2> texUVs;
            std::vector<int> indices;
            std::string meshName;
        };
        enum POLYGON_TYPE {
            POLYLIST = -1, TRIANGLES = 3,
        };
    public:
        DAELoader(const char* path, LOAD_TYPE type, bool isLoad);

        ~DAELoader();

        void Load(const char* path, LOAD_TYPE type);

        void LoadTexture(const AssetMgr::AssetReference* asset);

        Skeleton* getSkeleton() const {
            return m_skeletonData;
        }

        SPrefab* GeneratePrefab(Animation* animation, SPrefab* prefab = nullptr);

        static SPrefab* GeneratePrefab(const char* path, Skeleton* skeleton, MeshSurface* mesh, Animation* animation,
                                       SPrefab* prefab = nullptr);

    private:
        bool LoadSkin(const XNode& root_s);

        bool LoadSkeleton(const XNode& root_s);

        bool LoadGeometry(const XNode& root_g, DAEMeshData* meshData);

//===================================================================
// GeometryLoader Functions
//===================================================================
        void ReadPositions(const XNode& data, std::vector<VertexSkinData*> vertexWeight, DAEMeshData* meshData) const;

        static void ReadNormals(const XNode& data, const std::string& normalsId, DAEMeshData* meshData);

        static void ReadUVs(const XNode& data, const std::string& texCoordsId, DAEMeshData* meshData);

        void AssembleVertices(const XNode& data, DAEMeshData* meshData);

        Vertex* processVertex(int posIndex, int normIndex, int texIndex, DAEMeshData* meshData);

        Vertex* dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex,
                                               DAEMeshData* meshData);

        static void removeUnusedVertices(DAEMeshData* meshData);

        void ConvertDataToVectors(DAEMeshData* meshData) const;

//===================================================================
// SkinLoader Functions
//===================================================================
        static std::vector<std::string> loadJointsList(const XNode& skinningData);

        static std::vector<float> loadWeights(const XNode& skinningData);

        std::vector<int> getEffectiveJointsCounts(const XNode& node);

        std::vector<VertexSkinData*>
        getSkinData(const XNode& weightsDataNode, const std::vector<int>& counts, std::vector<float> weights) const;

//===================================================================
// SkeletonLoader Functions
//===================================================================

        Joint* loadJointData(const XNode& jointNode, bool isRoot);

        Joint* extractMainJointData(const XNode& jointNode, bool isRoot);

        void LoadTexturePath(const XNode& imageNode);


        static void AttachDataToObjSurface(int vertices_size, std::vector<float> vertices, std::vector<float> normals,
                                    std::vector<float> texUVs, std::vector<int> indices, std::vector<short> jointIDs,
                                    std::vector<float> weights, DAEMeshData* meshData);

        void Exterminate();

    private:
        const XNode* m_root{};

        std::vector<DAEMeshData*> m_meshList;


        SkinningData* m_skinningData = nullptr;
        Skeleton* m_skeletonData = nullptr;
        DAEAnimationLoader* m_animationLoader = nullptr;


        int m_maxWeight = 3;
        int m_jointSize = 0;

        bool m_isSkinning = false;
        POLYGON_TYPE m_polygonType = POLYGON_TYPE::POLYLIST;

        std::string m_name;
        std::string m_texture_name;

        std::string m_resource_id;

    };
}