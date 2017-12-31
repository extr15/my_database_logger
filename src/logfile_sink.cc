#include "logfile_sink.h"
#include "filesinkhelper.ipp"
#include <cassert>
#include <chrono>
#include <unistd.h> // for unlink

using namespace g3::internal; // for filesinkhelper.ipp

LogFileSink::LogFileSink(const std::string &argv0, const std::string &log_directory, const std::string& logger_id)
  : _log_file_with_path(log_directory)
  , _log_prefix_backup(argv0)
    , _outptr(new std::ofstream)
{
  // the log_prefix usually is argv[0], i.e. the_path/bin/program, and we will use the real program name without path as log file prefix
  const char* slash = strrchr(argv0.c_str(), '/');
#ifdef OS_WINDOWS
  if (!slash)  slash = strrchr(argv0.c_str(), '\\');
#endif
  std::string log_prefix = slash ? slash + 1 : argv0;
  
  _log_prefix_backup = prefixSanityFix(log_prefix);
  if (!isValidFilename(_log_prefix_backup)) {
    std::cerr << "g3log: forced abort due to illegal log prefix [" << log_prefix << "]" << std::endl;
    abort();
  }

  std::string file_name = createLogFileName(_log_prefix_backup, logger_id);
  _log_file_with_path = pathSanityFix(_log_file_with_path, file_name);
  _outptr = createLogFile(_log_file_with_path);

  if (!_outptr) {
    std::cerr << "Cannot write log file to location, attempting current directory" << std::endl;
    _log_file_with_path = "./" + file_name;
    _outptr = createLogFile(_log_file_with_path);
  }
  assert(_outptr && "cannot open log file at startup");
  addLogFileHeader();

  // create symlink file, copy from glog.
  const std::string & symlink_basename_ = log_prefix;
  //
  // We try to create a symlink called <program_name>.<severity>,
  // which is easier to use.  (Every time we create a new logfile,
  // we destroy the old symlink and create a new one, so it always
  // points to the latest logfile.)  If it fails, we're sad but it's
  // no error.
  if (!symlink_basename_.empty()) {
    // take directory from filename
    const char * filename = file_name.c_str();
    const char* slash = strrchr(filename, '/');
    const std::string linkname = symlink_basename_ + ".INFO";
    std::string linkpath;
    if ( slash ) linkpath = std::string(filename, slash-filename+1);  // get dirname
    linkpath += linkname;
    unlink(linkpath.c_str());                    // delete old one if it exists

    // We must have unistd.h.
    // Make the symlink be relative (in the same dir) so that if the
    // entire log directory gets relocated the link is still valid.
    const char *linkdest = slash ? (slash + 1) : filename;
    if (symlink(linkdest, linkpath.c_str()) != 0) {
      // silently ignore failures
    }
  }

}


LogFileSink::~LogFileSink() {
  std::string exit_msg {"g3log g3FileSink shutdown at: "};
  auto now = std::chrono::system_clock::now();
  exit_msg.append(g3::localtime_formatted(now, g3::internal::time_formatted)).append("\n");
  filestream() << exit_msg << std::flush;

  exit_msg.append("Log file at: [").append(_log_file_with_path).append("]\n");
  std::cerr << exit_msg << std::flush;
}

// The actual log receiving function
void LogFileSink::fileWrite(g3::LogMessageMover message) {
  std::ofstream &out(filestream());
  out << message.get().toString() << std::flush;
}

std::string LogFileSink::changeLogFile(const std::string &directory, const std::string &logger_id) {

  auto now = std::chrono::system_clock::now();
  auto now_formatted = g3::localtime_formatted(now, {g3::internal::date_formatted + " " + g3::internal::time_formatted});

  std::string file_name = createLogFileName(_log_prefix_backup, logger_id);
  std::string prospect_log = directory + file_name;
  std::unique_ptr<std::ofstream> log_stream = createLogFile(prospect_log);
  if (nullptr == log_stream) {
    filestream() << "\n" << now_formatted << " Unable to change log file. Illegal filename or busy? Unsuccessful log name was: " << prospect_log;
    return {}; // no success
  }

  addLogFileHeader();
  std::ostringstream ss_change;
  ss_change << "\n\tChanging log file from : " << _log_file_with_path;
  ss_change << "\n\tto new location: " << prospect_log << "\n";
  filestream() << now_formatted << ss_change.str();
  ss_change.str("");

  std::string old_log = _log_file_with_path;
  _log_file_with_path = prospect_log;
  _outptr = std::move(log_stream);
  ss_change << "\n\tNew log file. The previous log file was at: ";
  ss_change << old_log << "\n";
  filestream() << now_formatted << ss_change.str();
  return _log_file_with_path;
}
std::string LogFileSink::fileName() {
  return _log_file_with_path;
}
void LogFileSink::addLogFileHeader() {
  filestream() << header();
}


