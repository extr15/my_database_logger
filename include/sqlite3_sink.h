#pragma once

#include <string>
#include <iostream>
#include <g3log/logmessage.hpp>

#include <SQLiteCpp/SQLiteCpp.h>

class Sqlite3Sink {
  public:
    void ReceiveLogMessage(g3::LogMessageMover log_entry) {
      using namespace std;
      //cout<<log_entry.get().message()<<endl;
      auto const & msg = log_entry.get().message();
      try {
        db->exec(msg);
      }
      catch (std::exception& e) {
        cout<<log_entry.get().toString() <<endl;
        std::cout << "SQLite exception in ReceiveLogMessage: "<<" : "<< e.what() << std::endl;
      }
    }
    void InitDB(const std::string & db_name);
  private:
    std::shared_ptr<SQLite::Database> db;
};

