#include "ObjectMetadata.hpp"
#include "ModifyHandler.hpp"
#include "ModifyHandlerImpl.hpp"

namespace alpha::utils {

template <typename T>
class NoHashHasher;

template <>
class NoHashHasher<uint64_t> {
public:
    size_t operator()(uint64_t key) const {
        return key;
    }
};

struct ObjectMetadata::Impl final {
    std::unordered_map<uint64_t, ObjectFieldContainer*, NoHashHasher<uint64_t>> m_classFieldContainers;
    geode::utils::StringMap<geode::Ref<cocos2d::CCObject>> m_userObjects;
};

ObjectMetadata::ObjectMetadata() : m_impl(std::make_unique<Impl>()) {}

ObjectMetadata::~ObjectMetadata() {
    for (auto& [_, container] : m_impl->m_classFieldContainers) {
        delete container;
    }
}

ObjectMetadata* ObjectMetadata::set(cocos2d::CCObject* target) {
    auto handler = ModifyHandler::get();
    if (target->m_nLuaID == 0) {
        target->m_nLuaID = handler->m_impl->allocateObjectData();
    }
    return handler->m_impl->m_arena[target->m_nLuaID].get();
}

ObjectFieldContainer* ObjectMetadata::getFieldContainer(cocos2d::CCObject* object, char const* forClass) {
    auto meta = ObjectMetadata::set(object);

    auto hash = geode::prelude::fnv1aHash(forClass);

    auto& container = meta->m_impl->m_classFieldContainers[hash];
    if (!container) {
        container = new ObjectFieldContainer();
    }

    return container;
}

cocos2d::CCObject* ObjectMetadata::getUserObject(cocos2d::CCObject* object, geode::ZStringView id) {
    auto meta = set(object);
    if (meta->m_impl->m_userObjects.count(id)) {
        return meta->m_impl->m_userObjects.at(id);
    }
    return nullptr;
}

void ObjectMetadata::setUserObject(cocos2d::CCObject* object, geode::ZStringView id, cocos2d::CCObject* value) {
    auto meta = set(object);

    if (value) {
        meta->m_impl->m_userObjects[id] = value;
    }
    else {
        meta->m_impl->m_userObjects.erase(id);
    }
}

}