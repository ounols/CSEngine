# CSEngine 도구 스크립트

PowerShell 스크립트로 에디터 빌드, 실행, API 호출을 자동화합니다.
하지만 **무언가를 찾거나 읽기, 쓰기를 실행할 땐 PowerShell을 사용하지 않습니다.** PowerShell은 생각보다 오류가 많기 때문입니다.

## 사용법

### 1. 빌드 (build.ps1)

```powershell
# 기본 빌드 (Debug)
.\build.ps1

# CMake 재구성 후 빌드
.\build.ps1 -Configure

# 클린 빌드
.\build.ps1 -Clean -Configure

# Release 빌드
.\build.ps1 -Config Release
```

### 2. 에디터 실행 (run-editor.ps1)

```powershell
# 기본 실행
.\run-editor.ps1

# API 준비될 때까지 대기 후 실행
.\run-editor.ps1 -WaitApi

# Release 버전 실행
.\run-editor.ps1 -Config Release
```

### 3. API 호출 (api.ps1)

```powershell
# 상태 확인
.\api.ps1 health

# 씬 로드
.\api.ps1 scene load Assets/Scene/DodgeMaster.scene
.\api.ps1 scene load new  # 새 씬

# 씬 저장
.\api.ps1 scene save

# 오브젝트 관리
.\api.ps1 object list
.\api.ps1 object create Player
.\api.ps1 object transform Player 0 -2 0

# 컴포넌트 추가
.\api.ps1 component add Player CustomComponent PlayerController
.\api.ps1 component add MainCamera CameraComponent

# 게임 실행
.\api.ps1 play
.\api.ps1 stop

# 로그 확인
.\api.ps1 log 50
```

## 전체 워크플로우 예시

```powershell
cd C:\Programming\CSEngine\prompt-files\tools

# 1. 빌드
.\build.ps1

# 2. 에디터 실행 (API 대기)
.\run-editor.ps1 -WaitApi

# 3. 씬 로드 및 플레이
.\api.ps1 scene load Assets/Scene/DodgeMaster.scene
Start-Sleep -Seconds 2
.\api.ps1 play

# 4. 로그 확인
Start-Sleep -Seconds 5
.\api.ps1 log 30

# 5. 정지
.\api.ps1 stop
```

## Claude Code에서 사용

```powershell
# 빌드
powershell -ExecutionPolicy Bypass -File "C:\Programming\CSEngine\prompt-files\tools\build.ps1"

# 에디터 실행
powershell -ExecutionPolicy Bypass -File "C:\Programming\CSEngine\prompt-files\tools\run-editor.ps1" -WaitApi

# API 호출
powershell -ExecutionPolicy Bypass -File "C:\Programming\CSEngine\prompt-files\tools\api.ps1" scene load Assets/Scene/DodgeMaster.scene
```

## 문제 해결

### CMake를 찾을 수 없음
- Visual Studio 2022 설치 확인
- C++ CMake tools 설치 확인

### 에디터 실행 실패
- build.ps1 먼저 실행
- 올바른 Config (Debug/Release) 지정

### API 연결 실패
- 에디터가 실행 중인지 확인
- 포트 8080이 사용 가능한지 확인
