//
// Created by ounols on 19. 1. 22.
//

#include "SkeletonContainer.h"

SkeletonContainer::SkeletonContainer() {
    SetUndestroyable(true);
}

SkeletonContainer::~SkeletonContainer() {
    SkeletonContainer::Exterminate();
}

void SkeletonContainer::Exterminate() {

}
