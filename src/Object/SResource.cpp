//
// Created by ounols on 19. 6. 10.
//

#include "SResource.h"
#include "../Manager/ResMgr.h"

SResource::SResource() {
    ResMgr::getInstance()->Register(this);
    m_name = "Resource " + std::to_string(ResMgr::getInstance()->GetSize());
}

SResource::~SResource() {

}

void SResource::SetName(std::string name) {
    m_name = ResMgr::getInstance()->RemoveDuplicatingName(name);
}
