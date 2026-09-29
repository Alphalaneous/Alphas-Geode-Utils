#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include "ObjectMetadata.hpp"

namespace alpha::utils {

template <class Base>
requires std::derived_from<Base, cocos2d::CCObject>
struct CCObjectExt : public Base {
    
    void setUserObject(geode::ZStringView id, cocos2d::CCObject* value) {
        ObjectMetadata::setUserObject(this, id, value);
    }

    cocos2d::CCObject* getUserObject(geode::ZStringView id) {
        return ObjectMetadata::getUserObject(this, id);
    }

    ObjectFieldContainer* getFieldContainer(char const* forClass) {
        return ObjectMetadata::getFieldContainer(this, forClass);
    }
};

}