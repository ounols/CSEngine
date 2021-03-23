//
// Created by ounol on 2021-03-22.
//
#pragma once


namespace CSE {

    class CoreBase {

    public:
        CoreBase() = default;

        virtual void Init() = 0;
        virtual void Update(float elapsedTime) {};

        virtual ~CoreBase() = default;
    };
}