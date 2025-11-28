# CSEngine 핵심 API 레퍼런스

## 엔진 개요
- **언어**: C++ (엔진), Squirrel (게임 스크립트)
- **렌더링**: OpenGL, PBR
- **에디터**: ImGui 기반, REST API 제공 (포트 8080)

## 아키텍처
```
CSEngine/
├── src/Manager/        # EngineCore, GameObjectMgr, SceneMgr, RenderMgr, InputMgr, ScriptMgr
├── src/Component/      # Transform, Render, Camera, Light, CustomComponent
├── src/Util/           # Vector, Quaternion, SafeLog
└── Editor/
    ├── src/Backend/    # 비즈니스 로직 (API + GUI 공유)
    ├── src/Objects/    # ImGui Window 클래스
    └── src/Manager/    # EditorAPIServer, EEngineCore
```

## Editor Backend 아키텍처 (신규)

### 개요
Editor Backend는 REST API와 GUI에서 공통으로 사용하는 비즈니스 로직을 분리한 계층입니다.
각 Backend 클래스는 싱글톤 패턴을 사용하며, API용 비동기 메서드와 GUI용 동기 메서드를 제공합니다.

### Backend 클래스 목록
| 클래스 | 역할 | 주요 Direct 메서드 |
|--------|------|-------------------|
| `ObjectBackend` | GameObject 생성/삭제/복제 | `CreateEmptyObjectDirect()`, `DeleteObjectDirect()`, `DuplicateObjectDirect()` |
| `ComponentBackend` | 컴포넌트 추가/제거 | `AddComponentDirect()`, `RemoveComponentDirect()`, `HasComponent()` |
| `SceneBackend` | 씬 로드/저장 | `LoadSceneDirect()`, `SaveSceneDirect()`, `CreateNewSceneDirect()` |
| `EditorBackend` | Play/Stop, Preview | `PlayDirect()`, `StopDirect()`, `IsPlaying()`, `GetPreviewTextureId()` |
| `LogBackend` | 로그 조회/삭제 | `GetRecentLogs()`, `ClearLogs()` |
| `DebugBackend` | 디버그 정보 | `GetCrashInfo()`, `GetContext()` |

### 사용 예시 (C++)
```cpp
// GUI에서 GameObject 생성
#include "Backend/ObjectBackend.h"

auto& backend = CSEditor::ObjectBackend::GetInstance();
auto* obj = backend.CreateEmptyObjectDirect("Player", nullptr);

// 컴포넌트 추가
#include "Backend/ComponentBackend.h"

auto& compBackend = CSEditor::ComponentBackend::GetInstance();
compBackend.AddComponentDirect(obj, "CustomComponent", "PlayerController");

// Play 모드 시작
#include "Backend/EditorBackend.h"

auto& editorBackend = CSEditor::EditorBackend::GetInstance();
editorBackend.PlayDirect(1280, 720);
```

## Squirrel 스크립트 기본

### 기본 구조
```squirrel
// 파일: Assets/Script/MyScript.nut
class MyScript extends CSEngineScript {
    // 멤버 변수
    speed = 2.0;
    health = 100;
    isActive = true;

    function Init() {
        // 컴포넌트 활성화 시 1회 호출
        Log("MyScript initialized");
    }

    function Tick(elapsedTime) {
        // 매 프레임 호출 (elapsedTime = 누적 시간, ms)
    }
}
```

### Delta Time 패턴 (중요!)
```squirrel
class MovingObject extends CSEngineScript {
    lastElapsedTime = 0.0;
    speed = 5.0;

    function Tick(elapsedTime) {
        // deltaTime을 초 단위로 계산 (elapsedTime은 ms)
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        // 프레임 독립적 이동
        local transform = GetTransform();
        local pos = transform.position;
        pos.x = pos.x + speed * deltaTime;  // 초당 5 유닛 이동
        transform.position = pos;
    }
}
```

