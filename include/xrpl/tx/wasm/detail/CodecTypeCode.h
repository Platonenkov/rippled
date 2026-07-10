#pragma once

#include <xrpl/tx/wasm/CodecEnvelope.h>

namespace xrpl::wasm::detail {

template <typename T>
struct CodecTypeCodeOf;

// Defines a type trait for registering a type code for each
// type in the codec.
template <typename T>
inline constexpr CodecTypeCode codecTypeCodeV = CodecTypeCodeOf<T>::kCodecTypeCode;

#define XRPL_WASM_CODEC_TYPE_CODE(CppType, Code) \
    template <> \
    struct CodecTypeCodeOf<CppType> { \
        static constexpr CodecTypeCode kCodecTypeCode = Code; \
    };

} // namespace xrpl::wasm::detail