#include "ecs/world.h"

namespace mkr {
    world::world() {
        // Add empty archetype.
        archetypes_.insert(std::pair(archetype_t{}, archetype::make()));
    }

    world::~world() {
        for (auto &iter: archetypes_) { delete iter.second; }
    }

    id_t world::create_entity() {
        id_t ent = entities_.create_id();
        archetype *arc = archetypes_[archetype_t{}];
        ent_to_arc_.insert(std::pair(ent, arc));
        arc->add(ent);
        return ent;
    }

    void world::destroy_entity(id_t _entity) {
        entities_.destroy_id(_entity);
        archetype *arc = ent_to_arc_[_entity];
        arc->remove(_entity);
        ent_to_arc_.erase(ent_to_arc_.find(_entity));
    }
}