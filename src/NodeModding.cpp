#include <Geode/Geode.hpp>
#include "../include/NodeModding.h"
#include "../include/Utils.h"

using namespace geode::prelude;
using namespace AlphaUtils;

std::unordered_map<std::string, std::vector<ModifyObjectInfo>> ObjectModding::getObjectsToModify() {
    return m_objectsToModify;
}

void ObjectModding::addObjectToModify(std::string className, int prio, std::function<void(FieldCCObject*)> func) {
    m_objectsToModify[className].push_back({prio, func});
}

void ObjectModding::handleObject(FieldCCObject* object) {
    std::string className = AlphaUtils::Cocos::getClassName(object);
    if (m_objectsToModify.contains(className)) {
        std::vector<ModifyObjectInfo> methods = m_objectsToModify[className];
        std::sort(methods.begin(), methods.end(), [](auto& left, auto& right) {
            return left.priority < right.priority;
        });

        for (auto& pair : methods) {
            pair.method(object);
        }
    }
}

ObjectModding* ObjectModding::get() {
    static ObjectModding* instance = nullptr;
    if (!instance) {
        instance = new ObjectModding();
    }
    return instance;
}

/*std::unordered_map<std::string, std::vector<ModifyBaseInfo>> BaseModding::getBasesToModify() {
    return m_basesToModify;
}

void BaseModding::addBaseToModify(std::string className, int prio, std::function<void(FieldCCObject*)> func) {
    m_basesToModify[className].push_back({prio, func});
}

void BaseModding::handleBase(FieldCCObject* object) {
    if (m_lock) return;
    for (auto& [key, methods] : m_basesToModify) {
        if (AlphaUtils::Cocos::checkBaseClassNames(object, key)) {
            std::sort(methods.begin(), methods.end(), [](auto& left, auto& right) {
                return left.priority < right.priority;
            });
            for (auto& pair : methods) {
                pair.method(object);
            }
        }
    }
}

BaseModding* BaseModding::get() {
    static BaseModding* instance = nullptr;
    if (!instance) {
        instance = new BaseModding();
    }
    return instance;
}*/

std::unordered_map<std::string, std::vector<ModifyInfo>> NodeModding::getNodesToModify() {
    return m_nodesToModify;
}

void NodeModding::addNodeToModify(std::string className, int prio, std::function<void(CCNode*)> func) {
    std::function<void(FieldCCObject*)> fieldFunc = [func](FieldCCObject* obj) { 
        func(reinterpret_cast<CCNode*>(obj));
    };
    ObjectModding::get()->addObjectToModify(className, prio, fieldFunc);
}

void NodeModding::handleNode(CCNode* node) {
    /* no operation */
}

NodeModding* NodeModding::get() {
    static NodeModding* instance = nullptr;
    if (!instance) {
        instance = new NodeModding();
    }
    return instance;
}