#pragma once

#include "ObjectFieldIntermediate.hpp"
#include "StaticLoad.hpp"
#include "ModifyHandler.hpp"

namespace alpha::utils {

template <class Derived, class Base, geode::utils::string::ConstexprString BaseStr, bool UsesStr>
struct ObjectWrapper : public CCObjectExt<Base> {
private:
    STATIC_LOAD (
        if constexpr (UsesStr) {
            ModifyHandler::get()->addObjectToModify(BaseStr.data(), Derived::modifyPrio(), [](CCObjectExt<cocos2d::CCObject>* self) {
                reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
            });
        }
        else {
            auto data = arc::getTypename<Base>();
            ModifyHandler::get()->addObjectToModify(std::string(data.first, data.second), Derived::modifyPrio(), [](CCObjectExt<cocos2d::CCObject>* self) {
                reinterpret_cast<Derived*>(reinterpret_cast<Base*>(self))->modify();
            });
        }
    )

public:
    using Self = Derived;

    ObjectFieldIntermediate<Derived, Base, BaseStr, UsesStr> m_fields;
    static int modifyPrio() { return 0; }
    void modify() {}
};

}