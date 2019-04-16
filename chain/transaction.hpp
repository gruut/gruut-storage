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

    base58_type new_txid = json::get<string>(tx_json, "txid").value();

    // 여기에 txid 계산해서 검증하는 코드 추가

    setTxId(new_txid);
    setTime(static_cast<gruut::timestamp_t>(stoll(json::get<string>(tx_json, "time").value())));
    setSeed(json::get<string>(tx_json, "txid").value());

    setCid(json::get<string>(tx_json["body"], "cid").value()); // 현재 cid 관련 내용은 message에 없음
    setReceiverId(json::get<string>(tx_json["body"], "receiver").value());
    setFee(stoi(json::get<string>(tx_json["body"], "fee").value()));
    setTxInput(nlohmann::json::to_cbor(tx_json["body"]["input"]));

    setTxProdId(json::get<string>(tx_json["user"], "id").value());
    setTxProdPk(json::get<string>(tx_json["user"], "pk").value());
    setTxProdSig(json::get<string>(tx_json["user"], "agga").value());

    setEndorsers(tx_json["endorser"]);

    // 아래 넷은 tx scope에는 저장되는 사항이지만, json으로 입력되는 내용은 아닌것으로 보임. 검토중.
    //    setTxAggCbor();
    //    setBlockId();
    //    setTxPosition();
    //    setTxOutput();

    return true;
  }

  void setTxId(base58_type new_txid) {
    m_txid = new_txid;
  }

  void setTime(timestamp_t new_time) {
    m_tx_time = new_time;
  }

  void setSeed(base64_type new_seed) {
    m_seed = new_seed;
  }

  void setCid(string new_cid) {
    m_contract_id = new_cid;
  }

  void setReceiverId(base58_type new_rid) {
    m_receiver_id = new_rid;
  }

  void setFee(int new_fee) {
    m_fee = new_fee;
  }

  void setTxInput(nlohmann::json input_array) {
    for (auto &each_input : input_array) {
    }
  }

  void setTxProdId(base58_type new_tx_prod_id) {
    m_tx_prod_id = new_tx_prod_id;
  }

  void setTxProdPk(string new_tx_prod_pk) {
    m_tx_prod_pk = new_tx_prod_pk;
  }

  void setTxProdSig(base64_type new_tx_prod_sig) {
    m_tx_prod_sig = new_tx_prod_sig;
  }

  bool setEndorsers(nlohmann::json endorser_array) {
    if (!endorser_array.is_array())
      return false;

    m_tx_endorsers.clear();
    for (auto &each_endorser : endorser_array) {
      Endorser tmp;
      tmp.endorser_id = json::get<std::string>(each_endorser, "id").value();
      tmp.endorser_pk = json::get<std::string>(each_endorser, "pk").value();
      tmp.endorser_signature = json::get<std::string>(each_endorser, "sig").value();
      m_tx_endorsers.emplace_back(tmp);
    }
    return true;
  }

  void setTxAggCbor(string new_txagg_Cbor) {
    m_tx_agg_cbor = new_txagg_Cbor;
  }

  void setBlockId(base58_type block_id) {
    m_block_id = block_id;
  }

  void setTxPosition(int new_tx_pos) {
    m_tx_pos = new_tx_pos;
  }

  void setTxOutput(base64_type new_output) {
    m_tx_output = new_output;
  }
};
} // namespace gruut
#endif
