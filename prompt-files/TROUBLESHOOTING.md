# CSEngine 문제 해결 가이드

> **중요:** 문제 발생 시 에디터를 재시작하기 전에 **반드시 로그를 먼저 확인**하세요!

## 🛡️ 예방 가이드 (문제를 미리 방지하기)

### 1. API 호출 시 항상 대기 시간 확보
**문제:** API 호출 후 즉시 다음 명령 실행 시 반영 안 됨

**예방법:**
```bash
# ❌ 나쁜 예
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"new\"}" http://localhost:8080/api/scene/load
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Player\"}" http://localhost:8080/api/object/create

# ✅ 좋은 예
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"new\"}" http://localhost:8080/api/scene/load
Start-Sleep -Seconds 2
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Player\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2
```

**이유:** REST API는 pending 큐를 통해 비동기로 처리됩니다.

### 2. CustomComponent는 해시 값으로 추가한다
**문제:** CustomComponent 추가 시 스크립트 로드 실패

**예방법:**
```bash
# ❌ 잘못된 방법 (파일명이나 클래스명 사용)
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Player\",\"type\":\"CustomComponent\",\"scriptPath\":\"PlayerController\"}" http://localhost:8080/api/component/add

# ✅ 올바른 방법 (rawValues로 해시 값 전달)
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Player\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value></comp>\"}" http://localhost:8080/api/component/add
```

**해시 값 찾기:**
- `.meta` 파일 확인: `Assets/Script/PlayerController.nut.meta`의 hash 값
- 각 스크립트 파일마다 고유한 해시 값 존재

### 3. C++ 수정 후 반드시 빌드
**문제:** 엔진 소스 수정 후 변경사항이 반영 안 됨

**예방법:**
- `src/` 폴더의 C++ 파일 수정 시 반드시 빌드
- `Assets/Script/` 폴더의 .nut 파일은 즉시 반영 (빌드 불필요)

```powershell
# C++ 수정 후
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi
```

### 4. 각 단계마다 검증하기
**예방법:**
```bash
# 오브젝트 생성 후 확인
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Player\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2
curl http://localhost:8080/api/object/list

# 로그로 에러 확인
curl "http://localhost:8080/api/log/recent?count=20"
```

### 5. Squirrel 문법 주의사항
**자주 하는 실수:**
```squirrel
// ❌ 실수 1: local 빼먹기
transform = GetTransform();  // 전역 변수가 됨

// ✅ 올바른 사용
local transform = GetTransform();

// ❌ 실수 2: deltaTime을 ms로 착각
pos.x = pos.x + speed * elapsedTime;  // 너무 빠름!

// ✅ 올바른 사용
local deltaTime = (elapsedTime - lastElapsedTime) / 1000.0;
pos.x = pos.x + speed * deltaTime;

// ❌ 실수 3: 벡터 직접 수정
transform.position.x = 5.0;  // 반영 안 됨

// ✅ 올바른 사용
local pos = transform.position;
pos.x = 5.0;
transform.position = pos;
```

---

## 빌드 문제

### CMake가 Clang 선택
**증상**: `-fexec-charset=CP949` 인식 불가
**해결**: Visual Studio generator 명시
```powershell
cmake -G "Visual Studio 17 2022" -A x64 ...
```

### vec3::Distance 오버로드 모호성
**증상**: `error C2672: 'Func': 일치하는 오버로드된 함수가 없습니다`
**해결**: 명시적 캐스팅
```cpp
.Func(_SC("Distance"), static_cast<float(vec3::*)(const vec3&) const>(&vec3::Distance))
```

### ConstTable → RootTable
**증상**: `error C2039: 'Bind': 'Sqrat::ConstTable'의 멤버가 아닙니다`
**해결**: `ConstTable(vm)` → `RootTable(vm)`

## 런타임 문제


### ScriptMgr NULL 크래시
**원인**: EEngineCore에 ScriptMgr 미초기화
**해결**: `EEngineCore::GenerateCores()`에 추가
```cpp
m_scriptMgr = new ScriptMgr();
m_cores.push_back(m_scriptMgr);
```

### 컴포넌트 생성 크래시
**원인**: SGameObject* 인자 누락
**해결**:
```cpp
auto* comp = new TransformComponent(gameObject);  // ✓
// auto* comp = new TransformComponent();  // ✗
```

## REST API 문제

### API 호출 즉시 반영 안됨
**원인**: 비동기 큐 처리
**해결**: 각 단계마다 2-3초 대기
```powershell
Start-Sleep -Seconds 2
```

### CustomComponent 스크립트 할당 실패
**확인**: rawValues에 해시 값 사용 (파일명/클래스명 아님)
```bash
# ✓ 올바른 방법
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Player\",\"type\":\"CustomComponent\",\"rawValues\":\"<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value></comp>\"}" http://localhost:8080/api/component/add

# ✗ 잘못된 방법
curl -X POST -H "Content-Type: application/json" -d "{\"object\":\"Player\",\"type\":\"CustomComponent\",\"scriptPath\":\"PlayerController\"}" http://localhost:8080/api/component/add
```

**해시 값 조회**: `Assets/Script/스크립트명.nut.meta` 파일의 hash 확인

