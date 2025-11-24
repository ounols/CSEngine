# CSEngine 프로젝트 상태 요약

## CSEngine이란?
C++로 작성된 3D 게임 엔진. Squirrel 스크립팅, Component 기반 GameObject, PBR 렌더링 지원.
에디터는 ImGui 기반이며 REST API(8080)로 외부 제어 가능.

## 현재 상태: ✅ Dodge Master 게임 완료

### Dodge Master 게임
- **장르**: 3D 회피 게임
- **조작**: A/D 키로 좌우 이동
- **목표**: 위에서 떨어지는 장애물 피하기
- **점수**: 장애물 통과시 +10점

### 완료된 작업
1. **엔진 수정**: InputMgr 바인딩 (키보드 입력), vec3 Distance 함수
2. **에디터 빌드**: MSVC, Visual Studio 2022
3. **게임 스크립트 3개**: PlayerController, ObstacleController, GameManager
4. **REST API 확장**: 오브젝트/컴포넌트 생성, Transform 설정, Play/Stop
5. **씬 자동 구성**: DodgeMaster.scene (14개 오브젝트)
6. **테스트 완료**: 30초 플레이, 140점, 정상 작동

### 생성된 파일
```
Assets/Script/
├── PlayerController.nut  # 플레이어 이동, 충돌
├── ObstacleController.nut  # 장애물 낙하, 오브젝트 풀링
└── GameManager.nut  # 스폰, 점수, 게임 상태 관리

Assets/Scene/
└── DodgeMaster.scene
```

### 수정된 엔진 파일
- `src/Manager/ScriptMgr.cpp` - InputMgr, KeyCode, MouseButton 스크립트 바인딩
- `src/Util/Vector.h` - Distance/DistanceSquared 멤버 함수
- `Editor/src/Manager/EditorAPIServer.cpp/h` - REST API 엔드포인트 추가

### 알려진 이슈
- **렌더링 문제**: Player/Obstacles에 mesh geometry 누락 → prefab 사용 필요
- 현재 게임은 **로그로 동작 확인** (콘솔에서 점수/충돌 출력)

### 빌드 & 실행
```powershell
# 빌드
cmake -G "Visual Studio 17 2022" -A x64 -S Editor/platforms/Windows -B Editor/platforms/Windows/build
cmake --build Editor/platforms/Windows/build --config Debug

# 에디터 실행 (반드시 이 경로에서!)
Start-Process "Editor\platforms\Windows\x64\Debug\CSEditor.exe" -WorkingDirectory "Editor\platforms\Windows\x64\Debug"
```
