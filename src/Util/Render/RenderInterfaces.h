#pragma once
#include <vector>
#include "../../SObject.h"
#include "../../Manager/ResMgr.h"
#include "../Matrix.h"
#include "GLMeshID.h"

namespace CSE {
    struct CameraMatrixStruct;
    class SMaterial;
    class CameraBase;
    class RenderMgr;

    class SISurface : public SResource {
    public:

        SISurface() {
            SetUndestroyable(true);
        }

        ~SISurface() override = default;

        virtual int GetVertexCount() const = 0;

        virtual int GetLineIndexCount() const = 0;

        virtual int GetTriangleIndexCount() const = 0;

        virtual void GenerateVertices(std::vector<float>& vertices, unsigned char flags = 0) const = 0;

        virtual void GenerateLineIndices(std::vector<unsigned short>& indices) const = 0;

        virtual void GenerateTriangleIndices(std::vector<unsigned short>& indices) const = 0;

    public:
        mutable GLMeshID m_meshId;
    };

    class SIRender {
    public:
        SIRender() = default;

        virtual ~SIRender() = default;

        virtual void
        SetMatrix(const CameraMatrixStruct& cameraMatrixStruct,
                  const GLProgramHandle* handle = nullptr) = 0;

        virtual void Render(const GLProgramHandle* handle = nullptr) const = 0;

        virtual SMaterial* GetMaterial() const = 0;

        SMaterial* GetMaterialReference() const {
            return material;
        }

    protected:
        SMaterial* material = nullptr;
    public:
        bool isRenderActive = false;
    };
}