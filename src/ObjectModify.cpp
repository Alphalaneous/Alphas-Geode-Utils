#include <Geode/Geode.hpp>
#include "ObjectModify.hpp"

using namespace geode::prelude;
using namespace alpha::utils;

geode::utils::StringMap<std::vector<ObjectModifyInfo>>& ObjectModify::getObjectsToModify() {
    return m_objectsToModify;
}

geode::utils::StringMap<std::vector<ObjectModifyInfo>>& ObjectModify::getObjectBasesToModify() {
    return m_objectBasesToModify;
}

void ObjectModify::addObjectToModify(geode::ZStringView className, int prio, geode::Function<void(ModifyCCObject<CCObject>*)> func) {
    auto& vec = m_objectsToModify[className];
    vec.push_back({prio, std::move(func)});
    std::sort(vec.begin(), vec.end(),
        [](auto const& a, auto const& b) { return a.priority < b.priority; });
}

void ObjectModify::addObjectToModifyBase(geode::ZStringView className, int prio, geode::Function<void(ModifyCCObject<CCObject>*)> func) {
    auto& vec = m_objectBasesToModify[className];
    vec.push_back({prio, std::move(func)});
    std::sort(vec.begin(), vec.end(),
        [](auto const& a, auto const& b) { return a.priority < b.priority; });
}

ObjectModify* ObjectModify::get() {
    static ObjectModify instance;
    return &instance;
}