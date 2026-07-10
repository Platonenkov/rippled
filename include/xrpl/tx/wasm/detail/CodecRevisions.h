#pragma once

#include <xrpl/basics/Expected.h>
#include <xrpl/tx/wasm/CodecEnvelope.h>

namespace xrpl::wasm::detail {

// Maps a contract's declared codec revision to the per-type wire version to
// emit for `code`. This is the revision/amendment POLICY layer, kept separate
// from both the generic dispatch machinery and the concrete value codecs.
//
// Defined in src/libxrpl/tx/wasm/codecs/CodecRevisions.cpp alongside the
// concrete CodecTrait<N> revision maps. (Amendment gating via Rules is a
// deliberate TODO there — see the .cpp.)
Expected<CodecVersion, CodecError>
wireVersionFor(CodecVersion declaredRevision, CodecTypeCode code);

}  // namespace xrpl::wasm::detail
