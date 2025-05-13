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

//all of this is geode's work, just retrofitted to work with CCObjects

constexpr auto METADATA_TAG = 0xB324ABC;

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

class FieldCCObject;

class ALPHA_UTILS_API_DLL ObjectFieldContainer {
private:
    std::vector<void*> m_containedFields;
    std::vector<std::function<void(void*)>> m_destructorFunctions;

public:
    ~ObjectFieldContainer();

    void* getField(size_t index);
    void* setField(size_t index, size_t size, std::function<void(void*)> destructor);
    static ObjectFieldContainer* from(FieldCCObject* object, char const* forClass);
};

size_t getFieldIndexForClass(char const* name);

class ALPHA_UTILS_API_DLL ObjectMetadata final : public cocos2d::CCObject {
public:
    // for performance reasons, this key is the hash of the class name
    std::unordered_map<uint64_t, ObjectFieldContainer*, NoHashHasher<uint64_t>> m_classFieldContainers;
    std::unordered_map<std::string, geode::Ref<cocos2d::CCObject>> m_userObjects;

    ObjectMetadata();

    virtual ~ObjectMetadata();

    static ObjectMetadata* set(FieldCCObject* target);

    ObjectFieldContainer* getFieldContainer(char const* forClass);
};

namespace AlphaUtils {
    template <class Derived>
    struct ObjectWrapper;
}

template <class Parent>
class ObjectFieldIntermediate {
    using Intermediate = AlphaUtils::ObjectWrapper<Parent>;
    // Padding used for guaranteeing any member of parents
    // will be in between sizeof(Intermediate) and sizeof(Parent)
    std::aligned_storage_t<std::alignment_of_v<cocos2d::CCObject>, std::alignment_of_v<cocos2d::CCObject>> m_padding;

public:
    // the constructor that constructs the fields.
    // we construct the Parent first,
    static void fieldConstructor(void* offsetField) {
        (void) new (offsetField) typename Parent::Fields();
    }

    static void fieldDestructor(void* offsetField) {
        static_cast<typename Parent::Fields*>(offsetField)->~Fields();
    }

    auto self() {
        // get the this pointer of the base
        // field intermediate is the first member of Modify
        // meaning we can get the base from ourself
        auto node = reinterpret_cast<Parent*>(reinterpret_cast<std::byte*>(this) - sizeof(cocos2d::CCObject));
        // static_assert(sizeof(Base) + sizeof() == sizeof(Intermediate), "offsetof not correct");

        // generating the container if it doesn't exist
        auto container = ObjectFieldContainer::from(node, typeid(cocos2d::CCObject).name());

        // the index is global across all mods, so the
        // function is defined in the loader source
        static size_t index = geode::modifier::getFieldIndexForClass(typeid(cocos2d::CCObject).name());

        // the fields are actually offset from their original
        // offset, this is done to save on allocation and space
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