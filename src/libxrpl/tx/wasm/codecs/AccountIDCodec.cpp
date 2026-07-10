#include "CodecTypes.h"

#include <cstring>

// ───────────────────────────────────────────────────────────────────────────
// Codec bodies for AccountID. One decode/encode pair per registered version.
// ───────────────────────────────────────────────────────────────────────────

namespace xrpl::wasm::detail {

// ── AccountID, version 0 : 20 raw bytes ─────────────────────────────────────

template <>
Expected<AccountID, CodecError>
CodecTypeTrait<codecTypeCodeV<AccountID>, 0>::decode(CodecEnvelope const& env)
{
    if (env.header.payload_size != kPayloadSize)
    {
        return Unexpected{CodecError::Truncated};
    }
    
    auto id = AccountID{};
    std::memcpy(id.data(), env.payload, id.size());
    return id;
}

template <>
Expected<CodecEnvelope, CodecError>
CodecTypeTrait<codecTypeCodeV<AccountID>, 0>::encode(AccountID const& value)
{
    // AccountID is fixed-width, so it always fits v0 (no NotRepresentable path).
    // TODO(payload-ownership): payload bytes (value.data(), value.size()) must
    // be owned by the wire layer; deferred per the current encode signature.
    (void)value;
    return CodecEnvelope{.header={.type_code=kCodecTypeCode, .version=kVersion, .payload_size=kPayloadSize}, .payload=nullptr};
}

}  // namespace xrpl::wasm::detail
