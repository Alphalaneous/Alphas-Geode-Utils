#include <Geode/Geode.hpp>
#include "../../include/Utils.h"
#include "../../include/NodeModding.h"
#include <Geode/modify/CCObject.hpp>

using namespace geode::prelude;
using namespace AlphaUtils;

class $modify(CCObject) {
    CCObject* autorelease() {
        if (CCNode* node = typeinfo_cast<CCNode*>(this)) {
            std::string className = AlphaUtils::Cocos::getClassName(node);
            if (NodeModding::get()->getNodesToModify().contains(className)) {
                NodeModding::get()->handleNode(node);
            }
        }
        return CCObject::autorelease();
    }
};