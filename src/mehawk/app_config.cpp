#include <filesystem>
#include <istream>
#include <sstream>

#include <spdlog/spdlog.h>

#include <mehawk/app_config.hpp>

auto AppConfig::write(std::ostream& config_stream) -> void
{
	config_stream << serialize_to_toml();
}

auto AppConfig::read(std::istream& config_stream, AppConfig defaults) -> AppConfig
{
	auto config_content = std::string(std::istreambuf_iterator<char>(config_stream), {});

	SPDLOG_INFO("The config ffs:\n```toml\n{}```", config_content);

	auto config = toml::parse(std::move(config_content));

	return AppConfig {
		.log_to_file = config["app"]["log_to_file"].value_or(static_cast<bool>(defaults.log_to_file)),
	};
}

auto AppConfig::serialize_to_toml() -> toml::table
{
	// clang-format off
	return toml::table
	{
		{
			"app",
			toml::table
			{
				{ "log_to_file", static_cast<bool>(log_to_file) }
			}
		},
	};
	// clang-format on
}
