#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/include/CCProtocols.h>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/casts.hpp>

namespace alpha::utils {
    namespace cocos {

        static inline std::optional<cocos2d::CCNode*> getChildByClassName(cocos2d::CCNode* node, geode::ZStringView name, int index = 0) {
            if (!node || node->getChildrenCount() == 0) return nullptr;

            size_t indexCounter = 0;
            const size_t childrenCount = node->getChildrenCount();

            bool isNegativeIndex = (index < 0);
            if (isNegativeIndex) {
                index = -index - 1; 
            }

            for (size_t i = (isNegativeIndex ? childrenCount - 1 : 0); 
                isNegativeIndex ? i >= 0 : i < childrenCount; 
                isNegativeIndex ? --i : ++i) {

                cocos2d::CCNode* idxNode = static_cast<cocos2d::CCNode*>(node->getChildren()->objectAtIndex(i));
                if (geode::cocos::getObjectName(idxNode) == name) {
                    if (indexCounter == index) {
                        return idxNode;
                    }
                    ++indexCounter;
                }

                if (isNegativeIndex && i == 0) break;
            }

            return std::nullopt;
        }

        template <class T>
        static inline std::string_view getObjectName() {
        #ifdef GEODE_IS_WINDOWS
            std::string_view tname = typeid(T).name();
            if (tname.starts_with("class ")) {
                tname.remove_prefix(6);
            } else if (tname.starts_with("struct ")) {
                tname.remove_prefix(7);
            }

            return tname;
        #else
            static std::unordered_map<std::type_index, std::string> s_typeNames;
            std::type_index key = typeid(T);

            auto it = s_typeNames.find(key);
            if (it != s_typeNames.end()) {
                return it->second;
            }

            std::string ret;

            int status = 0;
            auto demangle = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
            if (status == 0) {
                ret = demangle;
            }
            free(demangle);
            auto [iter, _] = s_typeNames.insert({key, std::move(ret)});

            return iter->second;
        #endif
        }

        static inline std::optional<cocos2d::CCSprite*> getSprite(geode::ZStringView sprName) {
            cocos2d::CCSprite* spr = cocos2d::CCSprite::create(sprName.data());
            if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
                return std::nullopt;
            }
            return spr;
        }

        static inline std::optional<cocos2d::CCSprite*> getSpriteByFrameName(geode::ZStringView sprFrameName) {
            cocos2d::CCSprite* spr = cocos2d::CCSprite::createWithSpriteFrameName(sprFrameName.data());
            if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
                return std::nullopt;
            }
            return spr;
        }

        template <typename Layer, typename = std::enable_if_t<std::is_pointer_v<Layer>>>
        static inline std::optional<Layer> getLayer() {
            auto scene = cocos2d::CCDirector::get()->getRunningScene();
            if (cocos2d::CCTransitionScene* trans = geode::cast::typeinfo_cast<cocos2d::CCTransitionScene*>(scene)) {
                scene = trans->m_pInScene;
            }
            if (scene) {
                if (Layer node = scene->getChildByType<Layer>(0)) {
                    return node;
                }
            }
            return std::nullopt;
        }

        static inline std::optional<cocos2d::CCNode*> getLayerByClassName(geode::ZStringView className) {
            auto scene = cocos2d::CCDirector::get()->getRunningScene();
            if (cocos2d::CCTransitionScene* trans = geode::cast::typeinfo_cast<cocos2d::CCTransitionScene*>(scene)) {
                scene = trans->m_pInScene;
            }
            if (scene) {
                return getChildByClassName(scene, className, 0);
            }
            return std::nullopt;
        }

        static inline bool setColorByHex(cocos2d::CCRGBAProtocol* node, geode::ZStringView colorHex) {
            geode::Result<cocos2d::ccColor3B> color = geode::cocos::cc3bFromHexString(colorHex);
            if (color.isOk()) {
                node->setColor(color.unwrap());
                return true;
            }
            return false;
        }

        static inline bool hasNode(cocos2d::CCNode* child, cocos2d::CCNode* parent) {
            while (child) {
                if (child == parent) {
                    return true;
                }
                child = child->getParent();
            }
            return false;
        }
    }
}