### 완전한 예시: 플레이어 컨트롤러
```squirrel
class PlayerController extends CSEngineScript {
    speed = 5.0;
    health = 100;
    lastElapsedTime = 0.0;

    function Init() {
        Log("Player initialized with health: " + health);
    }

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();
        local pos = transform.position;

        // WASD 이동
        if (Input.GetKey(KeyCode.W)) {
            pos.y = pos.y + speed * deltaTime;
        }
        if (Input.GetKey(KeyCode.S)) {
            pos.y = pos.y - speed * deltaTime;
        }
        if (Input.GetKey(KeyCode.A)) {
            pos.x = pos.x - speed * deltaTime;
        }
        if (Input.GetKey(KeyCode.D)) {
            pos.x = pos.x + speed * deltaTime;
        }

        // 경계 체크
        if (pos.x < -10.0) pos.x = -10.0;
        if (pos.x > 10.0) pos.x = 10.0;
        if (pos.y < -10.0) pos.y = -10.0;
        if (pos.y > 10.0) pos.y = 10.0;

        transform.position = pos;
    }
}
```

## 주요 API

### GameObject
```squirrel
gameobject                    // 현재 오브젝트 (this)
gameobject.Find("Player")     // 이름으로 검색
gameobject.SetEnable(false)   // 비활성화 (오브젝트 풀링에 사용)
GetTransform()                // TransformComponent 반환
```

### Transform
```squirrel
local transform = GetTransform();
transform.position            // vec3 (읽기/쓰기)
transform.rotation            // Quaternion
transform.scale               // vec3

// 위치 변경
transform.position = vec3(1, 2, 3);
```

### Input (이 프로젝트에서 추가됨)
```squirrel
Input.GetKey(KeyCode.A)       // 누르고 있는 동안 true
Input.GetKeyDown(KeyCode.Space)  // 눌린 순간 true
Input.GetKeyUp(KeyCode.D)     // 뗀 순간 true

// KeyCode 목록
KeyCode.A, KeyCode.D, KeyCode.W, KeyCode.S
KeyCode.Left, KeyCode.Right, KeyCode.Up, KeyCode.Down
KeyCode.Space, KeyCode.Escape
```

### Vector
```squirrel
local v = vec3(1, 2, 3);
v.x, v.y, v.z                 // 개별 접근
v.Set(x, y, z)                // 값 설정
v.Distance(other)             // 거리 계산 (추가됨)
v.DistanceSquared(other)      // 거리² (성능 최적화용)
v.Dot(other), v.Cross(other)
```

### Utility
```squirrel
Log("메시지")                 // 콘솔 출력
```

## 실전 패턴 모음

### 패턴 1: 오브젝트 풀링
재사용 가능한 오브젝트 (탄환, 적, 아이템 등)를 미리 생성하고 활성화/비활성화로 관리합니다.

```squirrel
class BulletController extends CSEngineScript {
    speed = 10.0;
    lastElapsedTime = 0.0;

    function Init() {
        // 초기화 시 비활성화 상태
        gameobject.SetEnable(false);
    }

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();
        local pos = transform.position;

        // 위로 이동
        pos.y = pos.y + speed * deltaTime;

        // 화면 밖으로 나가면 비활성화 (풀로 반환)
        if (pos.y > 15.0) {
            gameobject.SetEnable(false);
            Log("Bullet returned to pool");
        }

        transform.position = pos;
    }
}

// 스폰 시스템 (GameManager 등에서)
class GameManager extends CSEngineScript {
    function SpawnBullet(x, y) {
        // 비활성화된 탄환 찾기
        local bullet = gameobject.Find("Bullet");  // 실제로는 여러 개 관리
        if (bullet != null) {
            bullet.SetEnable(true);
            local bulletTransform = bullet.GetComponent("TransformComponent");
            bulletTransform.position = vec3(x, y, 0);
        }
    }
}
```

### 패턴 2: 충돌 검출 (거리 기반)
Physics 시스템 없이 간단한 충돌 검출을 구현합니다.

