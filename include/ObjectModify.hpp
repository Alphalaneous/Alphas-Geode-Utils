#pragma once

#include <functional>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/utils/string.hpp>
#include "ModifyCCObject.hpp"
#include "Fields.hpp"
#include "Utils.hpp"

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_UTILS_API_EXPORTING
        #define ALPHA_UTILS_API_DLL __declspec(dllexport)
    #else
        #define ALPHA_UTILS_API_DLL __declspec(dllimport)
    #endif
#else
    #define ALPHA_UTILS_API_DLL __attribute__((visibility("default")))
#endif

template <class, class>
class ObjectFieldIntermediate;

namespace alpha::utils {
    class ModifyHandler;

    struct ObjectModifyInfo {
        int priority;
        std::function<void(ModifyCCObject<cocos2d::CCObject>*)> method;
    };

    class ALPHA_UTILS_API_DLL ObjectModify {
    protected:
        geode::utils::StringMap<std::vector<ObjectModifyInfo>> m_objectsToModify;
    private:
        static ObjectModify* get();
        void addObjectToModify(geode::ZStringView className, int prio, std::function<void(ModifyCCObject<cocos2d::CCObject>*)> func);
        geode::utils::StringMap<std::vector<ObjectModifyInfo>>& getObjectsToModify();
        void handleObject(ModifyCCObject<cocos2d::CCObject>* object);

        template <class, class>
        friend class ClassModifyLoad;
        friend class ModifyHandler;
    };

    template <class Derived, class Base>
    class ClassModifyLoad {
    public:
        ClassModifyLoad(geode::ZStringView str, bool useStr) {
            std::string name;
            if (useStr) name = str;
            else name = alpha::utils::cocos::getObjectName<Base>();

            ObjectModify::get()->addObjectToModify(name, Derived::modifyPrio(), [](ModifyCCObject<cocos2d::CCObject>* self) {
                reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
            });
        }
    };

    template <class Derived, class Base, geode::utils::string::ConstexprString BaseStr, bool UsesStr>
    struct ObjectWrapper : public ModifyCCObject<Base> {
        private:
        static inline ClassModifyLoad<Derived, Base> s_apply{BaseStr.data(), UsesStr};
        static inline auto s_applyRef = &ObjectWrapper::s_apply;

        public:
        using Self = Derived;

        ObjectFieldIntermediate<Derived, Base, BaseStr, UsesStr> m_fields;
        static int modifyPrio() { return 0; }
        void modify() {}
    };
}

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