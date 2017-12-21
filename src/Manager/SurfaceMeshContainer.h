#pragma once
#include "../SObject.h"
#include <vector>

class SISurface;

class SurfaceMeshContainer : public SObject {
public:
	SurfaceMeshContainer();
	~SurfaceMeshContainer();

	void RegisterProgram(SISurface* m_surface);

	void Exterminate() override;

	SISurface* GetSurfaceMesh(int id) {
		return m_surfaces[id];
	}

	int getSize() const {
		return m_size;
	}

private:
	std::vector<SISurface*> m_surfaces;
	int m_size = 0;

};