```squirrel
class PlayerController extends CSEngineScript {
    lastElapsedTime = 0.0;

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();

        // 모든 적 오브젝트와 충돌 체크
        CheckCollisionWithEnemy(transform);
    }

    function CheckCollisionWithEnemy(transform) {
        local enemy = gameobject.Find("Enemy");
        if (enemy != null) {
            local enemyTransform = enemy.GetComponent("TransformComponent");
            local distance = transform.position.Distance(enemyTransform.position);

            if (distance < 1.0) {  // 충돌 반경 1.0
                Log("Player hit by enemy!");
                OnDamage(10);
            }
        }
    }

    function OnDamage(amount) {
        health = health - amount;
        Log("Health: " + health);
        if (health <= 0) {
            Log("Game Over!");
        }
    }
}
```

### 패턴 3: 타이머 & 주기적 이벤트
일정 시간마다 이벤트를 발생시킵니다.

```squirrel
class SpawnManager extends CSEngineScript {
    spawnInterval = 2.0;      // 2초마다 스폰
    timeSinceLastSpawn = 0.0;
    lastElapsedTime = 0.0;

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        timeSinceLastSpawn = timeSinceLastSpawn + deltaTime;

        if (timeSinceLastSpawn >= spawnInterval) {
            SpawnEnemy();
            timeSinceLastSpawn = 0.0;  // 타이머 리셋
        }
    }

    function SpawnEnemy() {
        Log("Enemy spawned!");
        // 적 생성 로직
    }
}
```

### 패턴 4: 상태 머신 (FSM)
오브젝트의 여러 상태를 관리합니다.

```squirrel
class EnemyAI extends CSEngineScript {
    // 상태 enum (Squirrel에는 없지만 정수로 대체)
    STATE_IDLE = 0;
    STATE_PATROL = 1;
    STATE_CHASE = 2;
    STATE_ATTACK = 3;

    currentState = 0;  // STATE_IDLE
    lastElapsedTime = 0.0;

    function Init() {
        currentState = STATE_PATROL;
    }

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        if (currentState == STATE_IDLE) {
            UpdateIdle(deltaTime);
        } else if (currentState == STATE_PATROL) {
            UpdatePatrol(deltaTime);
        } else if (currentState == STATE_CHASE) {
            UpdateChase(deltaTime);
        } else if (currentState == STATE_ATTACK) {
            UpdateAttack(deltaTime);
        }
    }

    function UpdateIdle(deltaTime) {
        // 대기 상태
    }

    function UpdatePatrol(deltaTime) {
        // 순찰 로직
        local player = gameobject.Find("Player");
        if (player != null) {
            local playerTransform = player.GetComponent("TransformComponent");
            local distance = GetTransform().position.Distance(playerTransform.position);
            if (distance < 5.0) {
                currentState = STATE_CHASE;
                Log("Enemy detected player!");
            }
        }
    }

    function UpdateChase(deltaTime) {
        // 추적 로직
    }

    function UpdateAttack(deltaTime) {
        // 공격 로직
    }
}
```

### 패턴 5: 다른 스크립트 참조 & 통신
여러 스크립트 간 데이터를 공유합니다.

```squirrel
// GameManager.nut - 전역 상태 관리
class GameManager extends CSEngineScript {
    score = 0;
    isGameOver = false;

    function AddScore(points) {
        score = score + points;
        Log("Score: " + score);
    }

    function GetScore() {
        return score;
    }
}

// Player.nut - GameManager 참조
class PlayerController extends CSEngineScript {
    function OnCollectCoin() {
        local manager = gameobject.Find("GameManager");
        if (manager != null) {
            // CustomComponent를 통해 스크립트 메서드 호출은 직접 불가
            // 대신 로그를 통한 간접 통신 또는 전역 변수 사용
            Log("Coin collected");  // GameManager가 로그 모니터링
        }
    }
}

// 참고: 현재 엔진에서는 스크립트 간 직접 메서드 호출이 제한적입니다.
// 로그, GameObject 상태(position 등), 또는 싱글톤 패턴을 사용하세요.
```

