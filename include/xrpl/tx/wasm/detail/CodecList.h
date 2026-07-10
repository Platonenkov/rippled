#pragma once

#include <tuple>

namespace xrpl::wasm::detail {

template <typename... Traits>
struct CodecList {
    using TupleType = std::tuple<Traits...>;
};

} // namespace xrpl::wasm::detail