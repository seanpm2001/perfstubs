// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/tool.h"
#include <iostream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>
#include <mutex>

static void __attribute__((constructor)) initme(void);
static void __attribute__((destructor)) finime(void);

static int tool_id;

using namespace std;

std::mutex my_mutex;

namespace external {
    namespace ps_implementation {
        class profiler {
            public:
                profiler(const std::string& name) : _name(name), _calls(0) {}
                void start() { _calls++; }
                void stop() { _calls++; }
                std::string _name;
                int _calls;
        };
        class counter {
            public:
                counter(const std::string& name) : _name(name) {}
                std::string _name;
        };
    }
}

namespace MINE = external::ps_implementation;
std::unordered_map<std::string, MINE::profiler*> profilers;
std::unordered_map<std::string, MINE::counter*> counters;

/* Function pointers */

extern "C"
{

    // On some systems, can't write output during pre-initialization
    void perftool_init(void) { /* cout << "Tool: " << __func__ << endl; */ }

    // On some systems, can't write output during pre-initialization
    void perftool_register_thread(void)
    {
        /* cout << "Tool: " << __func__ << endl; */
    }

    void perftool_exit(void) { cout << "Tool: " << __func__ << endl; }

    void perftool_dump_data(void) { cout << "Tool: " << __func__ << endl; }

    void * find_timer(const char * timer_name) {
        std::string name(timer_name);
        std::lock_guard<std::mutex> guard(my_mutex);
        auto iter = profilers.find(name);
        if (iter == profilers.end()) {
            MINE::profiler * p = new MINE::profiler(name);
            profilers.insert(std::pair<std::string,MINE::profiler*>(name,p));
            return (void*)p;
        }
        return (void*)iter->second;
    }

    void * perftool_timer_create(const char * timer_name)
    {
        cout << "Tool: " << __func__ << " " << timer_name << endl;
        return find_timer(timer_name);
    }

    void perftool_timer_start(const void *profiler)
    {
        MINE::profiler* p = (MINE::profiler*) profiler;
        cout << "Tool: " << __func__ << " " << p->_name << endl;
        p->start();
    }

    void perftool_timer_stop(const void *profiler)
    {
        MINE::profiler* p = (MINE::profiler*) profiler;
        cout << "Tool: " << __func__ << " " << p->_name << endl;
        p->stop();
    }

    void perftool_set_parameter(const char *parameter_name, int64_t parameter_value)
    {
        cout << "Tool: " << __func__ << " " << parameter_name
             << " " << parameter_value << endl;
    }

    void perftool_dynamic_phase_start(const char *phase_prefix,
                                      int iteration_index)
    {
        cout << "Tool: " << __func__ << " " << phase_prefix << ", "
             << iteration_index << endl;
    }

    void perftool_dynamic_phase_stop(const char *phase_prefix,
                                     int iteration_index)
    {
        cout << "Tool: " << __func__ << " " << phase_prefix << ", "
             << iteration_index << endl;
    }

    void* perftool_create_counter(const char *counter_name)
    {
        cout << "Tool: " << __func__ << " " << counter_name << endl;
        std::string name(counter_name);
        std::lock_guard<std::mutex> guard(my_mutex);
        auto iter = counters.find(name);
        if (iter == counters.end()) {
            MINE::counter * c = new MINE::counter(name);
            counters.insert(std::pair<std::string,MINE::counter*>(name,c));
            return (void*)c;
        }
        return (void*)iter->second;
    }

    void perftool_sample_counter(const void *counter, double value)
    {
        MINE::counter* c = (MINE::counter*) counter;
        cout << "Tool: " << __func__ << " " << c->_name << " = " << value
             << endl;
    }

    void perftool_set_metadata(const char *name, const char *value)
    {
        cout << "Tool: " << __func__ << " " << name << " = " << value << endl;
    }

    void perftool_get_timer_data(ps_tool_timer_data_t *timer_data)
    {
        cout << "Tool: " << __func__ << endl;
        memset(timer_data, 0, sizeof(ps_tool_timer_data_t));
        timer_data->num_timers = 1;
        timer_data->num_threads = 1;
        timer_data->num_metrics = 3;
        timer_data->timer_names = (char **)(calloc(1, sizeof(char *)));
        timer_data->metric_names = (char **)(calloc(3, sizeof(char *)));
        timer_data->values = (double *)(calloc(3, sizeof(double)));
        timer_data->timer_names[0] = strdup("Main Timer");
        timer_data->metric_names[0] = strdup("Calls");
        timer_data->metric_names[1] = strdup("Inclusive Time");
        timer_data->metric_names[2] = strdup("Exclusive Time");
        timer_data->values[0] = 1.0;
        timer_data->values[1] = 20.0;
        timer_data->values[2] = 10.0;
        return;
    }

    void perftool_free_timer_data(ps_tool_timer_data_t *timer_data)
    {
        if (timer_data == nullptr)
        {
            return;
        }
        if (timer_data->timer_names != nullptr)
        {
            free(timer_data->timer_names);
            timer_data->timer_names = nullptr;
        }
        if (timer_data->metric_names != nullptr)
        {
            free(timer_data->metric_names);
            timer_data->metric_names = nullptr;
        }
        if (timer_data->values != nullptr)
        {
            free(timer_data->values);
            timer_data->values = nullptr;
        }
    }

    void perftool_get_counter_data(ps_tool_counter_data_t *counter_data)
    {
        cout << "Tool: " << __func__ << endl;
        memset(counter_data, 0, sizeof(ps_tool_counter_data_t));
        counter_data->num_counters = 1;
        counter_data->num_threads = 1;
        counter_data->counter_names = (char **)(calloc(1, sizeof(char *)));
        counter_data->num_samples = (double *)(calloc(1, sizeof(double)));
        counter_data->value_total = (double *)(calloc(1, sizeof(double)));
        counter_data->value_min = (double *)(calloc(1, sizeof(double)));
        counter_data->value_max = (double *)(calloc(1, sizeof(double)));
        counter_data->value_sumsqr = (double *)(calloc(1, sizeof(double)));
        counter_data->counter_names[0] = strdup("A Counter");
        counter_data->num_samples[0] = 3.0;
        counter_data->value_total[0] = 6.0;
        counter_data->value_min[0] = 1.0;
        counter_data->value_max[0] = 3.0;
        counter_data->value_sumsqr[0] = 1.414213562373095;
        return;
    }

    void perftool_free_counter_data(ps_tool_counter_data_t *counter_data)
    {
        if (counter_data == nullptr)
        {
            return;
        }
        if (counter_data->counter_names != nullptr)
        {
            free(counter_data->counter_names);
            counter_data->counter_names = nullptr;
        }
        if (counter_data->num_samples != nullptr)
        {
            free(counter_data->num_samples);
            counter_data->num_samples = nullptr;
        }
        if (counter_data->value_total != nullptr)
        {
            free(counter_data->value_total);
            counter_data->value_total = nullptr;
        }
        if (counter_data->value_min != nullptr)
        {
            free(counter_data->value_min);
            counter_data->value_min = nullptr;
        }
        if (counter_data->value_max != nullptr)
        {
            free(counter_data->value_max);
            counter_data->value_max = nullptr;
        }
        if (counter_data->value_sumsqr != nullptr)
        {
            free(counter_data->value_sumsqr);
            counter_data->value_sumsqr = nullptr;
        }
    }

    void perftool_get_metadata(ps_tool_metadata_t *metadata)
    {
        cout << "Tool: " << __func__ << endl;
        memset(metadata, 0, sizeof(ps_tool_metadata_t));
        metadata->num_values = 1;
        metadata->names = (char **)(calloc(1, sizeof(char *)));
        metadata->values = (char **)(calloc(1, sizeof(char *)));
        metadata->names[0] = strdup("Name string");
        metadata->values[0] = strdup("Value string");
        return;
    }

    void perftool_free_metadata(ps_tool_metadata_t *metadata)
    {
        if (metadata == nullptr)
        {
            return;
        }
        if (metadata->names != nullptr)
        {
            free(metadata->names);
            metadata->names = nullptr;
        }
        if (metadata->values != nullptr)
        {
            free(metadata->values);
            metadata->values = nullptr;
        }
    }
}

