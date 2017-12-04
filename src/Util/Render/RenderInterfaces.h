#pragma once
#include <vector>
#include "../../SObject.h"

enum VertexFlags {
	VertexFlagsNone,
	VertexFlagsNormals,
	VertexFlagsTexCoords,
};

class SISurface : public SObject {
public:
	
	SISurface() {}
	virtual ~SISurface() {}

	virtual int GetVertexCount() const = 0;
	virtual int GetLineIndexCount() const = 0;
	virtual int GetTriangleIndexCount() const = 0;
	virtual void GenerateVertices(std::vector<float>& vertices, unsigned char flags = 0) const = 0;
	virtual void GenerateLineIndices(std::vector<unsigned short>& indices) const = 0;
	virtual void GenerateTriangleIndices(std::vector<unsigned short>& indices) const = 0;

};

class SIRender {
public:
	SIRender() {}
	virtual ~SIRender() {}

	virtual void Render() = 0;

};
