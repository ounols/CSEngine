#pragma once
#include <vector>
#include <algorithm>
#include "../SObject.h"
#include "../Manager/MemoryMgr.h"
#include "../Util/Interface/TransformInterface.h"
#include "../Component/SComponent.h"

class SComponent;

class SGameObject : public SObject {
public:
	SGameObject();
	explicit SGameObject(std::string name);
	~SGameObject();

	virtual void Init();
	virtual void Tick(float elapsedTime);
	virtual void Exterminate() override;
	/**
	 * \brief 자동 삭제가 아닌 특정한 상황에서 삭제될 때 호출되는 함수
	 */
	void Destroy();

	/**
	 * \brief 컴포넌트를 이 오브젝트에 추가합니다.
	 * \param component 추가할 오브젝트
	 */
	void AddComponent(SComponent* component);
	template <class T>
	T* GetComponent();
	//template <class T>
	//bool DeleteComponent();
	bool DeleteComponent(SComponent* component);
	template <class T>
	T* CreateComponent();

	std::string GetName() const {
		return m_name;
	}

	void SetName(std::string name) {
		m_name = name;
	}

	TransformInterface* GetTransform() const {
		return m_transform;
	}


	bool GetIsEnable() const;
	void SetIsEnable(bool is_enable);
private:
	void UpdateComponent(float elapsedTime);


private:
	std::vector<SComponent*> m_components;
	std::string m_name;
	TransformInterface* m_transform;
	bool isEnable = true;
};


template <class T>
T* SGameObject::GetComponent() {
	for (auto component : m_components) {
		if (component == nullptr) continue;
		if (dynamic_cast<T*>(component)) {
			return static_cast<T*>(component);
		}
	}

	return nullptr;
}

//로직 및 문법 상 문제가 제기됨
//template <class T>
//bool SGameObject::DeleteComponent() {
//	for (auto component : m_components) {
//		if (component == nullptr) continue;
//		
//		if (dynamic_cast<T*>(component)) {
//			
//			auto iCompObj = std::find(m_components.begin(), m_components.end(), component);
//
//			if(iCompObj != m_components.end()) {
//				m_components.erase(iCompObj);
//				//MemoryMgr::getInstance()->ReleaseObject((SObject*)component);
//			}
//			
//
//			return true;
//		}
//	}
//
//	return false;
//}


template <class T>
T* SGameObject::CreateComponent() {

	T* component = new T();
	AddComponent(component);
	component->Init();
	return component;

}
