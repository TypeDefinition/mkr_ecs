#pragma once

#include <cstdint>
#include <cstring>

namespace mkr {
    // Flags (Currently Unused)
    #define ECS_FLAG_00 (1ull << 63)
    #define ECS_FLAG_01 (1ull << 62)
    #define ECS_FLAG_02 (1ull << 61)
    #define ECS_FLAG_03 (1ull << 60)
    #define ECS_FLAG_04 (1ull << 59)
    #define ECS_FLAG_05 (1ull << 58)
    #define ECS_FLAG_06 (1ull << 57)
    #define ECS_FLAG_07 (1ull << 56)
    #define ECS_FLAG_08 (1ull << 55)
    #define ECS_FLAG_09 (1ull << 54)
    #define ECS_FLAG_10 (1ull << 53)
    #define ECS_FLAG_11 (1ull << 52)
    #define ECS_FLAG_12 (1ull << 51)
    #define ECS_FLAG_13 (1ull << 50)
    #define ECS_FLAG_14 (1ull << 49)
    #define ECS_FLAG_15 (1ull << 48)

    // Masks
    #define ECS_FLAGS_MASK      0xFFFF000000000000ull
    #define ECS_INDEX_MASK      0x00000000FFFFFFFFull
    #define ECS_GENERATION_MASK 0x0000FFFF00000000ull

    // Bit Size
    #define ECS_INDEX_BIT_SIZE 32
    #define ECS_GENERATION_BIT_SIZE 16
    #define ECS_FLAGS_BIT_SIZE 16

    // Bit Offset
    #define ECS_INDEX_BIT_OFFSET      0
    #define ECS_GENERATION_BIT_OFFSET 32
    #define ECS_FLAGS_BIT_OFFSET      48

    // Valid Indices: [0, ECS_MAX_INDEX)
    // Valid Generations: [0, ECS_MAX_GENERATION)
    // #define ECS_MAX_INDEX      0xFFFFFFFFull
    // #define ECS_MAX_GENERATION 0x0000FFFFull
    // For debugging purposes.
    #define ECS_MAX_INDEX      0xFFull
    #define ECS_MAX_GENERATION 0xFFull

    typedef uint64_t ecs_id_t;

    /**
     * An id is a 64 bit unsigned integer.
     *
     * <-- 16 bits --><-- 16 bits --><---- 32 bits ---->
     * [    Flags    |  Generation  |      Index       ]
     * <---- Hi Bits --                 -- Lo Bits ---->
     *
     * Note: Flags are not exposed to the user, and are only stored in the id master list, ids_.
     * The function create_id() will return an ID only consisting of the index and generation.
     */
    class ecs_id {
    public:
        static const ecs_id_t invalid_id = 0x0000FFFFFFFFFFFFull;

    private:
        /// Master list of ids. Used to keep track of which ids are currently alive or dead, as well as to store the flags of the ids.
        ecs_id_t* ids_;
        /**
         * Destroyed ids are stored in a linked list to be recycled. Each recyclable id stores the index of the next recyclable id in a chain.
         * Whenever a id is destroyed, ids_[destroyed_index] will point to next_index_, and next_index_ is updated to point to this latest destroyed index.
         */
        ecs_id_t next_index_ = ECS_MAX_INDEX;
        /// Used to count the total number of ids generated.
        ecs_id_t id_counter_ = 0;
        /// Used to count the number of alive ids.
        std::size_t num_alive_ = 0;

        ecs_id_t set_flag(ecs_id_t _index, ecs_id_t _flag);
        ecs_id_t reset_flag(ecs_id_t _index, ecs_id_t _flag);
        ecs_id_t clear_flags(ecs_id_t _index);

        ecs_id_t generate_new_id();
        ecs_id_t recycle_old_id();

    public:
        ecs_id();
        ~ecs_id();

        static ecs_id_t index_of(ecs_id_t _id) { return ECS_INDEX_MASK & _id; }
        static ecs_id_t generation_of(ecs_id_t _id) { return (ECS_GENERATION_MASK & _id) >> ECS_GENERATION_BIT_OFFSET; }

        std::size_t num_alive() const { return num_alive_; }

        bool is_valid(ecs_id_t _id) const;

        ecs_id_t create_id();
        bool destroy_id(ecs_id_t _id);
    };
}
