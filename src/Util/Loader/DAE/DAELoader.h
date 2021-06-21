#pragma once

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
        void LoadSkin(XNode root_s);

        void LoadSkeleton(XNode root_s);

        void LoadGeometry(XNode root_g, DAEMeshData* meshData);

//===================================================================
// GeometryLoader Functions
//===================================================================
        void ReadPositions(XNode data, std::vector<VertexSkinData*> vertexWeight, DAEMeshData* meshData);

        void ReadNormals(XNode data, std::string normalsId, DAEMeshData* meshData);

        void ReadUVs(XNode data, std::string texCoordsId, DAEMeshData* meshData);

        void AssembleVertices(XNode data, DAEMeshData* meshData);

        Vertex* processVertex(int posIndex, int normIndex, int texIndex, DAEMeshData* meshData);

        Vertex* dealWithAlreadyProcessedVertex(Vertex* previousVertex, int newTextureIndex, int newNormalIndex,
                                               DAEMeshData* meshData);

        void removeUnusedVertices(DAEMeshData* meshData);

        void ConvertDataToVectors(DAEMeshData* meshData);

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


        void AttachDataToObjSurface(int vertices_size, std::vector<float> vertices, std::vector<float> normals,
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

        std::string m_name;
        std::string m_texture_name;

        std::string m_resource_id;

    };
}