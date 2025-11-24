# CSEngine 문제 해결 가이드

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
**확인**: scriptPath는 클래스명만 (파일명 아님)
```json
{"scriptPath": "PlayerController"}  // ✓
{"scriptPath": "PlayerController.nut"}  // ✗
```

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

```powershell
# 로그 확인
Invoke-RestMethod "http://localhost:8080/api/log/recent?count=50"

# 오브젝트 목록
Invoke-RestMethod "http://localhost:8080/api/object/list"
```

## 크래시 히스토리 확인하는 로그 파일 경로
`Editor\platforms\Windows\x64\Debug\` : 여기서 `editor_log_[가장 최근 날짜 및 시간].json`을 확인.
