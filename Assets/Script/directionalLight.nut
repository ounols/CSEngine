class directionalLight extends CSEngineScript {

    //[public]//
    lightComp = null;
    diffuse = null;
    count = 0.0;

    function Init() {
        lightComp = gameobject.GetComponent<LightComponent>();
        Log("count = " + count);
    }

    function Tick(elapsedTime) {
        count += 0.01;
        local direction = vec4();
        local a = sin(count);
        direction.Set(sin(count), cos(count), 0.3, 1);
        lightComp.SetDirection(direction);
    }

}