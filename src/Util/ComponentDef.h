//
// Created by ounols on 19. 3. 14.
//

#ifndef CSENGINE_APP_COMPONENTDEF_H
#define CSENGINE_APP_COMPONENTDEF_H

#define Transform static_cast<TransformInterface*>(gameObject->GetTransform())
#define COMPONENT_DECLARE(CLASSNAME)    \
CLASSNAME();                            \
auto GetComponent() override

#define COMPONENT_DEFINE_CONSTRUCTOR(CLASSNAME, ...)                 \
static const char* GetClassStaticType() { return #CLASSNAME; }  \
explicit CLASSNAME(CSE::SGameObject* l_gameObject, __VA_ARGS__)

#define COMPONENT_CONSTRUCTOR(CLASSNAME, ...) \
namespace __REFELCTION_DUMP__ { namespace CLASSNAME {                    \
unsigned char* __CSE_REFLECTION_DUMP__ = \
CSE::ReflectionMgr::DefineWrapper::SetDefine(#CLASSNAME, []() { return new CSE::CLASSNAME(nullptr); });}} \
CSE::CLASSNAME::CLASSNAME(CSE::SGameObject* l_gameObject, __VA_ARGS__) : CSE::SComponent(#CLASSNAME, l_gameObject)

#define COMPONENT_DERIVED_CLASS_TYPE(CLASSNAME) \
SetClassType(#CLASSNAME)

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
