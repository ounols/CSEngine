#pragma once
#include "../Macrodef.h"
#include "Base/SContainer.h"

#define RESMGR ResMgr::getInstance()

class SObject;
class GLProgramHandle;
class SISurface;

class ResMgr {
private:
	~ResMgr();

public:
	DECLARE_SINGLETONE(ResMgr);

	void Init();
	void Exterminate();

	template <class CONTAINER, class TYPE>
	void Register(TYPE* m_object) const;

	GLProgramHandle* getShaderProgramHandle(int id) const;
	SISurface* GetSurfaceMesh(int id) const;
	template <class CONTAINER, class TYPE>
	TYPE* GetObject(int id) const;


	template <class CONTAINER, class TYPE>
	int GetSize() const;


	template <class CONTAINER, class TYPE>
	bool IsEmpty() const;


private:
	//ShaderProgramContainer* m_programContainer;
	//SurfaceMeshContainer* m_surfaceMeshContainer;
	std::vector<SIContainer*> m_containers;
};


template <class CONTAINER, class TYPE>
void ResMgr::Register(TYPE* m_object) const {

	for(auto container : m_containers) {
		if(dynamic_cast<CONTAINER*>(container)) {
			static_cast<SContainer<TYPE*>*>(container)->Register(m_object);
			return;
		}
	}

}


template <class CONTAINER, class TYPE>
TYPE* ResMgr::GetObject(int id) const {
	for (auto container : m_containers) {
		if (dynamic_cast<CONTAINER*>(container)) {
			return static_cast<SContainer<TYPE*>*>(container)->Get(id);
		}
	}

	return nullptr;
}


template <class CONTAINER, class TYPE>
int ResMgr::GetSize() const {
	for (auto container : m_containers) {
		if (dynamic_cast<CONTAINER*>(container)) {
			return static_cast<SContainer<TYPE*>*>(container)->getSize();
		}
	}

	return 0;
}


template <class CONTAINER, class TYPE>
bool ResMgr::IsEmpty() const {
	for (auto container : m_containers) {
		if (dynamic_cast<CONTAINER*>(container)) {
			auto container_casted = static_cast<SContainer<TYPE*>*>(container);
			return container_casted->getSize() == 0;
		}
	}

	return true;

}