### 패턴 6: 벡터 연산 활용
```squirrel
class HomingMissile extends CSEngineScript {
    speed = 8.0;
    lastElapsedTime = 0.0;

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();
        local target = gameobject.Find("Player");

        if (target != null) {
            local targetTransform = target.GetComponent("TransformComponent");
            local targetPos = targetTransform.position;
            local myPos = transform.position;

            // 목표 방향 계산
            local direction = vec3(
                targetPos.x - myPos.x,
                targetPos.y - myPos.y,
                targetPos.z - myPos.z
            );
            direction = direction.Normalized();

            // 방향으로 이동
            myPos.x = myPos.x + direction.x * speed * deltaTime;
            myPos.y = myPos.y + direction.y * speed * deltaTime;
            myPos.z = myPos.z + direction.z * speed * deltaTime;

            transform.position = myPos;
        }
    }
}
```

### 패턴 7: 경계 체크 & 랩어라운드
```squirrel
class WrapAroundObject extends CSEngineScript {
    minX = -10.0;
    maxX = 10.0;
    minY = -10.0;
    maxY = 10.0;

    function Tick(elapsedTime) {
        local transform = GetTransform();
        local pos = transform.position;

        // 화면 경계를 넘어가면 반대편으로 (팩맨 스타일)
        if (pos.x < minX) pos.x = maxX;
        if (pos.x > maxX) pos.x = minX;
        if (pos.y < minY) pos.y = maxY;
        if (pos.y > maxY) pos.y = minY;

        transform.position = pos;
    }
}

// 또는 경계에서 튕기기
class BounceObject extends CSEngineScript {
    velocity = null;

    function Init() {
        velocity = vec3(1.0, 1.0, 0.0);
    }

    function Tick(elapsedTime) {
        local transform = GetTransform();
        local pos = transform.position;

        pos.x = pos.x + velocity.x;
        pos.y = pos.y + velocity.y;

        // 벽에 부딪히면 반사
        if (pos.x < -10.0 || pos.x > 10.0) {
            velocity.x = -velocity.x;
        }
        if (pos.y < -10.0 || pos.y > 10.0) {
            velocity.y = -velocity.y;
        }

        transform.position = pos;
    }
}
```

## REST API (에디터 제어)

### 기본
```bash
# 서버 상태 확인
curl http://localhost:8080/api/health

# 오브젝트 목록
curl http://localhost:8080/api/object/list

# 최근 로그
curl "http://localhost:8080/api/log/recent?count=50"
```

### 씬 관리
```bash
# 새 씬 생성
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"new\"}" http://localhost:8080/api/scene/load

# 기존 씬 로드
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"Assets/Scene/DodgeMaster.scene\"}" http://localhost:8080/api/scene/load

# 씬 저장
curl -X POST http://localhost:8080/api/scene/save
```

### 오브젝트/컴포넌트
```bash
# 빈 오브젝트 생성
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Player\"}" http://localhost:8080/api/object/create

# 컴포넌트 추가 (스크립트) - rawValues로 해시 전달
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Player\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value></comp>\"}" http://localhost:8080/api/component/add

# 컴포넌트 추가 (카메라)
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"MainCamera\",\"type\":\"CameraComponent\"}" http://localhost:8080/api/component/add

# 컴포넌트 추가 (조명)
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Light\",\"type\":\"LightComponent\"}" http://localhost:8080/api/component/add

# Transform 설정
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Player\",\"posX\":0,\"posY\":-2,\"posZ\":0}" http://localhost:8080/api/object/set-transform
```

**참고**: CustomComponent 추가 시 `rawValues`에는 스크립트 리소스의 **해시 값**을 사용합니다.
해시 값은 `.meta` 파일이나 API를 통해 조회할 수 있습니다.

### 게임 실행
```bash
# 게임 시작
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command

# 게임 종료
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"stop\"}" http://localhost:8080/api/editor/command
```

## 파일 경로 규칙
| 항목 | 경로 |
|------|------|
| 스크립트 | `Assets/Script/*.nut` |
| 씬 | `Assets/Scene/*.scene` |
| 모델 | `Assets/Model/*.dae` |

## 리소스 해시 조회 방법

CustomComponent 추가 시 스크립트 리소스의 해시 값이 필요합니다.

**방법 1: .meta 파일 확인**
```
Assets/Script/PlayerController.nut.meta 파일의 hash 값
```

**방법 2: API로 조회** (구현 예정)
```bash
curl "http://localhost:8080/api/resource/hash?path=Assets/Script/PlayerController.nut"
```
