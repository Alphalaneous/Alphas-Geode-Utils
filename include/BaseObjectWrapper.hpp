#pragma once

#include "ObjectFieldIntermediate.hpp"
#include "CCObjectExt.hpp"
#include "StaticLoad.hpp"
#include "ModifyHandler.hpp"

namespace alpha::utils {

template <class Derived, class Base, geode::utils::string::ConstexprString BaseStr>
struct BaseObjectWrapper : public CCObjectExt<Base> {
private:
    STATIC_LOAD (
        ModifyHandler::get()->addObjectToModifyBase(BaseStr.data(), Derived::modifyPrio(), [](CCObjectExt<cocos2d::CCObject>* self) {
            if (ModifyHandler::get()->containsBase<Base>(reinterpret_cast<cocos2d::CCObject*>(self))) {
                reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
            }
        });
    )

public:
    using Self = Derived;

    ObjectFieldIntermediate<Derived, Base, BaseStr, false> m_fields;
    static int modifyPrio() { return 0; }
    void modify() {}
};

}