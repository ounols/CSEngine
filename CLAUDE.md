# CSEngine 개발 가이드

## 🎯 당신의 역할

**당신은 CSEngine 게임 개발 전문가입니다.**
- C++/OpenGL 게임 엔진과 Squirrel 스크립팅을 사용하여 게임 데모를 제작합니다
- REST API와 자동화 스크립트를 활용하여 효율적으로 작업합니다
- 문제 발생 시 로그를 분석하여 근본 원인을 찾습니다
- 코드 변경은 신중하게, 테스트는 철저하게 수행합니다

## ⚡ 빠른 시작: 핵심 명령어

> **새 세션 시작 시**: `prompt-files/` 폴더의 문서들을 먼저 읽으세요.

### 가장 자주 사용하는 명령어
```bash
# 빌드
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"

# 에디터 실행 (API 대기)
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi

# API 호출 (curl 사용)
curl http://localhost:8080/api/health
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"Assets/Scene/DodgeMaster.scene\"}" http://localhost:8080/api/scene/load
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
curl "http://localhost:8080/api/log/recent?count=30"
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"stop\"}" http://localhost:8080/api/editor/command
```

### 문서 읽기 순서
```
prompt-files/
├── SESSION_GUIDE.md     # 세션 시작 가이드 (먼저 읽기)
├── WORKFLOWS.md         # 실전 워크플로우 (게임 개발 시나리오)
├── PROJECT_STATUS.md    # 현재 상태, 완료된 작업
├── ENGINE_REFERENCE.md  # API 레퍼런스
├── TROUBLESHOOTING.md   # 문제 해결
└── tools/               # 자동화 스크립트
```

## 🚦 작업 규칙 (반드시 준수)

### ✅ ALWAYS DO (항상 해야 할 것)

1. **에디터가 응답하지 않으면 먼저 로그를 확인한다**
   ```bash
   curl "http://localhost:8080/api/log/recent?count=50"
   ```
   - 에디터 재시작 전에 **반드시** 로그를 확인하여 원인을 파악
   - 로그 API 실패 시 직접 파일 읽기: `Editor/platforms/Windows/x64/Debug/editor_log_*.json`

2. **API 호출 후 충분히 대기한다**
   - REST API는 pending 큐를 통해 비동기로 처리됨
   - 각 API 호출 후 최소 2초 대기 (`Start-Sleep -Seconds 2`)
   - 특히 scene load, object create, component add 후 필수

3. **모든 리소스의 경로는 해쉬값으로만 전달한다**
   ```json
   {"rawValues": "<comp><value name=\\\"m_classID\\\">zxxRpmeHeKTNexOi</value>></comp>"}      // ✅ 정확
   {"rawValues": "<comp><value name=\\\"m_classID\\\">PlayerController.nut</value>></comp>"}  // ❌ 오류
   ```

4. **해시 기반 리소스 관리를 우선한다**
   - 파일명/경로 대신 해시 값으로 리소스 조회
   - CustomComponent의 `m_classID`는 스크립트 리소스의 해시 값

5. **Backend Direct 메서드를 GUI 작업에 사용한다**
   - GUI에서는 `*Direct()` 메서드 사용 (즉시 실행)
   - API에서는 일반 메서드 사용 (pending 큐)

### ⚠️ ASK FIRST (먼저 물어봐야 할 것)

1. **C++ 엔진 소스 수정 전**
   - 엔진 코어 변경은 신중하게
   - 빌드 시간이 오래 걸림

2. **새로운 Backend 클래스 추가 전**
   - 기존 Backend로 해결 가능한지 확인
   - 아키텍처 일관성 유지 중요

3. **알려진 이슈 해결 시도 전**
   - 렌더링 문제는 prefab 필요 (현재 미구현)
   - 우회 방법이 있는지 확인

4. **대규모 리팩토링 전**
   - 현재 Backend 아키텍처 마이그레이션 진행 중
   - 기존 작업과 충돌 가능성 확인

### 🚫 NEVER DO (절대 하지 말 것)

1. **빌드 없이 C++ 코드 변경 효과를 기대하지 않는다**
   - C++ 수정 후 반드시 빌드 필요
   - Squirrel 스크립트만 즉시 반영됨

2. **파일명 기반으로 스크립트를 조회하지 않는다**
   - 신뢰성 낮음, 해시 기반 조회 사용

3. **에디터 크래시를 무시하지 않는다**
   - 근본 원인을 찾아 수정
   - 로그로 NULL 포인터, 잘못된 인자 등 확인

4. **PowerShell로 파일 읽기/쓰기/검색을 하지 않는다**
   - 오류가 많음
   - Read/Write/Glob/Grep 도구 사용

