#ifndef GRUUT_ENTERPRISE_MERGER_SIGNATURE_HPP
#define GRUUT_ENTERPRISE_MERGER_SIGNATURE_HPP

#include "types.hpp"

namespace gruut {
    class Signature {
    public:
        Signature() = default;
        Signature(base58_type &&signer_id_, base64_type &&signer_signature_)
                : signer_id(signer_id_), signer_signature(signer_signature_) {}
        Signature(base58_type &signer_id_, base64_type &signer_signature_)
                : signer_id(signer_id_), signer_signature(signer_signature_) {}
        base58_type signer_id;
        base64_type signer_signature;
        block_height_type height;
    };
} // namespace gruut
#endif