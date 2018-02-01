#pragma once

#include <vector>

class SIContainer {
public:
	SIContainer() {}
	virtual ~SIContainer(){}
};

template<class T>
class SContainer : public SIContainer {
public:
	typedef SContainer<T> BaseSContainer;
public:
	SContainer() {
	}


	virtual ~SContainer() {
	}


	virtual void Register(T object) {
		m_objects.push_back(object);
		m_size++;
	}

	T Get(int index) const {
		if (index < 0 || index > m_size || m_size < 1) return nullptr;
		return m_objects[index];
	}

	int getSize() const {
		return m_size;
	}


protected:
	std::vector<T> m_objects;
	int m_size = 0;
};

