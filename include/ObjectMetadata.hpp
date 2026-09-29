#pragma once

#include <Geode/utils/ZStringView.hpp>
#include "ObjectFieldContainer.hpp"
#include "Export.hpp"

namespace alpha::utils {

class ALPHA_UTILS_API_DLL ObjectMetadata final {
public:
    ObjectMetadata();
    ~ObjectMetadata();

    static ObjectMetadata* set(cocos2d::CCObject* target);
    static cocos2d::CCObject* getUserObject(cocos2d::CCObject* object, geode::ZStringView id);
    static void setUserObject(cocos2d::CCObject* object, geode::ZStringView id, cocos2d::CCObject* value);
    static ObjectFieldContainer* getFieldContainer(cocos2d::CCObject* object, char const* forClass);

protected:
    struct Impl;
    std::unique_ptr<Impl> m_impl; 
};

}