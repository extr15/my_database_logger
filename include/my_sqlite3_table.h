#pragma once

#include <string>
#include <g3log/sink.hpp>
#include "sqlite3_sink.h"
#include <mutex>

// DbTableIdx_Dummy is used in MyDatabaseLogInsert, when we want to write insert sql manually. 
enum DbTableIdx {
  // ("x real, y real")
  DbTableIdx_ReprojErrInTri3D,
  // ("x real")
  DbTableIdx_FeatureNumOfMappt,
  DbTableIdx_FeatureNumInFrameOfMappt,
  // ("frame integer, x real, y real, z real")
  DbTableIdx_Track2DPos,
  // ("frame integer, x real, y real")
  DbTableIdx_ReprojErrMeanInTri3D,
  DbTableIdx_Find2Dto3DCorrespondNum,
  DbTableIdx_CandidateFeatureNumInMapping,
  // ("frame integer, x real")
  DbTableIdx_NewAddMapptNum,
  DbTableIdx_RigNumBetweenKeyrig,
  DbTableIdx_DetectFeatureNum,
  DbTableIdx_MatchFeatureNumInMapping,
  DbTableIdx_KeyrigNum,
  DbTableIdx_MapptNum,

  DbTableIdx_CamTMapInTri3D,
  DbTableIdx_Tri3dCorres,
  DbTableIdx_Dummy,
  DbTableNum
};

const std::string g_db_table_name[DbTableNum] = {
  "table_reproj_err_",
  "table_feat_num_mappt_",
  "table_feat_num_frame_mappt_",
  "table_track_2d_pos_",
  "table_reproj_err_mean",
  "table_find2d3d_correspond",
  "table_candidate_feature_mapping",
  "table_new_add_mappt",
  "table_rignum_between_keyrig",
  "table_detect_feature",
  "table_match_feature_mapping",
  "table_keyrig_num",
  "table_mappoint_num",
  "table_cam_T_map_tri3d",
  "table_tri3d_corres",
  "table_dummy"
};

class MyDatabaseLogCapture {
  public:
    MyDatabaseLogCapture(DbTableIdx table_idx, const char *file, const int line, const char *function);
    ~MyDatabaseLogCapture();
    void CreateDbTable(const std::string & table_name, const std::string & value_str);

    std::ostringstream stream_table_;
    std::ostringstream stream_val_;
    std::string stream_tail_;
    const char *file_;
    const int line_;
    const char *function_;
  private:
    inline void CreateDbTableIfNotExist(DbTableIdx idx, const std::string & table_name, const std::string & value_str);
    inline void CreateDbTableIncCount(DbTableIdx idx, const std::string & value_str, std::string & table_name_out);
    void SetTableStringByIdx();
    void SendStringToDb(const std::string & _str);
    std::string db_string_;
    DbTableIdx table_idx_;
};

#define MyDatabaseLog(table_idx) MyDatabaseLogCapture(table_idx, __FILE__, __LINE__, static_cast<const char*>(__PRETTY_FUNCTION__)).stream_val_

// in MyDatabaseLogInsert, ss_string is the whole sql operation string.
#define MyDatabaseLogInsert(table_name, value_str, ss_string) {\
  MyDatabaseLogCapture _my_database_log_capture(DbTableIdx_Dummy, __FILE__, __LINE__, static_cast<const char*>(__PRETTY_FUNCTION__));\
  _my_database_log_capture.CreateDbTable(table_name, value_str);\
  _my_database_log_capture.stream_table_.str(std::string());\
  _my_database_log_capture.stream_val_.str(ss_string);\
  _my_database_log_capture.stream_tail_ = "";\
}

void MyDatabaseLogInit(const std::string & db_name);

