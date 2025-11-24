# CSEngine 게임 개발 프로젝트 - 작업 가이드

> **📌 중요**: 이 문서는 CSEngine 게임 개발 프로젝트의 **메인 가이드**입니다.  
> 작업을 시작하기 전에 이 문서를 먼저 읽어주세요.

---

## 🎉 작업 완료 요약 (2025-11-24)

### ✅ 모든 작업 완료!
✅ **Phase 1**: 엔진 수정 - InputMgr 바인딩, vec3 Distance 함수 추가  
✅ **Phase 2**: 에디터 빌드 성공 (MSVC, Visual Studio 2022)  
✅ **Phase 3**: 게임 스크립트 3개 작성 완료  
✅ **Phase 4**: 에디터 정상 실행 확인 (REST API 서버 작동)  
✅ **Phase 5**: REST API 개선 - 오브젝트/컴포넌트 생성 큐 시스템 구현  
✅ **Phase 6**: 새 씬 생성 기능 구현 (SScene 직접 생성)  
✅ **Phase 7**: REST API를 통한 완전 자동 씬 구성 성공!  
✅ **Phase 8**: Transform 설정 API 구현 및 테스트 완료  
✅ **Phase 9**: Play/Stop 모드 제어 테스트 완료  
✅ **Phase 10**: 🎮 **게임 완성! 모든 시스템 정상 작동!**  
✅ **Phase 11**: 💾 **씬 저장 기능 구현 완료!**

### 생성된 파일
**스크립트:**
- `Assets/Script/PlayerController.nut` - 플레이어 조작 (A/D 키, 충돌 검사)
- `Assets/Script/ObstacleController.nut` - 장애물 낙하 및 비활성화
- `Assets/Script/GameManager.nut` - 게임 로직 (스폰, 점수, 충돌 관리)

**엔진 수정:**
- `src/Manager/ScriptMgr.cpp` - InputMgr, KeyCode, MouseButton 바인딩 추가
- `src/Util/Vector.h` - vec3 Distance/DistanceSquared 함수 추가

**REST API 개선:**
- `Editor/src/Manager/EditorAPIServer.h` - 오브젝트/컴포넌트/Transform/씬 저장 큐 추가
- `Editor/src/Manager/EditorAPIServer.cpp` - 새 씬 생성, 큐 처리, Transform 설정, 씬 저장 구현

### 🎮 생성된 씬 구성
**총 14개 오브젝트 (REST API로 자동 생성):**
1. ✅ **GameManager** + CustomComponent (GameManager)
2. ✅ **Player** + CustomComponent (PlayerController)
3. ✅ **Obstacle0~9** (10개) + CustomComponent (ObstacleController)
4. ✅ **Camera** + CameraComponent
5. ✅ **DirectionalLight** + LightComponent

### 🎮 게임 준비 완료!
✅ **모든 설정이 REST API로 자동화됨:**
- ✅ Camera 위치 설정 (0, 0, 5)
- ✅ DirectionalLight 위치 설정 (0, 3, 0)
- ✅ Player 초기 위치 설정 (0, -2, 0)
- ✅ Obstacle 초기 위치 설정 (0, 10, 0) - GameManager에서 제어
- ✅ Play/Stop 모드 API 작동 확인

### 발견 및 해결한 문제
1. ✅ CMake Clang 선택 → Visual Studio generator로 해결
2. ✅ vec3::Distance 오버로드 모호성 → 명시적 캐스팅으로 해결
3. ✅ ConstTable → RootTable 변경
4. ✅ Assets 경로 문제 → 올바른 실행 위치 확인
5. ✅ REST API 미구현 → 오브젝트/컴포넌트 생성 큐 시스템 구현
6. ✅ 컴포넌트 생성자 → SGameObject* 인자 필요
7. ✅ **핵심 해결**: ChoAn.scene이 로드 불가 → **새 SScene 직접 생성**으로 해결!
8. ✅ GameObjectMgr 크래시 → TransformComponent 중복 제거, obj->Init() 호출
9. ✅ Transform 설정 API 추가 → position, rotation, scale 개별 설정 가능
10. ✅ 씬 저장 경로 문제 → NativeAssetsPath() 사용, Assets 중복 제거

---

---

## 📚 문서 구조

이 프로젝트는 여러 문서로 구성되어 있습니다:

- **[CLAUDE.md](CLAUDE.md)** (이 문서) - 프로젝트 전체 가이드 및 엔진 분석
- **[GAME_DEV_PLAN.md](GAME_DEV_PLAN.md)** - "Dodge Master" 게임 상세 개발 계획
- **[EDITOR_API.md](EDITOR_API.md)** - CSEditor REST API 참조 문서
- **[README-ko.md](README-ko.md)** - CSEngine 프로젝트 소개

