#ifndef _PROCESSGROUP_HPP_
#define _PROCESSGROUP_HPP_

#include <vector>
#include <optional>
#include <functional>
#include <string>
#include <string_view>
#include "Process.hpp"

namespace async {

/**
 * @brief Manages a collection of Process objects as a single unit.
 * 
 * ProcessGroup allows you to start, stop, pause, or modify multiple
 * processes simultaneously. It acts as a container (inheriting from std::vector)
 * and provides bulk operations for process management.
 */
class ProcessGroup : public std::vector<Process*> {
private:
    std::string name = "";
    void new_id();

public:
    /**
     * @brief Construct a new Process Group object.
     */
    ProcessGroup();

    /**
     * @brief Construct a new Process Group object with initial processes.
     * 
     * @tparam Processes Variadic template for Process references.
     * @param processes One or more Process objects to add to the group.
     */
    template<typename ...Processes>
    ProcessGroup(Processes&... processes) {
        auto proc_list = {&processes...};
        this->reserve(proc_list.size());
        for (auto&& process : proc_list) {
            this->emplace_back(process);
        }
    }

    ~ProcessGroup() = default;

    /**
     * @brief Get the name of the process group.
     * @return Name of the group (view).
     */
    std::string_view get_name() const;

    /**
     * @brief Set the name of the process group.
     * @param name Name to assign.
     */
    void set_name(std::string_view name);

    void avg_looptime() const; // TODO: Implement average loop time calculation.

    /**
     * @brief Starts all processes in the group.
     */
    void start_all();

    /**
     * @brief Toggles the state (start/stop) of all processes in the group.
     */
    void toggle_all();

    /**
     * @brief Forces a single work iteration for all processes in the group.
     */
    void work_all();

    /**
     * @brief Stops all processes in the group.
     */
    void stop_all();

    /**
     * @brief Shuts down all processes in the group (triggers on_shutdown).
     */
    void shutdown_all();

    /**
     * @brief Kills all processes in the group (removes them from runtime).
     */
    void kill_all();

    /**
     * @brief Selects a process by name.
     * @param name Name of the process to find.
     * @return Process* Pointer to the found process or nullptr.
     */
    Process* select_process(std::string_view name);

    /**
     * @brief Selects a process by ID.
     * @param id ID of the process to find.
     * @return Process* Pointer to the found process or nullptr.
     */
    Process* select_process(uint16_t id);

    /**
     * @brief Safely selects a process by name returning an optional reference.
     * @param name Name of the process to find.
     * @return std::optional<std::reference_wrapper<Process>> Found process or std::nullopt.
     */
    std::optional<std::reference_wrapper<Process>> safe_select_process(std::string_view name) const;

    /**
     * @brief Safely selects a process by ID returning an optional reference.
     * @param id ID of the process to find.
     * @return std::optional<std::reference_wrapper<Process>> Found process or std::nullopt.
     */
    std::optional<std::reference_wrapper<Process>> safe_select_process(uint16_t id) const;
};
}

#endif
