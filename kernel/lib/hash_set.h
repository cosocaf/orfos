#ifndef ORFOS_KERNEL_LIB_HASH_SET_H_
#define ORFOS_KERNEL_LIB_HASH_SET_H_

#pragma once

#include "hash_map.h"

namespace orfos::kernel::lib {
  template <typename T>
  class HashSet {
    HashMap<T, T> container;

    template <typename Itr>
    class Iterator {
      Itr itr;

    public:
      Iterator(const Itr& itr) : itr(itr) {}
      const T& operator*() const {
        return itr->first;
      }
      T& operator*() {
        return itr->first;
      }
      T* operator->() {
        return &itr->first;
      }
      Iterator& operator++() {
        ++itr;
        return *this;
      }
      Iterator operator++(int) {
        auto prev = *this;
        ++(*this);
        return prev;
      }

      bool operator==(const Iterator& other) const {
        return itr == other.itr;
      }
      bool operator!=(const Iterator& other) const {
        return !(*this == other);
      }
    };

  public:
    constexpr HashSet(size_t capacity = 8)
      : container(HashMap<T, T>(capacity)) {}

    void insert(const T& value) {
      container[value] = value;
    }
    void erase(const T& value) {
      container.erase(value);
    }

    bool contains(const T& value) const {
      return container.contains(value);
    }
    bool empty() const {
      return container.empty();
    }
    size_t size() const {
      return container.size();
    }

    Iterator<decltype(container.begin())> begin() {
      if (empty()) {
        return end();
      }
      return Iterator(container.begin());
    }
    Iterator<decltype(container.begin())> end() {
      return Iterator(container.end());
    }

    Iterator<decltype(container.cbegin())> begin() const {
      return cbegin();
    }
    Iterator<decltype(container.cbegin())> end() const {
      return cend();
    }

    Iterator<decltype(container.cbegin())> cbegin() const {
      if (empty()) {
        return end();
      }
      return Iterator(container.begin());
    }
    Iterator<decltype(container.cbegin())> cend() const {
      return Iterator(container.begin());
    }
  };
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_HASH_SET_H_