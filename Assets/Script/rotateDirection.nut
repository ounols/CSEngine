class RotateDirection extends CSEngineScript {

    lightComp = null;
    startAngle = 0.0

	function Init(){
	    lightComp = gameobject.GetComponent<LightComponent>();
	}

	function Tick(elapsedTime) {
        local direction = vec4();
        local v = startAngle + elapsedTime * 0.001;
        direction.Set(sin(v), 0.3, cos(v), 1);
        lightComp.SetDirection(direction);
	}
}