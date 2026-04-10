#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ObjectDestroyedEvent : public GlobalEvent<ObjectDestroyedEvent, bool(), CCObject*> {
    public:
    using GlobalEvent::GlobalEvent;
};

class AGUEngine : public cocos2d::CCScriptEngineProtocol {

    cocos2d::ccScriptType getScriptType();
    void removeScriptObjectByCCObject(cocos2d::CCObject* pObj);
    int executeNodeEvent(cocos2d::CCNode* pNode, int nAction);
    int executeMenuItemEvent(cocos2d::CCMenuItem* pMenuItem);

    void removeScriptHandler(int nHandler) {}
    int reallocateScriptHandler(int nHandler) { return -1; }
    int executeString(const char* codes) { return -1; }
    int executeScriptFile(const char* filename) { return -1; }
    int executeGlobalFunction(const char* functionName) { return -1; }
    int executeNotificationEvent(cocos2d::CCNotificationCenter* pNotificationCenter, const char* pszName) { return -1; }
    int executeCallFuncActionEvent(cocos2d::CCCallFunc* pAction, cocos2d::CCObject* pTarget = nullptr) { return -1; }
    int executeSchedule(int nHandler, float dt, cocos2d::CCNode* pNode = nullptr) { return -1; }
    int executeLayerTouchesEvent(cocos2d::CCLayer* pLayer, int eventType, cocos2d::CCSet *pTouches) { return -1; }
    int executeLayerTouchEvent(cocos2d::CCLayer* pLayer, int eventType, cocos2d::CCTouch *pTouch) { return -1; }
    int executeLayerKeypadEvent(cocos2d::CCLayer* pLayer, int eventType) { return -1; }
    int executeAccelerometerEvent(cocos2d::CCLayer* pLayer, cocos2d::CCAcceleration* pAccelerationValue) { return -1; }
    int executeEvent(int nHandler, const char* pEventName, cocos2d::CCObject* pEventSource = nullptr, const char* pEventSourceClassName = nullptr) { return -1; }
    int executeEventWithArgs(int nHandler, cocos2d::CCArray* pArgs) { return -1; }
    bool handleAssert(const char *msg) { return false; }
    bool parseConfig(ConfigType type, const gd::string& str) { return false; }
};