#ifndef ORFOS_KERNEL_LIB_HASH_MAP_H_
#define ORFOS_KERNEL_LIB_HASH_MAP_H_

#pragma once

#include <console/console.h>

#include <functional>
#include <utility>
#include <vector>

namespace orfos::kernel::lib {
  template <typename Key, typename Value>
  class HashMap {
    struct Node {
      std::pair<Key, Value> kv;
      Node* next;
    };

    template <typename Thiz, typename T>
    class Iterator {
      Thiz thiz;
      size_t vecIndex;
      Node* elem;

    public:
      Iterator(Thiz thiz, size_t vecIndex, Node* elem)
        : thiz(thiz), vecIndex(vecIndex), elem(elem) {}
      const T& operator*() const {
        return elem->kv;
      }
      T& operator*() {
        return elem->kv;
      }
      T* operator->() {
        return &elem->kv;
      }
      Iterator& operator++() {
        if (elem->next) {
          elem = elem->next;
        } else if (vecIndex < thiz->nodes.size() - 1) {
          ++vecIndex;
          elem = thiz->nodes[vecIndex];
        }
        return *this;
      }
      Iterator operator++(int) {
        auto prev = *this;
        ++(*this);
        return prev;
      }

      bool operator==(const Iterator& other) const {
        if (thiz != other.thiz) {
          return false;
        }
        if (vecIndex != other.vecIndex) {
          return false;
        }
        if (elem != other.elem) {
          return false;
        }
        return true;
      }
      bool operator!=(const Iterator& other) const {
        return !(*this == other);
      }
    };

    std::hash<Key> makeHash;
    // 2^capacity
    size_t capacity;
    size_t capacityMask;
    std::vector<Node*> nodes;

  public:
    // 2^capacityの容量を確保する
    constexpr HashMap(size_t capacity = 8)
      : capacity(capacity), capacityMask(0) {
      nodes.resize(1ull << capacity);
      for (size_t i = 0; i < capacity; ++i) {
        capacityMask |= 1 << i;
      }
    }

    Value& operator[](const Key& key) {
      auto&& hash = makeHash(key);
      auto index  = hash & capacityMask;
      auto node   = nodes[index];
      Node* prev  = nullptr;
      while (node != nullptr) {
        if (node->kv.first == key) {
          break;
        }
        prev = node;
        node = node->next;
      }
      if (node == nullptr) {
        node = new Node{
          .kv   = { key, Value{} },
          .next = prev,
        };
        nodes[index] = node;
      }
      return node->kv.second;
    }
    void erase(const Key& key) {
      auto&& hash = makeHash(key);
      auto index  = hash & capacityMask;
      auto node   = nodes[index];
      Node* prev  = nullptr;
      while (node != nullptr) {
        if (node->kv.first == key) {
          break;
        }
        prev = node;
        node = node->next;
      }
      if (node == nullptr) {
        return;
      }
      if (prev != nullptr) {
        prev->next = node->next;
      } else {
        nodes[index] = node->next;
      }
      delete node;
    }
    bool contains(const Key& key) const {
      auto&& hash = makeHash(key);
      auto index  = hash & capacityMask;
      auto node   = nodes[index];
      while (node != nullptr) {
        if (node->kv.first == key) {
          break;
        }
        node = node->next;
      }
      return node != nullptr;
    }
    bool empty() const {
      for (auto& node : nodes) {
        if (node) return false;
      }
      return true;
    }

    auto begin() {
      if (nodes.empty()) {
        return end();
      }
      return Iterator<decltype(this), decltype(nodes[0]->kv)>(
        this, 0, nodes.front());
    }
    auto end() {
      return Iterator<decltype(this), decltype(nodes[0]->kv)>(
        this, nodes.size(), nullptr);
    }

    auto begin() const {
      return cbegin();
    }
    auto end() const {
      return cend();
    }

    auto cbegin() const {
      if (nodes.empty()) {
        return end();
      }
      return Iterator<decltype(this), decltype(nodes[0]->kv)>(
        this, 0, nodes.front());
    }
    auto cend() const {
      return Iterator<decltype(this), decltype(nodes[0]->kv)>(
        this, nodes.size(), nullptr);
    }
  };
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_HASH_MAP_H_