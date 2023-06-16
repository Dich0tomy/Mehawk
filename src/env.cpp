#include "haumea/env.hpp"

#include <unordered_map>
#include <string_view>
#include <filesystem>
#include <optional>
#include <fstream>
#include <utility>

namespace hm {

Env::Env(std::filesystem::path filename) {
  auto file = std::ifstream(filename.string());

  {
    auto line = std::string();
    while(std::getline(file, line)) {
      auto const [name, value] = split_env_line(std::move(line));

      keys.try_emplace(name, value);
    }
  }
}

auto Env::operator[](std::string name) const -> std::optional<std::string_view> {
  auto const it = keys.find(name);

  if(it == keys.cend()) return std::nullopt;

  return { it->second };
}

auto Env::split_env_line(std::string line) -> std::pair<std::string, std::string> {
  auto const pivot_pos = line.find('=');

  return std::pair(
    line.substr(0, pivot_pos),
    line.substr(pivot_pos + 1)
  );
}

} // namespace hm
