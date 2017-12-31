#pragma once

#include <string>
#include <memory>

#include "g3log/logmessage.hpp"

class LogFileSink {
  public:
    LogFileSink(const std::string &log_prefix, const std::string &log_directory, const std::string &logger_id="g3log");
    virtual ~LogFileSink();

    void fileWrite(g3::LogMessageMover message);
    std::string changeLogFile(const std::string &directory, const std::string &logger_id);
    std::string fileName();


  private:
    std::string _log_file_with_path;
    std::string _log_prefix_backup; // needed in case of future log file changes of directory
    std::unique_ptr<std::ofstream> _outptr;

    void addLogFileHeader();
    std::ofstream &filestream() {
      return *(_outptr.get());
    }


    LogFileSink &operator=(const LogFileSink &) = delete;
    LogFileSink(const LogFileSink &other) = delete;

};

