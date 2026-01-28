#include "DummyScriptEngineProtocol.hpp"
#include "ModifyHandler.hpp"

void DummyScriptEngineProtocol::removeScriptObjectByCCObject(CCObject* pObj) {
    alpha::utils::ModifyHandler::get()->releaseObjectData(pObj->m_nLuaID);
}

$execute {
    CCScriptEngineManager::sharedManager()->setScriptEngine(new DummyScriptEngineProtocol());
}