class PlayerController extends CSEngineScript {
    moveSpeed = 2.0;
    minX = -2.0;
    maxX = 2.0;
    gameManager = null;
    isAlive = true;
    collisionRadius = 0.4;
    lastElapsedTime = 0.0; // For delta time calculation
    
    function Init() {
        Log("PlayerController initialized");
        gameManager = gameobject.Find("GameManager").GetClass<GameManager>();
    }
    
    function Tick(elapsedTime) {
        if (!isAlive) return;
        
        // Calculate delta time
        local deltaTime = elapsedTime - lastElapsedTime;
        lastElapsedTime = elapsedTime;
        
        // 입력 처리
        local move = 0.0;
        if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.Left)) {
            move = -1.0;
        }
        if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.Right)) {
            move = 1.0;
        }
        
        // 이동
        if (move != 0.0) {
            local transform = GetTransform();
            local pos = transform.position;
            pos.x += move * moveSpeed * deltaTime * 0.001;
            
            // 범위 제한
            if (pos.x < minX) pos.x = minX;
            if (pos.x > maxX) pos.x = maxX;
            
            transform.position = pos;
        }
        
        // 충돌 체크는 GameManager에서 수행
    }
    
    function OnCollision() {
        isAlive = false;
        Log("Player Hit! Game Over!");
        gameManager.GameOver();
    }
    
    function GetPosition() {
        return GetTransform().position;
    }
    
    function GetCollisionRadius() {
        return collisionRadius;
    }
    
    function IsAlive() {
        return isAlive;
    }
}

