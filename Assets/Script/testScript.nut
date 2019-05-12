class TestScript extends SCEngineScript {

	materialComp = null;
	lightComp = null;
	ambient = null;

	function Init(){
		materialComp = gameobject.GetComponent<MaterialComponent>();
		lightComp = gameobject.GetComponent<LightComponent>();
		
		ambient = vec3();
		ambient.Set(0.3, 0.3, 0);
		local diffuse = vec4();
		diffuse.Set(1, 1, 0.5, 1);
		if(materialComp != null) {
			materialComp.SetRoughness(0.025);
			materialComp.SetMetallic(0.5);
			//materialComp.SetDiffuse(diffuse);
			//materialComp.SetAmbient(ambient);
		}


		
		Log("light component : " + lightComp);			//???? ????????? ???????? ??????? null
		Log("material component : " + materialComp);	//???? ????????? ???????? ?????? ??��??.

		gameobject.SetName("test");
	}

	function Tick(elapsedTime) {
//		materialComp.SetAmbient(ambient);

		local angle = gameobject.GetTransform().rotation.ToEulerAngle();
//		Log("(" +angle.x + ", " + angle.y + ", " + angle.z + ")");

		local axis = vec3();
		axis.Set(0, 1, 0);
		// rotation = Quaternion.AngleAxis(axis, 1);
		gameobject.GetTransform().rotation.Rotate(Quaternion.AngleAxis(axis, 0.01));
	}

	//로테이션을 받아오는 임의의 함수
	function GetRotation() {
		return gameobject.GetTransform().rotation;
	}
}