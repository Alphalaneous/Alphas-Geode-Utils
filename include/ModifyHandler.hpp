#pragma once

#include <queue>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/cocos.hpp>
#include "ObjectData.hpp"

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

    protected:
        std::vector<geode::Ref<ObjectData>> m_arena;
        std::queue<uint32_t> m_slots;
    };
}