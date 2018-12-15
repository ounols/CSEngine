#pragma once
class SISComponent {
public:

	SISComponent() {}

	virtual ~SISComponent() {}

	virtual void Init() = 0;
	virtual void Tick(float elapsedTime) = 0;

};

