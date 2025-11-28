#include "InputMgr.h"

using namespace CSE;

InputMgr* InputMgr::s_instance = nullptr;

InputMgr::InputMgr() {
    s_instance = this;
    m_mousePosition = vec2{0.0f, 0.0f};
    m_mousePositionLastFrame = vec2{0.0f, 0.0f};
}

InputMgr::~InputMgr() {
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

void InputMgr::Init() {
    // Initialize input states
    m_keyStates.clear();
    m_keyStatesLastFrame.clear();
    m_mouseButtonStates.clear();
    m_mouseButtonStatesLastFrame.clear();
}

void InputMgr::Update(float elapsedTime) {
    // Input state is updated by platform layer
    // This tick can be used for input smoothing if needed
}

bool InputMgr::GetKey(KeyCode key) {
    if (s_instance == nullptr) return false;
    int keyInt = static_cast<int>(key);
    auto it = s_instance->m_keyStates.find(keyInt);
    return it != s_instance->m_keyStates.end() && it->second;
}

bool InputMgr::GetKeyDown(KeyCode key) {
    if (s_instance == nullptr) return false;
    int keyInt = static_cast<int>(key);
    bool current = false;
    bool previous = false;

    const auto& it = s_instance->m_keyStates.find(keyInt);
    if (it != s_instance->m_keyStates.end()) {
        current = it->second;
    }

    const auto& itPrev = s_instance->m_keyStatesLastFrame.find(keyInt);
    if (itPrev != s_instance->m_keyStatesLastFrame.end()) {
        previous = itPrev->second;
    }

    return current && !previous;
}

bool InputMgr::GetKeyUp(KeyCode key) {
    if (s_instance == nullptr) return false;
    int keyInt = static_cast<int>(key);
    bool current = false;
    bool previous = false;

    auto it = s_instance->m_keyStates.find(keyInt);
    if (it != s_instance->m_keyStates.end()) {
        current = it->second;
    }

    auto itPrev = s_instance->m_keyStatesLastFrame.find(keyInt);
    if (itPrev != s_instance->m_keyStatesLastFrame.end()) {
        previous = itPrev->second;
    }

    return !current && previous;
}

bool InputMgr::GetMouseButton(MouseButton button) {
    if (s_instance == nullptr) return false;
    int buttonInt = static_cast<int>(button);
    auto it = s_instance->m_mouseButtonStates.find(buttonInt);
    return it != s_instance->m_mouseButtonStates.end() && it->second;
}

bool InputMgr::GetMouseButtonDown(MouseButton button) {
    if (s_instance == nullptr) return false;
    int buttonInt = static_cast<int>(button);
    bool current = false;
    bool previous = false;

    auto it = s_instance->m_mouseButtonStates.find(buttonInt);
    if (it != s_instance->m_mouseButtonStates.end()) {
        current = it->second;
    }

    auto itPrev = s_instance->m_mouseButtonStatesLastFrame.find(buttonInt);
    if (itPrev != s_instance->m_mouseButtonStatesLastFrame.end()) {
        previous = itPrev->second;
    }

    return current && !previous;
}

bool InputMgr::GetMouseButtonUp(MouseButton button) {
    if (s_instance == nullptr) return false;
    const int buttonInt = static_cast<int>(button);
    bool current = false;
    bool previous = false;

    const auto& it = s_instance->m_mouseButtonStates.find(buttonInt);
    if (it != s_instance->m_mouseButtonStates.end()) {
        current = it->second;
    }

    const auto& itPrev = s_instance->m_mouseButtonStatesLastFrame.find(buttonInt);
    if (itPrev != s_instance->m_mouseButtonStatesLastFrame.end()) {
        previous = itPrev->second;
    }

    return !current && previous;
}

vec2 InputMgr::GetMousePosition() {
    if (s_instance == nullptr) return vec2{0.0f, 0.0f};
    return s_instance->m_mousePosition;
}

vec2 InputMgr::GetMouseDelta() {
    if (s_instance == nullptr) return vec2{0.0f, 0.0f};
    return vec2{
        s_instance->m_mousePosition.x - s_instance->m_mousePositionLastFrame.x,
        s_instance->m_mousePosition.y - s_instance->m_mousePositionLastFrame.y
    };
}

float InputMgr::GetMouseScrollDelta() {
    if (s_instance == nullptr) return 0.0f;
    return s_instance->m_mouseScrollDelta;
}

void InputMgr::UpdateKeyState(int key, bool pressed) {
    if (s_instance == nullptr) return;
    s_instance->m_keyStates[key] = pressed;
}

void InputMgr::UpdateMousePosition(float x, float y) {
    if (s_instance == nullptr) return;
    s_instance->m_mousePosition = vec2{x, y};
}

void InputMgr::UpdateMouseButton(int button, bool pressed) {
    if (s_instance == nullptr) return;
    s_instance->m_mouseButtonStates[button] = pressed;
}

void InputMgr::UpdateMouseScroll(float delta) {
    if (s_instance == nullptr) return;
    s_instance->m_mouseScrollDelta = delta;
}

void InputMgr::EndFrame() {
    if (s_instance == nullptr) return;

    // Copy current state to last frame state
    s_instance->m_keyStatesLastFrame = s_instance->m_keyStates;
    s_instance->m_mouseButtonStatesLastFrame = s_instance->m_mouseButtonStates;
    s_instance->m_mousePositionLastFrame = s_instance->m_mousePosition;
    s_instance->m_mouseScrollDelta = 0.0f;
}
