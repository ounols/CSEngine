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
```squirrel
// 파일: Assets/Script/MyScript.nut
class MyScript extends CSEngineScript {
    // 멤버 변수
    speed = 2.0;

    function Init() {
        // 컴포넌트 활성화 시 1회 호출
        Log("Initialized!");
    }

    function Tick(elapsedTime) {
        // 매 프레임 호출 (elapsedTime = 누적 시간, ms)
    }
}
```

### Delta Time 패턴 (중요!)
```squirrel
lastElapsedTime = 0.0;

function Tick(elapsedTime) {
    local deltaTime = elapsedTime - lastElapsedTime;
    lastElapsedTime = elapsedTime;
    // deltaTime을 이동/계산에 사용
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

## REST API (에디터 제어)

### 기본
```bash
GET  /api/health              # 서버 상태 확인
GET  /api/object/list         # 오브젝트 목록
GET  /api/log/recent?count=50 # 최근 로그
```

### 씬 관리
```bash
# 새 씬 생성
POST /api/scene/load   {"path": "new"}

# 기존 씬 로드
POST /api/scene/load   {"path": "Assets/Scene/DodgeMaster.scene"}

# 씬 저장
POST /api/scene/save
```

### 오브젝트/컴포넌트
```bash
# 빈 오브젝트 생성
POST /api/object/create   {"type": "empty", "name": "Player"}

# 컴포넌트 추가 (스크립트)
POST /api/component/add   {"object": "Player", "type": "CustomComponent", "scriptPath": "PlayerController"}

# 컴포넌트 추가 (카메라/조명)
POST /api/component/add   {"object": "MainCamera", "type": "CameraComponent"}
POST /api/component/add   {"object": "Light", "type": "LightComponent"}

# Transform 설정
POST /api/object/set-transform  {"object": "Player", "posX": 0, "posY": -2, "posZ": 0}
```

### 게임 실행
```bash
POST /api/editor/command  {"command": "play"}   # 게임 시작
POST /api/editor/command  {"command": "stop"}   # 게임 종료
```

## 파일 경로 규칙
| 항목 | 경로 |
|------|------|
| 스크립트 | `Assets/Script/*.nut` |
| 씬 | `Assets/Scene/*.scene` |
| 모델 | `Assets/Model/*.dae` |
| scriptPath | 클래스명만 (예: `"PlayerController"`, 확장자 없음) |
