#ifndef ORFOS_KERNEL_LIB_FIXED_STRING_H_
#define ORFOS_KERNEL_LIB_FIXED_STRING_H_

#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>

#include "libc/string.h"
#include "panic.h"

namespace orfos::kernel::lib {
  /**
   * @brief 固定長文字列クラス
   *
   * このクラスは固定長の文字配列のラッパークラスです。
   * 主にカーネル起動直後など、可変長な長さを扱えない段階で使用します。
   *
   * 定義されるメンバ関数はstd::stringに準拠します。
   *
   * @tparam N 終端を含まない文字列の最大の長さ
   */
  template <size_t N = 127>
  class FixedString {
    char buf[N + 1];
    size_t len;
    static char nullchar;

  public:
    using value_type = char;
    using size_type  = size_t;

  public:
    constexpr static size_t npos = -1;

  public:
    constexpr FixedString() : len(0) {
      buf[0] = '\0';
    }
    template <size_t M>
    constexpr FixedString(const FixedString<M>& other) {
      auto maxLen = std::min(N, M);
      len         = std::min(other.len, maxLen);
      for (size_t i = 0; i < len; ++i) {
        buf[i] = other.buf[i];
      }
      buf[len] = '\0';
    }
    constexpr FixedString(const char* str) : len(0) {
      for (; len < N; ++len, ++str) {
        buf[len] = *str;
        if (*str == '\0') {
          break;
        }
      }
    }
    constexpr FixedString(const char* str, size_t size) : len(std::min(size, N)) {
      for (size_t i = 0; i < len; ++i) {
        buf[i] = str[i];
      }
      buf[len] = '\0';
    }

    template <size_t M>
    constexpr FixedString& operator=(const FixedString<M>& other) & {
      auto maxLen = std::min(N, M);
      len         = std::min(other.len, maxLen);
      for (size_t i = 0; i < len; ++i) {
        buf[i] = other.buf[i];
      }
      buf[len] = '\0';
      return *this;
    }

    constexpr char* begin() {
      return buf;
    }
    constexpr char* end() {
      return buf + len;
    }
    constexpr const char* begin() const {
      return buf;
    }
    constexpr const char* end() const {
      return buf + len;
    }
    constexpr const char* cbegin() const {
      return buf;
    }
    constexpr const char* cend() const {
      return buf + len;
    }

    constexpr size_t size() const {
      return len;
    }
    constexpr size_t length() const {
      return len;
    }
    constexpr size_t max_size() const {
      return N;
    }
    constexpr size_t capacity() const {
      return N;
    }
    constexpr void clear() {
      len    = 0;
      buf[0] = '\0';
    }
    constexpr bool empty() const {
      return len == 0;
    }

    constexpr char& operator[](size_t pos) {
      if (pos >= len) return nullchar;
      return buf[pos];
    }
    constexpr const char& operator[](size_t pos) const {
      if (pos >= len) return nullchar;
      return buf[pos];
    }

    constexpr char& at(size_t pos) {
      kernelAssert(pos < len);
      return buf[pos];
    }
    constexpr const char& at(size_t pos) const {
      kernelAssert(pos < len);
      return buf[pos];
    }

    constexpr char& front() {
      return (*this)[0];
    }
    constexpr const char& front() const {
      return (*this)[0];
    }

    constexpr char& back() {
      return (*this)[len - 1];
    }
    constexpr const char& back() const {
      return (*this)[len - 1];
    }

    template <size_t M>
    constexpr FixedString& operator+=(const FixedString<M>& str) {
      return append(str);
    }
    constexpr FixedString& operator+=(const char* str) {
      return append(str);
    }
    constexpr FixedString& operator+=(char c) {
      return append(1, c);
    }

    template <size_t M>
    constexpr FixedString& append(const FixedString<M>& str) {
      return append(str.data(), str.size());
    }
    template <size_t M>
    constexpr FixedString& append(const FixedString<M>& str, size_t pos, size_t n = npos) {
      return append(str.data() + pos, std::min(n, str.size() - pos));
    }
    constexpr FixedString& append(const char* str, size_t n) {
      for (size_t i = 0; i < n; ++i) {
        push_back(str[i]);
      }
      return *this;
    }
    constexpr FixedString& append(const char* str) {
      for (size_t i = 0; str[i] != '\0'; ++i) {
        push_back(str[i]);
      }
      return *this;
    }
    constexpr FixedString& append(size_t n, char c) {
      for (size_t i = 0; i < n && len < N; ++i) {
        push_back(c);
      }
      return *this;
    }

    template <typename InputIterator>
    constexpr FixedString& append(InputIterator first, InputIterator last) {
      while (first != last && len < N) {
        push_back(*first);
        ++first;
      }
      return *this;
    }

    constexpr void push_back(char c) {
      if (len >= N) {
        return;
      }
      buf[len] = c;
      ++len;
      buf[len] = '\0';
    }

    constexpr void pop_back() {
      kernelAssert(len > 0);
      --len;
      buf[len] = '\0';
    }

    constexpr const char* c_str() const {
      return buf;
    }
    constexpr char* data() {
      return buf;
    }
    constexpr const char* data() const {
      return buf;
    }

    // static constexpr FixedString format()
  };

  template <size_t N>
  char FixedString<N>::nullchar = '\0';

  template <size_t N, size_t M>
  constexpr bool operator<(const FixedString<N>& a, const FixedString<M>& b) {
    if (a.size() != b.size()) {
      return false;
    }
    for (size_t i = 0; i < N; ++i) {
      if (a[i] < b[i]) {
        return true;
      } else if (a[i] > b[i]) {
        return false;
      }
    }
    return false;
  }
  template <size_t N, size_t M>
  constexpr bool operator>(const FixedString<N>& a, const FixedString<M>& b) {
    return b < a;
  }
  template <size_t N, size_t M>
  constexpr bool operator<=(const FixedString<N>& a, const FixedString<M>& b) {
    return !(a > b);
  }
  template <size_t N, size_t M>
  constexpr bool operator>=(const FixedString<N>& a, const FixedString<M>& b) {
    return !(a < b);
  }

  template <size_t N, size_t M>
  constexpr bool operator==(const FixedString<N>& a, const FixedString<M>& b) {
    return !(a < b) && !(a > b);
  }
  template <size_t N, size_t M>
  constexpr bool operator!=(const FixedString<N>& a, const FixedString<M>& b) {
    return !(a == b);
  }
  template <size_t N>
  constexpr bool operator==(const FixedString<N>& a, const char* b) {
    if (a.size() != strlen(b)) {
      return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
      if (a[i] != b[i]) {
        return false;
      }
    }
    return true;
  }
  template <size_t N>
  constexpr bool operator==(const char* a, const FixedString<N>& b) {
    return b == a;
  }
  template <size_t N>
  constexpr bool operator!=(const FixedString<N>& a, const char* b) {
    return !(a == b);
  }
  template <size_t N>
  constexpr bool operator!=(const char* a, const FixedString<N>& b) {
    return !(a == b);
  }
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_FIXED_STRING_H_