#include "ModifyHandlerImpl.hpp"
#include "CCObjectExt.hpp"

namespace alpha::utils {

geode::ZStringView getObjectNameOptimized(const cocos2d::CCObject* obj) {
#ifdef GEODE_IS_WINDOWS
    static std::unordered_map<std::type_index, const char*> s_typeNames;

    auto key = std::type_index(typeid(*obj));

    auto it = s_typeNames.find(key);
    if (it != s_typeNames.end()) {
        return geode::ZStringView(it->second);
    }

    const char* raw = typeid(*obj).name();
    const char* name = raw;

    if (std::strncmp(name, "class ", 6) == 0) name += 6;
    else if (std::strncmp(name, "struct ", 7) == 0) name += 7;

    s_typeNames.emplace(key, name);
    return name;
#else
    static std::unordered_map<std::type_index, std::string> s_typeNames;
    std::type_index key = typeid(*obj);

    auto it = s_typeNames.find(key);
    if (it != s_typeNames.end()) {
        return it->second;
    }

    std::string ret;

    int status = 0;
    auto demangle = abi::__cxa_demangle(typeid(*obj).name(), 0, 0, &status);
    if (status == 0) {
        ret = demangle;
    }
    free(demangle);
    auto [iter, _] = s_typeNames.insert({key, std::move(ret)});

    return iter->second;
#endif
}

uint32_t ModifyHandler::Impl::allocateObjectData() {
    if (m_closing) return 0;

    auto data = std::make_shared<ObjectMetadata>();
    
    if (!m_slots.empty()) {
        auto id = m_slots.front();
        m_slots.pop();
        m_arena[id] = data;
        return id;
    }

    m_arena.emplace_back(data);
    return static_cast<uint32_t>(m_arena.size() - 1);
}

void ModifyHandler::Impl::releaseObjectData(uint32_t id) {
    if (m_closing) return;

    if (id < m_arena.size() && m_arena[id]) {
        m_arena[id] = nullptr;
        m_slots.push(id);
    }
}

void ModifyHandler::Impl::handleObject(cocos2d::CCObject* object) {
    if (m_closing) return;

    auto it = m_objectsToModify.find(getObjectNameOptimized(object));
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