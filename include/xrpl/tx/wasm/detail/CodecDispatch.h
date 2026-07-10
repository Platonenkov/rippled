#pragma once

#include <xrpl/basics/Expected.h>
#include <xrpl/tx/wasm/CodecEnvelope.h>
#include <xrpl/tx/wasm/detail/CodecTypeCode.h>
#include <xrpl/tx/wasm/detail/CodecTypeTrait.h>

#include <array>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace xrpl::wasm::detail {

// Sentinel that lets the registry's X-macro expansion end with a trailing
// comma:  CodecList<Trait0, Trait1, CodecEnd>. Its `void` CodecType never
// matches a real type, so every table builder simply skips it.
struct CodecEnd
{
    using CodecType = void;
};

template <typename Trait, typename T>
inline constexpr bool traitIsV = std::is_same_v<typename Trait::CodecType, T>;

template <typename T>
using DecodeFn = Expected<T, CodecError> (*)(CodecEnvelope const&);

template <typename T>
using EncodeFn = Expected<CodecEnvelope, CodecError> (*)(T const&);

// Highest registered version of T within List. Unregistered T ⇒ build error.
template <typename T, typename List, std::size_t... I>
consteval CodecVersion
maxVersionImpl(std::index_sequence<I...>)
{
    auto v = CodecVersion{};
    auto found = false;
    ([&] {
        using Trait = std::tuple_element_t<I, List>;
        if constexpr (traitIsV<Trait, T>)
        {
            found = true;
            v = std::max(v, Trait::kVersion);
        }
    }(), ...);
    if (!found) 
    {
        xrpl::Throw<std::runtime_error>("wasm codec: no codec registered for this type");
    }
    return v;
}

template <typename T, typename List>
consteval CodecVersion
maxVersion()
{
    return maxVersionImpl<T, List>(
        std::make_index_sequence<std::tuple_size_v<List>>{});
}

template <typename T, typename List, std::size_t... I>
consteval auto
makeDecoderTableImpl(std::index_sequence<I...>)
{
    constexpr auto n = maxVersion<T, List>() + 1;
    auto table = std::array<DecodeFn<T>, n>{};
    ([&] {
        using Trait = std::tuple_element_t<I, List>;
        if constexpr (traitIsV<Trait, T>)
        {
            table[Trait::kVersion] = &Trait::decode;
        }
    }(), ...);
    for (const auto& entry : table) 
    {
        if (!entry) 
        {
            xrpl::Throw<std::runtime_error>("wasm codec: decoder versions must be contiguous from 0");
        }
    }
    return table;
}

template <typename T, typename List>
inline constexpr auto decoderTableV = makeDecoderTableImpl<T, List>(
    std::make_index_sequence<std::tuple_size_v<List>>{});

template <typename T, typename List, std::size_t... I>
consteval auto
makeEncoderTableImpl(std::index_sequence<I...>)
{
    constexpr auto n = maxVersion<T, List>() + 1;
    auto table = std::array<EncodeFn<T>, n>{};
    ([&] {
        using Trait = std::tuple_element_t<I, List>;
        if constexpr (traitIsV<Trait, T>) 
        {
            table[Trait::kVersion] = &Trait::encode;
        }
    }(), ...);
    for (const auto& entry : table) 
    {
        if (!entry) 
        {
            xrpl::Throw<std::runtime_error>("wasm codec: encoder versions must be contiguous from 0");
        }
    }
    return table;
}

template <typename T, typename List>
inline constexpr auto encoderTableV = makeEncoderTableImpl<T, List>(
    std::make_index_sequence<std::tuple_size_v<List>>{});

template <typename T, typename List>
Expected<T, CodecError>
decodeImpl(CodecEnvelope const& env)
{
    if (env.header.type_code != codecTypeCodeV<T>)
    {
        return Unexpected{CodecError::TypeMismatch};
    }
    auto const& table = decoderTableV<T, List>;
    if (env.header.version >= table.size()) 
    {
        return Unexpected{CodecError::UnsupportedVersion};
    }
    return table[env.header.version](env);
}

template <typename T, typename List>
Expected<CodecEnvelope, CodecError>
encodeAt(CodecVersion wireVersion, T const& value)
{
    auto const& table = encoderTableV<T, List>;
    if (wireVersion >= table.size()) 
    {
        return Unexpected{CodecError::UnsupportedVersion};
    }
    return table[wireVersion](value);
}

}  // namespace xrpl::wasm::detail
