#include "ProcessHypervisor.hpp"

namespace async {

ProcessGroup ProcessHypervisor::defaultGroup;
std::vector<ProcessGroup> ProcessHypervisor::groups;

void ProcessHypervisor::register_process(Process& process) {
    defaultGroup.push_back(&process);
}

void ProcessHypervisor::register_group(ProcessGroup& group) {
    groups.push_back(group);
}

void ProcessHypervisor::start_all() {
    defaultGroup.start_all();
    for (auto& group : groups) {
        group.start_all();
    }
}

void ProcessHypervisor::toggle_all() {
    defaultGroup.toggle_all();
    for (auto& group : groups) {
        group.toggle_all();
    }
}

void ProcessHypervisor::work_all() {
    defaultGroup.work_all();
    for (auto& group : groups) {
        group.work_all();
    }
}

void ProcessHypervisor::stop_all() {
    defaultGroup.stop_all();
    for (auto& group : groups) {
        group.stop_all();
    }
}

void ProcessHypervisor::shutdown_all() {
    defaultGroup.shutdown_all();
    for (auto& group : groups) {
        group.shutdown_all();
    }
}

void ProcessHypervisor::kill_all() {
    defaultGroup.kill_all();
    for (auto& group : groups) {
        group.kill_all();
    }
}

std::optional<std::reference_wrapper<Process>> ProcessHypervisor::safe_select_process(std::string_view name) {
    auto process = defaultGroup.safe_select_process(name);
    if (process) {
        return process;
    }
    for (auto& group : groups) {
        process = group.safe_select_process(name);
        if (process) {
            return process;
        }
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Process>> ProcessHypervisor::safe_select_process(const uint16_t& id) {
    auto process = defaultGroup.safe_select_process(id);
    if (process) {
        return process;
    }
    for (auto& group : groups) {
        process = group.safe_select_process(id);
        if (process) {
            return process;
        }
    }
    return std::nullopt;
}

Process* ProcessHypervisor::select_process(std::string_view name) {
    auto process = defaultGroup.select_process(name);
    if (process) {
        return process;
    }
    for (auto& group : groups) {
        process = group.select_process(name);
        if (process) {
            return process;
        }
    }
    return nullptr;
}

Process* ProcessHypervisor::select_process(const uint16_t& id) {
    auto process = defaultGroup.select_process(id);
    if (process) {
        return process;
    }
    for (auto& group : groups) {
        process = group.select_process(id);
        if (process) {
            return process;
        }
    }
    return nullptr;
}

}
