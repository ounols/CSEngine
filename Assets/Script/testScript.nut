class TestScript extends CSEngineScript {

	renderComp = null;
	lightComp = null;
	ambient = null;

	function Init(){
		renderComp = gameobject.GetComponent<RenderComponent>();
		lightComp = gameobject.GetComponent<LightComponent>();

		local diffuse = vec3();
		diffuse.Set(0, 1, 0);
		if(renderComp != null) {
		    local material = renderComp.GetMaterial();
			material.SetFloat("FLOAT_ROUGHNESS", 0.4);
			material.SetFloat("FLOAT_METALLIC", 0.4);
			material.SetVec3("FLOAT_ALBEDO", diffuse);
			//materialComp.SetDiffuse(diffuse);
			//materialComp.SetAmbient(ambient);
		}


		
		Log("light component : " + lightComp);			//???? ????????? ???????? ??????? null

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