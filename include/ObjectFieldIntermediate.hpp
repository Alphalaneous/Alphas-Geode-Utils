#pragma once

#include "ObjectFieldContainer.hpp"

namespace alpha::utils {

template <class, class, geode::utils::string::ConstexprString, bool>
struct ObjectWrapper;

template <class Parent, class Base, geode::utils::string::ConstexprString BaseStr, bool UsesStr>
class ObjectFieldIntermediate {
    using Intermediate = alpha::utils::ObjectWrapper<Parent, Base, BaseStr, UsesStr>;
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
