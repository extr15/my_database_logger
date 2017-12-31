#include "sqlite3_sink.h"

void Sqlite3Sink::InitDB(const std::string & db_name) {
  try {
    remove(db_name.c_str());
    db = std::make_shared<SQLite::Database>(db_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  }
  catch (std::exception& e) {
    std::cout << "SQLite exception: "<<__LINE__<<" : "<< e.what() << std::endl;
  }
}

