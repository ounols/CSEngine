# CSEditor REST API Documentation

CSEditor에 통합된 REST API 서버로, 외부 도구(curl, MCP 등)에서 에디터를 제어할 수 있습니다.

## 서버 정보

- **기본 포트**: 8080
- **프로토콜**: HTTP
- **Content-Type**: application/json

## API 엔드포인트

### Health & Status

```bash
# 서버 상태 확인
curl http://localhost:8080/api/health
# Response: {"status":"ok","server":"CSEditor API"}

# 서버 통계
curl http://localhost:8080/api/status
# Response: {"running":true,"port":8080,"requestCount":5,"errorCount":0}
```

### Scene 관련

```bash
# 씬 정보 조회
curl http://localhost:8080/api/scene/info
# Response: {"name":"ChoAn"}

# 씬 로드 (POST)
curl -X POST http://localhost:8080/api/scene/load \
     -H "Content-Type: application/json" \
     -d '{"path":"Asset/Scene/ChoAn.scene"}'
# Response: {"status":"queued","path":"Asset/Scene/ChoAn.scene","message":"Scene will be loaded on next frame"}

# 씬 저장 (POST)
curl -X POST http://localhost:8080/api/scene/save
```

### Object 관련

```bash
# 오브젝트 목록
curl http://localhost:8080/api/object/list
# Response: {"objects":[{"name":"Player","enabled":true,...}]}

# 오브젝트 상세 정보
curl "http://localhost:8080/api/object/info?name=Player"
# Response: {"name":"Player","enabled":true,"transform":{...},"components":[...]}

# 오브젝트 선택
curl "http://localhost:8080/api/object/select?name=Player"

# 오브젝트 생성 (POST)
curl -X POST http://localhost:8080/api/object/create \
     -H "Content-Type: application/json" \
     -d '{"type":"empty","name":"NewObject"}'

# 오브젝트 삭제 (POST)
curl -X POST http://localhost:8080/api/object/delete \
     -H "Content-Type: application/json" \
     -d '{"name":"ObjectToDelete"}'
```

### Component 관련

```bash
# 컴포넌트 목록
curl "http://localhost:8080/api/component/list?object=Player"
# Response: {"object":"Player","components":[{"type":"TransformComponent"},{"type":"RenderComponent"}]}

# 컴포넌트 추가 (POST)
curl -X POST http://localhost:8080/api/component/add \
     -H "Content-Type: application/json" \
     -d '{"object":"Player","type":"LightComponent"}'

# 컴포넌트 제거 (POST)
curl -X POST http://localhost:8080/api/component/remove \
     -H "Content-Type: application/json" \
     -d '{"object":"Player","type":"LightComponent"}'
```

### Editor 제어

```bash
# 에디터 정보
curl http://localhost:8080/api/editor/info
# Response: {"version":"CSEditor 1.0","isPreview":false,"apiPort":8080,"sessionStart":"2025-11-23 17:26:25.949"}

# 에디터 명령 (play/stop)
curl -X POST http://localhost:8080/api/editor/command \
     -H "Content-Type: application/json" \
     -d '{"command":"play"}'

curl -X POST http://localhost:8080/api/editor/command \
     -H "Content-Type: application/json" \
     -d '{"command":"stop"}'
```

### Log 관련

```bash
# 최근 로그 조회
curl "http://localhost:8080/api/log/recent?count=20"

# 로그 클리어 (POST)
curl -X POST http://localhost:8080/api/log/clear
```

### Debug/Crash 정보

```bash
# 크래시 히스토리
curl http://localhost:8080/api/debug/crash
# Response: {"crashHistory":[...],"systemInfo":{...}}

# 디버그 컨텍스트
curl http://localhost:8080/api/debug/context
# Response: {"systemInfo":{...},"debuggerPresent":false,"currentThreadId":1234,"loadedModules":[...]}

# 액션 히스토리
curl "http://localhost:8080/api/debug/history?count=50"
# Response: {"history":[{"action":"...","context":"...","timestamp":"..."}],"count":50}

# 수동 디버그 덤프 트리거 (POST)
curl -X POST http://localhost:8080/api/debug/dump \
     -H "Content-Type: application/json" \
     -d '{"reason":"Manual test dump"}'
```

## 에러 핸들링

API 요청 중 예외가 발생하면 자동으로 디버그 정보가 포함된 에러 응답이 반환됩니다:

```json
{
  "error": "Exception: null pointer access",
  "endpoint": "/api/object/info",
  "recentActions": [
    {"action": "Select Object", "context": "Hierarchy", "timestamp": "..."}
  ],
  "systemInfo": {"os": "Windows", "arch": "x64"},
  "stackTrace": [
    {"function": "HandleObjectInfo", "file": "EditorAPIServer.cpp", "line": 450}
  ]
}
```

## 사용 가능한 씬 파일

```
Asset/Scene/ChoAn.scene
Asset/Scene/animation.scene
Asset/Scene/jack.scene
Asset/Scene/sdfgi.scene
```

## 현재 작업 상태

### 완료된 기능
- [x] REST API 서버 (cpp-httplib 기반)
- [x] 모든 기본 엔드포인트 구현
- [x] CORS 지원
- [x] 자동 에러 핸들링 (디버그 정보 포함)
- [x] 씬 로드 API (메인 스레드 큐잉)
- [x] 크래시/디버그 정보 API

### 테스트 결과
- `/api/health` - ✅ 정상
- `/api/status` - ✅ 정상
- `/api/editor/info` - ✅ 정상
- `/api/debug/history` - ✅ 정상
- `/api/log/recent` - ✅ 정상
- `/api/scene/load` (POST) - ✅ 정상 (큐잉됨)
- `/api/scene/info` - ⚠️ 씬 로드 후 확인 필요

### 관련 파일
- `Editor/src/Manager/EditorAPIServer.h` - API 서버 헤더
- `Editor/src/Manager/EditorAPIServer.cpp` - API 서버 구현
- `Editor/src/Util/httplib.h` - cpp-httplib 라이브러리
- `Editor/src/main.cpp` - ProcessMainThreadCommands 호출 추가
