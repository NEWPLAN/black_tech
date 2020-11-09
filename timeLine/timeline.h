

#ifndef __NEWPLAN_TIMELINE_H__
#define __NEWPLAN_TIMELINE_H__

#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "spsc_queue.h"

struct My_TensorTableEntry
{
  std::string tensor_name;
};

enum TimelineRecordType
{
  EVENT,
  MARKER
};

enum RequestType
{
  READ_REMOTE,
  WRITE_REMOTE,
};

enum ResponseType
{
  READ_REMOTE_DONE,
  WRITE_REMOTE_DONE,
};

struct TimelineRecord
{
  TimelineRecordType type;
  std::string tensor_name;
  char phase;
  std::string op_name;
  std::string args;
  std::string marker_name;
  long ts_micros;
};

class TimelineWriter
{
public:
  virtual ~TimelineWriter() {}
  void Initialize(std::string file_name,
                  std::chrono::steady_clock::time_point start_time_);
  void Shutdown();
  void EnqueueWriteEvent(const std::string &tensor_name, char phase,
                         const std::string &op_name, const std::string &args,
                         long ts_micros);
  void EnqueueWriteMarker(const std::string &name, long ts_micros);
  void SetPendingTimelineFile(std::string filename);
  short active();
  short healthy();
  TimelineWriter();

  // Similar to healthy, but allows queue to be drained before closing when set
  // to false.
  std::atomic_short active_{0};

private:
  void DoWriteEvent(const TimelineRecord &r);
  void DoWriteMarker(const TimelineRecord &r);
  void WriterLoop();
  void WriteAtFileStart();
  std::string PendingTimelineFile();
  void SetTimelineFile(std::string filename);

  // Are we healthy?  Queue no longer accepts new work items and stops draining
  // immediately when false.
  std::atomic_short healthy_{0};

  bool pending_status_;

  // Timeline file.
  std::ofstream file_;

  // Timeline record queue.

  SPSCQueue<TimelineRecord> *record_queue_; //= SPSCQueue<TimelineRecord>(1048576);

  // Mapping of tensor names to indexes. It is used to reduce size of the
  // timeline file.
  std::unordered_map<std::string, int> tensor_table_;

  std::thread writer_thread_;
  std::string cur_filename_;
  std::string new_pending_filename_;
  bool is_new_file_;
  // stores actual wall clock when horovod timeline was initialized
  long long start_time_since_epoch_utc_micros_;
  // mutex that protects timeline writer state
  std::recursive_mutex writer_mutex_;
};

enum TimelineState
{
  UNKNOWN = 0,
  NEGOTIATING = 1,
  TOP_LEVEL = 2,
  ACTIVITY = 3
};

// Writes timeline in Chrome Tracing format. Timeline spec is from:
// https://github.com/catapult-project/catapult/tree/master/tracing
class Timeline
{
public:
  void Initialize(std::string file_name, unsigned int horovod_size);
  void Shutdown();
  inline short Initialized() { return initialized_.fetch_and(1); }
  void NegotiateStart(const std::string &tensor_name,
                      RequestType request_type);
  void NegotiateRankReady(const std::string &tensor_name, int rank);
  void NegotiateEnd(const std::string &tensor_name);
  void Start(const std::string &tensor_name,
             const ResponseType response_type);
  void ActivityStartAll(const std::vector<My_TensorTableEntry> &entries,
                        const std::string &activity);
  void ActivityStart(const std::string &tensor_name,
                     const std::string &activity);
  void ActivityEndAll(const std::vector<My_TensorTableEntry> &entries);
  void ActivityEnd(const std::string &tensor_name);
  void End(const std::string &tensor_name, std::string *tensor);
  void MarkCycleStart();
  void SetPendingTimelineFile(std::string filename);

private:
  long TimeSinceStartMicros() const;
  void WriteEvent(const std::string &tensor_name, char phase,
                  const std::string &op_name = "",
                  const std::string &args = "");
  void WriteMarker(const std::string &name);

  // Boolean flag indicating whether Timeline was initialized (and thus should
  // be recorded).
  // weird that atomic bool doesn't support fetch_and operation.
  std::atomic_short initialized_{0};

  // Timeline writer.
  TimelineWriter writer_;

  // Time point when Horovod was started.
  std::chrono::steady_clock::time_point start_time_;

  // A mutex that guards timeline state from concurrent access.
  std::recursive_mutex mutex_;

  // Current state of each tensor in the timeline.
  std::unordered_map<std::string, TimelineState> tensor_states_;

  // Map of ranks to their string representations.
  // std::to_string() is very slow.
  std::vector<std::string> rank_strings_;
};

#endif // __NEWPLAN_TIMELINE_H__
