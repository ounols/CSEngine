# CSEngine 프로젝트 가이드

> **새 세션 시작 시**: `prompt-files/` 폴더의 문서들을 먼저 읽어주세요.

## 빠른 시작

### 1. 컨텍스트 파악
```
prompt-files/
├── SESSION_GUIDE.md     # 세션 시작 가이드 (먼저 읽기)
├── PROJECT_STATUS.md    # 현재 상태, 완료된 작업
├── ENGINE_REFERENCE.md  # API 레퍼런스
├── TROUBLESHOOTING.md   # 문제 해결
└── tools/               # 자동화 스크립트
    ├── build.ps1        # 빌드
    ├── run-editor.ps1   # 에디터 실행
    └── api.ps1          # REST API 호출
```

### 2. 도구 사용법
```powershell
# 빌드
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"

# 에디터 실행 (API 대기)
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi

# API 호출
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" health
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" scene load Assets/Scene/DodgeMaster.scene
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" play
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" log 30
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" stop
```

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
```powershell
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" log 50
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
```powershell
# 전체 워크플로우
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" scene load Assets/Scene/DodgeMaster.scene
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" play
```

### 알려진 이슈
- 렌더링: mesh geometry 누락 (prefab 필요)
- 게임은 로그로 동작 확인 가능

---

**작성일**: 2025-11-25
**상태**: Backend 리팩토링 진행 중
