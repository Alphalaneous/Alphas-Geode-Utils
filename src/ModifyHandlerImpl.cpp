#include "ModifyHandlerImpl.hpp"
#include "CCObjectExt.hpp"

namespace alpha::utils {

uint32_t ModifyHandler::Impl::allocateObjectData() {
    if (m_closing) return 0;

    auto data = std::make_shared<ObjectMetadata>();
    
    if (!m_slots.empty()) {
        auto id = m_slots.front();
        m_slots.pop();
        m_arena[id - 1] = data;
        return id;
    }

    m_arena.emplace_back(data);
    return static_cast<uint32_t>(m_arena.size());
}

void ModifyHandler::Impl::releaseObjectData(uint32_t id) {
    if (m_closing) return;

    if (id - 1 < m_arena.size() && m_arena[id - 1]) {
        m_arena[id - 1] = nullptr;
        m_slots.push(id);
    }
}

void ModifyHandler::Impl::handleObject(cocos2d::CCObject* object) {
    if (m_closing) return;

    auto it = m_objectsToModify.find(geode::cocos::getObjectName(object));
    if (it != m_objectsToModify.end()) {
        for (auto& data : it->second) {
            data->getMethod()(static_cast<CCObjectExt<cocos2d::CCObject>*>(object));
        }
    }

    for (auto& [k, v] : m_objectBasesToModify) {
        for (auto& data : v) {
            data->getMethod()(static_cast<CCObjectExt<cocos2d::CCObject>*>(object));
        }
    }
}

}
