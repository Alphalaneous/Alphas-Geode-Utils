#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <Geode/cocos/base_nodes/CCNode.h>

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_UTILS_API_EXPORTING
        #define ALPHA_UTILS_API_DLL __declspec(dllexport)
    #else
        #define ALPHA_UTILS_API_DLL __declspec(dllimport)
    #endif
#else
    #define ALPHA_UTILS_API_DLL __attribute__((visibility("default")))
#endif

namespace AlphaUtils {

    struct ModifyInfo {
        int priority;
        std::function<void(cocos2d::CCNode*)> method;
    };

    class ALPHA_UTILS_API_DLL NodeModding {
    protected:
        std::unordered_map<std::string, std::vector<ModifyInfo>> m_nodesToModify;

    public:
        static NodeModding* get();
        std::unordered_map<std::string, std::vector<ModifyInfo>> getNodesToModify();
        void addNodeToModify(std::string className, int prio, std::function<void(cocos2d::CCNode*)> func);
        void handleNode(cocos2d::CCNode* node);
    };  

    template <class Derived>
    struct NodeWrapper : public cocos2d::CCNode {
        geode::modifier::FieldIntermediate<Derived, cocos2d::CCNode> m_fields;
        static int modifyPrio() { return 0; }
    };

    template <class T>
    class ModifyLoad {
    public:
        ModifyLoad(std::string str) {
            NodeModding::get()->addNodeToModify(str, T::modifyPrio(), [](cocos2d::CCNode* self) {
                reinterpret_cast<T*>(self)->T::modify();
            });
        }
    };
}

//yoinked a lot of this from geode
#define ALPHA_MODIFY_DECLARE(base, derived) \
GEODE_CONCAT(GEODE_CONCAT(derived, __LINE__), Dummy);\
struct derived;\
class GEODE_CONCAT(GEODE_CONCAT(derived, Hook), __LINE__) : AlphaUtils::NodeWrapper<derived> {\
    private:\
    static inline AlphaUtils::ModifyLoad<derived> s_apply{#base};\
};\
struct derived : AlphaUtils::NodeWrapper<derived>

#define MODIFY1(base) ALPHA_MODIFY_DECLARE(base, GEODE_CONCAT(hook, __LINE__))
#define MODIFY2(derived, base) ALPHA_MODIFY_DECLARE(base, derived)

#define $nodeModify(...) \
    GEODE_INVOKE(GEODE_CONCAT(MODIFY, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)