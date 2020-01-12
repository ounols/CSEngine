//
// Created by ounols on 19. 10. 12.
//

#pragma once

#include "imgui.h"

namespace CSEditor {

    class WindowBase {
    public:
        WindowBase() {

        }

        virtual ~WindowBase() {

        }

        virtual void SetUI() = 0;


    protected:
        bool isClosed = true;
    };


}