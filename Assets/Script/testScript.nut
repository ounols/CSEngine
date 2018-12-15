class TestScript extends SCEngineScript {

	materialComp = null;
	lightComp = null;
	ambient = null;

	function Init(){
		materialComp = gameobject.GetComponent<MaterialComponent>();
		lightComp = gameobject.GetComponent<LightComponent>();
		
		ambient = vec3();
		ambient.Set(1, 0, 0);

		
		Log("light component : " + lightComp);			//???? ????????? ???????? ??????? null
		Log("material component : " + materialComp);	//???? ????????? ???????? ?????? ??��??.

		gameobject.SetName("test");
	}

	function Tick(elapsedTime) {
		materialComp.SetAmbient(ambient);

		gameobject.GetTransform().rotation.y += 0.5;
	}

	//로테이션을 받아오는 임의의 함수
	function GetRotation() {
		return gameobject.GetTransform().rotation;
	}
}