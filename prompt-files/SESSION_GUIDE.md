# CSEngine 새 세션 시작 가이드

## 프로젝트 개요
**CSEngine**: C++/OpenGL 기반 3D 게임 엔진
- **스크립팅**: Squirrel 언어 (.nut 파일)
- **에디터**: ImGui UI + REST API (localhost:8080)
- **현재 상태**: Backend 아키텍처 리팩토링 진행 중

## 이 폴더 파일 읽기 순서
1. `PROJECT_STATUS.md` - 현재 무엇이 완료되었는지
2. `ENGINE_REFERENCE.md` - API/스크립트 작성법 + Backend 아키텍처
3. `TROUBLESHOOTING.md` - 문제 발생 시 해결책
4. `tools/README.md` - 자동화 스크립트 사용법

## 프로젝트 구조
```
CSEngine/
├── src/                       # 엔진 C++ 소스
│   ├── Manager/               # EngineCore, ScriptMgr, InputMgr 등
│   ├── Component/             # Transform, Render, Camera 등
│   └── Util/                  # Vector, SafeLog
├── Editor/                    # 에디터 소스
│   ├── src/Backend/           # ⭐ 비즈니스 로직 (API+GUI 공유)
│   ├── src/Objects/           # ImGui Window 클래스
│   ├── src/Manager/           # EditorAPIServer, EEngineCore
│   └── platforms/Windows/     # 빌드 설정
├── Assets/
│   ├── Script/                # Squirrel 게임 스크립트
│   └── Scene/                 # 씬 파일 (.scene)
└── prompt-files/              # 이 폴더 (AI 컨텍스트)
```

## 작업별 빠른 가이드

### A. 게임 스크립트 수정/추가
```
1. Assets/Script/ 폴더의 .nut 파일 편집
2. ENGINE_REFERENCE.md의 Squirrel API 참조
3. 에디터에서 Play로 테스트
```

### B. 엔진 C++ 수정
```
1. src/ 폴더 수정
2. powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"
3. TROUBLESHOOTING.md로 빌드 오류 해결
```

### C. Editor Backend 수정 (신규)
```
1. Editor/src/Backend/ 폴더의 *Backend.h/cpp 수정
2. Direct 메서드는 GUI에서 직접 호출됨
3. API 메서드는 pending 큐를 통해 비동기 처리
4. ENGINE_REFERENCE.md의 Backend 아키텍처 참조
```

### D. REST API로 씬 자동 구성
```powershell
# 에디터 실행 확인
Invoke-RestMethod "http://localhost:8080/api/health"

# 새 씬 생성
$body = '{"path": "new"}'
Invoke-RestMethod -Uri "http://localhost:8080/api/scene/load" -Method Post -ContentType "application/json" -Body $body
```

### E. 새 게임 개발
1. 기존 DodgeMaster 스크립트 참고 (Assets/Script/)
2. 씬 구성: REST API 또는 에디터 UI
3. 테스트: Play 모드 + 로그 확인

## 핵심 명령어 (tools/ 스크립트 사용)

```powershell
# 빌드
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"

# 에디터 실행 (API 대기)
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi

# API 호출
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" health
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" scene load Assets/Scene/DodgeMaster.scene
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" object list
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" play
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" log 30
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/api.ps1" stop
```

## 상세 정보가 필요하면
| 파일 | 내용 |
|------|------|
| `CLAUDE.md` | 전체 프로젝트 가이드 (700줄, 매우 상세) |
| `EDITOR_API.md` | REST API 전체 문서 |
| `GAME_DEV_PLAN.md` | Dodge Master 게임 설계 |

## 브랜치 정보
- **메인**: master
- **작업**: claude/game-editor-implementation-*
- **최근**: Dodge Master 데모 완료 (2025-11-24)
