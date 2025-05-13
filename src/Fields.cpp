#include <Geode/Geode.hpp>
#include "../include/Fields.h"
#include "../include/CCObject.h"

using namespace geode::prelude;

ObjectFieldContainer::~ObjectFieldContainer() {
    for (auto i = 0u; i < m_containedFields.size(); i++) {
        if (m_destructorFunctions[i] && m_containedFields[i]) {
            m_destructorFunctions[i](m_containedFields[i]);
            operator delete(m_containedFields[i]);
        }
    }
}

void* ObjectFieldContainer::getField(size_t index) {
    while (m_containedFields.size() <= index) {
        m_containedFields.push_back(nullptr);
        m_destructorFunctions.push_back(nullptr);
    }
    return m_containedFields.at(index);
}

void* ObjectFieldContainer::setField(size_t index, size_t size, std::function<void(void*)> destructor) {
    m_containedFields.at(index) = operator new(size);
    m_destructorFunctions.at(index) = std::move(destructor);
    return m_containedFields.at(index);
}

ObjectFieldContainer* ObjectFieldContainer::from(FieldCCObject* object, char const* forClass) {
    return object->getFieldContainer(forClass);
}

ObjectMetadata::ObjectMetadata() {}

ObjectMetadata::~ObjectMetadata() {
    for (auto& [_, container] : m_classFieldContainers) {
        delete container;
    }
}

ObjectMetadata* ObjectMetadata::set(FieldCCObject* target) {
    if (!target) return nullptr;

    auto old = target->getUserObject();
    // faster than dynamic_cast, technically can
    // but extremely unlikely to fail
    if (old && old->getTag() == METADATA_TAG) {
        return static_cast<ObjectMetadata*>(old);
    }
    auto meta = new ObjectMetadata();
    meta->autorelease();
    meta->setTag(METADATA_TAG);

    // set user object
    target->setUserObject(meta);
    meta->retain();

    if (old) {
        meta->m_userObjects.insert({ "", old });
        // the old user object is now managed by Ref
        old->release();
    }
    return meta;
}

ObjectFieldContainer* ObjectMetadata::getFieldContainer(char const* forClass) {
    auto hash = fnv1aHash(forClass);

    auto& container = m_classFieldContainers[hash];
    if (!container) {
        container = new ObjectFieldContainer();
    }

    return container;
}

