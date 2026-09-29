#include "ObjectFieldContainer.hpp"
#include "CCObjectExt.hpp"

namespace alpha::utils {

struct ObjectFieldContainer::Impl final {
    std::vector<void*> m_containedFields;
    std::vector<geode::Function<void(void*)>> m_destructorFunctions;
};

ObjectFieldContainer::ObjectFieldContainer() : m_impl(std::make_unique<Impl>()) {}

ObjectFieldContainer::~ObjectFieldContainer() {
    auto& fields = m_impl->m_containedFields;
    auto& dtors = m_impl->m_destructorFunctions;

    for (auto i = 0u; i < fields.size(); i++) {
        if (dtors[i] && fields[i]) {
            dtors[i](fields[i]);
            operator delete(fields[i]);
        }
    }
}

void* ObjectFieldContainer::getField(size_t index) {
    auto& fields = m_impl->m_containedFields;
    auto& dtors = m_impl->m_destructorFunctions;

    while (fields.size() <= index) {
        fields.push_back(nullptr);
        dtors.push_back(nullptr);
    }
    return fields.at(index);
}

void* ObjectFieldContainer::setField(size_t index, size_t size, geode::Function<void(void*)> destructor) {
    auto& fields = m_impl->m_containedFields;
    auto& dtors = m_impl->m_destructorFunctions;

    fields.at(index) = operator new(size);
    dtors.at(index) = std::move(destructor);
    return fields.at(index);
}

ObjectFieldContainer* ObjectFieldContainer::from(cocos2d::CCObject* object, char const* forClass) {
    return reinterpret_cast<CCObjectExt<cocos2d::CCObject>*>(object)->getFieldContainer(forClass);
}

}