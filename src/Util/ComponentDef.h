//
// Created by ounols on 19. 3. 14.
//

#ifndef CSENGINE_APP_COMPONENTDEF_H
#define CSENGINE_APP_COMPONENTDEF_H

#define Transform static_cast<TransformInterface*>(gameObject->GetTransform())
#define COMPONENT_DECLARE(CLASSNAME)    \
CLASSNAME();                            \
auto GetComponent() override

#define COMPONENT_DEFINE_CONSTRUCTOR(CLASSNAME) \
explicit CLASSNAME(SGameObject* l_gameObject)

#define COMPONENT_CONSTRUCTOR(CLASSNAME)    \
CLASSNAME::CLASSNAME(SGameObject* l_gameObject) : SComponent(#CLASSNAME, l_gameObject)

#define FIND_OBJ_REFERENCE(obj, src) \
{                           \
    auto targetObject = lists_obj.find(src->obj);    \
    obj = (targetObject != lists_obj.end()) ? targetObject->second : src->obj;    \
}
#define FIND_COMP_REFERENCE(comp, src, componentName) \
{                           \
    auto targetObject = lists_comp.find(src->comp);    \
    comp = (targetObject != lists_comp.end()) ? static_cast<componentName*>(targetObject->second) : src->comp;    \
}

#define INIT_COMPONENT_CLONE(className, cloneName) \
className* cloneName = nullptr;    \
if (object == nullptr) {            \
cloneName = new className(object);       \
} else {    \
cloneName = object->CreateComponent<className>();    \
}

#endif //CSENGINE_APP_COMPONENTDEF_H
