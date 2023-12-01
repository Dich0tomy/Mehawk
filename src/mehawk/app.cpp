#include <fstream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

#include <magic_enum.hpp>

#include <mehawk/toml_config/config.hpp>
#include <mehawk/enum_formatter.hpp>
#include <mehawk/standard_paths.hpp>
#include <mehawk/static_config.hpp>
#include <mehawk/app.hpp>

enum class ShouldLogToFile
{
	Yes,
	No
};

auto setup_logs(auto const& log_path, ShouldLogToFile should_log_to_file) -> void;

auto DebugConfigSection::from_toml(toml::value const& toml_config) -> void
{
	log_to_file = toml::find_or(toml_config, "log_to_file", log_to_file);
	log_to_console = toml::find_or(toml_config, "log_to_file", log_to_file);
}

auto DebugConfigSection::to_toml() const -> toml::value
{
	return toml::value {
		{ "log_to_file", log_to_file },
		{ "log_to_console", log_to_console },

		{ "file_log_level", magic_enum::enum_name(file_log_level) },
		{ "console_log_level", magic_enum::enum_name(console_log_level) },
	};
}

auto AppConfig::override_from_toml(toml::value const& toml_config) -> void
{
	debug = toml::find_or(toml_config, "debug", debug);
}

auto AppConfig::to_toml() const -> toml::value
{
	return toml::value {
		{ "debug", debug.to_toml() }
	};
}

auto App::run(int argc, char** argv) -> int
{
	spdlog::set_pattern(static_config::global_logger_pattern());

	auto const standard_paths = StandardPaths::get(StandardPaths::IncludeAppFolder);
	if(not standard_paths) {
		SPDLOG_ERROR(R"(Cannot read standard paths! "{}".)", magic_enum::enum_name(standard_paths.error()));
		return -1;
	}

	auto config_path = standard_paths->config;

	// TODO: actually use the config when setting up logs
	read_config(config_path);

	return 0;
}

auto App::read_config(std::filesystem::path const& config_path) -> void
{
	namespace fs = std::filesystem;

	auto const config_file_path = config_path / static_config::config_filename();
	if(fs::exists(config_file_path)) {
		try {
			auto config_file = std::ifstream(config_file_path);
			app_config = TomlConfig(config_file, AppConfig());
		} catch(std::exception const& ex) {
			unimplemented();
		}

		return;
	}

	fs::create_directories(config_path);
	auto config_file = std::ofstream(config_file_path);
	app_config.write(config_file);
}

namespace
{

auto setup_logs(auto const& log_path, ShouldLogToFile should_log_to_file) -> void
{
	try {
		auto sinks = std::vector<spdlog::sink_ptr>();

		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::debug);
		sinks.push_back(std::move(console_sink));

		if(should_log_to_file == ShouldLogToFile::Yes) {
			auto constexpr rotation_hour = 23;
			auto constexpr rotation_minute = 59;
			auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
				static_cast<std::filesystem::path>(log_path),
				rotation_hour,
				rotation_minute
			);

			daily_sink->set_level(spdlog::level::info);
			sinks.push_back(std::move(daily_sink));
		}

		spdlog::set_default_logger(
			std::make_unique<spdlog::logger>(
				"console_and_daily",
				std::make_move_iterator(sinks.begin()),
				std::make_move_iterator(sinks.end())
			)
		);
	} catch(spdlog::spdlog_ex const& ex) {
		SPDLOG_ERROR(R"X(Couldn't retrieve user log path. "{}")X", ex.what());
		return;
	}

	spdlog::set_pattern(static_config::global_logger_pattern());
}

} // namespace
