#include "ModifyHandler.hpp"
#include "ObjectModifyInfo.hpp"
#include "ModifyHandlerImpl.hpp"
#include <memory>
#include <typeindex>

using namespace geode::prelude;
using namespace alpha::utils;

ModifyHandler::ModifyHandler() : m_impl(std::make_unique<Impl>()) {}

ModifyHandler::~ModifyHandler() {}

ModifyHandler* ModifyHandler::get() {
    static ModifyHandler handler;
    return &handler;
}

std::unordered_map<std::type_index, std::unordered_map<std::type_index, bool>>& ModifyHandler::getTypes() {
    return m_impl->m_types;
}

void ModifyHandler::cleanup() {
    m_impl->m_closing = true;
    m_impl->m_arena.clear();
}

void ModifyHandler::addObjectToModify(geode::ZStringView className, int priority, geode::Function<void(CCObjectExt<CCObject>*)> method) {
    auto& vec = m_impl->m_objectsToModify[className];
    vec.emplace_back(std::make_shared<ObjectModifyInfo>(priority, std::move(method)));

    std::sort(vec.begin(), vec.end(), [] (const auto& a, const auto& b) { 
        return a->getPriority() < b->getPriority(); 
    });
}

void ModifyHandler::addObjectToModifyBase(geode::ZStringView className, int priority, geode::Function<void(CCObjectExt<CCObject>*)> method) {
    auto& vec = m_impl->m_objectBasesToModify[className];
    vec.emplace_back(std::make_shared<ObjectModifyInfo>(priority, std::move(method)));

    std::sort(vec.begin(), vec.end(), [] (const auto& a, const auto& b) { 
        return a->getPriority() < b->getPriority();
    });
}

$on_game(Exiting) {
    ModifyHandler::get()->cleanup();
}