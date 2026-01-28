#include <Geode/Geode.hpp>
#include "Fields.hpp"
#include "ModifyCCObject.hpp"

using namespace geode::prelude;
using namespace alpha::utils;

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

ObjectFieldContainer* ObjectFieldContainer::from(cocos2d::CCObject* object, char const* forClass) {
    return reinterpret_cast<ModifyCCObject<cocos2d::CCObject>*>(object)->getFieldContainer(forClass);
}

ObjectMetadata::ObjectMetadata() {}

ObjectMetadata::~ObjectMetadata() {
    for (auto& [_, container] : m_classFieldContainers) {
        delete container;
    }
}

ObjectMetadata* ObjectMetadata::set(cocos2d::CCObject* target) {
    if (!target) return nullptr;

    auto obj = reinterpret_cast<ModifyCCObject<cocos2d::CCObject>*>(target);

    auto old = obj->getUserObject();
    if (old && old->getTag() == METADATA_TAG) {
        return static_cast<ObjectMetadata*>(old);
    }
    auto meta = new ObjectMetadata();
    meta->autorelease();
    meta->setTag(METADATA_TAG);

    obj->setUserObject(meta);
    meta->retain();

    if (old) {
        meta->m_userObjects.insert({ "", old });
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