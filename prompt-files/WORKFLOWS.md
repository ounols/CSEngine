# CSEngine 실전 워크플로우

> 이 문서는 실제 게임 개발 시나리오를 단계별로 안내합니다. 각 단계에서 **정확히 무엇을 실행해야 하는지**, **어떻게 검증하는지**를 명시합니다.

## 목차
1. [새 게임 개발 전체 플로우](#1-새-게임-개발-전체-플로우)
2. [기존 게임 테스트 및 디버깅](#2-기존-게임-테스트-및-디버깅)
3. [새 스크립트 추가](#3-새-스크립트-추가)
4. [엔진 기능 추가](#4-엔진-기능-추가)
5. [에러 발생 시 대응](#5-에러-발생-시-대응)

---

## 1. 새 게임 개발 전체 플로우

### 시나리오: "Breakout" 게임 만들기
벽돌 깨기 게임 (공, 패들, 벽돌)을 만든다고 가정합니다.

### Step 1: 기획 & 설계 (5분)

**체크리스트:**
- [ ] 게임 메커니즘 정의 (조작, 목표, 규칙)
- [ ] 필요한 오브젝트 목록 작성
- [ ] 각 오브젝트의 동작 정의

**예시:**
```
오브젝트:
- Paddle: A/D 키로 좌우 이동
- Ball: 자동으로 이동, 충돌 시 반사
- Bricks: 정적, 공과 충돌 시 사라짐
- GameManager: 공 스폰, 점수 관리, 게임오버 판정
```

### Step 2: 에디터 준비 & 씬 생성

```bash
# 1. 에디터 실행
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi

# 2. 상태 확인
curl http://localhost:8080/api/health

# 3. 새 씬 생성
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"new\"}" http://localhost:8080/api/scene/load
Start-Sleep -Seconds 2

# 4. 확인
curl http://localhost:8080/api/object/list
```

**검증:**
- 에디터가 응답한다 (`health` OK)
- 새 씬이 로드되었다 (`object list` 결과 비어있거나 기본 오브젝트만)

### Step 3: 오브젝트 생성

```bash
# Paddle
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Paddle\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2

# Ball
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Ball\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2

# GameManager
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"GameManager\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2

# 확인
curl http://localhost:8080/api/object/list
```

**검증:**
- `object list`에 Paddle, Ball, GameManager가 나타난다

### Step 4: Transform 설정

```bash
# Paddle 위치 (하단 중앙)
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Paddle\",\"posX\":0,\"posY\":-3,\"posZ\":0}" http://localhost:8080/api/object/set-transform
Start-Sleep -Seconds 2

# Ball 위치 (중앙)
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Ball\",\"posX\":0,\"posY\":0,\"posZ\":0}" http://localhost:8080/api/object/set-transform
Start-Sleep -Seconds 2
```

### Step 5: 스크립트 작성

**파일: `Assets/Script/PaddleController.nut`**
```squirrel
class PaddleController extends CSEngineScript {
    speed = 5.0;
    lastElapsedTime = 0.0;

    function Init() {
        Log("PaddleController initialized");
    }

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();
        local pos = transform.position;

        // A/D 키로 좌우 이동
        if (Input.GetKey(KeyCode.A)) {
            pos.x = pos.x - speed * deltaTime;
        }
        if (Input.GetKey(KeyCode.D)) {
            pos.x = pos.x + speed * deltaTime;
        }

        // 범위 제한
        if (pos.x < -5.0) pos.x = -5.0;
        if (pos.x > 5.0) pos.x = 5.0;

        transform.position = pos;
    }
}
```

**파일: `Assets/Script/BallController.nut`**
```squirrel
class BallController extends CSEngineScript {
    velocity = null;
    speed = 3.0;
    lastElapsedTime = 0.0;

    function Init() {
        // 초기 속도 설정
        velocity = vec3(1.0, 1.0, 0.0);
        velocity = velocity.Normalized();
        Log("BallController initialized");
    }

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();
        local pos = transform.position;

        // 이동
        pos.x = pos.x + velocity.x * speed * deltaTime;
        pos.y = pos.y + velocity.y * speed * deltaTime;

        // 벽 충돌 (간단한 반사)
        if (pos.x < -6.0 || pos.x > 6.0) {
            velocity.x = -velocity.x;
            Log("Ball hit side wall");
        }
        if (pos.y > 4.0) {
            velocity.y = -velocity.y;
            Log("Ball hit top wall");
        }

        // 바닥 닿으면 게임오버 (여기서는 로그만)
        if (pos.y < -5.0) {
            Log("Game Over! Ball fell");
            pos.Set(0, 0, 0);  // 리셋
        }

        transform.position = pos;
    }
}
```

**파일: `Assets/Script/BreakoutManager.nut`**
```squirrel
class BreakoutManager extends CSEngineScript {
    score = 0;

    function Init() {
        Log("Breakout Game Started!");
    }

    function Tick(elapsedTime) {
        // 게임 로직 (점수, 상태 관리)
    }
}
```

### Step 6: 컴포넌트 추가

```bash
# Paddle에 스크립트 추가
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Paddle\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value>></comp>\"}" http://localhost:8080/api/component/add
Start-Sleep -Seconds 2

# Ball에 스크립트 추가
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Ball\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value>></comp>\"}" http://localhost:8080/api/component/add
Start-Sleep -Seconds 2

# GameManager에 스크립트 추가
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"GameManager\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value>></comp>\"}" http://localhost:8080/api/component/add
Start-Sleep -Seconds 2
```

**검증:**
- 로그 확인 시 에러 없음
- 컴포넌트 추가 성공 메시지

### Step 7: 씬 저장

```bash
curl -X POST http://localhost:8080/api/scene/save
```

**결과:** `Assets/Scene/new.scene` 파일 생성

### Step 8: 테스트

```bash
# Play 모드 시작
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
Start-Sleep -Seconds 5

# 로그 확인
curl "http://localhost:8080/api/log/recent?count=50"

# 정지
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"stop\"}" http://localhost:8080/api/editor/command
```

**검증:**
- 로그에 "PaddleController initialized" 출력
- 로그에 "BallController initialized" 출력
- 로그에 "Breakout Game Started!" 출력
- 에러 없음

### Step 9: 디버깅 & 개선

**예상 문제:**
1. **스크립트 에러**: 로그에 Squirrel 에러 확인 → 문법 수정
2. **초기화 안됨**: `Init()` 호출 안됨 → CustomComponent 추가 확인
3. **입력 안됨**: KeyCode 오류 → `ENGINE_REFERENCE.md` 참조

**개선 사항:**
- 충돌 검출 추가
- 벽돌 생성 (여러 오브젝트)
- 점수 표시

---

## 2. 기존 게임 테스트 및 디버깅

### 시나리오: DodgeMaster 게임 실행 & 로그 분석

```bash
# 1. 에디터 실행 (이미 실행 중이면 생략)
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi

# 2. 씬 로드
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"Assets/Scene/DodgeMaster.scene\"}" http://localhost:8080/api/scene/load
Start-Sleep -Seconds 3

# 3. Play
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
Start-Sleep -Seconds 10

# 4. 로그 확인
curl "http://localhost:8080/api/log/recent?count=100"

# 5. 정지
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"stop\"}" http://localhost:8080/api/editor/command
```

**로그 분석 포인트:**
- `"PlayerController: Init"` → Player 초기화 확인
- `"ObstacleController: Init"` → Obstacle 초기화 확인
- `"GameManager: Game Start"` → 게임 시작 확인
- `"Score: XX"` → 점수 증가 확인
- **에러 메시지** → `TROUBLESHOOTING.md` 참조

---

## 3. 새 스크립트 추가

### 시나리오: 기존 게임에 "PowerUp" 아이템 추가

### Step 1: 스크립트 작성

**파일: `Assets/Script/PowerUpController.nut`**
```squirrel
class PowerUpController extends CSEngineScript {
    fallSpeed = 2.0;
    lastElapsedTime = 0.0;

    function Init() {
        Log("PowerUp spawned");
    }

    function Tick(elapsedTime) {
        local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
        lastElapsedTime = elapsedTime;

        local transform = GetTransform();
        local pos = transform.position;

        // 아래로 낙하
        pos.y = pos.y - fallSpeed * deltaTime;

        if (pos.y < -10.0) {
            gameobject.SetEnable(false);  // 오브젝트 풀링
        }

        transform.position = pos;
    }
}
```

### Step 2: 오브젝트 생성 & 컴포넌트 추가

```bash
# 1. 씬 로드 (작업 중인 씬)
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"Assets/Scene/DodgeMaster.scene\"}" http://localhost:8080/api/scene/load
Start-Sleep -Seconds 2

# 2. PowerUp 오브젝트 생성
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"PowerUp\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2

# 3. Transform 설정
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"PowerUp\",\"posX\":0,\"posY\":5,\"posZ\":0}" http://localhost:8080/api/object/set-transform
Start-Sleep -Seconds 2

# 4. CustomComponent 추가
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"PowerUp\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value>></comp>\"}" http://localhost:8080/api/component/add
Start-Sleep -Seconds 2

# 5. 씬 저장
curl -X POST http://localhost:8080/api/scene/save
```

### Step 3: 테스트

```bash
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
Start-Sleep -Seconds 3
curl "http://localhost:8080/api/log/recent?count=30"
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"stop\"}" http://localhost:8080/api/editor/command
```

**검증:**
- 로그에 "PowerUp spawned" 출력
- 에러 없음

---

## 4. 엔진 기능 추가

### 시나리오: 새로운 Input API 추가 (예: GetMousePosition)

**⚠️ 주의:** C++ 코드 수정은 신중하게. 빌드 시간 고려.

### Step 1: 코드 수정

**파일: `src/Manager/InputMgr.h`**
```cpp
class InputMgr {
public:
    // 기존 메서드...
    Vector2 GetMousePosition() const;  // 새로 추가
};
```

**파일: `src/Manager/InputMgr.cpp`**
```cpp
Vector2 InputMgr::GetMousePosition() const {
    // 구현
    return m_mousePosition;
}
```

### Step 2: Squirrel 바인딩 추가

**파일: `src/Manager/ScriptMgr.cpp`** (Input 바인딩 부분)
```cpp
ConstTable(vm).Bind(_SC("Input"),
    Class<Input>(vm, _SC("Input"))
        .Func(_SC("GetKey"), &Input::GetKey)
        .Func(_SC("GetMousePosition"), &Input::GetMousePosition)  // 추가
);
```

### Step 3: 빌드

```powershell
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"
```

**예상 시간:** 2-5분

**에러 발생 시:** `TROUBLESHOOTING.md`의 빌드 문제 섹션 참조

### Step 4: 에디터 재시작 & 테스트

```powershell
# 에디터 재시작
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi
```

**테스트 스크립트:**
```squirrel
function Tick(elapsedTime) {
    local mousePos = Input.GetMousePosition();
    Log("Mouse: " + mousePos.x + ", " + mousePos.y);
}
```

---

## 5. 에러 발생 시 대응

### 시나리오 A: API 호출 후 응답 없음

**증상:**
- `api.ps1 play` 실행 후 아무 반응 없음
- 로그도 안 나옴

**대응 순서:**

```powershell
# 1. 에디터 상태 확인
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" health

# 2. 로그 확인
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" log 100
```

**가능한 원인:**
- **에디터 크래시**: `health` 실패 → 로그 파일 직접 읽기
- **API 큐 멈춤**: 로그에 에러 메시지 확인
- **대기 시간 부족**: 2초 → 5초로 증가

### 시나리오 B: 스크립트 에러

**로그 예시:**
```
[ERROR] Squirrel Error: the index 'velocty' does not exist
```

**대응:**
1. 오타 확인 (`velocty` → `velocity`)
2. 스크립트 수정
3. **재빌드 불필요** (Squirrel은 즉시 반영)
4. Play 다시 시도

### 시나리오 C: 에디터 크래시

**증상:**
- `health` API 실패
- 에디터 창 사라짐

**대응:**

```powershell
# 1. 로그 파일 직접 읽기
# (최신 파일 찾기)
```

Read tool로 `Editor/platforms/Windows/x64/Debug/editor_log_YYYYMMDD_HHMMSS.json` 읽기

**로그 분석:**
- `"level": "ERROR"` 항목 찾기
- 스택 트레이스 확인
- NULL 포인터, assertion 등 확인

**해결:**
- 근본 원인 수정
- 빌드 (C++ 수정 시)
- 에디터 재시작

---

## 빠른 참조: 자주 쓰는 패턴

### 패턴 1: 오브젝트 풀링
```squirrel
if (pos.y < -10.0) {
    gameobject.SetEnable(false);  // 비활성화
}
```

### 패턴 2: 다른 오브젝트 찾기
```squirrel
local player = gameobject.Find("Player");
if (player != null) {
    local playerTransform = player.GetComponent("TransformComponent");
}
```

### 패턴 3: 충돌 검출 (거리 기반)
```squirrel
local otherPos = otherTransform.position;
local distance = transform.position.Distance(otherPos);
if (distance < 1.0) {
    Log("Collision!");
}
```

### 패턴 4: deltaTime 계산
```squirrel
lastElapsedTime = 0.0;  // 클래스 멤버

function Tick(elapsedTime) {
    local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
    lastElapsedTime = elapsedTime;
    // deltaTime 사용
}
```

---

**작성일**: 2025-11-28
**버전**: 1.0
