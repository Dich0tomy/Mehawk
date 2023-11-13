#include <unordered_map>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <utility>
#include <string>
#include <ranges>

#include <tl/expected.hpp>
#include <tl/optional.hpp>

#include <mehawk/standard_paths.hpp>
#include <mehawk/os_detection.hpp>

namespace
{

using HomeDirResult = tl::optional<std::string_view>;

auto safe_getenv(char const* name) -> HomeDirResult
{
  auto const result = std::getenv(name);

  if(result) return { result };
  else return {};
}

#ifdef OS_LINUX

  #include <unistd.h>
  #include <pwd.h>

// NOTE: This function is not reentrant due to using getpwnam inside (which means it cannot be called in a signal and it's not thread safe)
auto get_home() -> std::string_view
{
  static auto const home = safe_getenv("HOME").or_else([]() -> HomeDirResult {
    auto const uid = getuid();
    auto const passwd = getpwuid(uid);

    if(not passwd or not passwd->pw_dir) return {};

    return passwd->pw_dir;
  });

  return home
    .or_else([]() { throw std::runtime_error("The $HOME variable couldn't be determined."); })
    .value();
}

auto get_linux_standard_paths() -> StandardPaths::GetResult
{
  namespace fs = std::filesystem;
  using enum StandardPaths::RetrievalError;

  static auto const home = get_home();

  static auto const config_path = safe_getenv("XDG_CONFIG_HOME").conjunction(fs::path(home) / ".config");
  static auto const data_path = safe_getenv("XDG_DATA_HOME").conjunction(fs::path(home) / ".local/share");
  static auto const cache_path = safe_getenv("XDG_CACHE_HOME").conjunction(fs::path(home) / ".cache");

  return StandardPaths::Paths {
    .config = *config_path,
    .data = *data_path,
    .cache = *cache_path,
  };
}

#elif defined(OS_MAC)
auto get_mac_standard_paths() -> hm::StandardPaths::GetResult
{
  namespace fs = std::filesystem;
  using enum hm::StandardPaths::RetrievalError;

  static auto const home = get_home();

  static auto const config_path = fs::path(home) / "Library/Preferences";
  static auto const data_path = fs::path(home) / "Library/Application Support";
  static auto const cache_path = fs::path(home) / "Library/Caches";

  return { hm::StandardPaths::Paths {
    .config = config_path,
    .data = data_path,
    .cache = cache_path,
  } };
}

#elif defined(OS_WINDOWS)
auto get_windows_standard_paths(std::string_view scope) -> hm::StandardPaths::GetResult
{
  UNIMPLEMENTED
}
#endif

} // namespace

auto StandardPaths::get(Scope scope) -> GetResult
{
// NOTE: we explicitly ignore the scope for linux & mac
#ifdef OS_LINUX
  return get_linux_standard_paths();
#elif defined(OS_MAC)
  return get_mac_standard_paths();
#else
  return get_windows_standard_paths(scope);
#endif
}
