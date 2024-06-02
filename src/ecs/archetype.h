#pragma once

#include <set>
#include <vector>
#include <unordered_map>
#include <functional>
#include "ecs/component_id.h"
#include "ecs/ecs_id.h"

namespace mkr {
    typedef std::set<component_id_t> archetype_t;

    class archetype {
    private:
        /// A set containing the component type ids of this archetype.
        archetype_t types_;
        /// A 2D array containing the components of this archetype.
        std::vector<void*> components_;
        /// For each component type of this archetype, what is its array index in components_?
        std::unordered_map<component_id_t, std::size_t> component_to_index_;
        /**
         * For each entity in this archetype, what is its array index in components_[type]?
         * For all component_array in components_, we store the components of an entity at the same index.
         * That is to say, in an archetype (let's say of position and rotation), components[position].elements_[i] == components[rotation].elements_[i].
         */
        std::unordered_map<ecs_id_t, std::size_t> entity_to_index_;
        std::vector<ecs_id_t> index_to_entity_;

        // Helper functions.
        std::vector<std::function<void()>> destroy_funcs_; // Delete arrays.
        std::vector<std::function<void()>> append_funcs_; // Append one element to arrays.
        std::vector<std::function<void(std::size_t, std::size_t)>> swap_funcs_; // Swap 2 elements in the arrays.
        std::vector<std::function<void()>> pop_back_funcs_; // Remove the last element in the arrays.
        std::vector<std::function<void(archetype*)>> create_funcs_; // Create arrays.
        std::vector<std::function<void(ecs_id_t, archetype*)>> copy_funcs_; // Copy components of an entity to another archetype.

        archetype() {}

        template<typename T>
        void create_array() {
            const auto comp_id = component_id::value<T>();
            types_.insert(comp_id);

            // Update component type indices.
            auto comp_idx = components_.size();
            component_to_index_[comp_id] = comp_idx;

            // Create array.
            auto arr = new std::vector<T>();
            components_.push_back(arr);

            // Helper functions.
            destroy_funcs_.push_back([arr]() { delete arr; });
            append_funcs_.push_back([arr]() { arr->push_back(T{}); });
            swap_funcs_.push_back([arr](std::size_t _a, std::size_t _b) { std::swap((*arr)[_a], (*arr)[_b]); });
            pop_back_funcs_.push_back([arr]() { arr->pop_back(); });
            create_funcs_.push_back([](archetype* _arc) { _arc->create_array<T>(); });
            copy_funcs_.push_back([this](ecs_id_t _entity, archetype* _dst) {
                if (_dst->has_type<T>()) { // There is a chance we're moving to an archetype with fewer types, such as when removing components.
                    _dst->set<T>(_entity, this->get<T>(_entity));
                }
            });
        }

        template<typename T, typename U, typename ...Args>
        void create_array() {
            create_array<T>();
            create_array<U, Args...>();
        }

        template<typename T>
        std::vector<T>& casted_arr() {
            const std::size_t comp_idx = component_to_index_.find(component_id::value<T>())->second;
            auto casted_arr = static_cast<std::vector<T>*>(components_[comp_idx]);
            return *casted_arr;
        }

        template<typename T>
        const std::vector<T>& casted_arr() const {
            const std::size_t comp_idx = component_to_index_.find(component_id::value<T>())->second;
            return *static_cast<std::vector<T>*>(components_[comp_idx]);
        }

    public:
        template<typename T, typename ...Args>
        static archetype* make() {
            auto arc = new archetype();
            arc->create_array<T, Args...>();
            return arc;
        }

        static archetype* make() {
            return new archetype();
        }

        ~archetype() {
            for (const auto& f : destroy_funcs_) { f(); }
        }

        const archetype_t& types() const { return types_; }

        template<typename T>
        bool has_type() const { return types_.contains(component_id::value<T>()); }

        template<typename T>
        const T& get(ecs_id_t _entity) const {
            const std::size_t ent_idx = entity_to_index_.find(_entity)->second;
            return casted_arr<T>()[ent_idx];
        }

        template<typename T>
        void set(ecs_id_t _entity, const T& _component) {
            const std::size_t ent_idx = entity_to_index_.find(_entity)->second;
            casted_arr<T>()[ent_idx] = _component;
        }

        void add(ecs_id_t _entity) {
            index_to_entity_.push_back(_entity);
            entity_to_index_.insert(std::pair(_entity, entity_to_index_.size()));
            for (const auto& f : append_funcs_) { f(); }
        }

        void remove(ecs_id_t _entity) {
            auto rm_iter = entity_to_index_.find(_entity);
            const std::size_t rm_idx = rm_iter->second;
            const std::size_t last_idx = entity_to_index_.size() - 1;
            const ecs_id_t last_entity = index_to_entity_[last_idx];

            // Swap the components to be removed with the last element in the arrays.
            if (rm_idx != last_idx) {
                index_to_entity_[rm_idx] = last_entity;
                entity_to_index_[last_entity] = rm_idx;
                for (const auto & f : swap_funcs_) { f(rm_idx, last_idx); }
            }

            // Remove last element.
            index_to_entity_.pop_back();
            entity_to_index_.erase(rm_iter);
            for (const auto & f : pop_back_funcs_) { f(); }
        }

        bool has_entity(ecs_id_t _entity) const { return entity_to_index_.contains(_entity); }

        template<typename T>
        archetype* branch_to() const {
            auto arc = new archetype();
            for (const auto& f : create_funcs_) { f(arc); }
            arc->create_array<T>();
            return arc;
        }

        void move_to(ecs_id_t _entity, archetype* _dst) {
            _dst->add(_entity);
            for (const auto& f : copy_funcs_) { f(_entity, _dst); }
            remove(_entity);
        }
    };
}