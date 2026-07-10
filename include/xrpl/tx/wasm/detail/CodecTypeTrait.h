#pragma once

#include <xrpl/tx/wasm/CodecEnvelope.h>

#include <limits>

namespace xrpl::wasm::detail {

// Defines a type trait for registering a specific type code and version 
// encoding/decoding implementation for each type in the codec.
template <CodecTypeCode TypeCode, CodecVersion Version>
struct CodecTypeTrait;

#define XRPL_WASM_CODEC_TYPE_TRAIT(CppType, Version, PayloadSize) \
    template <> \
    struct CodecTypeTrait<codecTypeCodeV<CppType>, Version> { \
        using CodecType = CppType; \
        static constexpr auto kCodecTypeCode = codecTypeCodeV<CppType>; \
        static constexpr auto kVersion = Version; \
        static constexpr auto kPayloadSize = PayloadSize; \
        static Expected<CodecType, CodecError> decode(CodecEnvelope const& env); \
        static Expected<CodecEnvelope, CodecError> encode(CodecType const& value); \
    };

inline constexpr auto kVariablePayloadSize = std::numeric_limits<CodecLength>::max();

} // namespace xrpl::wasm::detail