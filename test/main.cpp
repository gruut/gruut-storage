#include <cstring>

#include "../chain/state_merkle_tree.hpp"
#include "../storage/db_lib.hpp"
#include "../storage/storage.hpp"

using namespace std;

int main() {
  gruut::Storage g_storage;

  g_storage.setBlocksByJson();
  g_storage.testStorage();

  return 0;
}