#include <Geode/Geode.hpp>
#include "../../include/Utils.h"
#include "../../include/NodeModding.h"
#include "CCObject.h"
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

CCScriptEngineManager* MyCCScriptEngineManager::sharedManager() {
    auto ret = CCScriptEngineManager::sharedManager();
    if (!ret->m_pScriptEngine) {
        ret->setScriptEngine(new DummyScriptEngineProtocol());
    }

    return ret;
}

void FieldCCObject::tryCreateData() {
    if (m_nLuaID == 0) {
        ObjectData* data = new ObjectData();
        CCPoolManager::sharedPoolManager()->addObject(data);
        m_nLuaID = allocateObjectData(data);
    }
}

CCObject* FieldCCObject::autorelease() {
    if (CCNode* node = typeinfo_cast<CCNode*>(this)) {
        NodeModding::get()->handleNode(node);
    }
    tryCreateData();

    return CCObject::autorelease();
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