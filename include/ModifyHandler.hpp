#pragma once

#include <queue>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/cocos.hpp>
#include <vector>
#include "ObjectData.hpp"
#include <typeindex>

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_UTILS_API_EXPORTING
        #define ALPHA_UTILS_API_DLL __declspec(dllexport)
    #else
        #define ALPHA_UTILS_API_DLL __declspec(dllimport)
    #endif
#else
    #define ALPHA_UTILS_API_DLL __attribute__((visibility("default")))
#endif

namespace alpha::utils {

    class ALPHA_UTILS_API_DLL ModifyHandler {

    public:
        static ModifyHandler* get();

        uint32_t allocateObjectData(ObjectData* data);
        void releaseObjectData(uint32_t id);
        ObjectData* getObjectData(uint32_t id);
        void createObjectData(cocos2d::CCObject* object);
        void handleObject(cocos2d::CCObject* object);

        template <class Base>
        inline bool containsBase(const cocos2d::CCObject* obj) {
            auto key = std::type_index(typeid(*obj));
            auto base = std::type_index(typeid(Base));

            auto it = m_types.find(key);
            if (it != m_types.end()) {
                auto it2 = it->second.find(base);
                if (it2 != it->second.end()) {
                    return it2->second;
                }
            }

            bool ret = geode::cast::typeinfo_cast<Base*>(obj);
            m_types[key][base] = ret;
            return ret;
        }

    protected:
        std::unordered_map<std::type_index, std::unordered_map<std::type_index, bool>> m_types;
        std::vector<geode::Ref<ObjectData>> m_arena;
        std::queue<uint32_t> m_slots;
    };
}