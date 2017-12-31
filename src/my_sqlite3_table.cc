#include <g3log/g3log.hpp>
#include <g3log/std2_make_unique.hpp>
#include "my_sqlite3_table.h"
#include "sqlite3_sink.h"


static std::shared_ptr<g3::internal::Sink<Sqlite3Sink> > g_sink_db;
static bool g_db_if_create[DbTableNum] = {false};
static std::mutex g_create_table_mutex[DbTableNum];

void MyDatabaseLogCapture::CreateDbTable(const std::string & table_name, const std::string & value_str) {
  std::stringstream ss;
  ss.str(std::string());
  ss<<"drop table if exists "<<table_name;
  SendStringToDb(ss.str());
  ss.str(std::string());
  ss<<"create table "<<table_name;
  ss<<" (id integer primary key autoincrement not null, "<< value_str<<")";
  SendStringToDb(ss.str());
}
void MyDatabaseLogCapture::CreateDbTableIfNotExist(DbTableIdx idx, const std::string & table_name, const std::string & value_str) {
  std::lock_guard<std::mutex> lock(g_create_table_mutex[idx]);
  if(!g_db_if_create[idx]) {
    CreateDbTable(table_name, value_str);
    g_db_if_create[table_idx_] = true;
  }
}
void MyDatabaseLogCapture::CreateDbTableIncCount(DbTableIdx idx, const std::string & value_str, std::string & table_name_out) {
  static int s_table_cnt[DbTableNum] = {0};
  std::lock_guard<std::mutex> lock(g_create_table_mutex[idx]);
  std::stringstream ss_name;
  ss_name<<g_db_table_name[table_idx_]<<s_table_cnt[idx];
  s_table_cnt[idx]++;
  table_name_out = ss_name.str();
  CreateDbTable(table_name_out, value_str);
}
void MyDatabaseLogCapture::SetTableStringByIdx() {
  CHECK(table_idx_ < DbTableNum);
  const std::string & table_name = g_db_table_name[table_idx_];
  switch(table_idx_) {
    case DbTableIdx_ReprojErrInTri3D:
      {
        std::string value_str("x real, y real");
        std::string table_name_out;
        CreateDbTableIncCount(table_idx_, value_str, table_name_out);
        stream_table_<<"insert into "<<table_name_out<<" (x, y) values ("; 
      }
      break;
    case DbTableIdx_FeatureNumOfMappt:
    case DbTableIdx_FeatureNumInFrameOfMappt:
      {
        std::string value_str("x real");
        std::string table_name_out;
        CreateDbTableIncCount(table_idx_, value_str, table_name_out);
        stream_table_<<"insert into "<<table_name_out<<" (x) values ("; 
      }
      break;
    case DbTableIdx_Track2DPos:
      {
        std::string value_str("frame integer, x real, y real, z real");
        CreateDbTableIfNotExist(table_idx_, table_name, value_str);
        stream_table_<<"insert into "<<g_db_table_name[table_idx_]<<" (frame, x, y, z) values ("; 
      }
      break;
    case DbTableIdx_ReprojErrMeanInTri3D:
    case DbTableIdx_Find2Dto3DCorrespondNum:
    case DbTableIdx_CandidateFeatureNumInMapping:
      {
        std::string value_str("frame integer, x real, y real");
        CreateDbTableIfNotExist(table_idx_, table_name, value_str);
        stream_table_<<"insert into "<<g_db_table_name[table_idx_]<<" (frame, x, y) values ("; 
      }
      break;
    case DbTableIdx_NewAddMapptNum:
    case DbTableIdx_RigNumBetweenKeyrig:
    case DbTableIdx_DetectFeatureNum:
    case DbTableIdx_MatchFeatureNumInMapping:
    case DbTableIdx_KeyrigNum:
    case DbTableIdx_MapptNum:
      {
        std::string value_str("frame integer, x real");
        CreateDbTableIfNotExist(table_idx_, table_name, value_str);
        stream_table_<<"insert into "<<g_db_table_name[table_idx_]<<" (frame, x) values ("; 
      }
      break;
    case DbTableIdx_CamTMapInTri3D:
      {
        std::string value_str("idx integer, a00 real, a01 real, a02 real, a03 real,\
            a10 real, a11 real, a12 real, a13 real,\
            a20 real, a21 real, a22 real, a23 real,\
            a30 real, a31 real, a32 real, a33 real");
        CreateDbTableIfNotExist(table_idx_, table_name, value_str);
        stream_table_<<"insert into "<<g_db_table_name[table_idx_]<<" (idx, a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22, a23, a30, a31, a32, a33) values ("; 
      }
      break;
    case DbTableIdx_Tri3dCorres:
      {
        std::string value_str("camTMapIdx1 integer, camTMapIdx2 integer, locIdx integer");
        CreateDbTableIfNotExist(table_idx_, table_name, value_str);
        stream_table_<<"insert into "<<g_db_table_name[table_idx_]<<" (camTMapIdx1, camTMapIdx2, locIdx) values ("; 
      }
      break;
    case DbTableIdx_Dummy:
      break;
    default:
      std::cerr<<"Error MyDatabaseLogCapture::SetTableStringByIdx bad table_idx_: " << table_idx_ << std::endl;
      break;
  }
}

void MyDatabaseLogCapture::SendStringToDb(const std::string & _str) {
  LEVELS msg_level {INFO};
  g3::LogMessagePtr message_ptr {std2::make_unique<g3::LogMessage>(file_, line_, function_, msg_level)};
  message_ptr.get()->write().append(_str);
  {
    std::unique_ptr<g3::LogMessage> unique_msg(std::move(message_ptr.get()));
    g3::LogMessage msg(*unique_msg);
    g_sink_db->send(g3::LogMessageMover(std::move(msg)));
  }
}

MyDatabaseLogCapture::MyDatabaseLogCapture(DbTableIdx table_idx, const char *file, const int line, const char *function) : table_idx_(table_idx), file_(file), line_(line), function_(function), stream_tail_(");") {
  SetTableStringByIdx();
}

MyDatabaseLogCapture::~MyDatabaseLogCapture() {
  db_string_ = stream_table_.str() + stream_val_.str() + stream_tail_;
  SendStringToDb(db_string_);
}

void MyDatabaseLogInit(const std::string & db_name) {
  std::unique_ptr<Sqlite3Sink> sqlite3_sink = std2::make_unique<Sqlite3Sink>();
  g_sink_db = std::make_shared<g3::internal::Sink<Sqlite3Sink> >( std::move(sqlite3_sink),  &Sqlite3Sink::ReceiveLogMessage);
  g_sink_db->_real_sink->InitDB(db_name);
}

