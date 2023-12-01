#include <sstream>

#include <catch2/catch_test_macros.hpp>

#include <toml.hpp>

#include <mehawk/toml_config/config.hpp>

struct TestSection
{
	int id = -1;
	std::string name = "test default";

	auto from_toml(toml::value const& config) -> void
	{
		id = toml::find_or(config, "id", id);
		name = toml::find_or(config, "name", name);
	}

	[[nodiscard]] auto to_toml() const -> toml::value
	{
		return toml::value {
			{ "id", id },
			{ "name", name },
		};
	}

	auto operator==(TestSection const&) const noexcept -> bool = default;
};

struct TestConfigSchema
{
	int some_value = 15;
	std::string some_string = "test test";

	TestSection test = {};

	auto operator==(TestConfigSchema const&) const noexcept -> bool = default;

	auto override_from_toml(toml::value const& config) -> void
	{
		some_value = toml::find_or(config, "some_value", some_value);
		some_string = toml::find_or(config, "some_string", some_string);

		test = toml::find_or<TestSection>(config, "test", std::move(test));
	}

	[[nodiscard]] auto to_toml() const -> toml::value
	{
		return toml::value {
			{ "some_value", some_value },
			{ "some_string", some_string },
			{ "test", test.to_toml() },
		};
	}
};

using Config = TestConfigSchema;

TEST_CASE("Empty config returns defaults", "[config]")
{
	auto stream = std::stringstream();

	auto defaults = TestConfigSchema {};

	auto config = TomlConfig(stream, defaults);

	REQUIRE(defaults == config);
}

TEST_CASE("Config is read properly", "[config]")
{
	auto stream = std::stringstream(R"toml(
some_value = 10
some_string = "hello there :)"

[test]
id = 1
name = "hello"
	)toml");

	auto defaults = Config {};

	auto config = TomlConfig<Config>(stream);

	REQUIRE(config->some_value == 10);
	REQUIRE(config->some_string == "hello there :)");

	REQUIRE(config->test.id == 1);
	REQUIRE(config->test.name == "hello");
}

TEST_CASE("Config is written properly", "[config]")
{
	auto config = TomlConfig<Config>();

	auto stream = std::ostringstream();
	config.write(stream);

	auto const expected = std::stringstream(R"toml(some_string = "test test"
some_value = 15

[test]
name = "test default"
id = -1

)toml");

	REQUIRE(stream.str() == expected.str());
}
