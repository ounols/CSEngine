#pragma once
#include "../Manager/Base/Scene.h"
#include "../Object/SGameObject.h"
#include "../Util/Render/MeshSurface.h"

class WebDemoScene : public CSE::Scene {
public:
    WebDemoScene() = default;
    ~WebDemoScene() override = default;

    void Init() override;
    void Tick(float elapsedTime) override;
    void Destroy() override;
};
