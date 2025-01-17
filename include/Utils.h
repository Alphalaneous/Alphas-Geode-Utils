#pragma once

#include <string>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/include/CCProtocols.h>
#include <Geode/utils/cocos.hpp>
#include <random>

#define public_cast(value, member) [](auto* v) { \
    class FriendClass__; \
    using T = std::remove_pointer<decltype(v)>::type; \
    class FriendeeClass__: public T { \
    protected: \
        friend FriendClass__; \
    }; \
    class FriendClass__ { \
    public: \
        auto& get(FriendeeClass__* v) { return v->member; } \
    } c; \
    return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

namespace AlphaUtils {

    namespace Cocos {

        static inline std::string getClassName(cocos2d::CCObject* obj, bool removeNamespace = false) {
            if (!obj) return "nullptr";

            std::string ret;

        #ifdef GEODE_IS_WINDOWS
            ret = typeid(*obj).name() + 6;
        #else 
            int status = 0;
            auto demangle = abi::__cxa_demangle(typeid(*obj).name(), 0, 0, &status);
            if (status == 0) {
                ret = demangle;
            }
            free(demangle);
        #endif
            if (removeNamespace) {
                std::vector<std::string> colonSplit = geode::utils::string::split(ret, "::");
                ret = colonSplit[colonSplit.size()-1];
            }

            return ret;
        }

        //getChildByType but using a string instead for dynamic use.
        static inline std::optional<cocos2d::CCNode*> getChildByClassName(cocos2d::CCNode* node, std::string name, int index = 0) {
            size_t indexCounter = 0;

            if (!node || node->getChildrenCount() == 0) return std::nullopt;
            // start from end for negative index
            if (index < 0) {
                index = -index - 1;
                for (size_t i = node->getChildrenCount() - 1; i >= 0; i--) {
                    cocos2d::CCNode* idxNode = static_cast<cocos2d::CCNode*>(node->getChildren()->objectAtIndex(i));
                    std::string className = getClassName(idxNode);
                    if (className == name) {
                        if (indexCounter == index) {
                            return idxNode;
                        }
                        ++indexCounter;
                    }
                    if (i == 0) break;
                }
            }
            else {
                for (size_t i = 0; i < node->getChildrenCount(); i++) {
                    cocos2d::CCNode* idxNode = static_cast<cocos2d::CCNode*>(node->getChildren()->objectAtIndex(i));
                    std::string className = getClassName(idxNode);
                    if (className == name) {
                        if (indexCounter == index) {
                            return idxNode;
                        }
                        ++indexCounter;
                    }
                }
            }

            return std::nullopt;
        }

        static inline std::optional<cocos2d::CCSprite*> getSprite(const char* sprName) {
            cocos2d::CCSprite* spr = cocos2d::CCSprite::create(sprName);
            if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
                return std::nullopt;
            }
            return spr;
        }

        static inline std::optional<cocos2d::CCSprite*> getSpriteByFrameName(const char* sprFrameName) {
            cocos2d::CCSprite* spr = cocos2d::CCSprite::createWithSpriteFrameName(sprFrameName);
            if (!spr || spr->getUserObject("geode.texture-loader/fallback")) {
                return std::nullopt;
            }
            return spr;
        }

        template <typename Layer>
        static inline std::optional<Layer> getLayer() {

            auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
            if (cocos2d::CCTransitionScene* trans = geode::cast::typeinfo_cast<cocos2d::CCTransitionScene*>(scene)) {
                scene = public_cast(trans, m_pInScene);
            }
            if (scene) {
                if (Layer node = scene->getChildByType<Layer>(0)) {
                    return node;
                }
            }
            return std::nullopt;
        }

        static inline std::optional<cocos2d::CCNode*> getLayerByClassName(std::string className) {

            auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
            if (cocos2d::CCTransitionScene* trans = geode::cast::typeinfo_cast<cocos2d::CCTransitionScene*>(scene)) {
                scene = public_cast(trans, m_pInScene);
            }
            if (scene) {
                return getChildByClassName(scene, className, 0);
            }
            return std::nullopt;
        }

        static inline bool setColorByHex(cocos2d::CCRGBAProtocol* node, std::string colorHex) {
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

    namespace Misc {
        static inline int getRandomNumber(int lower, int upper) {
            if (lower > upper) {
                std::swap(lower, upper);
            }

            static std::random_device rd; 
            static std::mt19937 gen(rd()); 

            std::uniform_int_distribution<> dist(lower, upper);
            return dist(gen);
        }
    }
}