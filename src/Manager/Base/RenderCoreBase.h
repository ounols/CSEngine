//
// Created by ounol on 2021-03-23.
//

#pragma once

namespace CSE {

    class RenderCoreBase {
    public:
        RenderCoreBase() = default;
        virtual ~RenderCoreBase() = default;

        virtual void Render() const = 0;
    };

}
