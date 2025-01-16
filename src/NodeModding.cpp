#include <Geode/Geode.hpp>
#include "../include/NodeModding.h"
#include "../include/Utils.h"

using namespace geode::prelude;
using namespace AlphaUtils;

std::unordered_map<std::string, std::vector<ModifyInfo>> NodeModding::getNodesToModify() {
    return m_nodesToModify;
}

void NodeModding::addNodeToModify(std::string className, int prio, std::function<void(CCNode*)> func) {
    m_nodesToModify[className].push_back({prio, func});
}

void NodeModding::handleNode(CCNode* node) {
    std::string className = AlphaUtils::Cocos::getClassName(node);
    if (m_nodesToModify.contains(className)) {
        std::vector<ModifyInfo> methods = m_nodesToModify[className];
        std::sort(methods.begin(), methods.end(), [](auto& left, auto& right) {
            return left.priority < right.priority;
        });

        for (auto pair : methods) {
            pair.method(node);
        }
    }
}

NodeModding* NodeModding::get() {
    static NodeModding* instance = nullptr;
    if (!instance) {
        instance = new NodeModding();
    }
    return instance;
}