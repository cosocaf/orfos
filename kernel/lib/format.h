#ifndef ORFOS_KERNEL_LIB_FORMAT_H_
#define ORFOS_KERNEL_LIB_FORMAT_H_

#pragma once

#include <array>
#include <charconv>
#include <concepts>
#include <iterator>
#include <string_view>
#include <variant>

#include "fixed_string.h"
#include "panic.h"
#include "result.h"

namespace orfos::kernel::lib {
  namespace internal::format {
    struct FormatOptions {
      int argId     = -1;
      char fill     = ' ';
      char align    = '\0';
      char sign     = '-';
      char type     = '\0';
      bool alt      = false;
      bool zero     = false;
      int width     = 0;
      int precision = 0;
    };

    template <typename OutputIterator, typename... Args>
    class Parser {
      OutputIterator out;
      std::string_view fmt;
      std::array<std::variant<Args...>, sizeof...(Args)> args;
      size_t index;

      template <typename String>
      struct Visitor {
        Parser* thiz;
        FormatOptions options;
        constexpr Visitor(Parser* thiz, const FormatOptions& options)
          : thiz(thiz), options(options) {}

        Result<_, String> operator()(const char c) {
          if (options.align == '\0') {
            options.align = '<';
          }
          if (options.type == '\0') {
            options.type = 'c';
          }
          return thiz->template write<String>(options, c);
        }
        Result<_, String> operator()(const void* value) {
          if (options.align == '\0') {
            options.align = '>';
          }
          if (options.type == '\0') {
            options.type = 'p';
          }
          return thiz->template write<String>(options, value);
        }

        template <std::integral T>
        Result<_, String> operator()(const T value) {
          if (options.align == '\0') {
            options.align = '>';
          }
          if (options.type == '\0') {
            options.type = 'd';
          }
          return thiz->template write<String>(options, value);
        }
        template <std::floating_point T>
        Result<_, String> operator()(const T value) {
          if (options.align == '\0') {
            options.align = '>';
          }
          if (options.type == '\0') {
            options.type = 'f';
          }
          return thiz->template write<String>(options, value);
        }
      };

    public:
      constexpr Parser(OutputIterator out,
                       const std::string_view fmt,
                       const Args&... args)
        : out(out), fmt(fmt), args{ args... }, index(0) {}

      template <typename String>
      [[nodiscard]] constexpr Result<_, String> parse() {
        auto begin = fmt.begin();
        auto end   = fmt.end();
        for (; begin != end; ++begin) {
          if (*begin == '{') {
            auto parseFieldResult = parseField<String>(begin, end);
            if (!parseFieldResult) {
              return error(parseFieldResult.unwrapErr());
            }
            auto& options   = parseFieldResult.unwrap();
            auto pushResult = push<String>(options);
            if (!pushResult) {
              return error(pushResult.unwrapErr());
            }
          } else if (*begin == '}') {
            ++begin;
            if (begin == end || *begin != '}') {
              return error(
                "Failed to parse format. Missing '}' in format string.");
            }
            *out = *begin;
            ++out;
          } else {
            *out = *begin;
            ++out;
          }
        }

        return ok();
      }

    private:
      template <typename String>
      [[nodiscard]] constexpr Result<FormatOptions, String> parseField(
        const char* begin,
        const char* end) {
        kernelAssert(*begin == '{');
        ++begin;
        if (begin == end) {
          return error("Failed to parse format. Missing '}' in format string.");
        }
        if (*begin == '{') {
          *out = *begin;
          ++out;
          return ok(FormatOptions{});
        }

        FormatOptions options;
        if (*begin != '}') {
          begin = parseArgId(options, begin, end);
          begin = parseAlignment(options, begin, end);
          begin = parseSign(options, begin, end);
          begin = parseAlt(options, begin, end);
          begin = parseZero(options, begin, end);
          begin = parseWidth(options, begin, end);
          begin = parsePrecision(options, begin, end);
          begin = parseType(options, begin, end);
        }
        if (begin == end || *begin != '}') {
          return error("Failed to parse format. Missing '}' in format string.");
        }
        return ok(options);
      }

