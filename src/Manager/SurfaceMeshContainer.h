#pragma once
#include "../SObject.h"
#include <vector>
#include "Base/SContainer.h"

class SISurface;

class SurfaceMeshContainer : public SObject, public SContainer<SISurface*> {
public:
	SurfaceMeshContainer();
	~SurfaceMeshContainer();


	void Exterminate() override;


};

