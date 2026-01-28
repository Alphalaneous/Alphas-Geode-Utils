#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include "ModifyHandler.hpp"
#include "Fields.hpp"
#include "ObjectData.hpp"

namespace alpha::utils {

    class ObjectFieldContainer;

    template <class Base>
    requires std::derived_from<Base, cocos2d::CCObject>
    struct ModifyCCObject : public Base {
        
        void setUserObject(geode::ZStringView id, cocos2d::CCObject* value) {
            auto meta = ObjectMetadata::set(reinterpret_cast<ModifyCCObject<cocos2d::CCObject>*>(this));
            if (value) {
                meta->m_userObjects[id] = value;
            }
            else {
                meta->m_userObjects.erase(id);
            }
        }

        cocos2d::CCObject* getUserObject(geode::ZStringView id) {
            auto meta = ObjectMetadata::set(reinterpret_cast<ModifyCCObject<cocos2d::CCObject>*>(this));
            if (meta->m_userObjects.count(id)) {
                return meta->m_userObjects.at(id);
            }
            return nullptr;
        }

        ObjectFieldContainer* getFieldContainer(char const* forClass) {
            return ObjectMetadata::set(this)->getFieldContainer(forClass);
        }

        cocos2d::CCObject* getUserObject() {
            ObjectData* data = alpha::utils::ModifyHandler::get()->getObjectData(cocos2d::CCObject::m_nLuaID);
            return data ? data->m_object : nullptr;
        }

        void setUserObject(cocos2d::CCObject* object) {
            ObjectData* data = alpha::utils::ModifyHandler::get()->getObjectData(cocos2d::CCObject::m_nLuaID);
            if (data && data->m_object != object) {
                data->m_object = object;
            }
        }

        friend class ObjectFieldContainer;
    };
}