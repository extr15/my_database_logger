
#include <Eigen/Core>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <g3log/std2_make_unique.hpp>
#include <iostream>
#include "logfile_sink.h"
#include "my_database_log_format.h"
#include "my_sqlite3_table.h"
#include "sqlite3_sink.h"

int main(int argc, char *argv[]) {
  std::unique_ptr<g3::LogWorker> logworker{g3::LogWorker::createLogWorker()};

  // auto logfile_handle = logworker->addDefaultLogger(argv[0], "./");
  auto logfile_handle = logworker->addSink(std2::make_unique<LogFileSink>(argv[0], "./", "g3log"), &LogFileSink::fileWrite);
  // initialize the logger before it can receive LOG calls
  initializeLogging(logworker.get());

  LOG(INFO) << "init database";
  MyDatabaseLogInit("my_debug.db");
  LOG(INFO) << "start main loop";
  int tri3d_loc_idx = 0;
  for (int i = 0; i < 100; i++) {
    int camTMapIdx_1 = i, camTMapIdx_2 = 0;
    MyDatabaseLog(DbTableIdx_CamTMapInTri3D) << camTMapIdx_1 << ", " << MyDatabaseLogFormat(Eigen::Matrix4d::Identity());
    MyDatabaseLog(DbTableIdx_Tri3dCorres) << camTMapIdx_1 << ", " << camTMapIdx_2 << ", " << tri3d_loc_idx;

    std::stringstream ss_reproj_err;
    const int reproj_err_num = 100;
    for (int j = 0; j < reproj_err_num; j++) {
      Eigen::Vector2d _reproj_err = Eigen::Vector2d::Random();
      ss_reproj_err << _reproj_err(0) << ", " << _reproj_err(1);
      if (j != reproj_err_num - 1) {
        ss_reproj_err << "), (";
      }
    }
    MyDatabaseLog(DbTableIdx_ReprojErrInTri3D) << ss_reproj_err.str();
    tri3d_loc_idx++;

    // sometimes you may want to write sql manually, here is a example.
    const std::string tri3d_loc_prefix("table_tri3d_loc_");
    std::stringstream ss;
    std::stringstream ss2;
    ss << tri3d_loc_prefix << tri3d_loc_idx;
    const std::string value_str("mx real, my real, mz real, u1 real, v1 real, u2 real, v2 real");
    ss2 << "insert into " << ss.str() << " (mx, my, mz, u1, v1, u2, v2) values ";
    const int tri3d_num = 100;
    for (int _i = 0; _i < tri3d_num; _i++) {
      Eigen::Vector3d _m = Eigen::Vector3d::Random();
      Eigen::Vector2d _v1 = Eigen::Vector2d::Random();
      Eigen::Vector2d _v2 = Eigen::Vector2d::Random();
      ss2 << "(" << _m(0) << ", " << _m(1) << ", " << _m(2) << ", " << _v1(0)
          << ", " << _v1(1) << ", " << _v2(0) << ", " << _v2(1) << ")";
      if (_i == tri3d_num - 1) {
        ss2 << ";";
      } else {
        ss2 << ",";
      }
    }

    MyDatabaseLogInsert(ss.str(), value_str, ss2.str());
  }
  LOG(INFO) << "finish main loop";

  return 0;
}