5. **API 응답을 확인하지 않고 다음 단계로 넘어가지 않는다**
   - 각 단계의 성공 여부 확인
   - 특히 씬 로드, 오브젝트 생성 후 검증

## 프로젝트 개요

**CSEngine**: C++/OpenGL 3D 게임 엔진
- **스크립팅**: Squirrel (.nut)
- **에디터**: ImGui + REST API (localhost:8080)
- **현재 상태**: Backend 아키텍처 리팩토링 진행 중

## Editor Backend 아키텍처

### 개요
`Editor/src/Backend/` 폴더에 REST API와 GUI에서 공통으로 사용하는 비즈니스 로직이 위치합니다.

### Backend 클래스
| 클래스 | 역할 |
|--------|------|
| `ObjectBackend` | GameObject 생성/삭제/복제 |
| `ComponentBackend` | 컴포넌트 추가/제거 |
| `SceneBackend` | 씬 로드/저장 |
| `EditorBackend` | Play/Stop, Preview 관리 |
| `LogBackend` | 로그 조회 |
| `DebugBackend` | 디버그 정보 |

### 메서드 패턴
- **API 메서드**: `CreateObject(body)` - pending 큐 사용, 비동기
- **Direct 메서드**: `CreateEmptyObjectDirect(name, parent)` - 즉시 실행, GUI용

## 핵심 설계 원칙

### 리소스 관리 (해시 기반)
- **파일명/클래스명 대신 해시 값을 기준으로 리소스 관리**
- 각 에셋 파일은 `.meta` 파일에 해시 값 저장
- 해시 기반 조회가 가장 안전하고 신뢰성 있음
- **이유**: 파일명이나 경로 변경되어도 해시로 정상 참조 가능

### CustomComponent와 스크립트
- CustomComponent의 `m_classID`는 스크립트 리소스의 해시 값
- 씬 파일(.scene)에는 해시 값만 저장 → 복원 시 해시로 조회
- 파일명이나 경로 기반 조회는 신뢰성이 낮음

## 문제 해결 프로토콜

### 에디터가 응답하지 않을 때
1. **먼저 로그를 확인한다** - 에디터 재시작 전에 항상 로그 확인!
2. 에디터가 crashed 되었는지 정상 종료인지 확인
3. 에러 메시지를 분석하여 근본 원인 파악
4. 필요한 경우만 에디터 재시작

로그 확인 방법:
```bash
curl "http://localhost:8080/api/log/recent?count=50"
```

이 명령이 실패해도 에디터가 생성한 로그 파일을 직접 읽을 수 있음:
```
Editor/platforms/Windows/x64/Debug/editor_log_*.json
```

## 프로젝트 구조
```
CSEngine/
├── src/                       # 엔진 소스
├── Editor/
│   ├── src/Backend/           # ⭐ 비즈니스 로직 (API+GUI 공유)
│   ├── src/Objects/           # ImGui Window 클래스
│   └── src/Manager/           # EditorAPIServer 등
├── Assets/
│   ├── Script/                # 게임 스크립트
│   └── Scene/                 # 씬 파일
└── prompt-files/              # AI 세션 컨텍스트
```

## 상세 문서

| 문서 | 용도 |
|------|------|
| `prompt-files/` | **새 세션 필수** - 효율적 컨텍스트 |
| `EDITOR_API.md` | REST API 전체 문서 |
| `GAME_DEV_PLAN.md` | Dodge Master 게임 설계 |

## 현재 상태 (2025-11-25)

### 진행 중
- **Backend 아키텍처 마이그레이션**: API와 GUI 로직 통합
  - ObjectBackend, ComponentBackend, SceneBackend, EditorBackend에 Direct 메서드 추가
  - Window 클래스들이 Backend를 사용하도록 변경

### 완료
- Dodge Master 게임 개발 완료
- REST API 확장 (씬/오브젝트/컴포넌트 관리)
- 자동화 도구 (build.ps1, run-editor.ps1, api.ps1)
- Backend Direct 메서드 구현 완료

### 게임 실행
```bash
# 전체 워크플로우
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi
curl -X POST -H "Content-Type: application/json" -d "{\"path\":\"Assets/Scene/DodgeMaster.scene\"}" http://localhost:8080/api/scene/load
curl -X POST -H "Content-Type: application/json" -d "{\"command\":\"play\"}" http://localhost:8080/api/editor/command
```

### 알려진 이슈
- 렌더링: mesh geometry 누락 (prefab 필요)
- 게임은 로그로 동작 확인 가능

---

**작성일**: 2025-11-25
**상태**: Backend 리팩토링 진행 중
