// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#pragma once

#include "perfstubs_api/Config.h"

/* ------------------------------------------------------------------ */
/* Define the C++ API and PerfStubs glue class first */
/* ------------------------------------------------------------------ */

#ifdef __cplusplus

#if defined(PERFSTUBS_USE_TIMERS)

#include <memory>
#include <sstream>
#include <string>

namespace external
{

namespace profiler
{

class Timer
{
public:
    static void RegisterThread(void);
    // data measurement api
    static void Start(const char *timer_name);
    static void Start(const std::string &timer_name);
    static void StaticPhaseStart(const char *phase_name);
    static void StaticPhaseStart(const std::string &phase_name);
    static void DynamicPhaseStart(const char *phase_prefix,
                                  int iteration_index);
    static void DynamicPhaseStart(const std::string &phase_prefix,
                                  int iteration_index);
    static void Stop(const char *timer_name);
    static void Stop(const std::string &timer_name);
    static void StaticPhaseStop(const char *phase_name);
    static void StaticPhaseStop(const std::string &phase_name);
    static void DynamicPhaseStop(const char *phase_prefix,
                                 int iteration_index);
    static void DynamicPhaseStop(const std::string &phase_prefix,
                                 int iteration_index);
    static void SampleCounter(const char *name, const double value);
    static void MetaData(const char *name, const char *value);

    // data query api
    // data query function declarations
    static int GetTimerNames(char **timer_names[]);
    static int GetTimerMetricNames(char **metric_names[]);
    static int GetThreadCount(void);
    static int GetTimerData(double *timer_values[]);
    static int GetCounterNames(char **counter_names[]);
    static int GetCounterMetricNames(char **metric_names[]);
    static int GetCounterData(double *counter_values[]);
    static int GetMetaData(char **names[], char **values[]);

    // The only way to get an instance of this class
    static Timer &Get(void);
    // destructor
    ~Timer(void);
    bool m_Initialized;
    static thread_local bool m_ThreadSeen;

private:
    static void _RegisterThread(void);
    // Private constructor to prevent construction
    Timer(void);
    // Prevent copies
    Timer(const Timer &old);
    const Timer &operator=(const Timer &old);
};

class ScopedTimer
{
private:
    std::string m_Name;

public:
    ScopedTimer(const std::string &name) : m_Name(name)
    {
        Timer::Start(m_Name);
    }
    ~ScopedTimer() { Timer::Stop(m_Name); }
};

} // namespace external

} // namespace profiler

#define PERFSTUBS_INIT() external::profiler::Timer::Get();
#define PERFSTUBS_REGISTER_THREAD()                                            \
    external::profiler::Timer::RegisterThread();
#define PERFSTUBS_TIMER_START(_timer_name)                                     \
    external::profiler::Timer::Start(_timer_name);
#define PERFSTUBS_TIMER_STOP(_timer_name)                                      \
    external::profiler::Timer::Stop(_timer_name);
#define PERFSTUBS_STATIC_PHASE_START(_phase_name)                              \
    external::profiler::Timer::StaticPhaseStart(_phase_name);
#define PERFSTUBS_STATIC_PHASE_STOP(_phase_name)                               \
    external::profiler::Timer::StaticPhaseStop(_phase_name);
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)         \
    external::profiler::Timer::DynamicPhaseStart(_phase_prefix,                \
    _iteration_index);
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)          \
    external::profiler::Timer::DynamicPhaseStop(_phase_prefix,                 \
    _iteration_index);
#define PERFSTUBS_TIMER_START_FUNC()                                           \
    std::stringstream __perfstubsFuncNameSS;                                   \
    __perfstubsFuncNameSS <<  __func__ << " [{" << __FILE__ << "} {"           \
            << __LINE__ << ",0}]";                                             \
    std::string __perfStubsFuncName(__perfStubsFuncNameSS);                    \
    external::profiler::Timer::Start(__perfstubsFuncName);
#define PERFSTUBS_TIMER_STOP_FUNC()                                            \
    external::profiler::Timer::Stop(__perfstubsFuncName);
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)                                \
    external::profiler::Timer::SampleCounter(_name, _value);
#define PERFSTUBS_METADATA(_name, _value)                                      \
    external::profiler::Timer::MetaData(_name, _value);
