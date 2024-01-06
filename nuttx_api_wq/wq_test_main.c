/****************************************************************************
 * examples/nuttx_api_wq/wq_test_main.c
 *
 *   Copyright (C) 2008, 2011-2012 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/version.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <nuttx/wqueue.h>
#include <syslog.h>

#define STACKSIZE 2048
#define PRIORITY SCHED_PRIORITY_DEFAULT


/****************************************************************************
 * Public Functions
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/
//static const char *g_argv[5];
static struct work_s worker1;
static struct work_s worker2;
static uint16_t work1_delay = 1000;
static uint16_t work2_delay = 2000;
static uint16_t work1_stop = 0;
static uint16_t work2_stop = 0;



/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void work1_loop(int argc, char *argv[]) {
  clock_t start_ticks = g_system_timer;
  pid_t myPid = getpid();
  syslog(LOG_DEBUG,"work1_loop started PID:%d work1_delay:%d work1_stop:%d start_ticks:%ld\n",myPid, work1_delay, work1_stop, start_ticks);
  if (work1_stop == 0) {
    memset(&worker1, 0, sizeof(worker1));
    int ret = work_queue(HPWORK, &worker1, (worker_t)work1_loop, NULL, MSEC2TICK(work1_delay));
    if (ret != 0) printf("work_queue ret=%d\n",ret);
  }
#if 0
  clock_t last_ticks = g_system_timer;
  clock_t elapsed = last_ticks - start_ticks;
  syslog(LOG_DEBUG,"work1_loop ended last_ticks=%d elapsed\n",last_ticks,elapsed);
#endif
}

static void work2_loop(int argc, char *argv[]) {
  clock_t start_ticks = g_system_timer;
  pid_t myPid = getpid();
  syslog(LOG_DEBUG,"work2_loop started PID:%d work2_delay:%d work2_stop:%d start_ticks:%ld\n",myPid, work2_delay, work2_stop, start_ticks);
  if (work2_stop == 0) {
    memset(&worker2, 0, sizeof(worker2));
    int ret = work_queue(LPWORK, &worker2, (worker_t)work2_loop, NULL, MSEC2TICK(work2_delay));
    if (ret != 0) printf("work_queue ret=%d\n",ret);
  }
#if 0
  clock_t last_ticks = g_system_timer;
  clock_t elapsed = last_ticks - start_ticks;
  syslog(LOG_DEBUG,"work2_loop ended last_ticks=%d elapsed\n",last_ticks,elapsed);
#endif
}



/****************************************************************************
 * wq_test_main
 ****************************************************************************/

#if defined(BUILD_MODULE)
int main(int argc, FAR char *argv[])
#else
int wq_test_main(int argc, char *argv[])
#endif
{
//  int prio_std = SCHED_PRIORITY_DEFAULT;

  if (strcmp(argv[1],"tick") == 0) {
    printf("USEC_PER_TICK=%d[us]\n",USEC_PER_TICK);
    printf("MSEC_PER_TICK=%d[ms]\n",USEC_PER_TICK/1000);
  } else if (strcmp(argv[1],"start1") == 0) {
    printf("MSEC2TICK=%d\n",MSEC2TICK(work1_delay));
    memset(&worker1, 0, sizeof(worker1));
    int ret = work_queue(HPWORK, &worker1, (worker_t)work1_loop, NULL, MSEC2TICK(work1_delay));
    if (ret != 0) printf("work_queue ret=%d\n",ret);
  } else if (strcmp(argv[1],"stop1") == 0) {
    work1_stop = 1;
  } else if (strcmp(argv[1],"start2") == 0) {
    printf("MSEC2TICK=%d\n",MSEC2TICK(work2_delay));
    memset(&worker2, 0, sizeof(worker2));
    int ret = work_queue(LPWORK, &worker2, (worker_t)work2_loop, NULL, MSEC2TICK(work2_delay));
    if (ret != 0) printf("work_queue ret=%d\n",ret);
  } else if (strcmp(argv[1],"stop2") == 0) {
    work2_stop = 1;
  } else {
    printf("Work Queue Interfaces example\n");
    printf("CONFIG_VERSION_MAJOR=%d\n",CONFIG_VERSION_MAJOR);
    printf("CONFIG_VERSION_MINOR=%d\n",CONFIG_VERSION_MINOR);
    printf("CONFIG_VERSION_PATCH=%d\n",CONFIG_VERSION_PATCH);
  }
  return 0;
}
