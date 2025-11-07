#include "EcalFramePublisher.hpp"

#include <godot_cpp/variant/utility_functions.hpp>

#include <string>

using namespace godot;

std::atomic<int> EcalFramePublisher::init_refs_{0};

void EcalFramePublisher::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_topic", "topic"), &EcalFramePublisher::set_topic);
  ClassDB::bind_method(D_METHOD("get_topic"), &EcalFramePublisher::get_topic);
  ClassDB::bind_method(D_METHOD("get_last_value"), &EcalFramePublisher::get_last_value);

  ADD_PROPERTY(PropertyInfo(Variant::STRING, "topic"), "set_topic", "get_topic");
}

EcalFramePublisher::~EcalFramePublisher() { shutdown(); }

void EcalFramePublisher::set_topic(const String& topic) {
  if (publisher_created_) {
    UtilityFunctions::push_warning("EcalFramePublisher: cannot change topic while active.");
    return;
  }
  topic_ = topic;
}

String EcalFramePublisher::get_topic() const { return topic_; }

void EcalFramePublisher::_ready() {
  if (publisher_created_) {
    return;
  }

  if (topic_.is_empty()) {
    UtilityFunctions::push_warning("EcalFramePublisher: topic must not be empty.");
    return;
  }

  const int previous_refs = init_refs_.fetch_add(1, std::memory_order_acq_rel);
  registered_with_ecal_ = true;
  if (previous_refs == 0) {
    eCAL::Initialize("godot-frame-publisher");
  }

  topic_ascii_ = topic_.utf8().get_data();
  publisher_ = eCAL::CPublisher(topic_ascii_.c_str());
  publisher_created_ = publisher_.IsCreated();

  if (!publisher_created_) {
    UtilityFunctions::push_error(String("EcalFramePublisher: failed to create publisher for topic: ") + topic_);
    shutdown();
    return;
  }

  counter_ = 0;
  UtilityFunctions::print("EcalFramePublisher started on topic: ", topic_);
}

void EcalFramePublisher::_process(double) {
  if (!publisher_created_) {
    return;
  }

  ++counter_;
  const long sent = publisher_.Send(&counter_, static_cast<long>(sizeof(counter_)));
  if (sent != static_cast<long>(sizeof(counter_))) {
    UtilityFunctions::push_warning("EcalFramePublisher: send did not transmit full payload.");
  }

  UtilityFunctions::print("EcalFramePublisher value: ", counter_);
}

void EcalFramePublisher::_exit_tree() { shutdown(); }

void EcalFramePublisher::shutdown() {
  if (publisher_created_) {
    publisher_.Destroy();
    publisher_created_ = false;
  }

  if (registered_with_ecal_) {
    registered_with_ecal_ = false;
    if (init_refs_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      eCAL::Finalize();
    }
  }
}


