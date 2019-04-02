#ifndef WORKSPACE_STORAGE_HPP
#define WORKSPACE_STORAGE_HPP

#include <fstream>      // ifstream 클래스
#include <deque>
#include <map>

#include "../utils/template_singleton.hpp"
#include "easy_logging.hpp"
#include "state_merkle_tree.hpp"
#include "db_lib.hpp"

using namespace std;
using json = nlohmann::json;

typedef unsigned int uint;

#define _D_CUR_LAYER 4
enum {BLOCK_ID, USER_ID, VAR_TYPE, VAR_NAME, VAR_VALUE, PATH};
enum {SUCCESS=0, COIN_VALUE=-1, DATA_DUPLICATE=-2, DATA_NOT_EXIST=-3};
enum {NO_DATA=-2, DB_DATA=-1, CUR_DATA=_D_CUR_LAYER};

namespace gruut {

    class Storage : public TemplateSingleton<Storage> {
    private:
        int MAX_LAYER_SIZE;
        StateMerkleTree m_tree;
        mariaDb m_server;

        void readConfig();
        void setupDB();
        void setupMerkleTree();
        void destroyDB();

    public:
        Storage();
        ~Storage();

        void setBlocksByJson();

        void parseBlockToLayer(Block block);
        int addCommand(json transaction, Value &val);
        int sendCommand(json transaction);
        int newCommand(json transaction);
        int delCommand(json transaction);

        void testStorage();
        void testForward(Block block);
        void testBackward();
    };
}

#endif //WORKSPACE_STORAGE_HPP
