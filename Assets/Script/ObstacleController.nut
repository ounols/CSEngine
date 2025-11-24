class ObstacleController extends CSEngineScript {
    fallSpeed = 2.0;  // 낙하 속도 (단위/초)
    deleteY = -3.0;   // 삭제 Y 위치
    gameManager = null;
    isActive = false;
    collisionRadius = 0.3;
    lastElapsedTime = 0.0; // For delta time calculation
    
    function Init() {
        gameManager = gameobject.Find("GameManager").GetClass<GameManager>();
        isActive = false;
        // 비활성 위치로 이동 (렌더링은 유지)
        local transform = GetTransform();
        local pos = transform.position;
        pos.x = 0.0;
        pos.y = 100.0;
        pos.z = 0.0;
        transform.position = pos;
        Log("[ObstacleController] Initialized at position (0, 100, 0)");
    }
    
    function Tick(elapsedTime) {
        if (!isActive) return;
        
        // Calculate delta time (밀리초 단위)
        local deltaTime = elapsedTime - lastElapsedTime;
        
        // 첫 프레임이거나 deltaTime이 비정상적으로 큰 경우 처리
        if (lastElapsedTime == 0.0 || deltaTime > 100.0) {
            lastElapsedTime = elapsedTime;
            deltaTime = 16.0; // 기본 16ms (60fps 기준)
        } else {
            lastElapsedTime = elapsedTime;
        }
        
        // 아래로 이동 (deltaTime을 초 단위로 변환: * 0.001)
        local transform = GetTransform();
        local pos = transform.position;
        pos.y -= fallSpeed * deltaTime * 0.001;
        transform.position = pos;
        
        // 화면 밖으로 나가면 제거
        if (pos.y < deleteY) {
            Deactivate();
            if (gameManager != null) {
                gameManager.OnObstaclePassed();
            }
        }
    }
    
    function Activate(spawnX, spawnY) {
        // Transform 위치 설정
        local transform = GetTransform();
        local pos = transform.position;
        pos.x = spawnX;
        pos.y = spawnY;
        pos.z = 0.0;
        transform.position = pos;
        
        // 활성화 상태 설정
        isActive = true;
        
        // deltaTime 초기화 (첫 프레임 문제 방지)
        lastElapsedTime = 0.0;
        
        // Enable the game object so Tick runs
        gameobject.SetEnable(true);
        
        Log("[ObstacleController] Activated at (" + spawnX + ", " + spawnY + ")");
    }
    
    function Deactivate() {
        isActive = false;
        // Disable the game object to stop Tick
        gameobject.SetEnable(false);
        // 화면 밖으로 이동
        local transform = GetTransform();
        local pos = transform.position;
        pos.x = 0.0;
        pos.y = 100.0;
        pos.z = 0.0;
        transform.position = pos;
        Log("[ObstacleController] Deactivated");
    }
    
    function GetPosition() {
        return GetTransform().position;
    }
    
    function GetCollisionRadius() {
        return collisionRadius;
    }
    
    function IsActive() {
        return isActive;
    }
}

