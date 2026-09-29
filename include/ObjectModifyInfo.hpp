#pragma once

#include "CCObjectExt.hpp"

namespace alpha::utils {

class ObjectModifyInfo {
public:
    ObjectModifyInfo(int priority, geode::Function<void(CCObjectExt<cocos2d::CCObject>*)> method);
    ~ObjectModifyInfo();

    ObjectModifyInfo(ObjectModifyInfo&&) noexcept = default;
    ObjectModifyInfo& operator=(ObjectModifyInfo&&) noexcept = default;
    ObjectModifyInfo(const ObjectModifyInfo&) = delete;
    ObjectModifyInfo& operator=(const ObjectModifyInfo&) = delete;

    int getPriority() const;
    geode::Function<void(CCObjectExt<cocos2d::CCObject>*)>& getMethod() const;

protected:
    struct Impl;
    std::unique_ptr<Impl> m_impl; 
};

}