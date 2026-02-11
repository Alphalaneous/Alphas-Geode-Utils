#include "ModifyHandler.hpp"
#include "ObjectModify.hpp"
#include <typeindex>

using namespace geode::prelude;
using namespace alpha::utils;

ZStringView getObjectNameOptimized(const cocos2d::CCObject* obj) {
#ifdef GEODE_IS_WINDOWS
    static std::unordered_map<std::type_index, const char*> s_typeNames;

    auto key = std::type_index(typeid(*obj));

    auto it = s_typeNames.find(key);
    if (it != s_typeNames.end()) {
        return ZStringView(it->second);
    }

    const char* raw = typeid(*obj).name();
    const char* name = raw;

    if (std::strncmp(name, "class ", 6) == 0) name += 6;
    else if (std::strncmp(name, "struct ", 7) == 0) name += 7;

    s_typeNames.emplace(key, name);
    return name;
#else
    static std::unordered_map<std::type_index, std::string> s_typeNames;
    std::type_index key = typeid(*obj);

    auto it = s_typeNames.find(key);
    if (it != s_typeNames.end()) {
        return it->second;
    }

    std::string ret;

    int status = 0;
    auto demangle = abi::__cxa_demangle(typeid(*obj).name(), 0, 0, &status);
    if (status == 0) {
        ret = demangle;
    }
    free(demangle);
    auto [iter, _] = s_typeNames.insert({key, std::move(ret)});

    return iter->second;
#endif
}



ModifyHandler* ModifyHandler::get() {
    static ModifyHandler handler;
    return &handler;
}

uint32_t ModifyHandler::allocateObjectData(ObjectData* data) {
    if (!m_slots.empty()) {
        auto id = m_slots.front();
        m_slots.pop();
        m_arena[id] = data;
        return id;
    }

    m_arena.push_back(data);
    return static_cast<uint32_t>(m_arena.size() - 1);
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
    if (object->m_nLuaID != 0) return;

    ObjectData* data = new ObjectData();
    CCPoolManager::sharedPoolManager()->addObject(data);
    object->m_nLuaID = allocateObjectData(data);
}

void ModifyHandler::handleObject(CCObject* object) {
    createObjectData(object);    
    auto& objectsToModify = ObjectModify::get()->getObjectsToModify();
    auto it = objectsToModify.find(getObjectNameOptimized(object));
    if (it != objectsToModify.end()) {
        for (auto& pair : it->second) {
            pair.method(static_cast<ModifyCCObject<CCObject>*>(object));
        }
    }

    auto& objectBasesToModify = ObjectModify::get()->getObjectBasesToModify();
    for (auto& [k, v] : objectBasesToModify) {
        for (auto& pair : v) {
            pair.method(static_cast<ModifyCCObject<CCObject>*>(object));
        }
    }
}