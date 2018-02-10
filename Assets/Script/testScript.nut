class SCEngineScript {
	//SCEngineComponent
	SCEngine = null;
	gameobject = null;

	function SetSCEngine(component){
		SCEngine = component;
		gameobject = SCEngine.GetGameObject();
	}

	
}

class TestScript extends SCEngineScript {

	function Tick(elapsedTime) {
		gameobject.GetTransform().rotation.y += 0.5;
	}

}