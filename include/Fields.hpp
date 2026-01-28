#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <unordered_map>
#include <functional>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/function.hpp>
#include <Geode/utils/StringMap.hpp>

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_UTILS_API_EXPORTING
        #define ALPHA_UTILS_API_DLL __declspec(dllexport)
    #else
        #define ALPHA_UTILS_API_DLL __declspec(dllimport)
    #endif
#else
    #define ALPHA_UTILS_API_DLL __attribute__((visibility("default")))
#endif

//all of this is geode's work, just retrofitted to work with CCObjects
namespace alpha::utils {

    constexpr auto METADATA_TAG = 0xB324ABC;

    template <typename T>
    class NoHashHasher;

    template <>
    class NoHashHasher<uint64_t> {
    public:
        size_t operator()(uint64_t key) const {
            return key;
        }
    };

    template <class Base>
    requires std::derived_from<Base, cocos2d::CCObject>
    class ModifyCCObject;

    class ALPHA_UTILS_API_DLL ObjectFieldContainer {
    private:
        std::vector<void*> m_containedFields;
        std::vector<std::function<void(void*)>> m_destructorFunctions;

    public:
        ~ObjectFieldContainer();

        void* getField(size_t index);
        void* setField(size_t index, size_t size, std::function<void(void*)> destructor);
        static ObjectFieldContainer* from(cocos2d::CCObject* object, char const* forClass);
    };

    size_t getFieldIndexForClass(char const* name);

    class ALPHA_UTILS_API_DLL ObjectMetadata final : public cocos2d::CCObject {
    public:
        std::unordered_map<uint64_t, ObjectFieldContainer*, NoHashHasher<uint64_t>> m_classFieldContainers;
        geode::utils::StringMap<geode::Ref<cocos2d::CCObject>> m_userObjects;

        ObjectMetadata();

        virtual ~ObjectMetadata();

        static ObjectMetadata* set(cocos2d::CCObject* target);

        ObjectFieldContainer* getFieldContainer(char const* forClass);
    };

    template <class, class>
    struct ObjectWrapper;

    template <class Parent, class Base>
    class ObjectFieldIntermediate {
        using Intermediate = alpha::utils::ObjectWrapper<Parent, Base>;
        alignas(Base) std::array<std::byte, alignof(Base)> m_padding;
        
    public:
        static void fieldConstructor(void* offsetField) {
            (void) new (offsetField) typename Parent::Fields();
        }

        static void fieldDestructor(void* offsetField) {
            static_cast<typename Parent::Fields*>(offsetField)->~Fields();
        }

        auto self() {
            auto object = reinterpret_cast<cocos2d::CCObject*>(reinterpret_cast<std::byte*>(this) - sizeof(Base));
            auto container = ObjectFieldContainer::from(object, typeid(Base).name());

            static size_t index = geode::modifier::getFieldIndexForClass(typeid(Base).name());

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
}