#ifndef GRUUT_ENTERPRISE_MERGER_TEMPORARY_BLOCK_HPP
#define GRUUT_ENTERPRISE_MERGER_TEMPORARY_BLOCK_HPP

#include "tx_merkle_tree.hpp"
#include "signature.hpp"
#include "transaction.hpp"
#include "types.hpp"

#include "../utils/lz4_compressor.hpp"
#include "../utils/ecdsa.hpp"

#include "easy_logging.hpp"
#include "nlohmann/json.hpp"

#include <vector>

using namespace std;

namespace gruut {

    class Block {
    private:
        base58_type m_block_id;
        timestamp_t m_block_time;
        world_id_type m_world_id;
        localchain_id_type m_chain_id;
        block_height_type m_block_height;
        base58_type m_block_prev_id;
        base64_type m_block_hash;

        std::vector<txagg_cbor_b64> m_txagg;           // txagg_cbor_b64 < 이 형태로 날아옴. 여기서 처리할 필요 없음.
        std::vector<Transaction> m_transactions;    // 어차피 RDB 블록/트랜잭션 정보에 넣어야하기 때문에 일단 유지. 불필요할 수 있음.

        base64_type m_aggz;     // aggregate signature 에 필요함

        base64_type m_tx_root;
        base64_type m_us_state_root;
        base64_type m_cs_state_root;
        base64_type m_sg_root;

        std::vector<Signature> m_signers;    // <signer_id, signer_sig> 형태. map으로 바꿔야 할 수도 있음.
        std::map<base58_type, std::string> m_user_certs;

        Signature m_block_prod_info;        // 마찬가지로 <signer_id, signer_sig> 형태

        bytes m_block_raw;


    public:
        Block() { el::Loggers::getLogger("BLOC"); };

        bool operator==(Block &other) const {
            return (m_block_height == other.getHeight() && m_block_hash == other.getHashB64());
        }


        bool initialize(json &msg_block) {

            if (msg_block.empty())
                return false;

            m_block_id = Safe::getString(msg_block["block"], "id");
            m_block_time = Safe::getTime(msg_block["block"], "time");
            m_world_id = TypeConverter::base64ToArray<CHAIN_ID_TYPE_SIZE>(
                    Safe::getString(msg_block["block"], "world"));
            m_chain_id = TypeConverter::base64ToArray<CHAIN_ID_TYPE_SIZE>(
                    Safe::getString(msg_block["block"], "chain"));
            m_block_height = Safe::getInt(msg_block["block"], "height");
            m_block_prev_id = Safe::getString(msg_block["block"], "pid");
            m_block_hash = Safe::getString(msg_block["block"], "hash");

            m_aggz = ;

            m_tx_root = ;
            m_us_state_root = ;
            m_cs_state_root = ;
            m_sg_root = ;


            m_signers = ;
            m_user_certs = ;

            m_block_prod_info = ;

            m_block_raw = ;



            if (!setSupportSignaturesFromJson(msg_block["SSig"]))
                return false;

            if (!setTransactions(block_txs))
                return false;

            return true;
        }

        bool setTxaggs(std::vector<txagg_cbor_b64> &txagg) {
            m_txagg = txagg;
            return true;
        }

        bool setTxaggs(json &txs_json) {
            if (!txs_json.is_array()) {
                return false;
            }

            m_txagg.clear();
            for (auto &each_tx_json : txs_json) {
                m_txagg.emplace_back(each_tx_json);     // 부정확할 수 있음
            }

            return true;
        }

        bool setSupportSignatures(std::vector<Signature> &ssigs) {
            if (ssigs.empty())
                return false;
            m_signers = ssigs;
            return true;
        }

        bool setSupportSignaturesFromJson(json &ssigs) {
            if (!ssigs.is_array())
                return false;

            m_signers.clear();
            for (auto &each_ssig : ssigs) {
                Signature tmp;
                tmp.signer_id = Safe::getString(each_ssig["signer"], "id");
                tmp.signer_signature =
                        Safe::getString(each_ssig["signer"], "sig");
                m_signers.emplace_back(tmp);
            }

            return true;
        }

        bool setSupportSignaturesFromJson(json &ssigs) {
            if (!ssigs.is_array())
                return false;

            m_signers.clear();
            for (auto &each_ssig : ssigs) {
                Signature tmp;
                tmp.signer_id = Safe::getString(each_ssig["signer"], "id");
                tmp.signer_signature =
                        Safe::getString(each_ssig["signer"], "sig");
                m_signers.emplace_back(tmp);
            }

            return true;
        }



        std::vector<tx_id_type> getTxIds() {
            std::vector<tx_id_type> ret_txids;
            for (auto &each_tx : m_transactions) {
                ret_txids.emplace_back(each_tx.getId());
            }
            return ret_txids;
        }



