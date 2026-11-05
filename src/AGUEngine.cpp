#include "AGUEngine.hpp"
#include <Geode/modify/MenuLayer.hpp>
#include "ModifyHandler.hpp"
#include "ModifyHandlerImpl.hpp"

namespace alpha::utils {

static bool s_closing = false;

void AGUEngine::removeScriptObjectByCCObject(CCObject* pObj) {
    if (s_closing) return;
    alpha::utils::ModifyHandler::get()->m_impl->releaseObjectData(pObj->m_nLuaID);
}

cocos2d::ccScriptType AGUEngine::getScriptType() { 
    // Javascript is called without needing to change m_nLuaID
    return cocos2d::ccScriptType::kScriptTypeJavascript; 
}

int AGUEngine::executeNodeEvent(cocos2d::CCNode* pNode, int nAction) { 
    if (s_closing) return -1;

    NodeEvent(pNode, static_cast<NodeEventType>(nAction)).send();
    return -1; 
}

int AGUEngine::executeMenuItemEvent(cocos2d::CCMenuItem* pMenuItem) { 
    if (s_closing) return -1;

    MenuItemActivatedEvent(pMenuItem).send(pMenuItem);
    return -1; 
}

}

$on_game(Exiting) {
    alpha::utils::s_closing = true;
    alpha::utils::ModifyHandler::get()->cleanup();
}

$on_mod(Loaded) {
    CCScriptEngineManager::sharedManager()->setScriptEngine(new alpha::utils::AGUEngine());
}