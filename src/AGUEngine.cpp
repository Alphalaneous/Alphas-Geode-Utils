#include "AGUEngine.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include "ModifyHandler.hpp"
#include "ModifyHandlerImpl.hpp"

namespace alpha::utils {

void AGUEngine::removeScriptObjectByCCObject(CCObject* pObj) {
    alpha::utils::ModifyHandler::get()->m_impl->releaseObjectData(pObj->m_nLuaID);
}

cocos2d::ccScriptType AGUEngine::getScriptType() { 
    // Javascript is called without needing to change m_nLuaID
    return cocos2d::ccScriptType::kScriptTypeJavascript; 
}

int AGUEngine::executeNodeEvent(cocos2d::CCNode* pNode, int nAction) { 
    NodeEvent(pNode, static_cast<NodeEventType>(nAction)).send();
    return -1; 
}

int AGUEngine::executeMenuItemEvent(cocos2d::CCMenuItem* pMenuItem) { 
    MenuItemActivatedEvent(pMenuItem).send(pMenuItem);
    return -1; 
}

}

$on_mod(Loaded) {
    CCScriptEngineManager::sharedManager()->setScriptEngine(new alpha::utils::AGUEngine());
}