        block_height_type getHeight() { return m_block_height; }
        size_t getNumTransactions() { return m_txagg.size(); }
        size_t getNumSigners() { return m_signers.size(); }
        timestamp_t getBlockTime() { return m_block_time; }

        string getBlockIdRaw() { return TypeConverter::decodeBase<58>(m_block_id); }
        string getBlockIdB58() { return m_block_id; }

        string getHashRaw() { return TypeConverter::decodeBase<64>(m_block_hash); }
        string getHashB64() { return m_block_hash; }

        string getPrevBlockIdRaw() { return TypeConverter::decodeBase<58>(m_block_prev_id); }
        string getPrevBlockIdB58() { return m_block_prev_id; }

        bytes getBlockRaw() { return m_block_raw; }


        // Support signature cannot be verified unless storage or block itself has
        // suitable certificates Therefore, the verification of support signatures
        // should be delayed until the previous block has been saved.
        bool isValidLate(
                std::function<std::string(std::string &, timestamp_t)> &get_user_cert) {
            // step - check support signatures
            bytes ssig_msg_after_sid = getSupportSigMessageCommon();

            for (auto &each_ssig : m_ssigs) {
                BytesBuilder ssig_msg_builder;
                ssig_msg_builder.append(each_ssig.signer_id);
                ssig_msg_builder.append(ssig_msg_after_sid);

                std::string user_id_b64 =
                        TypeConverter::encodeBase64(each_ssig.signer_id);
                std::string user_pk_pem;

                auto it_map = m_user_certs.find(user_id_b64);
                if (it_map != m_user_certs.end()) {
                    user_pk_pem = it_map->second;
                } else {
                    user_pk_pem =
                            get_user_cert(user_id_b64, m_time); // this is from storage
                }

                if (user_pk_pem.empty()) {
                    CLOG(ERROR, "BLOC") << "No suitable user certificate";
                    return false;
                }

                if (!ECDSA::doVerify(user_pk_pem, ssig_msg_builder.getBytes(),
                                     each_ssig.signer_signature)) {
                    CLOG(ERROR, "BLOC") << "Invalid support signature";
                    return false;
                }
            }

            return true;
        }

        bool isValidEarly(std::function<std::string(id_type &)> &get_cert) {

            if (m_block_raw.empty() || m_signature.empty()) {
                CLOG(ERROR, "BLOC") << "Empty blockraw or signature";
                return false;
            }

            // step - check merkle tree
            if (m_tx_root != m_tx_root.back()) {
                CLOG(ERROR, "BLOC") << "Invalid Merkle-tree root";
                return false;
            }

            // step - transactions

            for (auto &each_tx : m_transactions) {
                id_type requster_id = each_tx.getRequesterId();
                std::string pk_cert = get_cert(requster_id);
                if (!each_tx.isValid(pk_cert)) {
                    CLOG(ERROR, "BLOC") << "Invalid transaction";
                    return false;
                }
            }

            // step - check merger's signature

            std::string merger_pk_cert = get_cert(m_block_prod_id);

            if (merger_pk_cert.empty()) {
                CLOG(ERROR, "BLOC") << "No suitable merger certificate";
                return false;
            }

            bytes meta_header_raw = getBlockMetaHeaderRaw(m_block_raw);

            if (!ECDSA::doVerify(merger_pk_cert, meta_header_raw, m_signature)) {

                CLOG(ERROR, "BLOC") << "Invalid merger signature";
                return false;
            }

            return true;
        }


    private:

//        bytes getSupportSigMessageCommon() {
//            BytesBuilder ssig_msg_common_builder;
//            ssig_msg_common_builder.append(m_block_time);
//            ssig_msg_common_builder.append(m_block_prod_id);
//            ssig_msg_common_builder.append(m_chain_id);
//            ssig_msg_common_builder.append(m_block_height);
//            ssig_msg_common_builder.append(m_tx_root);
//            return ssig_msg_common_builder.getBytes();
//        }

        std::vector<hash_t> calcTxMerkleTreeNode() {
            std::vector<hash_t> merkle_tree_node;
            std::vector<hash_t> tx_digests;

            for (auto &each_tx : m_transactions) {
                tx_digests.emplace_back(each_tx.getDigest());
            }

            TxMerkleTree merkle_tree;
            merkle_tree.generate(tx_digests);
            merkle_tree_node = merkle_tree.getTxMerkleTree();
            return merkle_tree_node;
        }

        std::map<std::string, std::string> extractUserCertsIf() {
            std::map<std::string, std::string> ret_map;
            for (auto &each_tx : m_transactions) {
                std::map<std::string, std::string> tx_user_certs = each_tx.getCertsIf();
                if (!tx_user_certs.empty())
                    ret_map.insert(tx_user_certs.begin(), tx_user_certs.end());
            }

            return ret_map;
        }

    };
} // namespace gruut
#endif