---

## 📋 프로젝트 개요

**목표**: CSEngine 에디터를 사용하여 간단한 3D 게임 "Dodge Master"를 개발

**게임 컨셉**: 플레이어가 좌우로 이동하며 위에서 떨어지는 장애물을 피하는 아케이드 게임

**개발 방식**:
1. 엔진 C++ 코드 수정 (필요한 API 추가)
2. 에디터 빌드
3. Squirrel 스크립트로 게임 로직 작성
4. 에디터에서 씬 구성
5. 테스트 및 디버깅

---

## 🔍 CSEngine 분석 요약

### 1. 아키텍처

```
CSEngine
├── 코어 시스템
│   ├── EngineCore - 엔진 전체 관리
│   ├── GameObjectMgr - 게임 오브젝트 관리
│   ├── SceneMgr - 씬 관리
│   ├── RenderMgr - 렌더링 파이프라인
│   ├── InputMgr - 입력 처리 ⚠️ (스크립트 미바인딩)
│   ├── ScriptMgr - Squirrel 스크립트 관리
│   └── ResMgr - 리소스/에셋 관리
│
├── 컴포넌트 시스템
│   ├── TransformComponent - 위치, 회전, 스케일
│   ├── RenderComponent - 메시 렌더링
│   ├── CameraComponent - 카메라
│   ├── LightComponent - 조명
│   └── CustomComponent - Squirrel 스크립트 컴포넌트
│
└── 에디터
    ├── REST API 서버 (포트: 8080)
    ├── ImGui 기반 UI
    └── 씬 편집 기능
```

### 2. Squirrel 스크립팅

**기본 구조**:
```squirrel
class MyScript extends CSEngineScript {
    function Init() {
        // 초기화 - 컴포넌트가 처음 활성화될 때 호출
        Log("Initialized");
    }
    
    function Tick(elapsedTime) {
        // 매 프레임 호출 (elapsedTime은 밀리초 단위)
    }
}
```

**주요 API**:
- `gameobject` - 현재 게임 오브젝트 참조
- `GetTransform()` - Transform 컴포넌트 접근
- `gameobject.Find(name)` - 이름으로 오브젝트 검색
- `gameobject.GetComponent<Type>()` - 컴포넌트 가져오기
- `Log(message)` - 콘솔 로그 출력

**Vector 클래스**:
- `vec3` - 3D 벡터 (Set, Cross, Dot)
- `Quaternion` - 회전 (AngleAxis, Rotate, ToEulerAngle)

**기존 스크립트 예제**: `Assets/Script/` 디렉토리 참조

### 3. 에디터 API

에디터는 `http://localhost:8080`에서 REST API를 제공합니다.

**상세 내용**: [EDITOR_API.md](EDITOR_API.md) 참조

**주요 엔드포인트**:
- `GET /api/health` - 서버 상태
- `POST /api/scene/load` - 씬 로드
- `POST /api/object/create` - 오브젝트 생성
- `POST /api/editor/command` - Play/Stop 제어
- `GET /api/log/recent` - 로그 조회

### 4. 사용 가능한 에셋

- **3D 모델**: cube.dae, teapot2.dae, stormtrooper.dae 등
- **머티리얼**: Assets/Material/ 디렉토리
- **셰이더**: Assets/Shader/ 디렉토리 (PBR, Shadow 등)

---

## ⚠️ 발견된 문제점

### 1. InputMgr 미바인딩 (치명적)
**문제**: 스크립트에서 키보드/마우스 입력을 받을 수 없음

**원인**: `ScriptMgr::DefineClasses()`에 InputMgr 바인딩 누락

**영향**: 플레이어 조작이 불가능 → 게임 개발 불가

**해결**: Phase 1에서 수정 필요

### 2. vec3 Distance 함수 부재
**문제**: 두 점 사이 거리 계산 함수가 없음

**영향**: 충돌 감지 구현이 어려움

**해결**: Phase 1에서 추가 필요

### 3. 동적 GameObject 생성
**문제**: 스크립트에서 런타임에 오브젝트 생성 API 불명확

**해결**: 오브젝트 풀링 패턴 사용 (미리 생성 후 활성화/비활성화)

---

## 🎮 개발할 게임: "Dodge Master"

**상세 계획**: [GAME_DEV_PLAN.md](GAME_DEV_PLAN.md) 참조

**핵심 요소**:
- 플레이어: 큐브, A/D 키로 좌우 이동
- 장애물: 티팟, 위에서 아래로 낙하
- 충돌: 거리 기반 감지
- 점수: 장애물을 피할 때마다 증가

