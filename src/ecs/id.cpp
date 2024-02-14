#include "id.h"

namespace mkr {
    id::id() {
        ids_ = new id_t[ECS_MAX_INDEX]; // Note that on initialisation, ids_ contains garbage values.
    }

    id::~id() {
        delete[] ids_;
    }

    id_t id::set_flag(id_t index, id_t flag) {
        ids_[index] |= flag;
        return ids_[index];
    }

    id_t id::reset_flag(id_t index, id_t flag) {
        ids_[index] &= (~flag);
        return ids_[index];
    }

    id_t id::clear_flags(id_t index) {
        ids_[index] &= (~ECS_FLAGS_MASK);
        return ids_[index];
    }

    id_t id::generate_new_id() {
        // Ensure that we have not run out of possible ids.
        if (ECS_MAX_INDEX == id_counter_) { return invalid_id; }

        const id_t index = id_counter_++;
        const id_t flags = ECS_ALIVE | ECS_ENABLE;

        // Update master list.
        ids_[index] = index | flags;

        // Increment alive counter.
        ++alive_counter_;

        return index;
    }

    id_t id::recycle_old_id() {
        const id_t index = next_index_;
        const id_t generation = ids_[next_index_] & ECS_GENERATION_MASK;
        const id_t flags = ECS_ALIVE | ECS_ENABLE;

        // Update linked list of recyclable ids.
        next_index_ = index_of(ids_[next_index_]);

        // Update master list.
        ids_[index] = index | generation | flags;

        // Increment alive counter.
        ++alive_counter_;

        return index | generation;
    }

    bool id::is_valid(id_t _id) const {
        const id_t index = index_of(_id);
        const id_t generation = generation_of(_id);
        return _id != invalid_id
               && index < id_counter_
               && generation == generation_of(ids_[index]);
    }

    bool id::is_alive(id_t _id) const {
        const id_t index = index_of(_id);
        return is_valid(_id) && static_cast<bool>(ids_[index] & ECS_ALIVE);
    }

    bool id::is_enabled(id_t _id) const {
        const id_t index = index_of(_id);
        return is_valid(_id) && static_cast<bool>(ids_[index] & ECS_ENABLE);
    }

    id_t id::create_id() {
        // Case 1: There are no free ids.
        if (ECS_MAX_INDEX == next_index_) {
            return generate_new_id();
        }
        // Case 2: There are old ids that can be recycled.
        return recycle_old_id();
    }

    bool id::destroy_id(id_t _id) {
        // Check if the id is valid.
        if (!is_alive(_id)) { return false; }

        const id_t index = index_of(_id);
        const id_t generation = generation_of(_id);
        const id_t next_generation = generation + 1;

        // If this id has not exhausted its generations yet, append it to the linked list of recyclable ids.
        if (next_generation < ECS_MAX_GENERATION) {
            ids_[index] = next_index_ | (next_generation
                    << ECS_GENERATION_BIT_OFFSET); // At this point, if this is the first element in the linked list, next_index_ is guaranteed to be ECS_MAX_INDEX.
            next_index_ = index;
        }
        // Else, set its generation to ECS_MAX_GENERATION.
        else {
            ids_[index] = index | (ECS_MAX_GENERATION << ECS_GENERATION_BIT_OFFSET);
        }

        // Decrement alive counter.
        --alive_counter_;

        return true;
    }
}