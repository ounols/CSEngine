# CSEngine 게임 개발 계획서

## 📋 프로젝트 개요

### 게임 제목
**"Dodge Master" (회피의 달인)**

### 게임 장르
3D 액션 회피 게임

### 게임 컨셉
플레이어가 좌우로 이동하면서 위에서 떨어지는 장애물을 피하는 간단한 아케이드 스타일 게임입니다.

---

## 🎮 게임 메커니즘

### 핵심 게임플레이
1. **플레이어 조작**
   - 키보드 A/D 또는 ←/→ 키로 좌우 이동
   - 플레이어는 X축(좌우)으로만 이동 가능
   - 이동 속도는 일정하게 유지

2. **장애물 시스템**
   - 화면 위쪽에서 무작위 X 위치에 장애물 생성
   - 장애물은 Y축을 따라 아래로 낙하
   - 일정 시간마다 새로운 장애물 생성
   - 화면 아래로 벗어난 장애물은 자동 제거

3. **충돌 감지**
   - 플레이어와 장애물 간 거리 기반 충돌 감지
   - 충돌 시 게임 오버

4. **점수 시스템**
   - 장애물을 성공적으로 피할 때마다 점수 획득
   - 시간이 지날수록 난이도 증가 (장애물 생성 속도/낙하 속도 증가)

---

## 🛠 필요한 기능 구현

### 1. 엔진 수정 사항

#### InputMgr 스크립트 바인딩 (필수)
현재 ScriptMgr에 InputMgr가 바인딩되어 있지 않습니다.

**파일**: `src/Manager/ScriptMgr.cpp`

```cpp
// DefineClasses() 함수에 추가할 내용:

// InputMgr 바인딩
SQRClassDef<InputMgr>(_SC("Input"), vm)
    .StaticFunc(_SC("GetKey"), &InputMgr::GetKey)
    .StaticFunc(_SC("GetKeyDown"), &InputMgr::GetKeyDown)
    .StaticFunc(_SC("GetKeyUp"), &InputMgr::GetKeyUp);

// KeyCode enum 바인딩
Enumeration keyCodeEnum(vm);
keyCodeEnum.Const(_SC("A"), static_cast<int>(KeyCode::A));
keyCodeEnum.Const(_SC("D"), static_cast<int>(KeyCode::D));
keyCodeEnum.Const(_SC("Left"), static_cast<int>(KeyCode::Left));
keyCodeEnum.Const(_SC("Right"), static_cast<int>(KeyCode::Right));
keyCodeEnum.Const(_SC("Space"), static_cast<int>(KeyCode::Space));
keyCodeEnum.Const(_SC("Escape"), static_cast<int>(KeyCode::Escape));
ConstTable(vm).Bind(_SC("KeyCode"), keyCodeEnum);
```

#### vec3 거리 계산 함수 추가 (선택)
충돌 감지를 위한 거리 계산 함수가 있는지 확인 필요. 없다면 추가.

**파일**: `src/Util/Vector.h`

```cpp
// vec3에 Distance 함수 추가
float Distance(const vec3& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

float DistanceSquared(const vec3& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return dx*dx + dy*dy + dz*dz;
}
```

그리고 ScriptMgr.cpp에서 바인딩:
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

### 2. 게임 에셋

#### 필요한 3D 모델
- **플레이어**: `cube.dae` 사용 (작은 큐브)
- **장애물**: `teapot2.dae` 또는 `cube.dae` 사용

#### 머티리얼
- **플레이어 머티리얼**: 파란색 (RGB: 0, 0.5, 1)
- **장애물 머티리얼**: 빨간색 (RGB: 1, 0, 0)
- **바닥/배경**: 기본 회색 유지

---

## 📝 스크립트 구조

### 1. PlayerController.nut
플레이어의 이동과 상태를 관리하는 스크립트

**주요 기능**:
- 키보드 입력 처리 (A/D, ←/→)
- 좌우 이동 (X축 제한)
- 이동 범위 제한 (-2 ~ 2)
- 충돌 감지

**변수**:
- `moveSpeed`: 이동 속도
- `minX`, `maxX`: 이동 범위
- `gameManager`: 게임 매니저 참조
- `isAlive`: 생존 상태

### 2. ObstacleController.nut
개별 장애물의 낙하와 제거를 관리

**주요 기능**:
- Y축을 따라 아래로 이동
- 화면 밖으로 나가면 자동 제거
- 플레이어와의 거리 체크

**변수**:
- `fallSpeed`: 낙하 속도
- `deleteY`: 제거될 Y 좌표

### 3. GameManager.nut
게임의 전체 흐름을 관리하는 메인 스크립트

**주요 기능**:
- 장애물 생성 (일정 시간마다)
- 점수 관리
- 게임 상태 관리 (시작, 진행, 종료)
- 난이도 조절 (시간에 따라)

**변수**:
- `score`: 현재 점수
- `spawnInterval`: 장애물 생성 간격
- `spawnTimer`: 생성 타이머
- `gameState`: 게임 상태 (READY, PLAYING, GAMEOVER)
- `playerObject`: 플레이어 참조
- `obstaclePrefab`: 장애물 프리팹

### 4. UIManager.nut (선택 사항)
점수 표시 및 게임 오버 UI 관리

---

## 🎨 씬 구조

### Scene: dodge_game.scene

