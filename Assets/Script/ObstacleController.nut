class ObstacleController extends CSEngineScript {
    fallSpeed = 1.5;
    deleteY = -2.5;
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
        
        // Calculate delta time
        local deltaTime = elapsedTime - lastElapsedTime;
        lastElapsedTime = elapsedTime;
        
        // 아래로 이동
        local transform = GetTransform();
        local pos = transform.position;
        pos.y -= fallSpeed * deltaTime * 0.001;
        transform.position = pos;
        
        // 화면 밖으로 나가면 제거
        if (pos.y < deleteY) {
            Deactivate();
            gameManager.OnObstaclePassed();
        }
    }
    
    function Activate(spawnX, spawnY) {
        local transform = GetTransform();
        local pos = transform.position;
        pos.x = spawnX;
        pos.y = spawnY;
        pos.z = 0.0;
        transform.position = pos;
        
        isActive = true;
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