      [[nodiscard]] constexpr const char* parseArgId(FormatOptions& options,
                                                     const char* begin,
                                                     const char* end) {
        if (end - begin < 1) {
          return begin;
        }
        if (*begin != ':') {
          // TODO: implement
          kernelAssert(
            false && "Failed to parse format. Sorry, Arg ID is not supported.");
        }
        ++begin;
        return begin;
      }
      [[nodiscard]] constexpr const char* parseAlignment(FormatOptions& options,
                                                         const char* begin,
                                                         const char* end) {
        if (end - begin < 2) {
          return begin;
        }
        if (begin[1] == '>' || begin[1] == '<' || begin[1] == '^') {
          options.fill  = begin[0];
          options.align = begin[1];
          begin += 2;
        }
        return begin;
      }
      [[nodiscard]] constexpr const char* parseSign(FormatOptions& options,
                                                    const char* begin,
                                                    const char* end) {
        if (end - begin < 1) {
          return begin;
        }
        if (begin[0] == '+' || begin[0] == '-' || begin[0] == ' ') {
          options.sign = begin[0];
          ++begin;
        }
        return begin;
      }
      [[nodiscard]] constexpr const char* parseAlt(FormatOptions& options,
                                                   const char* begin,
                                                   const char* end) {
        if (end - begin < 1) {
          return begin;
        }
        if (begin[0] == '#') {
          options.alt = true;
          ++begin;
        }
        return begin;
      }
      [[nodiscard]] constexpr const char* parseZero(FormatOptions& options,
                                                    const char* begin,
                                                    const char* end) {
        if (end - begin < 1) {
          return begin;
        }
        if (begin[0] == '0') {
          options.zero = true;
          ++begin;
        }
        return begin;
      }
      [[nodiscard]] constexpr const char* parseWidth(FormatOptions& options,
                                                     const char* begin,
                                                     const char* end) {
        if (end - begin < 1) {
          return begin;
        }
        if (begin[0] < '0' || begin[0] > '9') {
          return begin;
        }
        while (begin[0] >= '0' && begin[0] <= '9') {
          options.width *= 10;
          options.width += begin[0] - '0';
          ++begin;
        }
        return begin;
      }
      [[nodiscard]] constexpr const char* parsePrecision(FormatOptions& options,
                                                         const char* begin,
                                                         const char* end) {
        if (end - begin < 2) {
          return begin;
        }
        if (begin[0] != '.' || begin[1] < '0' || begin[1] > '9') {
          return begin;
        }
        ++begin;
        while (begin[0] >= '0' && begin[0] <= '9') {
          options.precision *= 10;
          options.precision += begin[0] - '0';
          ++begin;
        }
        return begin;
      }
      [[nodiscard]] constexpr const char* parseType(FormatOptions& options,
                                                    const char* begin,
                                                    const char* end) {
        for (const auto c : "bBcdoxXfFeEaAgGp") {
          if (begin[0] == c) {
            options.type = c;
            ++begin;
            break;
          }
        }
        return begin;
      }
      template <typename String>
      [[nodiscard]] constexpr Result<_, String> push(
        const FormatOptions& options) {
        if (index >= sizeof...(Args)) {
          return error("Failed to parse format. Too many arguments.");
        }

        return std::visit(Visitor<String>(this, options), args[index]);
      }

      template <typename String, std::integral T>
      Result<_, String> write(const FormatOptions& options, const T& value) {
        FixedString<> prefix;
        FixedString<> buffer;
        auto first = buffer.data();
        auto last  = first + buffer.size();
        std::to_chars_result result;
        switch (options.type) {
          case 'b':
          case 'B':
            if (options.alt) {
              prefix.push_back('0');
              prefix.push_back(options.type);
            }
            result = std::to_chars(first, last, value, 2);
            break;
          case 'c':
            *first     = static_cast<char>(value);
            result.ptr = first + 1;
            break;
          case 'd':
            result = std::to_chars(first, last, value);
            break;
          case 'o':
            if (options.alt && value != 0) {
              prefix = "0";
            }
            result = std::to_chars(first, last, value, 8);
            break;
          case 'x':
          case 'X':
            if (options.alt) {
              prefix.push_back('0');
              prefix.push_back(options.type);
            }
            result = std::to_chars(first, last, value, 16);
            break;
          default:
            result.ptr = first;
        }

        if (result.ec != std::errc{}) {
          return error("Failed to format.");
        }

        write(prefix, first, result);

        return ok();
      }

      template <typename String, std::floating_point T>
      Result<_, String> write(const FormatOptions& options, const T& value) {
        FixedString<> prefix;
        FixedString<> buffer;
        auto first = buffer.data();
        auto last  = first + buffer.size();
        std::to_chars_result result;
        switch (options.type) {
          case 'f':
          case 'F':
            result = std::to_chars(first,
                                   last,
                                   value,
                                   std::chars_format::fixed,
                                   options.precision || 6);
            break;
          case 'e':
          case 'E':
            result = std::to_chars(first,
                                   last,
                                   value,
                                   std::chars_format::scientific,
                                   options.precision || 6);
            break;
          case 'a':
          case 'A':
            result = std::to_chars(
              first, last, value, std::chars_format::hex, options.precision);
            break;
          case 'g':
          case 'G':
            result = std::to_chars(first,
                                   last,
                                   value,
                                   std::chars_format::general,
                                   options.precision || 6);
            break;
          default:
            result.ptr = first;
        }

        if (result.ec != std::errc{}) {
          return error("Failed to format.");
        }

        write(prefix, first, result);

        return ok();
      }

      template <typename String>
      Result<_, String> write(const FormatOptions& options, const void* value) {
        FixedString<> prefix;
        FixedString<> buffer;
        auto first = buffer.data();
        auto last  = first + buffer.size();
        std::to_chars_result result;
        switch (options.type) {
          case 'p':
            prefix = "0x";
            result = std::to_chars(
              first, last, reinterpret_cast<uintptr_t>(value), 16);
            break;
          default:
            result.ptr = first;
        }

        if (result.ec != std::errc{}) {
          return error("Failed to format.");
        }

        write(prefix, first, result);

        return ok();
      }

      void write(const FixedString<>& prefix,
                 const char* first,
                 const std::to_chars_result& result) {
        for (const auto c : prefix) {
          *out = c;
          ++out;
        }
        for (auto ptr = first; ptr < result.ptr; ++ptr) {
          *out = *ptr;
          ++out;
        }
      }
    };
  } // namespace internal::format

  template <typename String, typename... Args>
  constexpr Result<String, String> format(const std::string_view fmt,
                                          const Args&... args) {
    String str;
    std::back_insert_iterator itr(str);
    internal::format::Parser parser(std::move(itr), fmt, args...);
    auto result = parser.template parse<String>();
    if (!result) {
      return error(result.unwrapErr());
    }
    return ok(str);
  }
} // namespace orfos::kernel::lib

#endif // ORFOS_KERNEL_LIB_FORMAT_H_