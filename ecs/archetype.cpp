#include "archetype.h"

namespace mkr {
    std::unordered_map<uint64_t, std::function<void*()>> archetype::arr_constructors_;
    std::unordered_map<uint64_t, std::function<void(void*)>> archetype::arr_destructors_;
    std::unordered_map<uint64_t, std::function<void(std::shared_ptr<archetype>, uint64_t)>> archetype::remove_funcs_;
    std::unordered_map<uint64_t, std::function<void(std::shared_ptr<archetype>, std::shared_ptr<archetype>, uint64_t)>> archetype::copy_funcs;
}