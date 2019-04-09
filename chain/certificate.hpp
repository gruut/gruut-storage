#ifndef GRUUT_PUBLIC_MERGER_CERTIFICATE_HPP
#define GRUUT_PUBLIC_MERGER_CERTIFICATE_HPP

#include "types.hpp"

namespace gruut {
    class Certificate {
    public:
        base58_type cert_id;
        string cert_content;

        Certificate() = default;
        Certificate(base58_type &&cert_id, string &&cert_content)
                : cert_id(cert_id), cert_content(cert_content) {}
        Certificate(base58_type &cert_id, string &cert_content)
                : cert_id(cert_id), cert_content(cert_content) {}
    };
} // namespace gruut
#endif