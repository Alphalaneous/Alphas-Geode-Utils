#include <Geode/Geode.hpp>
#include "ObjectModify.hpp"

using namespace geode::prelude;
using namespace alpha::utils;

geode::utils::StringMap<std::vector<ObjectModifyInfo>>& ObjectModify::getObjectsToModify() {
    return m_objectsToModify;
}

void ObjectModify::addObjectToModify(geode::ZStringView className, int prio, std::function<void(ModifyCCObject<CCObject>*)> func) {
    m_objectsToModify[className].push_back({prio, func});
}

void ObjectModify::handleObject(ModifyCCObject<CCObject>* object) {
    auto className = std::string(geode::cocos::getObjectName(object));
    if (m_objectsToModify.contains(className)) {
        std::vector<ObjectModifyInfo> methods = m_objectsToModify[className];
        std::sort(methods.begin(), methods.end(), [](auto& left, auto& right) {
            return left.priority < right.priority;
        });

        for (auto& pair : methods) {
            pair.method(object);
        }
    }
}

ObjectModify* ObjectModify::get() {
    static ObjectModify* instance = nullptr;
    if (!instance) {
        instance = new ObjectModify();
    }
    return instance;
}