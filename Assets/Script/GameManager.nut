class GameManager extends CSEngineScript {
    score = 0;
    spawnInterval = 1500.0; // ms (1.5초마다 spawn)
    spawnTimer = 0.0;
    gameState = 0; // 0: READY, 1: PLAYING, 2: GAMEOVER
    playerObject = null;
    playerController = null;
    obstaclePool = [];
    obstaclePoolSize = 10;
    spawnY = 4.0;  // Obstacle spawn 높이
    spawnMinX = -1.5;
    spawnMaxX = 1.5;
    tickCount = 0; // DEBUG
    lastElapsedTime = 0.0; // For delta time calculation
    
    function Init() {
        Log("GameManager initialized");
        
        // 플레이어 참조
        playerObject = gameobject.Find("Player");
        if (playerObject != null) {
            playerController = playerObject.GetClass<PlayerController>();
        }
        
        // 장애물 풀 초기화
        InitObstaclePool();
        
        // 게임 시작
        StartGame();
    }
    
    function InitObstaclePool() {
        // 이미 존재하는 장애물 오브젝트들을 찾아서 풀에 추가
        // (에디터에서 미리 생성해두어야 함)
        // 실제 obstacle 이름은 Obstacle1~Obstacle10
        for (local i = 1; i <= obstaclePoolSize; i++) {
            local obstacleName = "Obstacle" + i;
            local obstacle = gameobject.Find(obstacleName);
            if (obstacle != null) {
                obstaclePool.append(obstacle);
                // 장애물을 멀리 이동 (비활성화)
                local transform = obstacle.GetTransform();
                if (transform != null) {
                    local pos = transform.position;
                    pos.x = 0.0;
                    pos.y = 100.0;
                    pos.z = 0.0;
                    transform.position = pos;
                }
                // CustomComponent만 비활성화 (TransformComponent는 유지)
                local controller = obstacle.GetClass<ObstacleController>();
                if (controller != null) {
                    controller.Deactivate();
                }
                Log("Added to pool: " + obstacleName);
            } else {
                Log("ERROR: Cannot find " + obstacleName);
            }
        }
        Log("Obstacle pool initialized: " + obstaclePool.len() + " obstacles");
    }
    
    function Tick(elapsedTime) {
        // Calculate delta time (difference from last frame, 밀리초 단위)
        local deltaTime = elapsedTime - lastElapsedTime;
        
        // 첫 프레임이거나 deltaTime이 비정상적으로 큰 경우 처리
        if (lastElapsedTime == 0.0 || deltaTime > 100.0) {
            lastElapsedTime = elapsedTime;
            deltaTime = 16.0; // 기본 16ms (60fps 기준)
        } else {
            lastElapsedTime = elapsedTime;
        }
        
        // DEBUG: Tick 호출 확인
        tickCount++;
        if (tickCount == 1 || tickCount % 60 == 0) {
            Log("[GameManager Tick] Frame: " + tickCount + ", deltaTime: " + deltaTime + ", spawnTimer: " + spawnTimer + ", gameState: " + gameState);
        }
        
        if (gameState != 1) {
            if (tickCount == 1) Log("[GameManager] Not PLAYING, gameState = " + gameState);
            return; // PLAYING 상태가 아니면 리턴
        }
        
        // 장애물 생성 타이머 (밀리초 단위)
        spawnTimer += deltaTime;
        if (spawnTimer >= spawnInterval) {
            Log("[GameManager] Spawning obstacle! spawnTimer: " + spawnTimer + " >= " + spawnInterval);
            spawnTimer = 0.0;
            SpawnObstacle();
        }
        
        // 충돌 체크 (주석 처리됨 - 나중에 활성화)
        // CheckCollisions();
    }
    
    function StartGame() {
        score = 0;
        spawnTimer = spawnInterval * 0.5; // 첫 spawn을 빨리 (1초 후)
        gameState = 1; // PLAYING
        Log("Game Started! First spawn in " + (spawnInterval * 0.5) + "ms");
    }
    
    function SpawnObstacle() {
        // 풀에서 비활성 장애물 찾기
        local obstacle = GetInactiveObstacle();
        if (obstacle == null) {
            Log("No inactive obstacles in pool!");
            return;
        }
        
        // 무작위 X 위치
        local randomX = spawnMinX + (spawnMaxX - spawnMinX) * (rand() % 1000) / 1000.0;
        
        // 장애물 활성화
        local controller = obstacle.GetClass<ObstacleController>();
        if (controller != null) {
            controller.Activate(randomX, spawnY);
        }
    }
    
    function GetInactiveObstacle() {
        foreach (obstacle in obstaclePool) {
            local controller = obstacle.GetClass<ObstacleController>();
            if (controller != null && !controller.IsActive()) {
                return obstacle;
            }
        }
        return null;
    }
    
    function CheckCollisions() {
        if (playerController == null) return;
        if (!playerController.IsAlive()) return;
        
        local playerPos = playerController.GetPosition();
        local playerRadius = playerController.GetCollisionRadius();
        
        foreach (obstacle in obstaclePool) {
            local controller = obstacle.GetClass<ObstacleController>();
            if (controller != null && controller.IsActive()) {
                local obstaclePos = controller.GetPosition();
                local obstacleRadius = controller.GetCollisionRadius();
                
                // 2D 거리 기반 충돌 체크 (X, Y만 사용)
                local dx = playerPos.x - obstaclePos.x;
                local dy = playerPos.y - obstaclePos.y;
                local distance = sqrt(dx * dx + dy * dy);
                local collisionDistance = playerRadius + obstacleRadius;
                
                if (distance < collisionDistance) {
                    // 충돌!
                    Log("Collision! Player: (" + playerPos.x + ", " + playerPos.y + "), Obstacle: (" + obstaclePos.x + ", " + obstaclePos.y + "), Distance: " + distance + " < " + collisionDistance);
                    playerController.OnCollision();
                    GameOver();
                    return;
                }
            }
        }
    }
    
    function OnObstaclePassed() {
        score += 10;
        Log("Score: " + score);
        
        // 난이도 증가 (10개마다)
        if (score % 100 == 0) {
            IncreaseDifficulty();
        }
    }
    
    function IncreaseDifficulty() {
        spawnInterval *= 0.9; // 생성 간격 10% 감소
        if (spawnInterval < 500) spawnInterval = 500; // 최소 0.5초
        Log("Difficulty increased! Spawn interval: " + spawnInterval);
    }
    
    function GameOver() {
        gameState = 2; // GAMEOVER
        Log("===== GAME OVER =====");
        Log("Final Score: " + score);
        Log("====================");
        
        // 모든 장애물 비활성화
        foreach (obstacle in obstaclePool) {
            local controller = obstacle.GetClass<ObstacleController>();
            if (controller != null) {
                controller.Deactivate();
            }
        }
    }
}

