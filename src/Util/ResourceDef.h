#pragma once


#define RESOURCE_DEFINE_CONSTRUCTOR(CLASSNAME)                 \
static const char* GetClassStaticType() { return #CLASSNAME; }  \
explicit CLASSNAME()

#define RESOURCE_CONSTRUCTOR(CLASSNAME) \
namespace __REFELCTION_DUMP__ { namespace CLASSNAME {                    \
unsigned char* __CSE_REFLECTION_DUMP__ = \
CSE::ReflectionMgr::DefineWrapper::SetDefine(#CLASSNAME, []() { return new CSE::CLASSNAME(); });}} \
CSE::CLASSNAME::CLASSNAME() : CSE::SResource(#CLASSNAME)

#define RESOURCE_SUB_CONSTRUCTOR(CLASSNAME) \
CSE::CLASSNAME::CLASSNAME() : CSE::SResource(#CLASSNAME)