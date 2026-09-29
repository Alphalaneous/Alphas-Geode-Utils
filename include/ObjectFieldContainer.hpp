#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/function.hpp>
#include <Geode/utils/StringMap.hpp>
#include "Export.hpp"

namespace alpha::utils {

class ALPHA_UTILS_API_DLL ObjectFieldContainer {
public:
    ObjectFieldContainer();
    ~ObjectFieldContainer();

    ObjectFieldContainer(const ObjectFieldContainer&) = delete;
    ObjectFieldContainer& operator=(const ObjectFieldContainer&) = delete;
    ObjectFieldContainer(ObjectFieldContainer&&) = delete;
    ObjectFieldContainer& operator=(ObjectFieldContainer&&) = delete;

    void* getField(size_t index);
    void* setField(size_t index, size_t size, geode::Function<void(void*)> destructor);
    static ObjectFieldContainer* from(cocos2d::CCObject* object, char const* forClass);

protected:
    struct Impl;
    std::unique_ptr<Impl> m_impl; 
};

}