#include "ModifyHandler.hpp"
#include "ObjectModify.hpp"

using namespace geode::prelude;
using namespace alpha::utils;

ModifyHandler* ModifyHandler::get() {
    static ModifyHandler handler;
    return &handler;
}

uint32_t ModifyHandler::allocateObjectData(ObjectData* data) {
    if (!m_slots.empty()) {
        uint32_t id = m_slots.front(); m_slots.pop();
        m_arena[id] = data;
        return id;
    } else {
        m_arena.push_back(data);
        return static_cast<uint32_t>(m_arena.size() - 1);
    }
}

void ModifyHandler::releaseObjectData(uint32_t id) {
    if (id < m_arena.size() && m_arena[id]) {
        m_arena[id] = nullptr;
        m_slots.push(id);
    }
}

ObjectData* ModifyHandler::getObjectData(uint32_t id) {
    if (id < m_arena.size()) {
        return m_arena[id];
    }
    return nullptr;
}

void ModifyHandler::createObjectData(CCObject* object) {
    if (object->m_nLuaID == 0) {
        ObjectData* data = new ObjectData();
        CCPoolManager::sharedPoolManager()->addObject(data);
        object->m_nLuaID = allocateObjectData(data);
    }
}

void ModifyHandler::handleObject(CCObject* object) {
    createObjectData(object);
    auto className = std::string(geode::cocos::getObjectName(object));
    auto& objectsToModify = ObjectModify::get()->getObjectsToModify();
    if (objectsToModify.contains(className)) {

        std::vector<ObjectModifyInfo> methods = objectsToModify[className];
        std::sort(methods.begin(), methods.end(), [](auto& left, auto& right) {
            return left.priority < right.priority;
        });

        for (auto& pair : methods) {
            pair.method(static_cast<ModifyCCObject<CCObject>*>(object));
        }
    }
}