/**
 * @file result.h
 * @author cosocaf (cosocaf@gmail.com)
 * @brief ある処理の成否をあらわす型を定義する。
 * @version 0.1
 * @date 2022-12-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef ORFOS_KERNEL_LIB_RESULT_H_
#define ORFOS_KERNEL_LIB_RESULT_H_

#pragma once

#include <utility>

#include "panic.h"

namespace orfos::kernel::lib {
  /**
   * @brief 処理の成否をあらわす。
   *
   *
   *
   * @tparam Ok 成功したときの型
   * @tparam Error 失敗したときの型
   */
  template <typename Ok, typename Error>
  class [[nodiscard]] Result {
    enum struct Tag {
      TagNone,
      TagOk,
      TagError,
    } tag;

    union {
      Ok ok;
      Error error;
    };

    template <typename T>
    friend class ok_t;
    template <typename T>
    friend class error_t;

  private:
    constexpr Result(const Ok& ok, [[maybe_unused]] int)
      : tag(Tag::TagOk), ok(ok) {}
    constexpr Result(Ok&& ok, [[maybe_unused]] int)
      : tag(Tag::TagOk), ok(std::move(ok)) {}
    constexpr Result(const Error& error) : tag(Tag::TagError), error(error) {}
    constexpr Result(Error&& error)
      : tag(Tag::TagError), error(std::move(error)) {}

  public:
    constexpr Result(const Result& other) : tag(other.tag) {
      switch (tag) {
        case Tag::TagNone:
          // Do nothing.
          break;
        case Tag::TagOk:
          new (&ok) Ok(other.ok);
          break;
        case Tag::TagError:
          new (&error) Error(other.error);
          break;
        default:
          panic("Unknown tag: %d", tag);
      }
    }
    constexpr Result(Result&& other) : tag(other.tag) {
      switch (tag) {
        case Tag::TagNone:
          // Do nothing.
          break;
        case Tag::TagOk:
          new (&ok) Ok(std::move(other.ok));
          break;
        case Tag::TagError:
          new (&error) Error(std::move(other.error));
          break;
        default:
          panic("Unknown tag: %d", tag);
      }
      other.tag = Tag::TagNone;
    }
    ~Result() {
      switch (tag) {
        case Tag::TagNone:
          break;
        case Tag::TagOk:
          ok.~Ok();
          break;
        case Tag::TagError:
          error.~Error();
          break;
        default:
          panic("Unknown tag: %d", tag);
      }
      tag = Tag::TagNone;
    }

    constexpr Result& operator=(const Result& other) & {
      if (&other != this) {
        switch (other.tag) {
          case Tag::TagNone:
            this->~Result();
            break;
          case Tag::TagOk:
            *this = other.ok;
            break;
          case Tag::TagError:
            *this = other.error;
            break;
          default:
            panic("Unknown tag: %d", other.tag);
        }
      }
      return *this;
    }
    constexpr Result& operator=(Result&& other) & {
      kernelAssert(this != &other);
      switch (other.tag) {
        case Tag::TagNone:
          this->~Result();
          break;
        case Tag::TagOk:
          *this = std::move(other.ok);
          break;
        case Tag::TagError:
          *this = std::move(other.error);
          break;
        default:
          panic("Unknown tag: %d", tag);
      }
      other.tag = Tag::TagNone;
      return *this;
    }

    constexpr operator bool() const {
      kernelAssert(tag != Tag::TagNone);
      return tag == Tag::TagOk;
    }

    constexpr bool operator!() const {
      kernelAssert(tag != Tag::TagNone);
      return tag == Tag::TagError;
    }

    constexpr const Ok& unwrap() const {
      kernelAssert(tag == Tag::TagOk);
      return ok;
    }

    constexpr Ok& unwrap() {
      kernelAssert(tag == Tag::TagOk);
      return ok;
    }

    constexpr const Error& unwrapErr() const {
      kernelAssert(tag == Tag::TagError);
      return error;
    }

    constexpr Error& unwrapErr() {
      kernelAssert(tag == Tag::TagError);
      return error;
    }
  };

  struct _ {};

  template <typename T>
  class ok_t {
    T ok;

  public:
    constexpr explicit ok_t(const T& ok) : ok(ok) {}
    constexpr explicit ok_t(T&& ok) : ok(std::move(ok)) {}

    template <typename Ok, typename Error>
    constexpr operator Result<Ok, Error>() {
      return Result<Ok, Error>(std::move(ok), 0);
    }
  };

  template <typename T>
  constexpr decltype(auto) ok(T&& ok) {
    return ok_t(std::forward<T>(ok));
  }

  template <typename T = _>
  constexpr ok_t<_> ok() {
    return ok(_{});
  }

  template <typename T>
  class error_t {
    T error;

  public:
    constexpr explicit error_t(const T& error) : error(error) {}
    constexpr explicit error_t(T&& error) : error(std::move(error)) {}

    template <typename Ok, typename Error>
    constexpr operator Result<Ok, Error>() {
      return Result<Ok, Error>(std::move(error));
    }
  };

  template <typename T>
  constexpr decltype(auto) error(T&& error) {
    return error_t(std::forward<T>(error));
  }
  template <typename T, size_t N>
  constexpr decltype(auto) error(const T (&error)[N]) {
    return error_t<const T*>(error);
  }
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_RESULT_H_