#pragma once

#include "ModifyHandler.hpp"
#include "ObjectMetadata.hpp"
#include "ObjectModifyInfo.hpp"
#include <memory>
#include <queue>
#include <typeindex>

namespace alpha::utils {

struct ModifyHandler::Impl final {
    std::unordered_map<std::type_index, std::unordered_map<std::type_index, bool>> m_types;
    std::vector<std::shared_ptr<ObjectMetadata>> m_arena;
    std::queue<uint32_t> m_slots;

    geode::utils::StringMap<std::vector<std::shared_ptr<ObjectModifyInfo>>> m_objectsToModify;
    geode::utils::StringMap<std::vector<std::shared_ptr<ObjectModifyInfo>>> m_objectBasesToModify;

    bool m_closing;

    uint32_t allocateObjectData();
    void releaseObjectData(uint32_t id);
    void handleObject(cocos2d::CCObject* object);

    friend class ObjectMetadata;
};

}