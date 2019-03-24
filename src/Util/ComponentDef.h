//
// Created by ounols on 19. 3. 14.
//

#ifndef CSENGINE_APP_COMPONENTDEF_H
#define CSENGINE_APP_COMPONENTDEF_H

#define Transform static_cast<TransformInterface*>(gameObject->GetTransform())
#define COMPONENT_CONSTRUCTOR(CLASSNAME) CLASSNAME::CLASSNAME() : SComponent(#CLASSNAME)
#define FIND_REFERENCE(obj, src) \
{                           \
    auto targetObject = lists_obj.find(src->obj);    \
    obj = (targetObject != lists_obj.end()) ? targetObject->second : src->obj;    \
}

#define INIT_COMPONENT_CLONE(className, cloneName) \
className* cloneName = nullptr;    \
if (object == nullptr) {            \
cloneName = new className();       \
} else {    \
cloneName = object->CreateComponent<className>();    \
}

#endif //CSENGINE_APP_COMPONENTDEF_H
