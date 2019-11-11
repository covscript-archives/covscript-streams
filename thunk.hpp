//
// Created by kiva on 2019/11/11.
//
#pragma once

#include <functional>

namespace imkiva {
    template <typename T>
    class Thunk {
    public:
        virtual T eval() = 0;
    };
}