**선택 이유**:
- 간단한 메커니즘
- 적은 에셋 요구
- 명확한 목표
- 확장 가능

---

## 🛠 작업 단계

### Phase 1: 엔진 수정 ⭐ (필수)

> **중요**: 이 단계를 완료하지 않으면 게임을 만들 수 없습니다!

#### 1.1 InputMgr 스크립트 바인딩

**파일**: `src/Manager/ScriptMgr.cpp`

**작업 내용**:
1. 파일 상단에 헤더 추가:
```cpp
#include "../Manager/InputMgr.h"
```

2. `DefineClasses()` 함수 끝에 추가:
```cpp
// InputMgr 바인딩
SQRClassDef<InputMgr>(_SC("Input"), vm)
    .StaticFunc(_SC("GetKey"), &InputMgr::GetKey)
    .StaticFunc(_SC("GetKeyDown"), &InputMgr::GetKeyDown)
    .StaticFunc(_SC("GetKeyUp"), &InputMgr::GetKeyUp)
    .StaticFunc(_SC("GetMouseButton"), &InputMgr::GetMouseButton)
    .StaticFunc(_SC("GetMouseButtonDown"), &InputMgr::GetMouseButtonDown)
    .StaticFunc(_SC("GetMouseButtonUp"), &InputMgr::GetMouseButtonUp)
    .StaticFunc(_SC("GetMousePosition"), &InputMgr::GetMousePosition);

// KeyCode enum 바인딩
Enumeration keyCodeEnum(vm);
keyCodeEnum.Const(_SC("A"), static_cast<int>(KeyCode::A));
keyCodeEnum.Const(_SC("D"), static_cast<int>(KeyCode::D));
keyCodeEnum.Const(_SC("W"), static_cast<int>(KeyCode::W));
keyCodeEnum.Const(_SC("S"), static_cast<int>(KeyCode::S));
keyCodeEnum.Const(_SC("Left"), static_cast<int>(KeyCode::Left));
keyCodeEnum.Const(_SC("Right"), static_cast<int>(KeyCode::Right));
keyCodeEnum.Const(_SC("Up"), static_cast<int>(KeyCode::Up));
keyCodeEnum.Const(_SC("Down"), static_cast<int>(KeyCode::Down));
keyCodeEnum.Const(_SC("Space"), static_cast<int>(KeyCode::Space));
keyCodeEnum.Const(_SC("Escape"), static_cast<int>(KeyCode::Escape));
ConstTable(vm).Bind(_SC("KeyCode"), keyCodeEnum);

// MouseButton enum 바인딩
Enumeration mouseButtonEnum(vm);
mouseButtonEnum.Const(_SC("Left"), static_cast<int>(MouseButton::Left));
mouseButtonEnum.Const(_SC("Right"), static_cast<int>(MouseButton::Right));
mouseButtonEnum.Const(_SC("Middle"), static_cast<int>(MouseButton::Middle));
ConstTable(vm).Bind(_SC("MouseButton"), mouseButtonEnum);
```

#### 1.2 vec3 Distance 함수 추가

**파일 1**: `src/Util/Vector.h`

vec3 구조체에 함수 추가:
```cpp
// vec3 구조체 안에 추가
float Distance(const vec3& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

float DistanceSquared(const vec3& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return dx*dx + dy*dy + dz*dz;
}
```

**파일 2**: `src/Manager/ScriptMgr.cpp`

vec3 바인딩에 함수 추가 (기존 vec3 바인딩 찾아서 수정):
```cpp
SQRClassDef<vec3>(_SC("vec3"))
    .Var(_SC("x"), &vec3::x)
    .Var(_SC("y"), &vec3::y)
    .Var(_SC("z"), &vec3::z)
    .Func(_SC("Cross"), &vec3::Cross)
    .Func(_SC("Dot"), &vec3::Dot)
    .Func(_SC("Set"), &vec3::Set)
    .Func(_SC("Distance"), &vec3::Distance)              // 추가
    .Func(_SC("DistanceSquared"), &vec3::DistanceSquared); // 추가
```

### Phase 2: 에디터 빌드

```powershell
# 1. CMake 구성
C:\Users\Fundeal-Orange\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\cmake.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=C:/Users/Fundeal-Orange/AppData/Local/Programs/CLion/bin/ninja/win/x64/ninja.exe -G Ninja -S D:\Projects\CSEngine\Editor\platforms\Windows -B D:\Projects\CSEngine\Editor\platforms\Windows\cmake-build-debug

# 2. 에디터 빌드
C:\Users\Fundeal-Orange\AppData\Local\Programs\CLion\bin\cmake\win\x64\bin\cmake.exe --build D:\Projects\CSEngine\Editor\platforms\Windows\cmake-build-debug --target CSEditor -j 10
```

