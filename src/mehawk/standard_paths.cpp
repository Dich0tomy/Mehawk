#include <unordered_map>
#include <string_view>
#include <filesystem>
#include <algorithm>
#include <utility>
#include <string>
#include <ranges>

#include <spdlog/spdlog.h>

#include <tl/expected.hpp>
#include <tl/optional.hpp>

#include <mehawk/standard_paths.hpp>
#include <mehawk/static_config.hpp>
#include <mehawk/os_detection.hpp>
#include <mehawk/prelude.hpp>

namespace
{

using PathType = std::filesystem::path;
using HomeDirResult = tl::optional<PathType>;

auto safe_getenv(char const* name) -> HomeDirResult
{
	auto* const result = std::getenv(name);

	if(result) return { result };

	return {};
}

#ifdef OS_LINUX

	#include <unistd.h>
	#include <pwd.h>

// NOTE: This function is not reentrant due to using getpwnam inside (which means it cannot be called in a signal and it's not thread safe)
auto get_home() -> PathType
{
	static auto const home = safe_getenv("HOME").or_else([]() -> HomeDirResult {
		auto const uid = getuid();
		auto* const passwd = getpwuid(uid);

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

	static auto const home = get_home();

	static auto const config_path = safe_getenv("XDG_CONFIG_HOME").value_or(fs::path(home) / ".config");
	static auto const data_path = safe_getenv("XDG_DATA_HOME").value_or(fs::path(home) / ".local/share");
	static auto const cache_path = safe_getenv("XDG_CACHE_HOME").value_or(fs::path(home) / ".cache");

	static auto const paths = StandardPaths::Paths {
		.config = config_path,
		.data = data_path,
		.cache = cache_path,
	};

	return paths;
}

#elif defined(OS_MAC)
auto get_mac_standard_paths() -> hm::StandardPaths::GetResult
{
	namespace fs = std::filesystem;

	static auto const home = get_home();

	static auto const config_path = fs::path(home) / "Library/Preferences";
	static auto const data_path = fs::path(home) / "Library/Application Support";
	static auto const cache_path = fs::path(home) / "Library/Caches";

	static auto const paths = StandardPaths::Paths {
		.config = config_path,
		.data = data_path,
		.cache = cache_path,
	};

	return paths;
}

#elif defined(OS_WINDOWS)
auto get_windows_standard_paths() -> hm::StandardPaths::GetResult
{
	unimplemented();
}
#endif

} // namespace

auto StandardPaths::get(IncludeAppFolderTag const tag) -> GetResult
{
	auto paths =
#ifdef OS_LINUX
		get_linux_standard_paths();
#elif defined(OS_MAC)
		get_mac_standard_paths();
#else
		get_windows_standard_paths();
#endif

	if(tag != IncludeAppFolder) {
		return paths;
	}

	return paths.map([](Paths const& paths) {
		auto const app_folder = static_config::app_name() + "/";

		return StandardPaths::Paths {
			.config = paths.config / app_folder,
			.data = paths.data / app_folder,
			.cache = paths.cache / app_folder
		};
	});
}
