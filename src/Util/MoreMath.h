//
// Created by ounols on 19. 3. 23.
//

#ifndef CSENGINE_APP_MOREMATH_H
#define CSENGINE_APP_MOREMATH_H

#ifdef WIN32
#define M_PI 3.14159265358979323846
#endif


template <typename T>
static int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif //CSENGINE_APP_MOREMATH_H
