#include "CodecTypes.h"

#include <xrpl/protocol/Serializer.h>

#include <cstdint>
#include <limits>

// ───────────────────────────────────────────────────────────────────────────
// Codec bodies for Number. One decode/encode pair per registered version.
// (GLOB_RECURSE compiles this file automatically; it is wired into the dispatch
// tables by the explicit instantiations in CodecRegistry.cpp.)
// ───────────────────────────────────────────────────────────────────────────

namespace xrpl::wasm::detail {

// ── Number, version 0 : int64 mantissa + int32 exponent = 12 bytes ──────────
// Layout deliberately matches the frozen on-ledger STNumber serialization.

template <>
Expected<Number, CodecError>
CodecTypeTrait<codecTypeCodeV<Number>, 0>::decode(CodecEnvelope const& env)
{
    if (env.header.payload_size != kPayloadSize)
    {
        return Unexpected{CodecError::Truncated};
    }

    auto sit = SerialIter{env.payload, env.header.payload_size};
    auto const mantissa = sit.geti64();
    auto const exponent = sit.geti32();
    return Number{mantissa, exponent};
}

template <>
Expected<CodecEnvelope, CodecError>
CodecTypeTrait<codecTypeCodeV<Number>, 0>::encode(Number const& value)
{
    // v0 carries an int64 mantissa; Number's mantissa is int64 today so it
    // always fits. The guard documents the boundary for a future wider Number
    // (the "graceful fail" path when a value can't be represented in v0).
    if (value.exponent() < std::numeric_limits<std::int32_t>::min() ||
        value.exponent() > std::numeric_limits<std::int32_t>::max())
    {
        return Unexpected{CodecError::NotRepresentable};
    }

    // TODO(payload-ownership): the returned envelope's payload must reference
    // storage that outlives the call; that ownership belongs to the wire layer.
    // The encode signature is intentionally left as-is for now, so the actual
    // byte production is deferred there.
    return CodecEnvelope{.header={.type_code=kCodecTypeCode, .version=kVersion, .payload_size=kPayloadSize}, .payload=nullptr};
}

}  // namespace xrpl::wasm::detail
