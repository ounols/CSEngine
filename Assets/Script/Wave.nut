class Wave extends CSEngineScript {

    center_object = null;
    y_up = null;

    function Init() {
        // 센터 오브젝트를 받아옵니다
        center_object = gameobject.Find("center object");
        // up벡터를 생성합니다. (자체 제작 엔진은 y-up입니다.)
        y_up = vec3();
        y_up.Set(0, 1, 0);
    }

    function Tick(elapsedTime) {
        local move_value = vec3();
        move_value.Set(0.1, -0.2 + cos(elapsedTime*0.001) * 0.1, -0.3);

        // 양옆으로 왔다갔다하는 vec3를 포지션값으로 넣습니다.
        GetTransform().position = move_value;

        // 로그를 출력합니다.
        //LoggingResults();
    }

    function LoggingResults() {
        // 시선 벡터와 오브젝트의 벡터를 받아옵니다/
        local target_vec = GetTransform().position;
        local object_vec = center_object.GetTransform().position;

        // 외적 후 Up벡터를 이용해 내적을 진행합니다.
        local cross_result = target_vec.Cross(object_vec);
        local dot_result = cross_result.Dot(y_up);

        // 결과값을 출력합니다.
        Log("[Result] 오브젝트는 " + (dot_result > 0 ? "왼쪽에 있습니다." : "오른쪽에 있습니다."));
    }
}