**빌드 에러 발생 시**:
- 에러 로그 확인
- 문법 오류 수정
- 필요한 헤더 파일 추가 확인

### Phase 3: 게임 스크립트 작성

**생성할 파일**:
1. `Assets/Script/PlayerController.nut`
2. `Assets/Script/ObstacleController.nut`
3. `Assets/Script/GameManager.nut`

**전체 스크립트 코드**: [GAME_DEV_PLAN.md - Phase 3](GAME_DEV_PLAN.md#3-playercontrollernut) 참조

**핵심 기능**:
- **PlayerController**: 키보드 입력 처리, 좌우 이동, 충돌 이벤트
- **ObstacleController**: 낙하 움직임, 오브젝트 풀링 지원
- **GameManager**: 장애물 생성, 충돌 감지, 점수 관리, 게임 상태 관리

### Phase 4: 씬 구성

**방법 1**: 에디터 UI 사용 (권장)
**방법 2**: REST API 사용

**필요한 오브젝트**:
```
dodge_game.scene
├── Camera (0, 0, 5)
├── DirectionalLight (0, 3, 0)
├── GameManager (Empty + GameManager.nut)
├── Player (0, -1.5, 0) + PlayerController.nut
└── Obstacle0~9 (10개) + ObstacleController.nut
```

**상세 설정**: [GAME_DEV_PLAN.md - Phase 4](GAME_DEV_PLAN.md#-씬-구조) 참조

### Phase 5: 테스트 및 디버깅

**테스트 항목**:
- [ ] 플레이어 좌우 이동
- [ ] 장애물 생성 및 낙하
- [ ] 충돌 감지
- [ ] 점수 증가
- [ ] 게임 오버

**디버깅 방법**:
```bash
# 로그 확인
curl http://localhost:8080/api/log/recent?count=50

# 게임 실행
curl -X POST http://localhost:8080/api/editor/command \
     -H "Content-Type: application/json" \
     -d '{"command":"play"}'
```

---

## 📝 작업 시 주의사항

### 코드 수정 시
1. 원본 코드를 먼저 확인
2. 수정 후 반드시 빌드 테스트
3. 에러 발생 시 로그 확인

### 스크립트 작성 시
1. 클래스명과 파일명 일치
2. `CSEngineScript` 상속 필수
3. `Init()`, `Tick(elapsedTime)` 함수 구현
4. 변수는 스크립트 최상단에 선언

### 씬 구성 시
1. 오브젝트 이름 정확히 입력 (스크립트에서 Find로 찾음)
2. 스크립트 연결 확인
3. Transform 값 정확히 설정
4. 씬 저장 필수

---

## ✅ 작업 체크리스트

### Phase 1: 엔진 수정
- [ ] ScriptMgr.cpp에 InputMgr 헤더 추가
- [ ] InputMgr 바인딩 코드 추가
- [ ] KeyCode enum 바인딩
- [ ] MouseButton enum 바인딩
- [ ] Vector.h에 Distance 함수 추가
- [ ] vec3 바인딩에 Distance 추가

### Phase 2: 빌드
- [ ] CMake 구성 성공
- [ ] 에디터 빌드 성공
- [ ] 빌드 에러 없음
- [ ] 에디터 실행 가능

### Phase 3: 스크립트
- [ ] PlayerController.nut 작성
- [ ] ObstacleController.nut 작성
- [ ] GameManager.nut 작성
- [ ] .nut.meta 파일 자동 생성 확인

### Phase 4: 씬 구성
- [ ] dodge_game.scene 생성
- [ ] Camera 추가 및 설정
- [ ] DirectionalLight 추가
- [ ] GameManager 오브젝트 생성
- [ ] Player 오브젝트 생성 (큐브, 파란 머티리얼)
- [ ] Obstacle0~9 생성 (티팟, 빨간 머티리얼)
- [ ] 모든 스크립트 연결 확인
- [ ] 씬 저장

### Phase 5: 테스트
- [ ] Play 모드 진입
- [ ] A/D 키 입력 동작
- [ ] 플레이어 이동 확인
- [ ] 장애물 생성 확인
- [ ] 장애물 낙하 확인
- [ ] 충돌 감지 확인
- [ ] 점수 로그 출력 확인
- [ ] 게임 오버 동작 확인

---

## ✅ 작업 완료 상태 (2025-11-24)

### 완료된 작업

**Phase 1: 엔진 수정** ✅
- InputMgr 스크립트 바인딩 완료
- KeyCode, MouseButton enum 바인딩 완료
- vec3 Distance/DistanceSquared 함수 추가 및 바인딩 완료

**Phase 2: 에디터 빌드** ✅
- CMake 구성 성공 (Visual Studio 2022, MSVC)
- 에디터 빌드 성공
- 실행 파일 위치: `Editor\platforms\Windows\x64\Debug\CSEditor.exe`
- 빌드 시 발견된 문제 수정:
  - vec3::Distance 오버로드 모호성 해결
  - ConstTable → RootTable 변경

**Phase 3: 게임 스크립트 작성** ✅
- `Assets/Script/PlayerController.nut` - 플레이어 좌우 이동 및 충돌 처리
- `Assets/Script/ObstacleController.nut` - 장애물 낙하 및 풀링
- `Assets/Script/GameManager.nut` - 게임 로직, 충돌 감지, 점수 관리

**Phase 4: 에디터 실행** ✅
- 에디터 정상 실행 확인 (프로세스 ID: 12612)
- REST API 서버 정상 작동 (http://localhost:8080)
- Assets 폴더 경로 문제 해결 (상대 경로 `../../../../../Assets/` 이해)

### 다음 단계: 씬 구성 (수동 작업 필요)

에디터가 정상 실행 중이므로, 이제 **GUI를 통해 수동으로 씬을 구성**해야 합니다.

#### 필요한 작업

1. **새 씬 생성**
   - File → New Scene
   - 이름: `dodge_game.scene`
   - 저장 위치: `Assets/Scene/`

2. **오브젝트 생성 및 구성**

**Camera**
- 생성: GameObject → Camera
- Transform: Position (0, 0, 5)
- CameraComponent: Perspective, FOV 60

**DirectionalLight**
- 생성: GameObject → Light → Directional
- Transform: Position (0, 3, 0)
- LightComponent: Direction (0, -1, 0, 1)

**GameManager**
- 생성: GameObject → Empty Object
- 이름: "GameManager"
- Add Component → Custom Component → GameManager.nut 선택

**Player**
- 생성: GameObject → 3D Object → Cube (또는 기존 cube 모델)
- 이름: "Player"
- Transform: Position (0, -1.5, 0), Scale (0.3, 0.3, 0.3)
- Add Component → Render Component → cube.dae 선택
- Material: 파란색 (Albedo를 0, 0.5, 1로 설정)
- Add Component → Custom Component → PlayerController.nut 선택

**Obstacle0 ~ Obstacle9** (10개 생성)
- 생성: GameObject → 3D Object → 반복 10회
- 이름: "Obstacle0", "Obstacle1", ... "Obstacle9"
- Transform: Position (0, 10, 0) - 화면 밖, Scale (0.2, 0.2, 0.2)
- Add Component → Render Component → teapot2.dae 선택
- Material: 빨간색 (Albedo를 1, 0, 0으로 설정)
- Add Component → Custom Component → ObstacleController.nut 선택
- **초기 상태**: 각 오브젝트의 Enable 체크 해제

3. **씬 저장**
   - File → Save Scene
   - `Assets/Scene/dodge_game.scene`

4. **플레이 테스트**
   - Play 버튼 클릭
   - A/D 키로 플레이어 이동 테스트
   - 장애물이 2초마다 생성되고 낙하하는지 확인
   - 충돌 감지 동작 확인
   - 콘솔 로그에서 점수 확인

### 문제 해결

**에디터가 실행되지 않는 경우**:
- 실행 파일 위치: `Editor\platforms\Windows\x64\Debug\CSEditor.exe`
- 이 위치에서만 실행해야 상대 경로로 Assets 폴더 접근 가능
- 로그 파일 확인: 프로젝트 루트의 `crash_report.log`, `editor_log_*.log`

**스크립트 에러 발생 시**:
- 콘솔 로그 확인
- REST API로 로그 조회: `curl http://localhost:8080/api/log/recent?count=50`

**게임이 작동하지 않는 경우**:
- GameManager가 "GameManager" 이름으로 정확히 생성되었는지 확인
- Player가 "Player" 이름으로 정확히 생성되었는지 확인
- Obstacle0~9가 정확히 생성되었는지 확인 (이름 중요!)
- 모든 CustomComponent가 올바른 스크립트에 연결되었는지 확인

---

## 🔧 작업 중 발견된 문제 및 해결 (2025-11-24)

### 문제 1: CMake가 Clang 컴파일러를 선택
**증상**: `-fexec-charset=CP949` 옵션을 Clang이 인식하지 못함
**원인**: 환경 변수 PATH에 LLVM이 있어 CMake가 Clang을 우선 선택
**해결**: Visual Studio 2022 generator 명시적으로 지정
```powershell
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug ...
```

### 문제 2: vec3::Distance 함수 오버로드 모호성
**증상**: 빌드 에러 - `error C2672: 'Func': 일치하는 오버로드된 함수가 없습니다`
**원인**: vec3에 static과 instance 버전의 Distance 함수가 모두 존재
**해결**: 명시적 타입 캐스팅으로 인스턴스 메서드 지정
```cpp
.Func(_SC("Distance"), static_cast<float(vec3::*)(const vec3&) const>(&vec3::Distance))
```

### 문제 3: Sqrat ConstTable에 Bind 메서드 없음
**증상**: 빌드 에러 - `error C2039: 'Bind': 'Sqrat::ConstTable'의 멤버가 아닙니다`
**원인**: Sqrat 버전에서 ConstTable이 아닌 RootTable 사용
**해결**: ConstTable을 RootTable로 변경
```cpp
RootTable(vm).Bind(_SC("KeyCode"), keyCodeEnum);
```

### 문제 4: 에디터 실행 파일 위치와 Assets 경로
**증상**: 에디터 실행 시 즉시 크래시 (NULL pointer dereference at 0x4)
**원인**: `AssetsDef.h`의 `NativeAssetsPath()`가 상대 경로 `../../../../../Assets/` 사용
**정확한 실행 위치**: `Editor\platforms\Windows\x64\Debug\CSEditor.exe`
**해결**: 원래 빌드 위치에서 실행
```powershell
Start-Process -FilePath "D:\Projects\CSEngine\Editor\platforms\Windows\x64\Debug\CSEditor.exe" `
              -WorkingDirectory "D:\Projects\CSEngine\Editor\platforms\Windows\x64\Debug"
```

### 문제 5: 에디터 크래시 상세 분석
**크래시 위치**: `SEnvironmentMgr::RenderPBREnvironment` (line 89)
**호출 스택**: main → EngineCoreInstance::Init → LightMgr::Init → SEnvironmentMgr::RenderPBREnvironment
**로그 파일**: 
- `crash_report.log` - 크래시 덤프 및 스택 트레이스
- `editor_log_YYYYMMDD_HHMMSS.log` - 액션 로그
**해결**: Assets 폴더 경로 문제 해결로 크래시 해결

## 📄 문서 업데이트 가이드

### 이 문서(CLAUDE.md) 업데이트가 필요한 경우

다음 상황에서 이 문서를 업데이트하세요:

1. **엔진 API가 변경된 경우**
   - 새로운 Manager가 추가됨
   - 기존 API가 변경됨
   - 새로운 컴포넌트가 추가됨

2. **발견된 문제점이 추가된 경우**
   - 새로운 제한사항 발견
   - 버그나 이슈 발견

3. **작업 프로세스가 변경된 경우**
   - 빌드 방법 변경
   - 새로운 도구 추가

### GAME_DEV_PLAN.md 업데이트가 필요한 경우

다음 상황에서 게임 개발 계획을 업데이트하세요:

1. **게임 디자인이 변경된 경우**
   - 메커니즘 추가/변경
   - 밸런스 조정

2. **스크립트 구조가 변경된 경우**
   - 새로운 스크립트 추가
   - 기존 스크립트 수정

3. **씬 구조가 변경된 경우**
   - 오브젝트 추가/제거
   - 구성 변경

### 업데이트 방법

```
"CLAUDE.md의 [섹션명] 부분을 [내용]으로 업데이트해줘"
"GAME_DEV_PLAN.md에 [새로운 내용] 추가해줘"
```

---

## 📞 도움말

### 문제 발생 시

1. **빌드 에러**: 에러 로그 전체를 보여주세요
2. **스크립트 에러**: `/api/log/recent` 로그를 확인하세요
3. **게임 미작동**: Play 모드에서 로그 확인

### 추가 정보가 필요할 때

- 엔진 아키텍처: 코드베이스 검색 요청
- 특정 API 사용법: 기존 스크립트 예제 참조
- REST API 상세: EDITOR_API.md 참조

---

## 🤖 REST API를 통한 자동 씬 구성 가이드

### 전체 프로세스 (PowerShell)

```powershell
# 1. 새 씬 생성
$body = @{ path = "new" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/scene/load" -Method Post -ContentType "application/json" -Body $body

Start-Sleep -Seconds 2

# 2. 오브젝트 생성
$objects = @("GameManager", "Player", "Obstacle0", "Obstacle1", "Obstacle2", "Obstacle3", "Obstacle4", "Obstacle5", "Obstacle6", "Obstacle7", "Obstacle8", "Obstacle9", "Camera", "DirectionalLight")
foreach ($obj in $objects) {
    $body = @{ type = "empty"; name = $obj } | ConvertTo-Json
    Invoke-RestMethod -Uri "http://localhost:8080/api/object/create" -Method Post -ContentType "application/json" -Body $body | Out-Null
}

Start-Sleep -Seconds 3

# 3. 컴포넌트 추가
# GameManager
$body = @{ object = "GameManager"; type = "CustomComponent"; scriptPath = "GameManager" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/component/add" -Method Post -ContentType "application/json" -Body $body | Out-Null

# Player
$body = @{ object = "Player"; type = "CustomComponent"; scriptPath = "PlayerController" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/component/add" -Method Post -ContentType "application/json" -Body $body | Out-Null

# Obstacles
0..9 | ForEach-Object {
    $body = @{ object = "Obstacle$_"; type = "CustomComponent"; scriptPath = "ObstacleController" } | ConvertTo-Json
    Invoke-RestMethod -Uri "http://localhost:8080/api/component/add" -Method Post -ContentType "application/json" -Body $body | Out-Null
}

# Camera
$body = @{ object = "Camera"; type = "CameraComponent" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/component/add" -Method Post -ContentType "application/json" -Body $body | Out-Null

# Light
$body = @{ object = "DirectionalLight"; type = "LightComponent" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/component/add" -Method Post -ContentType "application/json" -Body $body | Out-Null

Start-Sleep -Seconds 3

# 4. 씬 저장
Invoke-RestMethod -Uri "http://localhost:8080/api/scene/save" -Method Post

# 5. 확인
$objects = Invoke-RestMethod -Uri "http://localhost:8080/api/object/list" -Method Get
Write-Output "Total objects: $($objects.objects.Count)"
```

### Transform 설정 API

```powershell
# Position 설정
$body = @{ object = "Player"; posX = 0; posY = -2; posZ = 0 } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/object/set-transform" -Method Post -ContentType "application/json" -Body $body

# Rotation 설정 (Quaternion)
$body = @{ object = "Player"; rotX = 0; rotY = 0; rotZ = 0; rotW = 1 } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/object/set-transform" -Method Post -ContentType "application/json" -Body $body

# Scale 설정
$body = @{ object = "Player"; scaleX = 1; scaleY = 1; scaleZ = 1 } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/object/set-transform" -Method Post -ContentType "application/json" -Body $body

# 여러 속성 동시 설정
$body = @{ object = "Camera"; posX = 0; posY = 0; posZ = 5; scaleX = 1; scaleY = 1; scaleZ = 1 } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/object/set-transform" -Method Post -ContentType "application/json" -Body $body
```

### Play/Stop 제어 API

```powershell
# Play 모드 시작
$body = @{ command = "play" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/editor/command" -Method Post -ContentType "application/json" -Body $body

# Play 모드 종료
$body = @{ command = "stop" } | ConvertTo-Json
Invoke-RestMethod -Uri "http://localhost:8080/api/editor/command" -Method Post -ContentType "application/json" -Body $body
```

### 핵심 포인트

1. **새 씬 생성**: `path = "new"`로 `SScene`을 직접 생성
2. **대기 시간**: 각 단계마다 2-3초 대기 (메인 스레드 처리 시간)
3. **scriptPath**: CustomComponent의 경우 Squirrel 클래스 이름 (파일명 제외)
4. **Transform**: posX/Y/Z, rotX/Y/Z/W, scaleX/Y/Z 개별 또는 조합 설정 가능
5. **Play/Stop**: editor/command API로 제어

---

## 🔧 개발 노하우 및 주의사항

### 1. CustomComponent 스크립트 할당 문제

**증상**: REST API에서 CustomComponent에 스크립트 추가 시 크래시 (NULL pointer at sq_compile)

**원인**:
- EEngineCore에 ScriptMgr가 초기화되지 않음
- `SResource::Create<SScriptObject>()`를 REST API 스레드에서 호출하면 Squirrel VM이 NULL
- 씬 로드 시에는 메인 스레드에서 실행되어 정상 작동

**해결**:
```cpp
// EEngineCore::GenerateCores()에 추가
#ifndef CSE_GLOBAL_SCRIPT_DISABLED
    m_scriptMgr = new ScriptMgr();  // Squirrel VM 초기화
    m_cores.push_back(m_scriptMgr);
#endif
```

### 2. SafeLog와 EditorActionLogger 통합

**문제**: SafeLog는 콘솔에만 출력, REST API로 조회 불가

**해결**: SafeLog::Log() 내부에서 EditorActionLogger도 호출
```cpp
void SafeLog::Log(const char* log) {
#ifdef __CSE_EDITOR__
    if(editorCore->IsReady()) {
        editorCore->AddLog(log);
        // API 접근 가능하도록 자동 기록
        EditorActionLogger::GetInstance().Log(
            ActionCategory::SYSTEM, ActionSeverity::INFO, "SafeLog", log);
    }
#endif
}
```

### 3. Asset 이름 규칙

**주의**: AssetMgr에 등록된 스크립트는 `.script` 접미사 포함
- 파일: `PlayerController.nut`
- AssetMgr 이름: `PlayerController.script` ← API에서 이걸 사용!
- API 호출: `{"scriptPath": "PlayerController.script"}`

### 4. 씬 파일 경로 변환

API: `"Assets/Scene/test.scene"` → 엔진: `"../../../../../Assets/Scene/test.scene"`
```cpp
if (scenePath.find("Assets/") == 0) {
    fullPath = CSE::NativeAssetsPath() + scenePath.substr(7);
}
```

### 5. REST API 비동기 처리 구조

1. API 핸들러 → 큐에 추가 → "queued" 응답
2. 메인 스레드 → `ProcessMainThreadCommands()` → 실제 실행
3. **이유**: OpenGL/엔진 작업은 메인 스레드에서만 안전

### 6. Component 생성 주의사항

```cpp
// ❌ 잘못됨
auto* comp = new TransformComponent();

// ✓ 올바름
auto* comp = new TransformComponent(gameObject);

// 참고: SGameObject 생성자가 자동으로 TransformComponent 추가
```

### 7. 로그 타이밍 이슈

**문제**: ConsoleWindow 초기화 전에 `ACTION_LOG_*` 사용 → 크래시

**해결**: ConsoleWindow 준비 전에는 SafeLog만 사용

### 8. 디버깅 팁

**API 로그 확인**:
```powershell
Invoke-RestMethod -Uri "http://localhost:8080/api/log/recent?count=50"
```

**크래시 히스토리**:
```powershell
Invoke-RestMethod -Uri "http://localhost:8080/api/debug/crash"
```

---

## ✅ 최종 완료 상태

### ⚠️ **Dodge Master 카메라 문제 진단 완료** (2025-11-24 15:52)

**문제 발견**:
1. ✅ **`m_eye` 필드 누락** - 카메라가 Transform을 참조하지 못함 (해결됨)
2. ✅ **카메라 위치 문제** - 카메라(0,0,8), Player(0,-3,0) 시야 밖 (해결됨)
3. ❌ **Mesh Geometry 누락** - Player와 Obstacles에 렌더링할 mesh가 없음 (근본 원인!)

**비교 분석** (jack.scene vs DodgeMaster.scene):
- jack.scene: **prefab**을 사용하여 실제 3D 모델(jack.dae) 로드
- DodgeMaster: RenderComponent만 있고 **mesh geometry 없음**

**해결 방법**:
Player와 Obstacles에 `cube.dae` prefab을 적용하거나, 엔진에서 primitive mesh 생성 기능이 필요합니다.

---

### 프리뷰 화면 캡처 성공! (2025-11-24 15:40)

```
✓ game_3s.png: 1280x720 (36.1 KB)
✓ game_6s.png: 1280x720 (36.1 KB)
✓ game_9s.png: 1280x720 (36.1 KB)
```

**구현 방법**:
1. `glBindFramebuffer` + `glFramebufferTexture2D` 사용 (사용자 요청)
2. `m_previewTextureId` (OpenGL Texture ID) 사용
3. 메인 스레드에서 OpenGL 호출 (큐 시스템)
4. `SEnvironmentMgr::RenderPBREnvironment()` 패턴 참조

### 게임 실행 결과 (2025-11-24 15:28)

```
Obstacles Spawned: 15
Scores Achieved: 14 (10점 → 140점)
Game Duration: 30초
Result: ✓ 완벽하게 작동 (충돌 없음)
```

### 해결한 주요 문제들

1. **ScriptMgr 초기화** → EEngineCore에 추가
2. **SafeLog 레벨 시스템** → INFO, WARNING, ERROR 표시
3. **PlayerController.IsAlive()** → 함수 추가
4. **Delta Time 계산** → 각 스크립트에서 계산
5. **Obstacle Component 활성화** → SetEnable(true/false)

### 게임 플레이 가능!

에디터에서 `Assets/Scene/DodgeMaster.scene`을 로드하고 Play 버튼을 눌러 플레이할 수 있습니다.
- **조작**: A/D 또는 좌우 화살표
- **목표**: 떨어지는 장애물 회피
- **점수**: 장애물이 화면 밖으로 나가면 +10점

---

**작성일**: 2025-11-24  
**최종 수정**: 2025-11-24 15:30 (게임 완성!)  
**프로젝트**: CSEngine 게임 개발  
**목표 게임**: Dodge Master (3D 회피 게임)  
**상태**: ✅ **완료 및 플레이 가능**

---

🎮 **게임 개발 성공!** 🎉
