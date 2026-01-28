#include <Geode/Geode.hpp>
#include <Geode/modify/CCObject.hpp>
#include "ModifyHandler.hpp"

using namespace geode::prelude;

class $modify(CCObject) {
    CCObject* autorelease() {
        auto ret = CCObject::autorelease();
        if (ret) alpha::utils::ModifyHandler::get()->handleObject(this);
        return ret;
    }
};