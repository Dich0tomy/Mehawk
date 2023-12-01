#pragma once
// Ideally this would be PCH but clangd no likey

#include <concepts>
#include <stdexcept>
#include <assert.hpp>

/// Stops the application, useful for optimization and warning silencing in cases in which the compiler is trippin
[[noreturn]] inline auto unreachable() noexcept -> void
{
#ifdef __GNUC__ // GCC, Clang, ICC
	__builtin_unreachable();
#elif defned(_MSC_VER)
	__assume(false);
#endif
}

/// Stops the application, and signals that a certain path is not implemented.
[[noreturn]] inline auto unimplemented() noexcept -> void
{
	// TODO: figure out how to instruct clang-tidy to nolint all of these macros
	ASSERT(false, "Unimplemented."); // NOLINT
	unreachable();
}

namespace impl
{

template<typename Type, auto Distinct>
class DistinctType
{
public:
	constexpr explicit DistinctType() = default;

	template<typename T>
		requires(not std::same_as<T, DistinctType>)
	constexpr explicit DistinctType(T&& data) // NOLINT
		: data(static_cast<T&&>(data))
	{}

	constexpr explicit operator decltype(auto)() & noexcept { return data; }

	constexpr explicit operator decltype(auto)() && noexcept { return std::move(data); }

	constexpr explicit operator decltype(auto)() const& noexcept { return data; }

	constexpr explicit operator decltype(auto)() const&& noexcept { return std::move(data); }

private:
	Type data;
};

} // namespace impl

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define MH_TRAP(...) (ASSERT(false, "This should never happen.\n" #__VA_ARGS__), unreachable())
#define DISTINCT(Type) impl::DistinctType<Type, [] {}>
// NOLINTEND(cppcoreguidelines-macro-usage)
