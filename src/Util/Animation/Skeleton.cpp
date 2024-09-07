#include "Skeleton.h"

RESOURCE_CONSTRUCTOR(Skeleton), m_jointCount(0) {
    SetUndestroyable(true);
}

void CSE::Skeleton::SetValue(std::string name_str, CSE::VariableBinder::Arguments value) {

}

std::string CSE::Skeleton::PrintValue() const {
    return {};
}