```
root
├── Camera
│   └── CameraComponent (orthographic or perspective)
├── DirectionalLight
│   └── LightComponent
├── GameManager (Empty Object)
│   └── CustomComponent (GameManager.nut)
├── Player
│   ├── TransformComponent (position: 0, -1.5, 0)
│   ├── RenderComponent (cube mesh, blue material)
│   └── CustomComponent (PlayerController.nut)
├── Ground (Optional)
│   ├── TransformComponent (position: 0, -2, 0)
│   └── RenderComponent (plane mesh)
└── ObstaclePrefab (Prefab, will be instantiated)
    ├── TransformComponent
    ├── RenderComponent (teapot/cube mesh, red material)
    └── CustomComponent (ObstacleController.nut)
```

---

## 📐 좌표 및 수치 설정

### 게임 공간
- **X축 범위**: -2 ~ 2 (플레이어 이동 영역)
- **Y축 범위**: -2 ~ 3 (게임 화면, 아래가 -2, 위가 3)
- **Z축**: 고정 (0)

### 오브젝트 크기
- **플레이어**: scale(0.3, 0.3, 0.3)
- **장애물**: scale(0.2, 0.2, 0.2)

### 게임 밸런스
- **플레이어 이동 속도**: 2.0 units/sec
- **장애물 초기 낙하 속도**: 1.5 units/sec
- **장애물 생성 간격**: 2초 (초기)
- **충돌 반경**: 0.4 units
- **난이도 증가**: 10초마다 장애물 속도 10% 증가

---

## 🔄 개발 단계

### Phase 1: 엔진 수정 ✅
1. InputMgr를 Squirrel 스크립트에 바인딩
2. vec3 Distance 함수 추가 및 바인딩
3. 에디터 빌드 및 테스트

### Phase 2: 기본 씬 구성 ✅
1. 새 씬 생성 (dodge_game.scene)
2. 카메라 설정
3. 조명 설정
4. 플레이어 오브젝트 생성 및 배치

### Phase 3: 플레이어 구현 ✅
1. PlayerController.nut 스크립트 작성
2. 입력 처리 테스트
3. 이동 제한 테스트

### Phase 4: 장애물 시스템 ✅
1. ObstacleController.nut 스크립트 작성
2. 장애물 프리팹 생성
3. 낙하 테스트

### Phase 5: 게임 매니저 ✅
1. GameManager.nut 스크립트 작성
2. 장애물 생성 시스템 구현
3. 충돌 감지 구현
4. 점수 시스템 구현

### Phase 6: 테스트 및 밸런싱 ✅
1. 게임플레이 테스트
2. 난이도 조정
3. 버그 수정

---

## 🚀 확장 가능성

게임이 완성된 후 추가할 수 있는 기능들:

1. **파워업 아이템**
   - 쉴드 (한 번 충돌 무시)
   - 슬로우 모션 (시간 느리게)
   - 점수 2배

2. **다양한 장애물 타입**
   - 빠른 장애물
   - 좌우로 움직이는 장애물
   - 큰 장애물

3. **비주얼 효과**
   - 충돌 시 파티클 효과
   - 트레일 효과
   - 점수 획득 시 이펙트

4. **사운드**
   - BGM
   - 효과음 (이동, 충돌, 점수)

5. **최고 점수 저장**
   - 파일 시스템을 이용한 저장/로드

---

## ⚠️ 잠재적 문제점 및 해결 방안

### 1. 프리팹 인스턴스화
**문제**: 스크립트에서 프리팹을 동적으로 생성하는 API가 없을 수 있음
**해결**: 
- GameObject.Clone() 또는 유사 함수 확인
- 없다면 미리 여러 개의 장애물을 생성하고 오브젝트 풀링 사용
- 또는 C++에서 인스턴스화 함수 추가

### 2. 오브젝트 제거
**문제**: 스크립트에서 GameObject.Destroy()가 제대로 작동하지 않을 수 있음
**해결**:
- SetEnable(false)로 비활성화 후 재사용
- 오브젝트 풀링 패턴 사용

### 3. 텍스트 렌더링
**문제**: 점수를 화면에 표시할 UI 시스템이 없을 수 있음
**해결**:
- Log()로 콘솔에만 출력
- 또는 3D 텍스트/라벨 시스템 활용 (label.jpg 에셋 존재)

### 4. 게임 재시작
**문제**: 게임 오버 후 재시작 기능 구현
**해결**:
- 모든 오브젝트 상태 리셋
- 또는 씬 리로드 (SceneMgr 활용)

---

## 📊 성공 기준

이 게임 프로젝트가 성공적으로 완료되었다고 판단하는 기준:

✅ 플레이어가 좌우로 부드럽게 이동
✅ 장애물이 정상적으로 생성되고 낙하
✅ 충돌 감지가 정확하게 작동
✅ 점수가 정상적으로 증가
✅ 게임 오버 후 로그에 최종 점수 출력
✅ 최소 10회 이상 안정적으로 플레이 가능

---

## 📚 참고 자료

- **엔진 문서**: CSEngine README.md
- **에디터 API**: EDITOR_API.md
- **기존 스크립트 예제**: 
  - Assets/Script/testScript.nut
  - Assets/Script/Wave.nut
  - Assets/Script/rotateDirection.nut

---

**작성일**: 2025-11-24
**예상 개발 시간**: 4-6시간
**난이도**: 초급-중급

