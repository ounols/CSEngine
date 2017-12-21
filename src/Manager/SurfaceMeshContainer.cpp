#include "SurfaceMeshContainer.h"


SurfaceMeshContainer::SurfaceMeshContainer() {
	SetUndestroyable(true);
}


SurfaceMeshContainer::~SurfaceMeshContainer() {}


void SurfaceMeshContainer::RegisterProgram(SISurface* m_surface) {
	m_surfaces.push_back(m_surface);
	m_size++;
}


void SurfaceMeshContainer::Exterminate() {
}