## 게임 렌더링 문제

### 오브젝트가 보이지 않음
**원인**: Mesh geometry 누락
**해결**: prefab 사용 또는 RenderComponent에 mesh 할당
- 참고: `jack.scene`은 prefab으로 모델 로드

### 카메라 시야 밖
**확인**: Camera 위치와 대상 오브젝트 위치 확인
```
Camera: (0, 0, 8), 대상: (0, -3, 0) → 시야 밖 가능성
```

## 디버깅 명령

```bash
# 로그 확인
curl "http://localhost:8080/api/log/recent?count=50"

# 오브젝트 목록
curl http://localhost:8080/api/object/list
```

## 크래시 히스토리 확인하는 로그 파일 경로
`Editor\platforms\Windows\x64\Debug\` : 여기서 `editor_log_[가장 최근 날짜 및 시간].json`을 확인.

---

## 🔍 상황별 디버깅 플로우

### 상황 1: "게임이 실행되는데 동작이 이상해요"

**단계별 점검:**
1. **로그 확인**
   ```bash
   curl "http://localhost:8080/api/log/recent?count=100"
   ```
   - `Init()` 호출 확인 → "XXX initialized" 메시지 있는지
   - Squirrel 에러 확인 → `[ERROR] Squirrel` 검색
   - 예상 로그 출력 확인 → "Score: ", "Player moved" 등

2. **스크립트 로직 확인**
   - deltaTime 계산이 올바른가?
   - Input.GetKey() KeyCode가 올바른가?
   - 벡터 연산이 올바른가?

3. **Transform 확인**
   - 오브젝트가 화면 밖에 있는가?
   - 스케일이 너무 작거나 큰가?

### 상황 2: "스크립트가 실행되지 않아요"

**단계별 점검:**
1. **CustomComponent 추가 확인**
   ```bash
   curl http://localhost:8080/api/object/list
   ```
   - 해당 오브젝트가 존재하는지 확인

2. **로그에서 Init() 확인**
   - `Init()` 메시지가 없다면 → 스크립트 로드 실패
   - 파일명과 클래스명 일치 확인
   - `PlayerController.nut` → `class PlayerController`

3. **문법 에러 확인**
   ```bash
   curl "http://localhost:8080/api/log/recent?count=50"
   ```
   - Squirrel 에러 메시지 확인
   - 오타, 괄호 누락 등

### 상황 3: "API 호출이 응답 없어요"

**단계별 점검:**
1. **에디터 상태 확인**
   ```bash
   curl http://localhost:8080/api/health
   ```
   - 성공: 에디터 정상
   - 실패: 에디터 크래시 또는 미실행

2. **대기 시간 확인**
   - 이전 API 호출 후 2초 이상 대기했는가?
   - 특히 scene load, object create 후 충분한 대기 필요

3. **로그로 에러 확인**
   - pending 큐에서 처리되었는지 확인
   - 에러 메시지 있는지 확인

### 상황 4: "빌드가 실패해요"

**단계별 점검:**
1. **에러 메시지 읽기**
   - `error C2672`: 오버로드 모호성 → 명시적 캐스팅 필요
   - `error C2039`: 멤버 없음 → 클래스/네임스페이스 확인
   - `LNK2019`: 링커 에러 → 구현 누락

2. **일반적 해결법**
   ```powershell
   # 클린 빌드
   powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1" -Clean -Configure
   ```

3. **TROUBLESHOOTING.md의 빌드 문제 섹션 참조**

---

## 💡 자주 묻는 질문 (FAQ)

### Q1: 로그에 아무것도 안 나와요
**A:** Play 모드에서만 Squirrel 스크립트가 실행됩니다.
```bash
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
Start-Sleep -Seconds 5
curl "http://localhost:8080/api/log/recent?count=50"
```

### Q2: 오브젝트가 보이지 않아요
**A:** 현재 렌더링 문제가 있습니다 (mesh geometry 누락). 로그로 동작 확인하세요.
```squirrel
function Tick(elapsedTime) {
    local transform = GetTransform();
    Log("Position: " + transform.position.x + ", " + transform.position.y);
}
```

### Q3: API로 만든 씬을 저장하려면?
**A:**
```bash
curl -X POST http://localhost:8080/api/scene/save
```
기본 경로: `Assets/Scene/new.scene`

### Q4: 스크립트를 수정했는데 반영이 안 돼요
**A:** Squirrel 스크립트는 즉시 반영됩니다. Stop → Play로 재시작하세요.
```bash
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"stop\"}" http://localhost:8080/api/editor/command
Start-Sleep -Seconds 1
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
```

### Q5: 여러 오브젝트를 같은 이름으로 만들 수 있나요?
**A:** 가능하지만 권장하지 않습니다. `gameobject.Find("Player")`는 첫 번째만 반환합니다.
```bash
# 좋은 예: 고유한 이름
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Enemy1\"}" http://localhost:8080/api/object/create
Start-Sleep -Seconds 2
curl -X POST -H "Content-Type: application/json" -d "{\"type\":\"empty\",\"name\":\"Enemy2\"}" http://localhost:8080/api/object/create
```

---

**최종 업데이트**: 2025-11-28
