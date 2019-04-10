//
// Created by msk on 2019-04-09.
//

#include "../chain/block.hpp"

namespace gruut {

// transaction root, signer group root를 구할 때 쓰이는 고정 크기 머클 트리
hash_t calcStaticMerkleRoot(std::vector<string> &material) {

  std::vector<hash_t> merkle_tree_vector;
  std::vector<hash_t> sha256_material;

  // 입력으로 들어오는 material 벡터는 해시만 하면 되도록 처리된 상태로 전달되어야 합니다
  for (auto &each_element : material) {
    sha256_material.emplace_back(Sha256::hash(each_element));
  }

  StaticMerkleTree merkle_tree;
  merkle_tree.generate(sha256_material);
  merkle_tree_vector = merkle_tree.getStaticMerkleTree();
  return merkle_tree_vector.back(); // 가장 뒤에 있는 원소가 root
}

// user scope, contract scope의 root를 구할 때 쓰이는 동적 머클 트리
bool calcStateRoot() {
  // 이건 구현하려면 ledger가 필요. 그리고 enterprise에 없던 부분이라서 새로 만들어야 함.
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

bool isValidEarly(std::function<std::string(id_type &)> &get_cert) {

  if (m_block_raw.empty() || something.empty()) {
    CLOG(ERROR, "BLOC") << "Empty blockraw or signature";
    return false;
  }

  // step - check tx merkle tree
  if (m_tx_root != asb.back()) { // 수정 필요
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

  std::string merger_pk_cert = get_cert(m_block_prod_info.signer_id);

  if (merger_pk_cert.empty()) {
    CLOG(ERROR, "BLOC") << "No suitable merger certificate";
    return false;
  }

  if (!ECDSA::doVerify(merger_pk_cert, meta_header_raw, m_signature)) {

    CLOG(ERROR, "BLOC") << "Invalid merger signature";
    return false;
  }

  return true;
}

// Support signature cannot be verified unless storage or block itself has
// suitable certificates Therefore, the verification of support signatures
// should be delayed until the previous block has been saved.
bool isValidLate(std::function<std::string(std::string &, timestamp_t)> &get_user_cert) {
  // step - check support signatures
  bytes ssig_msg_after_sid = getSupportSigMessageCommon();

  for (auto &each_ssig : m_signers) {
    BytesBuilder ssig_msg_builder;
    ssig_msg_builder.append(each_ssig.signer_id);
    ssig_msg_builder.append(ssig_msg_after_sid);

    std::string user_id_b64 = TypeConverter::encodeBase<64>(each_ssig.signer_id);
    std::string user_pk_pem;

    auto it_map = m_user_certs.find(user_id_b64);
    if (it_map != m_user_certs.end()) {
      user_pk_pem = it_map->second;
    } else {
      user_pk_pem = get_user_cert(user_id_b64, m_block_time); // this is from storage
    }

    if (user_pk_pem.empty()) {
      CLOG(ERROR, "BLOC") << "No suitable user certificate";
      return false;
    }

    if (!ECDSA::doVerify(user_pk_pem, ssig_msg_builder.getBytes(), each_ssig.signer_signature)) {
      CLOG(ERROR, "BLOC") << "Invalid support signature";
      return false;
    }
  }

  return true;
}

} // namespace gruut