#pragma once

#include <filesystem>
#include <string_view>
#include <string>

#include <tl/expected.hpp>

#include <mehawk/os_detection.hpp>

class StandardPaths
{
public:
  enum class RetrievalError
  {
#ifndef OS_WINDOWS
    NoHomeVariable,
#endif
  };

  enum class GetOption
  {
    None,
    IncludeAppFolder
  };

  struct Paths
  {
    std::filesystem::path config;
    std::filesystem::path data;
    std::filesystem::path cache;
  };

  using GetResult = tl::expected<Paths, RetrievalError>;

  /**
   * @brief Returns a standard user-local path for host os
   *
   * @description
   * Config:
   *  - Linux: $XDG_CONFIG_HOME (defaults to $HOME/.config)
   *  - Mac: $HOME/Library/Preferences
   *  - Windows: %APPDATA%
   *
   * Data:
   *  - Linux: $XDG_DATA_HOME (defaults to $HOME/.local/share)
   *  - Mac: $Home/Library/Application Support
   *  - Windows: %APPDATA%
   *
   * Cache:
   *  - Linux: $XDG_CACHE_HOME (defaults to $HOME/.cache)
   *  - Mac: $HOME/Library/Cache
   *  - Windows: %LOCALAPPDATA%
   */
  static auto get(GetOption const options = GetOption::None) -> GetResult;
};
