class TestScript2 extends CSEngineScript {

    meshObj = null;
	materialComp = null;
    testScript = null;

    count = 3;

    function Init() {
		materialComp = gameobject.GetComponent<RenderComponent>();

		//재질 컴포넌트에 주변광을 노란색 (255, 255, 0)으로 설정
        if(materialComp != null) {
			local diffuse = vec3();
			diffuse.Set(1, 0, 0);
			local material = materialComp.GetMaterial();
            material.SetFloat("FLOAT_ROUGHNESS", 0.2);
            material.SetFloat("FLOAT_METALLIC", 0.2);
            material.SetVec3("FLOAT_ALBEDO", diffuse);
            //materialComp.SetDiffuse(diffuse);
            //materialComp.SetAmbient(ambient);
        }
        
        //test라는 오브젝트에 있는 TestScript를 가져온다.
        testScript = gameobject.Find("test").GetClass<TestScript>();
		Log("this is materialComp : " + materialComp);
    }

    function Tick(elapsedTime) {
        //이 오브젝트의 회전값을 TestScript의 회전값으로 설정한다.
        GetTransform().rotation = testScript.GetRotation();
		Log("count : " + count);
//        if(materialComp != null) {
//            local value = sin(count);
//            material.SetFloat("FLOAT_ROUGHNESS", value);
//            Log("value : " + value);
//        }
//        count += 0.01;
        /*
        test{
        }
        }}}
         */
    }/*
*/

	function GetCount() {
		return count;
	}
    test_int = "this is test string!! 문자열이다 이거슨";
}