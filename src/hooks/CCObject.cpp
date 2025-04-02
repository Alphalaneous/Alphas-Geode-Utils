#include <Geode/Geode.hpp>
#include "../../include/Utils.h"
#include "../../include/NodeModding.h"
#include <Geode/modify/CCObject.hpp>

using namespace geode::prelude;
using namespace AlphaUtils;

class $modify(CCObject) {
    CCObject* autorelease() {
        //if (CCNode* node = typeinfo_cast<CCNode*>(this)) {
            NodeModding::get()->handleNode(node);
        //}
        return CCObject::autorelease();
    }
};