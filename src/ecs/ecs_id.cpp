#include "ecs/ecs_id.h"

namespace mkr {
    ecs_id::ecs_id() {
        // Note that on initialisation, ids_ contains garbage values.
        ids_ = new ecs_id_t[ECS_MAX_INDEX];
    }

    ecs_id::~ecs_id() {
        delete[] ids_;
    }

    ecs_id_t ecs_id::set_flag(ecs_id_t _index, ecs_id_t _flag) {
        ids_[_index] |= _flag;
        return ids_[_index];
    }

    ecs_id_t ecs_id::reset_flag(ecs_id_t _index, ecs_id_t _flag) {
        ids_[_index] &= (~_flag);
        return ids_[_index];
    }

    ecs_id_t ecs_id::clear_flags(ecs_id_t _index) {
        ids_[_index] &= (~ECS_FLAGS_MASK);
        return ids_[_index];
    }

    ecs_id_t ecs_id::generate_new_id() {
        // Ensure that we have not run out of possible ids.
        if (ECS_MAX_INDEX == id_counter_) { return invalid_id; }

        const ecs_id_t index = id_counter_++;
        const ecs_id_t generation = 0;
        const ecs_id_t flags = 0;

        // Update master list.
        ids_[index] = index | generation | flags;

        // Increment alive counter.
        ++num_alive_;

        return index;
    }

    ecs_id_t ecs_id::recycle_old_id() {
        const ecs_id_t index = next_index_;
        const ecs_id_t generation = ids_[next_index_] & ECS_GENERATION_MASK;
        const ecs_id_t flags = 0;

        // Update linked list of recyclable ids.
        next_index_ = index_of(ids_[next_index_]);

        // Update master list.
        ids_[index] = index | generation | flags;

        // Increment alive counter.
        ++num_alive_;

        return index | generation;
    }

    bool ecs_id::is_valid(ecs_id_t _id) const {
        const auto index = index_of(_id);
        const auto generation = generation_of(_id);
        return _id != invalid_id
            && index < id_counter_
            && generation == generation_of(ids_[index]);
    }

    ecs_id_t ecs_id::create_id() {
        // Case 1: There are no free ids.
        if (ECS_MAX_INDEX == next_index_) {
            return generate_new_id();
        }
        // Case 2: There are old ids that can be recycled.
        return recycle_old_id();
    }

    bool ecs_id::destroy_id(ecs_id_t _id) {
        // Check if the id is valid.
        if (!is_valid(_id)) { return false; }

        const ecs_id_t index = index_of(_id);
        const ecs_id_t generation = generation_of(_id);
        const ecs_id_t next_generation = generation + 1;

        // If this id has not exhausted its generations yet, append it to the linked list of recyclable ids.
        if (next_generation < ECS_MAX_GENERATION) {
            // At this point, if this is the first element in the linked list, next_index_ is guaranteed to be ECS_MAX_INDEX.
            ids_[index] = next_index_ | (next_generation << ECS_GENERATION_BIT_OFFSET);
            next_index_ = index;
        }
        // Else, set its generation to ECS_MAX_GENERATION.
        else {
            ids_[index] = index | (ECS_MAX_GENERATION << ECS_GENERATION_BIT_OFFSET);
        }

        // Decrement alive counter.
        --num_alive_;

        return true;
    }
}
