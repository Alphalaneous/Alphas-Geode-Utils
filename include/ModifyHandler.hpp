#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/casts.hpp>
#include "ObjectMetadata.hpp"
#include "CCObjectExt.hpp"
#include "Export.hpp"
#include <typeindex>
#include <memory>

namespace alpha::utils {

class ALPHA_UTILS_API_DLL ModifyHandler {
public:
    ModifyHandler();
    ~ModifyHandler();

    static ModifyHandler* get();

    void cleanup();

    void addObjectToModify(geode::ZStringView className, int priority, geode::Function<void(CCObjectExt<cocos2d::CCObject>*)> method);
    void addObjectToModifyBase(geode::ZStringView className, int priority, geode::Function<void(CCObjectExt<cocos2d::CCObject>*)> method);

    std::unordered_map<std::type_index, std::unordered_map<std::type_index, bool>>& getTypes();

    template <class Base>
    inline bool containsBase(const cocos2d::CCObject* obj) {
        auto key = std::type_index(typeid(*obj));
        auto base = std::type_index(typeid(Base));
        auto& types = getTypes();

        auto it = types.find(key);
        if (it != types.end()) {
            auto it2 = it->second.find(base);
            if (it2 != it->second.end()) {
                return it2->second;
            }
        }

        bool ret = geode::cast::typeinfo_cast<Base*>(obj);
        types[key][base] = ret;
        return ret;
    }

protected:
    struct Impl;
    std::unique_ptr<Impl> m_impl; 

    friend class ObjectMetadata;
    friend class AGUEngine;
    friend class AGUCCObject;
};

}