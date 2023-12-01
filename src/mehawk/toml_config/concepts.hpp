#pragma once

#include <concepts>

#include <toml.hpp>

// clang-format off
template<typename T>
concept TomlSerializable = requires(T t)
{
	{ std::default_initializable<T> };
	{ t.override_from_toml(toml::value()) } -> std::same_as<void>;
	{ t.to_toml() } -> std::same_as<toml::value>;
};

// clang-format on
