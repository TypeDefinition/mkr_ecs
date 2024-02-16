#pragma once

#include "common/type_id.h"

namespace mkr {
    using component_id_t = mkr::type_id_t;
    struct component_id : public mkr::type_id<component_id> { component_id() = delete; };
}