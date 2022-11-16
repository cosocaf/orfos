#ifndef ORFOS_KERNEL_LIB_QUEUE_H_
#define ORFOS_KERNEL_LIB_QUEUE_H_

#pragma once

#include <console/console.h>

#include <cassert>
#include <cstddef>
#include <queue>

namespace orfos::kernel::lib {
  template <typename T>
  class QueueContainer {
    struct Node {
      Node* next;
      T value;
    };
    Node* head;
    Node* tail;

  public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = size_t;

    QueueContainer() : head(nullptr), tail(nullptr) {}
    ~QueueContainer() {
      auto elem = head;
      while (elem != nullptr) {
        auto next = elem->next;
        delete elem;
        elem = next;
      }
      head = nullptr;
      tail = nullptr;
    }

    T& front() {
      assert(!empty());
      return head->value;
    }
    const T& front() const {
      assert(!empty());
      return head->value;
    }
    T& back() {
      return tail->value;
    }
    const T& back() const {
      return tail->value;
    }
    void push_back(const T& value) {
      auto node = new Node{
        .next  = nullptr,
        .value = value,
      };
      if (head == nullptr) {
        head = node;
        tail = node;
      } else {
        tail->next = node;
        tail       = node;
      }
    }
    void pop_front() {
      assert(!empty());
      auto node = head;
      head      = head->next;
      if (head == nullptr) {
        tail = nullptr;
      }
      delete node;
    }
    template <typename... Args>
    T& emplace_back(Args&&... args) {
      push_back(T(args...));
    }

    bool empty() const {
      return head == nullptr && tail == nullptr;
    }
  };

  template <typename T>
  using Queue = std::queue<T, QueueContainer<T>>;
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_QUEUE_H_