#pragma once

#include <string_view>
#include <filesystem>
#include <algorithm>
#include <span>

#include <mehawk/toml_config/concepts.hpp>
#include <mehawk/toml_config/config.hpp>

#include <spdlog/logger.h>

struct DebugConfigSection
{
	bool log_to_file = true;
	bool log_to_console = true;

	spdlog::level::level_enum file_log_level = spdlog::level::info;
	spdlog::level::level_enum console_log_level = spdlog::level::info;

	auto from_toml(toml::value const& toml_config) -> void;
	[[nodiscard]] auto to_toml() const -> toml::value;
};

struct AppConfig
{
	DebugConfigSection debug;

	auto override_from_toml(toml::value const& toml_config) -> void;
	[[nodiscard]] auto to_toml() const -> toml::value;
};

static_assert(TomlSerializable<AppConfig>);

class App
{
public:
	auto run(int argc, char** argv) -> int;

private:
	auto read_config(std::filesystem::path const& config_path) -> void;

	TomlConfig<AppConfig> app_config;
};
