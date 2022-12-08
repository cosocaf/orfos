#include "device_tree.h"

#include <lib/align.h>
#include <lib/endian.h>
#include <lib/libc/string.h>

#include <charconv>

namespace orfos::kernel::driver {
  StringList::StringList(const char* strs, size_t length)
    : begin(strs), strs(strs), length(length) {}
  const char* StringList::next() {
    if (static_cast<size_t>(strs - begin) >= length) {
      return nullptr;
    }
    auto str = strs;
    strs += strlen(strs) + 1;
    return str;
  }

  DeviceTreeProperty::DeviceTreeProperty(const char* name, uint32_t length, uintptr_t value)
    : name(name), length(length), value(value) {}

  const decltype(DeviceTreeProperty::name)& DeviceTreeProperty::getName() const {
    return name;
  }
  uint32_t DeviceTreeProperty::getValueAsU32() const {
    return lib::fromBigEndian(*reinterpret_cast<uint32_t*>(value));
  }
  uint64_t DeviceTreeProperty::getValueAsU64() const {
    return lib::fromBigEndian(*reinterpret_cast<uint64_t*>(value));
  }
  const char* DeviceTreeProperty::getValueAsString() const {
    return reinterpret_cast<const char*>(value);
  }
  phandle_t DeviceTreeProperty::getValueAsPhandle() const {
    return getValueAsU32();
  }
  StringList DeviceTreeProperty::getValueAsStringList() const {
    return StringList(getValueAsString(), length);
  }
  uintptr_t DeviceTreeProperty::getValueAsAddress() const {
    return value;
  }
  size_t DeviceTreeProperty::getSize() const {
    return length;
  }

  DeviceTreeNode::DeviceTreeNode(const DeviceTreeStructure* beginStruct,
                                 std::span<char> stringsBlock)
    : beginStruct(beginStruct), stringsBlock(stringsBlock) {
    auto name = beginStruct->beginNode.unitName;
    auto end  = name + strlen(name);
    auto pos  = std::find(name, end, '@');
    this->name.append(name, pos);
    if (pos != end) {
      ++pos;
      uintptr_t addr;
      if (auto [ptr, ec] = std::from_chars(pos, end, addr, 16); ec == std::errc{}) {
        unitAddress = addr;
      }
    }
  }

  const decltype(DeviceTreeNode::name)& DeviceTreeNode::getName() const {
    return name;
  }
  std::optional<uintptr_t> DeviceTreeNode::getUnitAddress() const {
    return unitAddress;
  }

  std::optional<DeviceTreeNode> DeviceTreeNode::findNode(const char* name, bool full) const {
    if (full) {
      auto end = name + strlen(name);
      auto pos = std::find(name, name + strlen(name), '@');
      if (this->name.size() == static_cast<size_t>(pos - name)
          && std::equal(this->name.begin(), this->name.end(), name)) {
        ++pos;
        uintptr_t addr;
        if (auto [ptr, ec] = std::from_chars(pos, end, addr, 16);
            ec == std::errc{} && unitAddress == addr) {
          return *this;
        }
      }
    } else if (this->name == name) {
      return *this;
    }

    for (auto&& child : *this) {
      if (auto found = child.findNode(name, full)) {
        return found;
      }
    }

    return std::nullopt;
  }
  std::optional<DeviceTreeNode> DeviceTreeNode::findNode(phandle_t phandle) const {
    if (auto phandleProp = findProperty("phandle")) {
      if (phandleProp->getValueAsPhandle() == phandle) {
        return *this;
      }
    }

    for (auto&& child : *this) {
      if (auto found = child.findNode(phandle)) {
        return found;
      }
    }

    return std::nullopt;
  }

  std::optional<DeviceTreeProperty> DeviceTreeNode::findProperty(const char* name) const {
    kernelAssert(lib::fromBigEndian(beginStruct->token) == DeviceTreeStructureToken::BeginNode);

    auto cur  = next(beginStruct);
    int depth = 0;
    while (true) {
      switch (lib::fromBigEndian(cur->token)) {
        case DeviceTreeStructureToken::BeginNode:
          ++depth;
          break;
        case DeviceTreeStructureToken::EndNode:
          --depth;
          if (depth == 0) {
            return std::nullopt;
          }
          break;
        case DeviceTreeStructureToken::Prop:
          if (depth == 0) {
            auto offset = lib::fromBigEndian(cur->prop.offsetOfName);
            if (strcmp(&stringsBlock[offset], name) == 0) {
              return DeviceTreeProperty(&stringsBlock[offset],
                                        lib::fromBigEndian(cur->prop.length),
                                        reinterpret_cast<uintptr_t>(&cur->prop.value));
            }
          }
          break;
        case DeviceTreeStructureToken::Nop:
          break;
        case DeviceTreeStructureToken::End:
          return std::nullopt;
      }
      cur = next(cur);
    }
  }

