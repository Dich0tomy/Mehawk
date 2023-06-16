#pragma once

#include <unordered_map>
#include <string_view>
#include <filesystem>
#include <optional>
#include <utility>

namespace hm {

class Env {
public:
  Env(std::filesystem::path filename);

  auto operator[](std::string name) const -> std::optional<std::string_view>;

private:
  auto split_env_line(std::string line) -> std::pair<std::string, std::string>;

  std::unordered_map<std::string, std::string> keys;
};

} // namespace hm
