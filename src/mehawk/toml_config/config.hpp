#pragma once

#include <spdlog/spdlog.h>

#include <toml.hpp>

#include <mehawk/toml_config/concepts.hpp>

#include <mehawk/prelude.hpp>

template<TomlSerializable ConfigType>
struct TomlConfig
{
public:
	TomlConfig() = default;

	TomlConfig(std::istream& config_stream, ConfigType default_config = {})
		: config(read_config_from_stream(config_stream, std::move(default_config)))
	{}

	auto write(std::ostream& out_stream) const -> void
	{
		out_stream << config.to_toml();
	}

	auto operator==(ConfigType const& other) const noexcept -> bool
	{
		return other == config;
	}

	auto operator->() const noexcept -> ConfigType const*
	{
		return &config;
	}

	auto operator*() const noexcept -> ConfigType const&
	{
		return config;
	}

private:
	static auto read_config_from_stream(std::istream& config_stream, ConfigType default_config) -> ConfigType
	{
		auto config_toml = toml::parse(config_stream);

		SPDLOG_INFO("Config content:\n```toml\n{}```", toml::format(config_toml));

		default_config.override_from_toml(std::move(config_toml));

		return default_config;
	}

	ConfigType config;
};
