#pragma once
class SISComponent {
public:

	SISComponent() {}
	SISComponent(const SISComponent& src) {}

	virtual ~SISComponent() {}

	virtual void Start() = 0;
	virtual void Init() = 0;
	virtual void Tick(float elapsedTime) = 0;

};