  DeviceTreeNode::Iterator DeviceTreeNode::begin() const {
    kernelAssert(lib::fromBigEndian(beginStruct->token) == DeviceTreeStructureToken::BeginNode);
    auto cur = next(beginStruct);
    while (true) {
      switch (lib::fromBigEndian(cur->token)) {
        case DeviceTreeStructureToken::Prop:
        case DeviceTreeStructureToken::Nop:
          break;
        default:
          return Iterator(cur, stringsBlock);
      }
      cur = next(cur);
    }
  }
  DeviceTreeNode::Iterator DeviceTreeNode::end() const {
    kernelAssert(lib::fromBigEndian(beginStruct->token) == DeviceTreeStructureToken::BeginNode);
    auto cur  = next(beginStruct);
    int depth = 1;
    while (true) {
      switch (lib::fromBigEndian(cur->token)) {
        case DeviceTreeStructureToken::BeginNode:
          ++depth;
          break;
        case DeviceTreeStructureToken::EndNode:
          --depth;
          if (depth == 0) {
            return Iterator(cur, stringsBlock);
          }
          break;
        case DeviceTreeStructureToken::End:
          return Iterator(cur, stringsBlock);
        default:
          break;
      }
      cur = next(cur);
    }
  }

  const DeviceTreeStructure* DeviceTreeNode::next(const DeviceTreeStructure* cur) {
    auto addr = reinterpret_cast<uintptr_t>(cur);
    switch (lib::fromBigEndian(cur->token)) {
      case DeviceTreeStructureToken::BeginNode:
        addr += sizeof(cur->beginNode.token);
        addr += strlen(cur->beginNode.unitName);
        addr += 1;
        break;
      case DeviceTreeStructureToken::EndNode:
        addr += sizeof(cur->endNode);
        break;
      case DeviceTreeStructureToken::Prop:
        addr += sizeof(cur->prop.token);
        addr += sizeof(cur->prop.length);
        addr += sizeof(cur->prop.offsetOfName);
        addr += lib::fromBigEndian(cur->prop.length);
        break;
      case DeviceTreeStructureToken::Nop:
        addr += sizeof(cur->nop);
        break;
      case DeviceTreeStructureToken::End:
        addr += sizeof(cur->end);
        break;
    }
    addr = lib::align(addr, sizeof(uint32_t));
    return reinterpret_cast<const DeviceTreeStructure*>(addr);
  }

  DeviceTreeNode::Iterator::Iterator(const DeviceTreeStructure* beginStruct,
                                     std::span<char> stringsBlock)
    : beginStruct(beginStruct), stringsBlock(stringsBlock) {}

  DeviceTreeNode::Iterator& DeviceTreeNode::Iterator::operator++() {
    kernelAssert(lib::fromBigEndian(beginStruct->token) == DeviceTreeStructureToken::BeginNode);

    int depth = 0;
    while (true) {
      beginStruct = next(beginStruct);
      switch (lib::fromBigEndian(beginStruct->token)) {
        case DeviceTreeStructureToken::BeginNode:
          ++depth;
          break;
        case DeviceTreeStructureToken::EndNode:
        case DeviceTreeStructureToken::End:
          if (depth == 0) {
            beginStruct = next(beginStruct);
            return *this;
          }
          --depth;
          break;
        default:
          break;
      }
    }
  }
  DeviceTreeNode DeviceTreeNode::Iterator::operator*() const {
    kernelAssert(lib::fromBigEndian(beginStruct->token) == DeviceTreeStructureToken::BeginNode);
    return DeviceTreeNode(beginStruct, stringsBlock);
  }
  bool DeviceTreeNode::Iterator::operator==(const Iterator& other) const {
    return beginStruct == other.beginStruct;
  }

  DeviceTree::DeviceTree(std::span<DeviceTreeReserveEntry> memoryReservationBlock,
                         std::span<DeviceTreeStructure> structureBlock,
                         std::span<char> stringsBlock)
    : memoryReservationBlock(memoryReservationBlock),
      structureBlock(structureBlock),
      stringsBlock(stringsBlock),
      rootNode(structureBlock.data(), stringsBlock) {}

  std::optional<DeviceTreeNode> DeviceTree::findNode(const char* path) const {
    kernelAssert(*path == '/');

    auto end = path + strlen(path);
    ++path;
    std::optional<DeviceTreeNode> node = rootNode;
    while (path < end) {
      auto pos = std::find(path, end, '/');
      if (pos - path >= 32) {
        return std::nullopt;
      }
      lib::FixedString<31> name(path, pos - path);
      bool full = pos == end && std::find(path, end, '@') != end;
      if (auto found = node->findNode(name.c_str(), full)) {
        node = found;
      } else {
        return std::nullopt;
      }
      path = pos + 1;
    }
    return node;
  }
  std::optional<DeviceTreeNode> DeviceTree::findNode(const char* name, bool full) const {
    return rootNode.findNode(name, full);
  }
  std::optional<DeviceTreeNode> DeviceTree::findNode(phandle_t phandle) const {
    return rootNode.findNode(phandle);
  }
} // namespace orfos::kernel::driver