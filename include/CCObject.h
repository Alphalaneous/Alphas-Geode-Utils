#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include <Geode/cocos/cocoa/CCObject.h>

#ifdef GEODE_IS_WINDOWS
    #ifdef ALPHALANEOUS_UTILS_API_EXPORTING
        #define ALPHA_UTILS_API_DLL __declspec(dllexport)
    #else
        #define ALPHA_UTILS_API_DLL __declspec(dllimport)
    #endif
#else
    #define ALPHA_UTILS_API_DLL __attribute__((visibility("default")))
#endif

struct ObjectData : public cocos2d::CCObject {
    geode::Ref<cocos2d::CCObject> m_object;
};

class DummyScriptEngineProtocol : public cocos2d::CCScriptEngineProtocol {
    void removeScriptObjectByCCObject(cocos2d::CCObject* pObj);
    cocos2d::ccScriptType getScriptType() { return cocos2d::kScriptTypeNone; };
    void removeScriptHandler(int nHandler) {};
    int reallocateScriptHandler(int nHandler) { return -1;}
    int executeString(const char* codes) { return -1; };
    int executeScriptFile(const char* filename) { return -1; };
    int executeGlobalFunction(const char* functionName) { return -1; };
    int executeNodeEvent(cocos2d::CCNode* pNode, int nAction) { return -1; };
    int executeMenuItemEvent(cocos2d::CCMenuItem* pMenuItem) { return -1; };
    int executeNotificationEvent(cocos2d::CCNotificationCenter* pNotificationCenter, const char* pszName) { return -1; };
    int executeCallFuncActionEvent(cocos2d::CCCallFunc* pAction, cocos2d::CCObject* pTarget = NULL) { return -1; };
    int executeSchedule(int nHandler, float dt, cocos2d::CCNode* pNode = NULL) { return -1; };
    int executeLayerTouchesEvent(cocos2d::CCLayer* pLayer, int eventType, cocos2d::CCSet *pTouches) { return -1; };
    int executeLayerTouchEvent(cocos2d::CCLayer* pLayer, int eventType, cocos2d::CCTouch *pTouch) { return -1; };
    int executeLayerKeypadEvent(cocos2d::CCLayer* pLayer, int eventType) { return -1; };
    int executeAccelerometerEvent(cocos2d::CCLayer* pLayer, cocos2d::CCAcceleration* pAccelerationValue) { return -1; };
    int executeEvent(int nHandler, const char* pEventName, cocos2d::CCObject* pEventSource = NULL, const char* pEventSourceClassName = NULL) { return -1; };
    int executeEventWithArgs(int nHandler, cocos2d::CCArray* pArgs) { return 0; }
    bool handleAssert(const char *msg) { return false; };
    bool parseConfig(ConfigType type, const gd::string& str) { return false; };
};

class ObjectFieldContainer;

struct ALPHA_UTILS_API_DLL FieldCCObject : public cocos2d::CCObject {
    ObjectData* getObjectData();
    cocos2d::CCObject* getUserObject();
    void setUserObject(cocos2d::CCObject* object);
    void tryCreateData();
    ObjectFieldContainer* getFieldContainer(char const* forClass);
    void setUserObject(std::string const& id, CCObject* value);
    CCObject* getUserObject(std::string const& id);
};