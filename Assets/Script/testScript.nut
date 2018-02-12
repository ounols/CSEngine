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

		Log("light component : " + lightComp);			//���� ������Ʈ�� �������� �����Ƿ� null
		Log("material component : " + materialComp);	//���� ������Ʈ�� �����ϹǷ� �ּҰ��� ��µȴ�.

	}

	function Tick(elapsedTime) {
		materialComp.SetAmbient(ambient);

		gameObject.GetTransform().rotation.y += 0.5;

	}

}