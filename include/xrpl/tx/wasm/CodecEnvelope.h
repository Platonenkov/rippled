#pragma once

#include <cstdint>

namespace xrpl::wasm {

using CodecTypeCode = std::uint16_t;
using CodecVersion = std::uint8_t;
using CodecLength = std::uint32_t;

enum class CodecError : std::uint8_t {
    Ok = 0,
    UnknownType = 1,                 // no type code registered for the type
    UnsupportedVersion = 2,          // The envelope's version has no registered codec
    TypeMismatch = 3,                // When decoding, the codec called does not match the type of the envelope
    Truncated = 4,                   // payload size != fixed payload size
    NotRepresentable = 5,            // when encoding, the value doesn't fit within the requested version
    TypeNotInRevision = 6,           // There is no codec for the type in the requested version
    UnknownCodecVersion = 7,         // The codec version is not registered
};

// Defines the header of the binary payload for each parameter based through the host functions
struct CodecEnvelopeHeader {
    CodecTypeCode type_code;
    CodecVersion version;
    CodecLength payload_size;  // I would prefer a variable sized integer, will need to revisit this part.
};

// Defines the binary payload for each parameter based through the host functions
struct CodecEnvelope {
    CodecEnvelopeHeader header;
    std::uint8_t const* payload; // Unclear if this can be a pointer into the wasm payload or a vector holding a copy.
};

} // namespace xrpl::wasm