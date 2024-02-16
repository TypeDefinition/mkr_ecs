#pragma once

#include <set>
#include <vector>
#include <unordered_map>
#include <functional>
#include "common/type_id.h"
#include "ecs/id.h"
#include "ecs/component.h"

namespace mkr {
    typedef std::set<type_id_t> archetype_t;

    class archetype {
    private:
        /// A set containing the component type ids of this archetype.
        archetype_t types_;
        /// A 2D array containing the components of this archetype.
        std::vector<void*> components_;
        /// For each component type of this archetype, what is its array index in components_?
        std::unordered_map<type_id_t, std::size_t> component_to_index_;
        /**
         * For each entity in this archetype, what is its array index in components_[type]?
         * For all component_array in components_, we store the components of an entity at the same index.
         * That is to say, in an archetype (let's say of position and rotation), components[position].elements_[i] == components[rotation].elements_[i].
         */
        std::unordered_map<id_t, std::size_t> entity_to_index_;
        std::vector<id_t> index_to_entity_;
        /// Helpers to clean up our type-erased arrays during destruction.
        std::vector<std::function<void()>> destroy_funcs_;
        /// Helpers to append 1 more element to type-erased arrays.
        std::vector<std::function<void()>> append_funcs_;
        std::vector<std::function<void(std::size_t, std::size_t)>> swap_funcs_;
        std::vector<std::function<void()>> remove_last_funcs_;

        archetype() {}

        template<typename T>
        void create_array() {
            // Update component type indices.
            auto comp_idx = components_.size();
            component_to_index_[type_id<component>::value<T>()] = comp_idx;

            // Create array.
            components_.push_back(new std::vector<T>());

            destroy_funcs_.push_back([this, comp_idx]() {
                auto* arr_ptr = static_cast<std::vector<T>*>(components_[comp_idx]);
                delete arr_ptr;
            });

            append_funcs_.push_back([this, comp_idx]() {
                auto* arr_ptr = static_cast<std::vector<T>*>(components_[comp_idx]);
                arr_ptr->push_back(T{});
            });

            swap_funcs_.push_back([this, comp_idx](std::size_t _a, std::size_t _b) {
                auto* arr_ptr = static_cast<std::vector<T>*>(components_[comp_idx]);
                std::swap((*arr_ptr)[_a], (*arr_ptr)[_b]);
            });

            remove_last_funcs_.push_back([this, comp_idx]() {
                auto* arr_ptr = static_cast<std::vector<T>*>(components_[comp_idx]);
                arr_ptr->pop_back();
            });
        }

        template<typename T, typename U, typename ...Args>
        void create_array() {
            create_array<T>();
            create_array<U, Args...>();
        }

        template<typename T>
        std::vector<T>& casted_arr() {
            const std::size_t comp_idx = component_to_index_.find(type_id<component>::value<T>())->second;
            auto casted_arr = static_cast<std::vector<T>*>(components_[comp_idx]);
            return *casted_arr;
        }

        template<typename T>
        const std::vector<T>& casted_arr() const {
            const std::size_t comp_idx = component_to_index_.find(type_id<component>::value<T>())->second;
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

        template<typename T>
        bool has_component() const { return types_.contains(type_id<component>::value<T>()); }

        template<typename T>
        const T* get(id_t _entity) const {
            const std::size_t ent_idx = entity_to_index_.find(_entity)->second;
            return &casted_arr<T>()[ent_idx];
        }

        template<typename T>
        void set(id_t _entity, T _component) {
            const std::size_t ent_idx = entity_to_index_.find(_entity)->second;
            casted_arr<T>()[ent_idx] = std::move(_component);
        }

        void add(id_t _entity) {
            index_to_entity_.push_back(_entity);
            entity_to_index_.insert(std::pair(_entity, entity_to_index_.size()));
            for (const auto& f : append_funcs_) { f(); }
        }

        void remove(id_t _entity) {
            auto rm_iter = entity_to_index_.find(_entity);
            const std::size_t rm_idx = rm_iter->second;
            const std::size_t last_idx = entity_to_index_.size() - 1;
            const id_t last_entity = index_to_entity_[last_idx];

            // Swap the components to be removed with the last element in the arrays.
            if (rm_idx != last_idx) {
                index_to_entity_[rm_idx] = last_entity;
                entity_to_index_[last_entity] = rm_idx;
                for (const auto & f : swap_funcs_) { f(rm_idx, last_idx); }
            }

            // Remove last element.
            index_to_entity_.pop_back();
            entity_to_index_.erase(rm_iter);
            for (const auto & f : remove_last_funcs_) { f(); }
        }
    };
}