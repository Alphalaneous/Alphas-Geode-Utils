#include <Geode/Geode.hpp>
#include "../../include/Utils.h"
#include "../../include/NodeModding.h"
#include "../../include/CCObject.h"
#include "../../include/Fields.h"
#include <Geode/modify/CCObject.hpp>
#include <queue>

using namespace geode::prelude;
using namespace AlphaUtils;

std::vector<Ref<ObjectData>>& getObjectArena() {
    static auto* arena = new std::vector<Ref<ObjectData>>();
    return *arena;
}

std::queue<uint32_t>& getFreeSlots() {
    static auto* slots = new std::queue<uint32_t>();
    return *slots;
}

uint32_t allocateObjectData(ObjectData* data) {
    auto& arena = getObjectArena();
    auto& slots = getFreeSlots();
    if (!slots.empty()) {
        uint32_t id = slots.front(); slots.pop();
        arena[id] = data;
        return id;
    } else {
        arena.push_back(data);
        return static_cast<uint32_t>(arena.size() - 1);
    }
}

void releaseObjectData(uint32_t id) {
    auto& arena = getObjectArena();
    auto& slots = getFreeSlots();
    if (id < arena.size() && arena[id]) {
        arena[id] = nullptr;
        slots.push(id);
    }
}

void DummyScriptEngineProtocol::removeScriptObjectByCCObject(CCObject* pObj) {
    releaseObjectData(pObj->m_nLuaID);
}

class $modify(CCObject) {
    CCObject* autorelease() {
        FieldCCObject* obj = reinterpret_cast<FieldCCObject*>(this);
        obj->tryCreateData();
        ObjectModding::get()->handleObject(obj);
        return CCObject::autorelease();
    }
};

void FieldCCObject::tryCreateData() {
    if (m_nLuaID == 0) {
        ObjectData* data = new ObjectData();
        CCPoolManager::sharedPoolManager()->addObject(data);
        m_nLuaID = allocateObjectData(data);
    }
}

ObjectData* FieldCCObject::getObjectData() {
    tryCreateData();
    auto& arena = getObjectArena();
    if (m_nLuaID < arena.size()) {
        return arena[m_nLuaID];
    }
    return nullptr;
}

CCObject* FieldCCObject::getUserObject() {
    ObjectData* data = getObjectData();
    return data ? data->m_object : nullptr;
}

void FieldCCObject::setUserObject(CCObject* object) {
    ObjectData* data = getObjectData();
    if (data && data->m_object != object) {
        data->m_object = object;
    }
}

ObjectFieldContainer* FieldCCObject::getFieldContainer(char const* forClass) {
    return ObjectMetadata::set(this)->getFieldContainer(forClass);
}

void FieldCCObject::setUserObject(std::string const& id, CCObject* value) {
    auto meta = ObjectMetadata::set(this);
    if (value) {
        meta->m_userObjects[id] = value;
    }
    else {
        meta->m_userObjects.erase(id);
    }
}

CCObject* FieldCCObject::getUserObject(std::string const& id) {
    auto meta = ObjectMetadata::set(this);
    if (meta->m_userObjects.count(id)) {
        return meta->m_userObjects.at(id);
    }
    return nullptr;
}

$execute {
    CCScriptEngineManager::sharedManager()->setScriptEngine(new DummyScriptEngineProtocol());
}