#ifndef MAP_DB_HEADER
#define MAP_DB_HEADER
#include "common_irrlicht.h"
#include <sqlite3.h>
#include <string>
// #define SERVER_MAP_DB "server_map.sqlite"
// #define CLIENT_MAP_DB "client_map.sqlite"
struct ReturnCols
{
  // Maybe have additional columns
  s16 nodeType;
} ;
class MapDB{
public:
  MapDB(std::string dbFile, sqlite3* db):m_dbName(dbFile),m_db(db){
    int rc;
    rc = sqlite3_open("map.db",&m_db);
     rc = sqlite3_open("test.db", &db);

     if( rc ) {
        dout_map<<"Can't open database: "<<sqlite3_errmsg(db)<<std::endl;
        return(0);
     } else {
      dout_map<<"Opened database: "<<sqlite3_errmsg(db)<<std::endl;
     }
    const char* checkTableSQL = "select count(type) from sqlite_master where type='table' and name='Map'";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(m_db, checkTableSQL, -1, &stmt, NULL);
    rc = sqlite3_step(stmt);
    if (rc==0)
    {
      const char* createTableSQL = "create table Map (X smallint, Y smallint, Z smallint,NodeType smallint PRIMARY KEY (X,Y,Z))";
      sqlite3_exec(db, createTableSQL, NULL, NULL, &m_errMsg);
    }
  }
  ~MapDB(){
    free(m_db);
    dout_map<<"Errors: "<<m_errMsg<<std::endl;
    free(m_errMsg);
  }
  void stop();
  s16 getNodeType(v3s16 pos);
  void removeNode(v3s16 pos);
  void addNode(v3s16 pos, s16 nType);
private:
  std::string m_dbName;
  sqlite3* m_db;
  char* m_errMsg = 0;
};
#endif

