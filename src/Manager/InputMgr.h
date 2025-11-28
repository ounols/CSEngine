#pragma once

#include "Base/CoreBase.h"
#include "../Util/Vector.h"
#include <unordered_map>

namespace CSE {

    /**
     * @brief Input key codes compatible with GLFW
     */
    enum class KeyCode {
        // Alphabet keys
        A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Number keys
        Num0 = 48, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

        // Function keys
        F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // Special keys
        Space = 32,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,

        // Modifier keys
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346
    };

    /**
     * @brief Mouse button codes
     */
    enum class MouseButton {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    /**
     * @brief Input Manager for handling keyboard and mouse input
     *
     * This class provides a unified interface for querying input state.
     * Platform-specific implementations should call the Update* methods
     * to feed input data into this manager.
     */
    class InputMgr final : public CoreBase {
    public:
        InputMgr();
        ~InputMgr() override;

        void Init() override;
        void Update(float elapsedTime) override;

        // Keyboard input queries
        static bool GetKey(KeyCode key);
        static bool GetKeyDown(KeyCode key);
        static bool GetKeyUp(KeyCode key);

        // Mouse input queries
        static bool GetMouseButton(MouseButton button);
        static bool GetMouseButtonDown(MouseButton button);
        static bool GetMouseButtonUp(MouseButton button);
        static vec2 GetMousePosition();
        static vec2 GetMouseDelta();
        static float GetMouseScrollDelta();

        // Update methods (called by platform layer)
        static void UpdateKeyState(int key, bool pressed);
        static void UpdateMousePosition(float x, float y);
        static void UpdateMouseButton(int button, bool pressed);
        static void UpdateMouseScroll(float delta);

        // Frame update (call at end of frame)
        static void EndFrame();

    private:
        static InputMgr* s_instance;

        // Keyboard state
        std::unordered_map<int, bool> m_keyStates;
        std::unordered_map<int, bool> m_keyStatesLastFrame;

        // Mouse state
        std::unordered_map<int, bool> m_mouseButtonStates;
        std::unordered_map<int, bool> m_mouseButtonStatesLastFrame;
        vec2 m_mousePosition;
        vec2 m_mousePositionLastFrame;
        float m_mouseScrollDelta = 0.0f;
    };

}