static void initme(void) {
    ps_plugin_data_t data;
    ps_register_t reg_function;
    reg_function = &ps_register_tool;
    if (reg_function != NULL) {
        memset(&data, 0, sizeof(ps_plugin_data_t));
        data.tool_name = strdup("tool one");
        /* Logistical functions */
        data.initialize = &perftool_init;
        data.finalize = &perftool_exit;
        data.register_thread = &perftool_register_thread;
        data.dump_data = &perftool_dump_data;
        /* Data entry functions */
        data.timer_create = &perftool_timer_create;
        data.timer_start = &perftool_timer_start;
        data.timer_stop = &perftool_timer_stop;
        data.set_parameter = &perftool_set_parameter;
        data.dynamic_phase_start = &perftool_dynamic_phase_start;
        data.dynamic_phase_stop = &perftool_dynamic_phase_stop;
        data.create_counter = &perftool_create_counter;
        data.sample_counter = &perftool_sample_counter;
        data.set_metadata = &perftool_set_metadata;
        /* Data Query Functions */
        data.get_timer_data = &perftool_get_timer_data;
        data.get_counter_data = &perftool_get_counter_data;
        data.get_metadata = &perftool_get_metadata;
        data.free_timer_data = &perftool_free_timer_data;
        data.free_counter_data = &perftool_free_counter_data;
        data.free_metadata = &perftool_free_metadata;
        tool_id = reg_function(&data);
    }
}

static void finime(void) {
    ps_deregister_t dereg_function;
    dereg_function = &ps_deregister_tool;
    if (dereg_function != NULL) {
        dereg_function(tool_id);
    }
}

__attribute__((visibility("default")))
__attribute__((weak)) int ps_register_tool(ps_plugin_data_t * tool);

__attribute__((visibility("default")))
__attribute__((weak)) void ps_deregister_tool(int tool_id);