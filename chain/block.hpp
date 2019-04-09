#ifndef GRUUT_ENTERPRISE_MERGER_TEMPORARY_BLOCK_HPP
#define GRUUT_ENTERPRISE_MERGER_TEMPORARY_BLOCK_HPP

#include "tx_merkle_tree.hpp"
#include "signature.hpp"
#include "certificate.hpp"
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

        std::vector<txagg_cbor_b64> m_txagg;        // Tx의 Json을 CBOR로 처리하고 그 데이터를 b64인코딩한 결과 vector
        std::vector<Transaction> m_transactions;    // RDB 블록/트랜잭션 정보에 넣어야하기 때문에 유지. tx_root 계산할 때에도 사용

        base64_type m_aggz;     // aggregate signature 에 필요함

        base64_type m_tx_root;
        base64_type m_us_state_root;
        base64_type m_cs_state_root;
        base64_type m_sg_root;

        std::vector<Signature> m_signers;           // <signer_id, signer_sig> 형태
        std::vector<Certificate> m_user_certs;      // <cert_id, cert_content> 형태

        Signature m_block_prod_info;        // <signer_id, signer_sig> 형태

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

            setTxaggs(msg_block["tx"]);
            setTransaction(m_txagg);   // txagg에서 트랜잭션 정보를 뽑아낸다

            m_aggz = Safe::getString(msg_block, "aggz");

            m_tx_root = Safe::getString(msg_block["state"], "txroot");          // 추후 계산, 검증 필요
            m_us_state_root = Safe::getString(msg_block["state"], "usroot");    // 추후 계산, 검증 필요
            m_cs_state_root = Safe::getString(msg_block["state"], "csroot");    // 추후 계산, 검증 필요
            m_sg_root = Safe::getString(msg_block["state"], "sgroot");;         // 추후 계산, 검증 필요

            if (!setSigners(msg_block["signer"]))
                return false;
            setUserCerts(msg_block["certificate"]);

            m_block_prod_info.signer_id = Safe::getString(msg_block["producer"], "id");
            m_block_prod_info.signer_signature = Safe::getString(msg_block["producer"], "sig");

//          m_block_raw = ;     // 이건 어떻게 처리하지..

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
                m_txagg.emplace_back(each_tx_json);
            }
            return true;
        }

        bool setTransaction(std::vector<txagg_cbor_b64> &txagg) {
            m_transactions.clear();
            for (auto &each_txagg : txagg) {
                json each_txs_json;
                each_txs_json = json::from_cbor(TypeConverter::decodeBase<64>(each_txagg));

                Transaction each_tx;
                each_tx.setJson(each_txs_json);
                m_transactions.emplace_back(each_tx);
            }
            return true;
        }

        bool setSupportSignatures(std::vector<Signature> &signers) {
            if (signers.empty())
                return false;
            m_signers = signers;
            return true;
        }

        bool setSigners(json &signers) {
            if (!signers.is_array())
                return false;

            m_signers.clear();
            for (auto &each_signer : signers) {
                Signature tmp;
                tmp.signer_id = Safe::getString(each_signer, "id");
                tmp.signer_signature = Safe::getString(each_signer, "sig");
                m_signers.emplace_back(tmp);
            }
            return true;
        }

        bool setUserCerts(json &certificates) {
            m_user_certs.clear();
            for (auto &each_cert : certificates) {
                Certificate tmp;
                tmp.cert_id = Safe::getString(each_cert, "id");
                tmp.cert_content = Safe::getString(each_cert, "cert");
                m_user_certs.emplace_back(tmp);
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

    };
} // namespace gruut
#endif
