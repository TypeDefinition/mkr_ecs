#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>
#include "ecs/component.h"

namespace mkr {
    // "Dumb" container for components. Logic is handled by world.
    // Due to limitations of templates, many of these functions are somewhat "unsafe" and "ugly".
    // Therefore, it is up to world to handle the logic properly.
    struct archetype {
        static std::unordered_map<uint64_t, std::function<void *()>> arr_constructors_;
        static std::unordered_map<uint64_t, std::function<void(void *)>> arr_destructors_;
        static std::unordered_map<uint64_t, std::function<void(std::shared_ptr<archetype>, uint64_t)>> remove_funcs_;
        static std::unordered_map<uint64_t, std::function<void(std::shared_ptr<archetype>, std::shared_ptr<archetype>, uint64_t)>> copy_funcs;
        std::unordered_map<uint64_t, void *> components_;

        explicit archetype(const component_bitset &_bitset) {
            for (uint64_t i = 0; i < component::MAX_TYPES; ++i) {
                if (_bitset[i]) {
                    components_[i] = arr_constructors_[i]();
                }
            }
        }

        ~archetype() {
            for (auto &iter: components_) {
                arr_destructors_[iter.first](iter.second);
            }
        }

        template<class C>
        static void register_component_type() {
            const uint64_t family_id = FAMILY_ID(component, C);
            arr_constructors_[family_id] = []() {
                return static_cast<void *>(new std::vector<C>{});
            };
            arr_destructors_[family_id] = [](void *_arr) {
                delete static_cast<std::vector<C> *>(_arr);
            };
            remove_funcs_[family_id] = [=](std::shared_ptr<archetype> _arc, uint64_t _index) {
                auto *arr = static_cast<std::vector<C> *>(_arc->components_[family_id]);
                arr->erase(arr->begin() + _index);
            };
            copy_funcs[family_id] = [=](std::shared_ptr<archetype> _src, std::shared_ptr<archetype> _dst, uint64_t _src_index) {
                auto *src_arr = static_cast<std::vector<C> *>(_src->components_[family_id]);
                auto *dst_arr = static_cast<std::vector<C> *>(_dst->components_[family_id]);
                dst_arr->push_back(src_arr->operator[](_src_index));
            };
        }

        // Add a component to this archetype.
        template<class C>
        static uint64_t add_component(std::shared_ptr<archetype> _arc, C _component) {
            const uint64_t family_id = FAMILY_ID(component, C);
            auto *arr = static_cast<std::vector<C> *>(_arc->components_[family_id]);
            uint64_t new_index = arr->size();
            arr->push_back(std::move(_component));
            return new_index;
        }

        // Given a bitset, copy the components from one archetype to another.
        static void copy_components(std::shared_ptr<archetype> _src, std::shared_ptr<archetype> _dst, uint64_t _index, const component_bitset &_bitset) {
            for (uint64_t i = 0; i < component::MAX_TYPES; ++i) {
                if (_bitset[i]) {
                    copy_funcs[i](_src, _dst, _index);
                }
            }
        }

        // Remove all components at an index.
        static void remove_components(std::shared_ptr<archetype> _arc, uint64_t _index) {
            for (auto &iter: _arc->components_) {
                remove_funcs_[iter.first](_arc, _index);
            }
        }

        template<class C>
        static const C* get_component(std::shared_ptr<archetype> _arc, uint64_t _index) {
            const uint64_t family_id = FAMILY_ID(component, C);
            auto *arr = static_cast<std::vector<C> *>(_arc->components_[family_id]);
            return &arr->operator[](_index);
        }
    };
}