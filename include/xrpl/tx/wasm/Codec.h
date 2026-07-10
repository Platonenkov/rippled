#pragma once

#include <xrpl/basics/Expected.h>
#include <xrpl/tx/wasm/CodecEnvelope.h>

namespace xrpl::wasm {

// ─────────────────────────────────────────────────────────────────────────
// Public wasm codec API.
//
// This header is what host-function code includes. It intentionally contains
// NO codec machinery, NO registered-codec list, and NO concrete type
// registrations — only the two entry points. Their definitions are provided by
// explicit instantiation in src/libxrpl/tx/wasm/codecs/CodecRegistry.cpp, so
// including this header does not instantiate the (consteval) dispatch tables.
//
// A decode<T>/encode<T> for an unregistered T is a link error by design.
//
// To add a codec, see include/xrpl/tx/wasm/detail/codec_types.macro.
// ─────────────────────────────────────────────────────────────────────────

// Decode a value of the statically-known type T from a self-describing
// envelope. The envelope carries its own (type_code, version), so no revision
// context is needed here.
template <typename T>
[[nodiscard]] Expected<T, CodecError>
decode(CodecEnvelope const& env);

// Encode `value` toward a contract that declared codec revision
// `declaredRevision`. The wire version emitted is chosen by the revision policy
// (wireVersionFor); if the value cannot be represented in that version the
// result is NotRepresentable (the graceful-fail path).
template <typename T>
[[nodiscard]] Expected<CodecEnvelope, CodecError>
encode(CodecVersion declaredRevision, T const& value);

}  // namespace xrpl::wasm
