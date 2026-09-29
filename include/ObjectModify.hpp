#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include "ObjectWrapper.hpp"
#include "BaseObjectWrapper.hpp"

#define ALPHA_MODIFY(baseStr, derived, baseType, useStr) \
    GEODE_CONCAT(derived, Dummy);                        \
    struct derived : alpha::utils::ObjectWrapper<derived, baseType, #baseStr, useStr>

#define ALPHA_MODIFY_AUTO(baseStr, baseType, useStr) \
    ALPHA_MODIFY(baseStr, GEODE_CONCAT(hook, __LINE__), baseType, useStr)

#define MODIFY1(base)          ALPHA_MODIFY_AUTO(base, cocos2d::CCObject, true)
#define MODIFY2(derived, base) ALPHA_MODIFY(base, derived, cocos2d::CCObject, true)

#define MODIFYNODE1(base)          ALPHA_MODIFY_AUTO(base, cocos2d::CCNode, true)
#define MODIFYNODE2(derived, base) ALPHA_MODIFY(base, derived, cocos2d::CCNode, true)

#define MODIFYCLASS1(base)          ALPHA_MODIFY_AUTO(base, base, false)
#define MODIFYCLASS2(derived, base) ALPHA_MODIFY(base, derived, base, false)

#define $objectModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFY, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define $nodeModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFYNODE, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define $classModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFYCLASS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)


#define ALPHA_MODIFY_BASE(baseStr, derived, baseType) \
    GEODE_CONCAT(derived, Dummy);                        \
    struct derived : alpha::utils::BaseObjectWrapper<derived, baseType, #baseStr>

#define ALPHA_MODIFY_BASE_AUTO(baseStr, baseType) \
    ALPHA_MODIFY_BASE(baseStr, GEODE_CONCAT(hook, __LINE__), baseType)

#define MODIFYBASE1(base)          ALPHA_MODIFY_BASE_AUTO(base, base)
#define MODIFYBASE2(derived, base) ALPHA_MODIFY_BASE(base, derived, base)

#define $baseModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFYBASE, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)