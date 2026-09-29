#include <Geode/modify/CCObject.hpp>
#include "Geode/modify/Modify.hpp"
#include "ModifyHandler.hpp"
#include "../ModifyHandlerImpl.hpp"

using namespace geode::prelude;

namespace alpha::utils {

struct AGUCCObject : geode::Modify<AGUCCObject, CCObject> {
    CCObject* autorelease() {
        auto ret = CCObject::autorelease();
        if (ret) alpha::utils::ModifyHandler::get()->m_impl->handleObject(this);
        return ret;
    }
};

}