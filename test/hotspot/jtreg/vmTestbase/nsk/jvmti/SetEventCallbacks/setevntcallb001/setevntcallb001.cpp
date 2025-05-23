/*
 * Copyright (c) 2003, 2024, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <string.h>
#include "jvmti.h"
#include "agent_common.hpp"
#include "jni_tools.h"
#include "jvmti_tools.h"

extern "C" {

/* ============================================================================= */

/* scaffold objects */
static jlong timeout = 0;

/* constants */
#define EVENTS_COUNT    3

/* tested events */
static jvmtiEvent eventsList[EVENTS_COUNT] = {
    JVMTI_EVENT_VM_INIT,
    JVMTI_EVENT_THREAD_START,
    JVMTI_EVENT_THREAD_END
};

static const char* eventsNameList[EVENTS_COUNT] = {
    "JVMTI_EVENT_VM_INIT",
    "JVMTI_EVENT_THREAD_START",
    "JVMTI_EVENT_THREAD_END"
};

static int eventsCountList[EVENTS_COUNT];

/* ============================================================================= */

/** Clear events counters. */
static void cleanEventCounts() {
    int i;

    for (i = 0; i < EVENTS_COUNT; i++) {
        eventsCountList[i] = 0;
    }
}

/** Check if all expected events received. */
static int checkEventCounts() {
    int success = NSK_TRUE;
    int i;

    NSK_DISPLAY0("Callbacks invoked:\n");
    for (i = 0; i < EVENTS_COUNT; i++) {
        NSK_DISPLAY2("   %s: %d times\n",
                            eventsNameList[i], eventsCountList[i]);
    }

    for (i = 0; i < EVENTS_COUNT; i++) {
        if (eventsCountList[i] <= 0) {
            NSK_COMPLAIN2("# No %s event callback invoked:\n"
                          "#   invoked: %d times\n",
                          eventsNameList[i], eventsCountList[i]);
            success = NSK_FALSE;
        }
    }

    return success;
}

/* ============================================================================= */

/** Agent algorithm. */
static void JNICALL
agentProc(jvmtiEnv* jvmti, JNIEnv* jni, void* arg) {
    NSK_DISPLAY0("Wait for debugee to generate events\n");
    if (!nsk_jvmti_waitForSync(timeout))
        return;

    NSK_DISPLAY0(">>> Testcase #3: Check if all events callbacks were invoked\n");
    if (!checkEventCounts()) {
        nsk_jvmti_setFailStatus();
    }

    NSK_DISPLAY0("Disable events\n");
    nsk_jvmti_enableEvents(JVMTI_DISABLE, EVENTS_COUNT, eventsList, nullptr);

    NSK_DISPLAY0("Let debugee to finish\n");
    if (!nsk_jvmti_resumeSync())
        return;
}

/* ============================================================================= */

/**
 * Callback for THREAD_START event.
 */
JNIEXPORT void JNICALL
callbackThreadStart(jvmtiEnv* jvmti, JNIEnv* jni, jthread thread) {
    NSK_DISPLAY1("  <THREAD_START>: thread: 0x%p\n", (void*)thread);
    eventsCountList[1]++;
}

/**
 * Callback for THREAD_END event.
 */
JNIEXPORT void JNICALL
callbackThreadEnd(jvmtiEnv* jvmti, JNIEnv* jni, jthread thread) {
    NSK_DISPLAY1("  <THREAD_END>:   thread: 0x%p\n", (void*)thread);
    eventsCountList[2]++;
}

/**
 * Callback for VM_INIT event.
 */
JNIEXPORT void JNICALL
callbackVMInit(jvmtiEnv* jvmti, JNIEnv* jni, jthread thread) {
    NSK_DISPLAY1("  <VM_INIT>: thread: 0x%p\n", (void*)thread);
    eventsCountList[0]++;

    NSK_DISPLAY0(">>> Testcase #2: Set callbacks for THREAD_START and THREAD_END events\n");
    {
        jvmtiEventCallbacks eventCallbacks;

        memset(&eventCallbacks, 0, sizeof(eventCallbacks));
        eventCallbacks.ThreadStart = callbackThreadStart;
        eventCallbacks.ThreadEnd = callbackThreadEnd;
        if (!NSK_JVMTI_VERIFY(jvmti->SetEventCallbacks(&eventCallbacks, sizeof(eventCallbacks)))) {
            nsk_jvmti_setFailStatus();
        } else {
            nsk_jvmti_enableEvents(JVMTI_ENABLE, EVENTS_COUNT - 1, eventsList + 1, nullptr);
        }
    }
}

/* ============================================================================= */

/** Agent library initialization. */
#ifdef STATIC_BUILD
JNIEXPORT jint JNICALL Agent_OnLoad_setevntcallb001(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNICALL Agent_OnAttach_setevntcallb001(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNI_OnLoad_setevntcallb001(JavaVM *jvm, char *options, void *reserved) {
    return JNI_VERSION_1_8;
}
#endif
jint Agent_Initialize(JavaVM *jvm, char *options, void *reserved) {
    jvmtiEnv* jvmti = nullptr;

    if (!NSK_VERIFY(nsk_jvmti_parseOptions(options)))
        return JNI_ERR;

    timeout = nsk_jvmti_getWaitTime() * 60 * 1000;

    if (!NSK_VERIFY((jvmti =
            nsk_jvmti_createJVMTIEnv(jvm, reserved)) != nullptr))
        return JNI_ERR;

    if (!NSK_VERIFY(nsk_jvmti_setAgentProc(agentProc, nullptr)))
        return JNI_ERR;

    NSK_DISPLAY0(">>> Testcase #1: Set callbacks for VM_INIT event\n");
    {
        jvmtiEventCallbacks eventCallbacks;

        cleanEventCounts();

        memset(&eventCallbacks, 0, sizeof(eventCallbacks));
        eventCallbacks.VMInit = callbackVMInit;
        if (!NSK_JVMTI_VERIFY(jvmti->SetEventCallbacks(&eventCallbacks, sizeof(eventCallbacks)))) {
            nsk_jvmti_setFailStatus();
        } else {
            nsk_jvmti_enableEvents(JVMTI_ENABLE, 1, eventsList, nullptr);
        }
    }
    return JNI_OK;
}

/* ============================================================================= */

}
