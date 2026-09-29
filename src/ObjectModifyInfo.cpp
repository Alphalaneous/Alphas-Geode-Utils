#include "ObjectModifyInfo.hpp"

namespace alpha::utils {

struct ObjectModifyInfo::Impl final {
    int m_priority;
    geode::Function<void(CCObjectExt<cocos2d::CCObject>*)> m_method;
};

ObjectModifyInfo::ObjectModifyInfo(int priority, geode::Function<void(CCObjectExt<cocos2d::CCObject>*)> method) : m_impl(std::make_unique<Impl>()) {
    m_impl->m_priority = priority;
    m_impl->m_method = std::move(method);
}

ObjectModifyInfo::~ObjectModifyInfo() {}

int ObjectModifyInfo::getPriority() const {
    return m_impl->m_priority;
}

geode::Function<void(CCObjectExt<cocos2d::CCObject>*)>& ObjectModifyInfo::getMethod() const {
    return m_impl->m_method;
}

}