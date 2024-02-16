#pragma once

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>
#include <stdexcept>
#include "ecs/id.h"
#include "ecs/component_id.h"
#include "ecs/archetype.h"
#include "ecs/exception.h"

namespace mkr {
    class world {
    private:
        id entities_;
        std::map<archetype_t, archetype*> archetypes_;
        std::unordered_map<id_t, archetype*> ent_to_arc_; /// Maps an entity to its archetype.

    public:
        world();

        ~world();

        id_t create_entity();

        void destroy_entity(id_t _id);

        template<typename T>
        bool has_component(id_t _entity) const {
            auto iter = ent_to_arc_.find(_entity);
            if (iter == ent_to_arc_.end()) { return false; }
            return iter->second->has_type<T>();
        }

        template<typename T>
        const T &get_component(id_t _entity) const {
            // Ensure that entity exists and has component.
            if (!has_component<T>(_entity)) {
                throw missing_component();
            }

            const archetype *arc = ent_to_arc_.find(_entity)->second;
            return arc->get<T>(_entity);
        }

        template<typename T>
        world &add_component(id_t _entity) {
            // Get current archetype.
            archetype *curr_arc = ent_to_arc_[_entity];

            // If the entity already has this component, do nothing.
            if (curr_arc->has_type<T>()) { return *this; }

            // Get new archetype.
            archetype_t new_types = curr_arc->types();
            new_types.insert(component_id::value<T>());
            if (auto iter = archetypes_.find(new_types); iter == archetypes_.end()) {
                archetypes_[new_types] = curr_arc->branch_to<T>();
            }
            auto new_arc = archetypes_[new_types];

            // Move entity from current to new archetype.
            curr_arc->move_to(_entity, new_arc);
            ent_to_arc_[_entity] = new_arc;
            return *this;
        }

        template<typename T>
        world &remove_component(id_t _entity) {
            // Get current archetype.
            archetype *curr_arc = ent_to_arc_[_entity];

            // If the entity does not have this component, do nothing.
            if (!curr_arc->has_type<T>()) { return *this; }

            // Get new archetype.
            archetype_t new_types = curr_arc->types();
            new_types.erase(component_id::value<T>());
            auto new_arc = archetypes_[new_types];

            // Move entity from current to new archetype.
            curr_arc->move_to(_entity, new_arc);
            ent_to_arc_[_entity] = new_arc;
            return *this;
        }
    };
}