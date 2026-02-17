#ifndef _PROCESSHYPERVISOR_HPP_
#define _PROCESSHYPERVISOR_HPP_

#include "Arduino.h"
#include <vector>
#include <optional>
#include <functional>
#include "ProcessGroup.hpp"

namespace async {
class ProcessHypervisor {
private:
    static ProcessGroup defaultGroup;
    static std::vector<ProcessGroup> groups;

public:
    // Add a process to the default group
    static void register_process(Process& process);

    // Add a group to the hypervisor
    static void register_group(ProcessGroup& group);

    // Functions to operate on all groups including the default group
    static void start_all();
    static void toggle_all();
    static void work_all();
    static void stop_all();
    static void shutdown_all();
    static void kill_all();

    // Functions to select processes
    static std::optional<std::reference_wrapper<Process>> safe_select_process(const String& name);
    static std::optional<std::reference_wrapper<Process>> safe_select_process(const uint16_t& id);
    static Process* select_process(const String& name);
    static Process* select_process(const uint16_t& id);
};
}

#endif
