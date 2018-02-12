class TestScript extends SCEngineScript {

	materialComp = null;
	lightComp = null;
	ambient = null;

	function Init(){
		materialComp = gameObject.GetComponent<MaterialComponent>();
		lightComp = gameObject.GetComponent<LightComponent>();
		ambient = vec3();
		ambient.x = 1;
		ambient.y = 0;
		ambient.z = 0;

		Log("light component : " + lightComp);			//게임 오브젝트에 존재하지 않으므로 null
		Log("material component : " + materialComp);	//게임 오브젝트에 존재하므로 주소값이 출력된다.

	}

	function Tick(elapsedTime) {
		materialComp.SetAmbient(ambient);

		gameObject.GetTransform().rotation.y += 0.5;

	}

}