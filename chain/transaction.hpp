#ifndef GRUUT_PUBLIC_MERGER_TRANSACTION_HPP
#define GRUUT_PUBLIC_MERGER_TRANSACTION_HPP

#include "../lib/json.hpp"

#include "../utils/bytes_builder.hpp"
#include "../utils/sha256.hpp"
#include "../utils/type_converter.hpp"
#include "endorser.hpp"
#include "types.hpp"

#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace gruut {
class Transaction {
private:
  base58_type m_txid;
  timestamp_t m_tx_time;
  base64_type m_seed;

  string m_contract_id;
  base58_type m_receiver_id;
  int m_fee;
  string m_tx_input_cbor; // to_cbor 된 상태

  base58_type m_tx_prod_id;
  string m_tx_prod_pk;
  base64_type m_tx_prod_sig;

  vector<Endorser> m_tx_endorsers;

  base64_type m_tx_agg_cbor;
  base58_type m_block_id;
  int m_tx_pos; // static_merkle_tree에서의 idx
  base64_type m_tx_output;

public:
  bool setJson(nlohmann::json &tx_json) {

    m_txid = json::get<string>(tx_json, "txid").value();
    m_tx_time = static_cast<gruut::timestamp_t>(stoll(json::get<string>(tx_json, "time").value()));
    m_seed = json::get<string>(tx_json, "txid").value());

    m_contract_id = json::get<string>(tx_json["body"], "cid").value(); // 현재 cid 관련 내용은 message에 없음
    m_receiver_id = json::get<string>(tx_json["body"], "receiver").value();
    m_fee = stoi(json::get<string>(tx_json["body"], "fee").value());
    setTxInputCbor(tx_json["body"]["input"]);

    m_tx_prod_id = json::get<string>(tx_json["user"], "id").value();
    m_tx_prod_pk = json::get<string>(tx_json["user"], "pk").value();
    m_tx_prod_sig = json::get<string>(tx_json["user"], "agga").value();

    setEndorsers(tx_json["endorser"]);

    // 아래 넷은 tx scope에는 저장되는 사항이지만, json으로 입력되는 내용은 아닌것으로 보임. 검토중.
    //    setTxAggCbor();
    //    setBlockId();
    //    setTxPosition();
    //    setTxOutput();

    return true;
  }

  void setTxInputCbor(nlohmann::json input_array) {
    m_tx_input_cbor = TypeConverter::encodeBase<64>(nlohmann::json::to_cbor(input_array));
  }

  bool setEndorsers(nlohmann::json endorser_array) {
    if (!endorser_array.is_array())
      return false;

    m_tx_endorsers.clear();
    for (auto &each_endorser : endorser_array) {
      Endorser tmp;
      tmp.endorser_id = json::get<string>(each_endorser, "id").value();
      tmp.endorser_pk = json::get<string>(each_endorser, "pk").value();
      tmp.endorser_signature = json::get<string>(each_endorser, "sig").value();
      m_tx_endorsers.emplace_back(tmp);
    }
    return true;
  }
};
} // namespace gruut
#endif
