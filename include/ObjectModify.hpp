#pragma once

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
        geode::Function<void(ModifyCCObject<cocos2d::CCObject>*)> method;

        ObjectModifyInfo(int p, geode::Function<void(ModifyCCObject<cocos2d::CCObject>*)>&& m)
            : priority(p), method(std::move(m)) {}

        ObjectModifyInfo(ObjectModifyInfo&&) noexcept = default;
        ObjectModifyInfo& operator=(ObjectModifyInfo&&) noexcept = default;

        ObjectModifyInfo(const ObjectModifyInfo&) = delete;
        ObjectModifyInfo& operator=(const ObjectModifyInfo&) = delete;
    };

    class ALPHA_UTILS_API_DLL ObjectModify {
    protected:
        geode::utils::StringMap<std::vector<ObjectModifyInfo>> m_objectsToModify;
        geode::utils::StringMap<std::vector<ObjectModifyInfo>> m_objectBasesToModify;
    private:
        ObjectModify() = default;
        ObjectModify(const ObjectModify&) = delete;
        ObjectModify& operator=(const ObjectModify&) = delete;
        ObjectModify(ObjectModify&&) = delete;
        ObjectModify& operator=(ObjectModify&&) = delete;

        static ObjectModify* get();
        void addObjectToModify(geode::ZStringView className, int prio, geode::Function<void(ModifyCCObject<cocos2d::CCObject>*)> func);
        void addObjectToModifyBase(geode::ZStringView className, int prio, geode::Function<void(ModifyCCObject<cocos2d::CCObject>*)> func);
        geode::utils::StringMap<std::vector<ObjectModifyInfo>>& getObjectsToModify();
        geode::utils::StringMap<std::vector<ObjectModifyInfo>>& getObjectBasesToModify();

        template <class, class>
        friend class ClassModifyLoad;

        template <class, class>
        friend class BaseModifyLoad;

        friend class ModifyHandler;
    };

    template <class Derived, class Base>
    class ClassModifyLoad {
    public:
        ClassModifyLoad(geode::ZStringView str, bool useStr) {
            if (useStr) {
                ObjectModify::get()->addObjectToModify(str, Derived::modifyPrio(), [](ModifyCCObject<cocos2d::CCObject>* self) {
                    reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
                });
            }
            else {
                ObjectModify::get()->addObjectToModify(alpha::utils::cocos::getObjectName<Base>(), Derived::modifyPrio(), [](ModifyCCObject<cocos2d::CCObject>* self) {
                    reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
                });
            }
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

    template <class Derived, class Base>
    class BaseModifyLoad {
    public:
        BaseModifyLoad(geode::ZStringView str) {
            ObjectModify::get()->addObjectToModifyBase(str, Derived::modifyPrio(), [](ModifyCCObject<cocos2d::CCObject>* self) {
                if (ModifyHandler::get()->containsBase<Base>(reinterpret_cast<cocos2d::CCObject*>(self))) {
                    reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
                }
            });
        }
    };

    template <class Derived, class Base, geode::utils::string::ConstexprString BaseStr>
    struct BaseObjectWrapper : public ModifyCCObject<Base> {
        private:
        static inline BaseModifyLoad<Derived, Base> s_apply{BaseStr.data()};
        static inline auto s_applyRef = &BaseObjectWrapper::s_apply;

        public:
        using Self = Derived;

        ObjectFieldIntermediate<Derived, Base, BaseStr, false> m_fields;
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


#define ALPHA_MODIFY_BASE(baseStr, derived, baseType) \
    GEODE_CONCAT(derived, Dummy);                        \
    struct derived : alpha::utils::BaseObjectWrapper<derived, baseType, #baseStr>

#define ALPHA_MODIFY_BASE_AUTO(baseStr, baseType) \
    ALPHA_MODIFY_BASE(baseStr, GEODE_CONCAT(hook, __LINE__), baseType)

#define MODIFYBASE1(base)          ALPHA_MODIFY_BASE_AUTO(base, base)
#define MODIFYBASE2(derived, base) ALPHA_MODIFY_BASE(base, derived, base)

#define $baseModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFYBASE, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)