#define PERFSTUBS_SCOPED_TIMER(__name)                                         \
    external::profiler::ScopedTimer __var##finfo(__name);
#define PERFSTUBS_SCOPED_TIMER_FUNC()                                          \
    std::stringstream __ss##finfo;                                             \
    __ss##finfo << __func__ << " [{" << __FILE__ << "} {" << __LINE__          \
                << ",0}]";                                                     \
    external::profiler::ScopedTimer __var##finfo(__ss##finfo.str());

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START(_timer_name)
#define PERFSTUBS_TIMER_STOP(_timer_name)
#define PERFSTUBS_STATIC_PHASE_START(_phase_name)
#define PERFSTUBS_STATIC_PHASE_STOP(_phase_name)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, iteration_index)
#define PERFSTUBS_TIMER_START_FUNC()
#define PERFSTUBS_TIMER_STOP_FUNC()
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)
#define PERFSTUBS_SCOPED_TIMER(__name)
#define PERFSTUBS_SCOPED_TIMER_FUNC()

#endif // defined(PERFSTUBS_USE_TIMERS)

#else // ifdef __cplusplus

/* ------------------------------------------------------------------ */
/* Now define the C API */
/* ------------------------------------------------------------------ */

#if defined(PERFSTUBS_USE_TIMERS)

/* regular C API */

void psInit(void);
void psRegisterThread(void);
void psTimerStart(const char *timerName);
void psTimerStop(const char *timerName);
void psStaticPhaseStart(const char *phaseName);
void psStaticPhaseStop(const char *phaseName);
void psDynamicPhaseStart(const char *phasePrefix, int iterationIndex);
void psDynamicPhaseStop(const char *phasePrefix, int iterationIndex);
void psSampleCounter(const char *name, const double value);
void psMetaData(const char *name, const char *value);

/* data query API */

int psGetTimerNames(char **timer_names[]);
int psGetTimerMetricNames(char **metric_names[]);
int psGetThreadCount(void);
int psGetTimerData(double *timer_values[]);
int psGetCounterNames(char **counter_names[]);
int psGetCounterMetricNames(char **metric_names[]);
int psGetCounterData(double *counter_values[]);
int psGetMetaData(char **names[], char **values[]);

/*
    Macro API for option of entirely disabling at compile time
    To use this API, set the Macro PERFSTUBS_USE_TIMERS on the command
    line or in a config.h file, however your project does it
 */

#define PERFSTUBS_INIT() psInit();
#define PERFSTUBS_REGISTER_THREAD() psRegisterThread();
#define PERFSTUBS_TIMER_START(_timer_name) psTimerStart(_timer_name);
#define PERFSTUBS_TIMER_STOP(_timer_name) psTimerStop(_timer_name);
#define PERFSTUBS_STATIC_PHASE_START(_phase_name)                              \
    psStaticPhaseStart(_phase_name);
#define PERFSTUBS_STATIC_PHASE_STOP(_phase_name)                               \
    psStaticPhaseStop(_phase_name);
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)         \
    psDynamicPhaseStart(_phase_prefix, _iteration_index);
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)          \
    psDynamicPhaseStop(_phase_prefix, _iteration_index);
#define PERFSTUBS_TIMER_START_FUNC()                                           \
    char __perfstubsFuncName[1024];                                            \
    sprintf(__perfstubsFuncName, "%s [{%s} {%d,0}]", __func__, __FILE__,       \
            __LINE__);                                                         \
    psTimerStart(__perfstubsFuncName);
#define PERFSTUBS_TIMER_STOP_FUNC() psTimerStop(__perfstubsFuncName);
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)                                \
    psSampleCounter(_name, _value);
#define PERFSTUBS_METADATA(_name, _value) psMetaData(_name, _value);

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START(_timer_name)
#define PERFSTUBS_TIMER_STOP(_timer_name)
#define PERFSTUBS_STATIC_PHASE_START(_phase_name)
#define PERFSTUBS_STATIC_PHASE_STOP(_phase_name)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)
#define PERFSTUBS_TIMER_START_FUNC()
#define PERFSTUBS_TIMER_STOP_FUNC()
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)

#endif // defined(PERFSTUBS_USE_TIMERS)

#endif // ifdef __cplusplus
