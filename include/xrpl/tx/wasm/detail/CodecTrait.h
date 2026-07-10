#pragma once

#include <xrpl/tx/wasm/CodecEnvelope.h>

namespace xrpl::wasm::detail {

template <CodecVersion Version>
struct CodecTrait;

// template <>
// struct CodecTrait<0> {
//     static constexpr auto kVersion = CodecVersion{0};
//     static constexpr auto versions = std::to_array<std::pair<CodecTypeCode, CodecVersion>>({
//         {codecTypeCodeV<STNumber>, 0},
//     });
// };

} // namespace xrpl::wasm::detail