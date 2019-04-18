#ifndef GRUUT_PUBLIC_MERGER_BLOCK_VALIDATOR_HPP
#define GRUUT_PUBLIC_MERGER_BLOCK_VALIDATOR_HPP

#include "block.hpp"

namespace gruut {

std::vector<hash_t> makeStaticMerkleTree(std::vector<string> &material) {

  std::vector<hash_t> merkle_tree_vector;
  std::vector<hash_t> sha256_material;

  // 입력으로 들어오는 material 벡터는 해시만 하면 되도록 처리된 상태로 전달되어야 합니다
  for (auto &each_element : material) {
    sha256_material.emplace_back(Sha256::hash(each_element));
  }

  StaticMerkleTree merkle_tree;
  merkle_tree.generate(sha256_material);
  merkle_tree_vector = merkle_tree.getStaticMerkleTree();

  return merkle_tree_vector;
}

// user scope, contract scope의 root를 구할 때 쓰이는 동적 머클 트리
bool calcStateRoot() {
  // 이건 구현하려면 ledger가 필요. 그리고 enterprise에 없던 부분이라서 새로 만들어야 함.
}

bool verifyTransaction(Transaction &tx, string world, string chain) {
  hash_t tx_id = Sha256::hash(tx.getProdId() + world + chain + to_string(tx.getTxTime()) + tx.getSeed() +
                              (tx.getReceiverId().empty() ? tx.getReceiverId() : "") + (tx.getFee() != 0 ? to_string(tx.getFee()) : "") +
                              tx.getContractId());

  if (tx.getTxid() != TypeConverter::encodeBase<64>(tx_id)) {
    return false;
  }

  string tx_plain = tx.getTxid() + tx.getTxInputCbor();

  vector<Endorser> endorsers = tx.getEndorsers();
  for (auto &each_end : endorsers) {
    if (each_end.endorser_signature != SignByEndorser(tx_plain)) { // SignByEndorser가 확정되면 변경해야 함
      return false;
    }
  }

  string endorsers_info = "";
  for (auto &each_end : endorsers) {
    endorsers_info += each_end.endorser_id;
    endorsers_info += each_end.endorser_pk;
    endorsers_info += each_end.endorser_signature;
  }
  base64_type user_sig = SignByUser(tx_plain + endorsers_info); // SignByUser가 확정되면 변경해야 함
  if (tx.getProdSig() != user_sig) {
    return false;
  }

  return true;
}

bool verifyBlock(Block &block) {

  //
  // SSig의 퀄리티, 수 검증 추가
  // block time validation 추가
  //

  hash_t block_id = Sha256::hash(block.getBlockProdId() + to_string(block.getBlockTime()) + block.getWorld() + block.getChain() +
                                 to_string(block.getHeight()) + block.getPrevBlockId());
  if (block.getBlockId() != TypeConverter::encodeBase<58>(block_id)) {
    return false;
  }

  vector<hash_t> tx_merkle_tree = makeStaticMerkleTree(block.getTxaggs());
  if (block.getTxRoot() != TypeConverter::encodeBase<64>(tx_merkle_tree.back())) {
    return false;
  }

  hash_t block_hash = Sha256::hash(block.getBlockId() + block.getTxRoot() + block.getUserStateRoot() + block.getContractStateRoot());
  if (block.getBlockHash() != TypeConverter::encodeBase<64>(block_hash)) {
    return false;
  }

  vector<Signature> signers = block.getSigners();
  vector<string> signer_id_sigs;
  signer_id_sigs.clear();
  for (auto &each_signer : signers) {
    string id_sig = each_signer.signer_id + each_signer.signer_sig;
    signer_id_sigs.emplace_back(id_sig);
  }
  vector<hash_t> sg_merkle_tree = makeStaticMerkleTree(signer_id_sigs);
  if (block.getSgRoot() != TypeConverter::encodeBase<64>(sg_merkle_tree.back())) {
    return false;
  }

  //
  // us_state_root 검증 추가
  // cs_state_root 검증 추가
  //

  string merger_sig = SignByMerger(to_string(block.getBlockPubTime()) + block.getBlockHash() + to_string(block.getNumTransaction()) +
                                   to_string(block.getNumSigners()) + block.getSgRoot());
  if (block.get() != merger_sig) {
    return false;
  }
}

void validateSSig() {
  // late stage에서 사용될 함수입니다
}

bool earlyStage(Block &block) {
  if (!verifyBlock(block))
    return false;

  vector<Transaction> transactions = block.getTransactions();
  for (auto &each_transaction : transactions) {
    if (!verifyTransaction(each_transaction, block.getWorldId(), block.getChainId()))
      return false;
  }

  return true;
}

bool lateStage() {
  // validateSSig()를 메인으로 합니다
}

} // namespace gruut

#endif