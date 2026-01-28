#pragma once

#include <functional>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
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

    template <class Derived, class Base>
    struct ObjectWrapper : public ModifyCCObject<Base> {
        using Self = Derived;

        ObjectFieldIntermediate<Derived, Base> m_fields;
        static int modifyPrio() { return 0; }
        void modify() {}
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
        friend class ObjectModifyLoad;
        template <class, class>
        friend class ClassModifyLoad;
        friend class ModifyHandler;
    };

    template <class Derived, class Base>
    class ObjectModifyLoad {
    public:
        ObjectModifyLoad(geode::ZStringView str) {
            ObjectModify::get()->addObjectToModify(str, Derived::modifyPrio(), [](ModifyCCObject<cocos2d::CCObject>* self) {
                reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
            });
        }
    };

    template <class Derived, class Base>
    class ClassModifyLoad {
    public:
        ClassModifyLoad() {
            auto str = alpha::utils::cocos::getObjectName<Base>();
            ObjectModify::get()->addObjectToModify(std::string(str), Derived::modifyPrio(), [](ModifyCCObject<cocos2d::CCObject>* self) {
                reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
            });
        }
    };
}

//yoinked a lot of this from geode
#define ALPHA_MODIFY_DECLARE(base, derived) \
GEODE_CONCAT(GEODE_CONCAT(derived, __LINE__), Dummy);\
struct derived;\
class GEODE_CONCAT(GEODE_CONCAT(derived, Hook), __LINE__) : alpha::utils::ObjectWrapper<derived, cocos2d::CCObject> {\
    private:\
    static inline alpha::utils::ObjectModifyLoad<derived, cocos2d::CCObject> s_apply{#base};\
};\
struct derived : alpha::utils::ObjectWrapper<derived, cocos2d::CCObject>

#define MODIFY1(base) ALPHA_MODIFY_DECLARE(base, GEODE_CONCAT(hook, __LINE__))
#define MODIFY2(derived, base) ALPHA_MODIFY_DECLARE(base, derived)

#define ALPHA_NODE_MODIFY_DECLARE(base, derived) \
GEODE_CONCAT(GEODE_CONCAT(derived, __LINE__), Dummy);\
struct derived;\
class GEODE_CONCAT(GEODE_CONCAT(derived, Hook), __LINE__) : alpha::utils::ObjectWrapper<derived, cocos2d::CCNode> {\
    private:\
    static inline alpha::utils::ObjectModifyLoad<derived, cocos2d::CCNode> s_apply{#base};\
};\
struct derived : alpha::utils::ObjectWrapper<derived, cocos2d::CCNode>

#define MODIFYNODE1(base) ALPHA_NODE_MODIFY_DECLARE(base, GEODE_CONCAT(hook, __LINE__))
#define MODIFYNODE2(derived, base) ALPHA_NODE_MODIFY_DECLARE(base, derived)

#define ALPHA_CLASS_MODIFY_DECLARE(base, derived) \
GEODE_CONCAT(GEODE_CONCAT(derived, __LINE__), Dummy);\
struct derived;\
class GEODE_CONCAT(GEODE_CONCAT(derived, Hook), __LINE__) : alpha::utils::ObjectWrapper<derived, base> {\
    private:\
    static inline alpha::utils::ClassModifyLoad<derived, base> s_apply{};\
};\
struct derived : alpha::utils::ObjectWrapper<derived, base>

#define MODIFYCLASS1(base) ALPHA_CLASS_MODIFY_DECLARE(base, GEODE_CONCAT(hook, __LINE__))
#define MODIFYCLASS2(derived, base) ALPHA_CLASS_MODIFY_DECLARE(base, derived)

#define $nodeModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFYNODE, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define $objectModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFY, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define $classModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFYCLASS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)