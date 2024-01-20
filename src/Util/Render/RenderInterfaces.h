#pragma once
#include <vector>
#include "../../SObject.h"
#include "../../Manager/ResMgr.h"
#include "../Matrix.h"
#include "GLMeshID.h"

#define SURFACE_CONSTRUCTOR(CLASSNAME) \
namespace __REFELCTION_DUMP__ { namespace CLASSNAME {                    \
unsigned char* __CSE_REFLECTION_DUMP__ = \
CSE::ReflectionMgr::DefineWrapper::SetDefine(#CLASSNAME, []() { return new CSE::CLASSNAME(); });}} \
CSE::CLASSNAME::CLASSNAME() : CSE::SISurface(#CLASSNAME)

#define SURFACE_SUB_CONSTRUCTOR(CLASSNAME, ...) \
CSE::CLASSNAME::CLASSNAME(__VA_ARGS__) : CSE::SISurface(#CLASSNAME)

namespace CSE {
    struct CameraMatrixStruct;
    class SMaterial;
    class CameraBase;
    class RenderMgr;

    class SISurface : public SResource {
    public:

        explicit SISurface(std::string&& classType) : SResource(classType) {
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
                  const GLProgramHandle* handle) = 0;

        virtual void Render(const GLProgramHandle* handle) const = 0;

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