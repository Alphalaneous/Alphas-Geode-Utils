#pragma once

#include <Geode/utils/function.hpp>

namespace alpha::utils {

class StaticLoad {
public:
    StaticLoad(geode::Function<void()> callback) {
        callback();
    }
};

#define STATIC_LOAD(callback) \
static inline StaticLoad s_apply{[] {    \
    callback                             \
}};                                      \
static inline auto s_applyRef = &s_apply;

}