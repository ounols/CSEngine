class TestScript2 extends CSEngineScript {

    meshObj = null;
	materialComp = null;
    testScript = null;

    count = 0;

    function Init() {
		materialComp = gameobject.GetComponent<MaterialComponent>();

		//재질 컴포넌트에 주변광을 노란색 (255, 255, 0)으로 설정
        local diffuse = vec3();
        diffuse.Set(1, 1, 0.3);
        if(materialComp != null) {
            materialComp.SetRoughness(0.2);
            materialComp.SetMetallic(0.2);
            materialComp.SetAlbedo(diffuse);
            //materialComp.SetDiffuse(diffuse);
            //materialComp.SetAmbient(ambient);
        }
        
        //test라는 오브젝트에 있는 TestScript를 가져온다.
        testScript = gameobject.Find("test").GetClass<TestScript>();
    }

    function Tick(elapsedTime) {
        //이 오브젝트의 회전값을 TestScript의 회전값으로 설정한다.
        GetTransform().rotation = testScript.GetRotation();
//        if(materialComp != null) {
//            local value = sin(count);
//            materialComp.SetRoughness(value);
//            Log("value : " + value);
//        }
//        count += 0.01;
    }


}