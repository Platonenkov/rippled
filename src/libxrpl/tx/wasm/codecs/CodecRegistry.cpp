#include <xrpl/tx/wasm/Codec.h>
#include <xrpl/tx/wasm/detail/CodecDispatch.h>
#include <xrpl/tx/wasm/detail/CodecList.h>
#include <xrpl/tx/wasm/detail/CodecRevisions.h>

#include "CodecTypes.h"

// 
// The registry translation unit. This is the ONE place that:
//   * assembles the complete registered-codec tuple from the .def, and
//   * explicitly instantiates decode<T>/encode<T> for every registered type.
//
// Because the definitions live here (not in the public header), callers get the
// codec machinery out of their compiles, and an unregistered decode<T>/encode<T>
// fails at link time rather than with a template error.
// 

namespace xrpl::wasm {

namespace {

// The single enumeration of every registered codec, built straight from the
// .def. The trailing detail::CodecEnd absorbs the final comma of the expansion.
using RegisteredCodecs = detail::CodecList<
#define CODEC_VERSION(CppType, Version, PayloadSize) \
    detail::CodecTypeTrait<detail::codecTypeCodeV<CppType>, Version>,
#include <xrpl/tx/wasm/detail/codec_types.macro>
    detail::CodecEnd>::TupleType;

}  // namespace

template <typename T>
Expected<T, CodecError>
decode(CodecEnvelope const& env)
{
    return detail::decodeImpl<T, RegisteredCodecs>(env);
}

template <typename T>
Expected<CodecEnvelope, CodecError>
encode(CodecVersion declaredRevision, T const& value)
{
    auto const wire =
        detail::wireVersionFor(declaredRevision, detail::codecTypeCodeV<T>);
    if (!wire) 
    {
        return Unexpected{wire.error()};
    }
    return detail::encodeAt<T, RegisteredCodecs>(*wire, value);
}

// Explicit instantiation for every registered type — the closed set.
#define CODEC_TYPE(CppType, TypeCode)                                       \
    template Expected<CppType, CodecError> decode<CppType>(                 \
        CodecEnvelope const&);                                              \
    template Expected<CodecEnvelope, CodecError> encode<CppType>(           \
        CodecVersion, CppType const&);
#include <xrpl/tx/wasm/detail/codec_types.macro>

}  // namespace xrpl::wasm
