#include <Geode/Geode.hpp>
#include "../../include/Utils.h"
#include "../../include/NodeModding.h"
#include "../../include/CCObject.h"
#include "../../include/Fields.h"
#include <Geode/modify/CCObject.hpp>
#include <queue>

using namespace geode::prelude;
using namespace AlphaUtils;

static std::vector<Ref<ObjectData>> s_objectArena;
static std::queue<uint32_t> s_freeArenaSlots;

uint32_t allocateObjectData(ObjectData* data) {
    if (!s_freeArenaSlots.empty()) {
        uint32_t id = s_freeArenaSlots.front(); s_freeArenaSlots.pop();
        s_objectArena[id] = data;
        return id;
    } else {
        s_objectArena.push_back(data);
        return static_cast<uint32_t>(s_objectArena.size() - 1);
    }
}

void releaseObjectData(uint32_t id) {
    if (id != 0 && id < s_objectArena.size() && s_objectArena[id]) {
        s_objectArena[id] = nullptr;
        s_freeArenaSlots.push(id);
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
    if (m_nLuaID < s_objectArena.size()) {
        return s_objectArena[m_nLuaID];
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