#include "mapdb.h"
static int selectCallBack(void *data, int argc, char **argv, char **azColName) {
  ReturnCols *cols=(ReturnCols *)data;
   if(argc==1){
    // Find one node, get the node type
    cols->nodeType = argv[4];
   }else if (argc==0)
   {
     // Node not exist
    cols->nodeType = -1;
   }
   return 0;
}
void MapDB::stop(){
  sqlite3_close(m_db);
}
s16 MapDB::getNodeType(v3s16 pos){
  const char* selectSQL = "select NodeType from Map where X="+std::to_string(p.X)+" and Y="+std::to_string(p.Y)+"and Z="+std::to_string(p.Z);
  ReturnCols cols;
  sqlite3_exec(db, selectSQL, selectCallBack, &cols, &m_errMsg);
  return cols.nodeType;
}
void MapDB::removeNode(v3s16 pos){
  const char* deleteSQL = "delete from Map where X="+std::to_string(p.X)+" and Y="+std::to_string(p.Y)+"and Z="+std::to_string(p.Z);
  sqlite3_exec(db, deleteSQL, NULL, NULL, &m_errMsg);
}
void MapDB::addNode(v3s16 pos, s16 nType){
  int rc = this.getNodeType(v3s16 pos);
  if (rc!=-1)
  {
    const char* sql = "update Map set NodeType="+std::to_string(nType)+" where X="+std::to_string(p.X)+" and Y="+std::to_string(p.Y)+"and Z="+std::to_string(p.Z);
  } else {
    const char* sql = "insert into Map (X,Y,Z,NodeType) values("+std::to_string(p.X)+","+std::to_string(p.Y)+","+std::to_string(p.Z)+","+std::to_string(nType)+")";
  }
  sqlite3_exec(db, sql, NULL, NULL, &m_errMsg);
}
