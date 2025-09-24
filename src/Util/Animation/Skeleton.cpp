#include "Skeleton.h"

RESOURCE_CONSTRUCTOR(Skeleton), m_jointCount(0) {
    SetUndestroyable(true);
}

void CSE::Skeleton::SetValue(const std::string& name_str, const Arguments& value) {
}

std::string CSE::Skeleton::PrintValue() const {
    return {};
}
