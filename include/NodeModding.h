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

    static uint64_t fnv1aHash(char const* str) {
        uint64_t hash = 0xcbf29ce484222325;
        while (*str) {
            hash ^= *str++;
            hash *= 0x100000001b3;
        }
        return hash;
    }
    
    template <typename T>
    class NoHashHasher;
    
    template <>
    class NoHashHasher<uint64_t> {
    public:
        size_t operator()(uint64_t key) const {
            return key;
        }
    };

    class ObjectFieldContainer {
    private:
        std::vector<void*> m_containedFields;
        std::vector<std::function<void(void*)>> m_destructorFunctions;

    public:
        ~ObjectFieldContainer() {
            for (auto i = 0u; i < m_containedFields.size(); i++) {
                if (m_destructorFunctions[i] && m_containedFields[i]) {
                    m_destructorFunctions[i](m_containedFields[i]);
                    operator delete(m_containedFields[i]);
                }
            }
        }

        void* getField(size_t index) {
            while (m_containedFields.size() <= index) {
                m_containedFields.push_back(nullptr);
                m_destructorFunctions.push_back(nullptr);
            }
            return m_containedFields.at(index);
        }

        void* setField(size_t index, size_t size, std::function<void(void*)> destructor) {
            m_containedFields.at(index) = operator new(size);
            m_destructorFunctions.at(index) = std::move(destructor);
            return m_containedFields.at(index);
        }

        static ObjectFieldContainer* from(cocos2d::CCObject* node, char const* forClass) {
            //return node->getFieldContainer(forClass);
        }
    };

    class ObjectMetadata final : public cocos2d::CCNode {
    private:
        std::unordered_map<uint64_t, ObjectFieldContainer*, NoHashHasher<uint64_t>> m_classFieldContainers;
    
        friend class cocos2d::CCObject;
    
        ObjectMetadata() {}
    
        virtual ~ObjectMetadata() {
            for (auto& [_, container] : m_classFieldContainers) {
                delete container;
            }
        }
    
    public:
        static ObjectMetadata* set(CCObject* target) {
            if (!target) return nullptr;
    
            auto meta = new ObjectMetadata();
            meta->autorelease();
    
            target->m_pUserObject = meta;
            meta->retain();
    
            return meta;
        }
    
        ObjectFieldContainer* getFieldContainer(char const* forClass) {
            auto hash = fnv1aHash(forClass);
    
            auto& container = m_classFieldContainers[hash];
            if (!container) {
                container = new ObjectFieldContainer();
            }
    
            return container;
        }
    };

    GEODE_DLL size_t getFieldIndexForClass(char const* name);

    template <class Parent, class Base>
    class ObjectFieldIntermediate {
        using Intermediate = Modify<Parent, Base>;
        std::aligned_storage_t<std::alignment_of_v<Base>, std::alignment_of_v<Base>> m_padding;

    public:
        static void fieldConstructor(void* offsetField) {
            (void) new (offsetField) typename Parent::Fields();
        }

        static void fieldDestructor(void* offsetField) {
            static_cast<typename Parent::Fields*>(offsetField)->~Fields();
        }

        auto self() {

            auto node = reinterpret_cast<Parent*>(reinterpret_cast<std::byte*>(this) - sizeof(Base));
            auto container = ObjectFieldContainer::from(node, typeid(Base).name());
            static size_t index = getFieldIndexForClass(typeid(Base).name());
            auto offsetField = container->getField(index);
            if (!offsetField) {
                offsetField = container->setField(
                    index, sizeof(typename Parent::Fields), &ObjectFieldIntermediate::fieldDestructor
                );
                ObjectFieldIntermediate::fieldConstructor(offsetField);
            }

            return reinterpret_cast<typename Parent::Fields*>(offsetField);
        }
        auto operator->() {
            return this->self();
        }
    };

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

    template <class Derived>
    struct ObjectWrapper : public cocos2d::CCObject {
        ObjectFieldIntermediate<Derived, cocos2d::CCObject> m_fields;
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

#define $objectModify(...) $nodeModify(__VA_ARGS__)