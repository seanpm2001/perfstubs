#!/usr/bin/env python3

"""Python instrumentation interface for PerfStubs"""

#__all__ = ["run", "runctx", "exitAllThreads", "help", "Profile"]

import os.path
import sys
import perfstubs
import pstubs_common as ps

def init_tracing():
    DEBUGGER_ID = sys.monitoring.DEBUGGER_ID
    sys.monitoring.use_tool_id(DEBUGGER_ID, "perfstubs")
    sys.monitoring.set_events(DEBUGGER_ID, sys.monitoring.events.PY_START | sys.monitoring.events.PY_RETURN )
    perfstubs.initialize()
    def pep_669_py_start_trace(code, instruction_offset):
        # If we should filter this event, do it
        if code.co_name in ps.exclude_timers:
            return sys.monitoring.DISABLE
        if not ps.internal_timers and (ps.python_system_path in code.co_filename or ps.python_frozen_path in code.co_filename):
            return sys.monitoring.DISABLE
        frame = sys._getframe(1)
        #perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
        perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
    def pep_669_py_stop_trace(code, instruction_offset, retval):
        # If we should filter this event, do it
        if code.co_name in ps.exclude_timers:
            return sys.monitoring.DISABLE
        if not ps.internal_timers and (ps.python_system_path in code.co_filename or ps.python_frozen_path in code.co_filename):
            return sys.monitoring.DISABLE
        frame = sys._getframe(1)
        #perfstubs.stop(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
        perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
    def pep_669_call_trace(code, instruction_offset, callable, arg0):
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    def pep_669_c_return_trace(code, instruction_offset, callable, arg0):
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_START,
        pep_669_py_start_trace)
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_RETURN,
        pep_669_py_stop_trace)

    # Not sure about these two events yet...
    #sys.monitoring.register_callback(
    #    DEBUGGER_ID,
    #    sys.monitoring.events.CALL,
    #    pep_669_call_trace)
    #sys.monitoring.register_callback(
    #    DEBUGGER_ID,
    #    sys.monitoring.events.C_RETURN,
    #    pep_669_c_return_trace)

def fini_tracing():
    DEBUGGER_ID = sys.monitoring.DEBUGGER_ID
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_START,
        None)
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_RETURN,
        None)
    perfstubs.finalize()

