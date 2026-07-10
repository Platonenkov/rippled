#pragma once

// 
// Internal aggregation header for the wasm codec registry (NOT public — lives
// under src/). Expands codec_types.macro to declare, in one shared place:
//
//   * CODEC_TYPE     -> the C++ type -> CodecTypeCode mapping
//   * CODEC_VERSION  -> each (type, version) CodecTypeTrait specialization
//
// Both the registry TU and every per-type codec .cpp include this, so the trait
// declarations they share are always generated from the same .def and can never
// drift.
// 

#include <xrpl/basics/Expected.h>  // used by the trait macro's decode/encode
#include <xrpl/tx/wasm/detail/CodecTypeCode.h>
#include <xrpl/tx/wasm/detail/CodecTypeTrait.h>

// One include per registered C++ *type* (not per version):
#include <xrpl/basics/Number.h>
#include <xrpl/protocol/AccountID.h>

namespace xrpl::wasm::detail {

// type -> type code
#define CODEC_TYPE(CppType, TypeCode) XRPL_WASM_CODEC_TYPE_CODE(CppType, TypeCode)
#include <xrpl/tx/wasm/detail/codec_types.macro>

// per-(type, version) trait declarations (bodies live in the type's .cpp)
#define CODEC_VERSION(CppType, Version, PayloadSize) \
    XRPL_WASM_CODEC_TYPE_TRAIT(CppType, Version, PayloadSize)
#include <xrpl/tx/wasm/detail/codec_types.macro>

}  // namespace xrpl::wasm::detail
