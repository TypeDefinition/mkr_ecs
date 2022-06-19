#pragma once

#include <bitset>
#include "common/concepts.h"
#include "common/family.h"

namespace mkr {
    class component {
    protected:
        component() = default;
        virtual ~component() = default;
    public:
        static constexpr uint64_t MAX_TYPES = 512;

        template<class ...Cs>
        std::bitset<component::MAX_TYPES> generate_bitset() {
            std::bitset<component::MAX_TYPES> bitset;
            (bitset.set(FAMILY_ID(component, Cs)), ...);
            return bitset;
        }
    };

    typedef std::bitset<component::MAX_TYPES> component_bitset;
}
