#pragma once

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include "common/type_id.h"
#include "ecs/id.h"
#include "ecs/archetype.h"
#include "ecs/component.h"

namespace mkr {
    class world {
    private:
        id entities_;
        std::map<archetype_t, archetype*> archetypes_;
        std::unordered_map<id_t, archetype*> ent_to_arc_; /// Maps an entity to its archetype.

    public:
        world() {
            archetypes_.insert(std::pair(archetype_t{}, archetype::make()));
        }

        ~world() {
            for (auto& iter : archetypes_) { delete iter.second; }
        }

        template<typename T>
        bool has_component(id_t _entity) const {
            // Check if entity exists.
            auto iter = ent_to_arc_.find(_entity);
            if (iter == ent_to_arc_.end()) { return false; }

            return iter->second->has_component<T>();
        }

        template<typename T>
        const T* get_component(id_t _entity) const {
            // Ensure that entity exists and has component.
            if (!has_component<T>(_entity)) { return nullptr; }

            const archetype* arc = ent_to_arc_.find(_entity)->second;
            return arc->get<T>(_entity);
        }
    };
}