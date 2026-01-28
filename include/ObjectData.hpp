#pragma once

namespace alpha::utils {
    struct ObjectData : public cocos2d::CCObject {
        geode::Ref<cocos2d::CCObject> m_object;
    };
}