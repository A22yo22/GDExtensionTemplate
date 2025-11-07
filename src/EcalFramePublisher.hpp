#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <ecal/ecal.h>
#include <ecal/pubsub/publisher.h>

#include <atomic>
#include <string>

namespace godot {

class EcalFramePublisher : public Node {
  GDCLASS(EcalFramePublisher, Node);

  static std::atomic<int> init_refs_;

  String topic_ = "frame_blob";
  std::string topic_ascii_;
  eCAL::CPublisher publisher_;
  bool publisher_created_ = false;
  bool registered_with_ecal_ = false;
  int64_t counter_ = 0;

  void shutdown();

protected:
  static void _bind_methods();

public:
  EcalFramePublisher() = default;
  ~EcalFramePublisher() override;

  void set_topic(const String& topic);
  String get_topic() const;

  int64_t get_last_value() const { return counter_; }

  void _ready() override;
  void _process(double delta) override;
  void _exit_tree() override;
};

} // namespace godot


