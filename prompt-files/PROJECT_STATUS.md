# CSEngine 프로젝트 상태 요약

## CSEngine이란?
C++로 작성된 3D 게임 엔진. Squirrel 스크립팅, Component 기반 GameObject, PBR 렌더링 지원.
에디터는 ImGui 기반이며 REST API(8080)로 외부 제어 가능.

## 현재 상태: 🔄 Editor Backend 리팩토링 진행 중

### 최근 작업 (2025-11-25)
**Backend 아키텍처 마이그레이션** - API와 GUI 로직 통합

#### 완료된 리팩토링
1. **ObjectBackend** - Direct 메서드 추가
   - `CreateEmptyObjectDirect()`, `CreatePrimitiveObjectDirect()`
   - `CreateLightObjectDirect()`, `CreateCameraObjectDirect()`
   - `DeleteObjectDirect()`, `DuplicateObjectDirect()`

2. **ComponentBackend** - Direct 메서드 추가
   - `AddComponentDirect()`, `RemoveComponentDirect()`
   - `HasComponent()` (static)

3. **SceneBackend** - Direct 메서드 추가
   - `LoadSceneDirect()`, `SaveSceneDirect()`
   - `CreateNewSceneDirect()`, `GetCurrentSceneName()`

4. **EditorBackend** - Direct 메서드 추가
   - `PlayDirect()`, `StopDirect()`, `ResizePreviewDirect()`
   - `IsPlaying()`, `GetPreviewTextureId()`, `InvokeEditorRender()`

5. **Window 클래스 마이그레이션**
   - `HierarchyWindow` → `ObjectBackend` 사용
   - `InspectorWindow` → `ComponentBackend` 사용
   - `AssetWindow` → `SceneBackend` 사용
   - `PreviewWindow` → `EditorBackend` 사용

#### 수정된 파일
```
Editor/src/Backend/
├── ObjectBackend.h/cpp      # GameObject 생성/삭제 로직
├── ComponentBackend.h/cpp   # 컴포넌트 추가/제거 로직
├── SceneBackend.h/cpp       # 씬 로드/저장 로직
├── EditorBackend.h/cpp      # Play/Stop, Preview 로직
├── LogBackend.h/cpp         # 로그 조회
├── DebugBackend.h/cpp       # 디버그 정보
└── BackendUtils.h/cpp       # 공용 유틸리티

Editor/src/Objects/
├── HierarchyWindow.cpp      # ObjectBackend 사용으로 변경
├── InspectorWindow.cpp      # ComponentBackend 사용으로 변경
├── AssetWindow.cpp          # SceneBackend 사용으로 변경
└── PreviewWindow.cpp        # EditorBackend 사용으로 변경
```

### 이전 완료 작업: Dodge Master 게임

#### Dodge Master 게임
- **장르**: 3D 회피 게임
- **조작**: A/D 키로 좌우 이동
- **목표**: 위에서 떨어지는 장애물 피하기
- **점수**: 장애물 통과시 +10점

#### 게임 관련 파일
```
Assets/Script/
├── PlayerController.nut     # 플레이어 이동, 충돌
├── ObstacleController.nut   # 장애물 낙하, 오브젝트 풀링
└── GameManager.nut          # 스폰, 점수, 게임 상태 관리

Assets/Scene/
└── DodgeMaster.scene
```

### 알려진 이슈
- **렌더링 문제**: Player/Obstacles에 mesh geometry 누락 → prefab 사용 필요
- 현재 게임은 **로그로 동작 확인** (콘솔에서 점수/충돌 출력)

### 빌드 & 실행
```powershell
# 빌드 (자동화 스크립트)
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/build.ps1"

# 에디터 실행 (API 대기)
powershell -ExecutionPolicy Bypass -File "prompt-files/tools/run-editor.ps1" -WaitApi
```
