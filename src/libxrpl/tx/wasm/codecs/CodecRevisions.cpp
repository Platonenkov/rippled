#include <xrpl/tx/wasm/detail/CodecRevisions.h>
#include <xrpl/tx/wasm/detail/CodecTrait.h>

#include "CodecTypes.h"

#include <array>
#include <utility>

// ───────────────────────────────────────────────────────────────────────────
// Revision / amendment POLICY.
//
// The CodecTrait<N> revision maps and the wireVersionFor switch are BOTH
// generated from codec_revisions.macro, exactly as the (type, version)
// registry drives the dispatch tables — so the maps and the dispatch can't
// drift from the registration list.
// ───────────────────────────────────────────────────────────────────────────

namespace xrpl::wasm::detail {

// (1) Generate the CodecTrait<N> revision maps.
#define CODEC_REVISION_BEGIN(Revision)                        \
    template <>                                               \
    struct CodecTrait<Revision>                               \
    {                                                         \
        static constexpr CodecVersion kVersion = Revision;    \
        static constexpr auto versions =                      \
            std::to_array<std::pair<CodecTypeCode, CodecVersion>>({
#define CODEC_REVISION_ENTRY(CppType, Version) \
                {codecTypeCodeV<CppType>, Version},
#define CODEC_REVISION_END(Revision) \
            });                              \
    };
#include <xrpl/tx/wasm/detail/codec_revisions.macro>

namespace {

template <CodecVersion N>
Expected<CodecVersion, CodecError>
versionFromRevision(CodecTypeCode code)
{
    for (auto const& [c, v] : CodecTrait<N>::versions)
    {
        if (c == code)
        {
            return v;
        }
    }
    return Unexpected{CodecError::TypeNotInRevision};
}

}  // namespace

Expected<CodecVersion, CodecError>
wireVersionFor(CodecVersion declaredRevision, CodecTypeCode code)
{
    // TODO(amendment-gating): each revision > 0 must additionally be gated
    // behind its amendment (via Rules) before a node may emit it, so no node
    // emits a wire version its peers can't agree on. Thread Rules in here.

    // (2) Generate the runtime-revision -> CodecTrait<N> dispatch.
    switch (declaredRevision)
    {
#define CODEC_REVISION_BEGIN(Revision) \
    case Revision:                     \
        return versionFromRevision<Revision>(code);
#define CODEC_REVISION_ENTRY(CppType, Version)
#define CODEC_REVISION_END(Revision)
#include <xrpl/tx/wasm/detail/codec_revisions.macro>
        default:
            return Unexpected{CodecError::UnknownCodecVersion};
    }
}

}  // namespace xrpl::wasm::detail
