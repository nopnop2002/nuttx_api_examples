/****************************************************************************
 * examples/nuttx_api_task/task_test3_main.c
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
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <syslog.h>

#if CONFIG_VERSION_MAJOR >= 12
#define g_system_timer g_system_ticks
#endif

#define STACKSIZE 2048
#define PRIORITY SCHED_PRIORITY_DEFAULT

/****************************************************************************
 * Public Functions
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/
static const char *g_argv[5];
static FAR struct task_tcb_s *tcb1;
static FAR struct task_tcb_s *tcb2;
static FAR uint32_t *stack1;
static FAR uint32_t *stack2;
static pid_t pid1;
static pid_t pid2;
static struct sched_param param1;
static struct sched_param param2;


/****************************************************************************
 * Private Functions
 ****************************************************************************/

// This function searches for prime numbers in the most primitive way possible.
// This is used to gain time.

static void get_primes(int *count, int *last)
{
  int number;
  int local_count = 0;

  *last = 0;    /* To make the compiler happy */

  for (number = 1; number < 100000; number++)
  {
    int div;
    bool is_prime = true;

    for (div = 2; div <= number / 2; div++)
      if (number % div == 0)
      {
        is_prime = false;
        break;
      }

    if (is_prime)
    {
      local_count++;
      *last = number;
#if 0 /* We don't really care what the numbers are */
      syslog(LOG_INFO, " Prime %d: %d\n", local_count, number);
#endif
    }
  }

  *count = local_count;
}


// Task Body
static void task_entry(int argc, char * argv[]) {
  pid_t myPid = getpid();
  int loop=atoi(argv[1]);
  int wait=atoi(argv[2]);
  syslog(LOG_INFO, "%s start PID:%d loop:%d wait:%d system_timer:%ld\n",argv[0],myPid,loop,wait,g_system_timer);
#if 0
  syslog(LOG_INFO, "argc=%d\n",argc);
  for(int i=0;i<argc;i++) {
    syslog(LOG_INFO, "argv=%s\n",argv[i]);
  }
#endif
  int count;
  int last;
  for(int i=0;i<loop;i++) {
    if(wait) {
      sleep(wait);
    } else {
      get_primes(&count, &last);
    }
  }
  syslog(LOG_INFO, "%s end PID:%d system_timer:%ld\n",argv[0],myPid,g_system_timer);
}

/****************************************************************************
 * task_test3_main
 ****************************************************************************/

#if defined(BUILD_MODULE)
int main(int argc, FAR char *argv[])
#else
int task_test3_main(int argc, char *argv[])
#endif
{
  int prio_std = SCHED_PRIORITY_DEFAULT;
  int prio_max = sched_get_priority_max(SCHED_FIFO);
  int prio_min = sched_get_priority_min(SCHED_FIFO);

  if (strcmp(argv[1],"init") == 0) {
    tcb1 = (FAR struct task_tcb_s *)zalloc(sizeof(struct task_tcb_s));
    tcb2 = (FAR struct task_tcb_s *)zalloc(sizeof(struct task_tcb_s));
    stack1 = (FAR uint32_t *)malloc(STACKSIZE);
    stack2 = (FAR uint32_t *)malloc(STACKSIZE);
    int loop = 50000000;
    char wk0[10];
    sprintf(wk0,"%d",loop);
    g_argv[0] = wk0;
    int wait = 0;
    char wk1[10];
    sprintf(wk1,"%d",wait);
    g_argv[1] = wk1;
    g_argv[2] = NULL;
    int ret;
    UNUSED(ret);
#if CONFIG_VERSION_MAJOR >= 11
    ret = nxtask_init((FAR struct task_tcb_s *)tcb1,"myTask1",prio_std,stack1,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv,NULL);
    ret = nxtask_init((FAR struct task_tcb_s *)tcb2,"myTask2",prio_std,stack2,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv,NULL);
#else
    ret = nxtask_init((FAR struct task_tcb_s *)tcb1,"myTask1",prio_std,stack1,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
    ret = nxtask_init((FAR struct task_tcb_s *)tcb2,"myTask2",prio_std,stack2,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
#endif
    pid1 = tcb1->cmn.pid;
    pid2 = tcb2->cmn.pid;
    syslog(LOG_INFO, "pid1=%d pid2=%d\n",pid1,pid2);
    sched_getparam(pid1,&param1);
    sched_getparam(pid2,&param2);
  } else if (strcmp(argv[1],"activate") == 0) {
    nxtask_activate((FAR struct tcb_s *)tcb1);
    nxtask_activate((FAR struct tcb_s *)tcb2);
  } else if (strcmp(argv[1],"rr") == 0) {
    sched_setscheduler(pid1,SCHED_RR,&param1);
    sched_setscheduler(pid2,SCHED_RR,&param2);
  } else if (strcmp(argv[1],"fifo") == 0) {
    sched_setscheduler(pid1,SCHED_FIFO,&param1);
    sched_setscheduler(pid2,SCHED_FIFO,&param2);
  } else if (strcmp(argv[1],"help") == 0) {
    syslog(LOG_INFO, "Usage:\n");
    syslog(LOG_INFO, "      task_test3 init : initialize task\n");
    syslog(LOG_INFO, "      task_test3 rr : setscheduler SCHED_RR\n");
    syslog(LOG_INFO, "      task_test3 fifo : setscheduler SCHED_FIFO\n");
    syslog(LOG_INFO, "      task_test3 activate : activate task\n");
  } else {
    syslog(LOG_INFO, "Task Control Interfaces example\n");
    syslog(LOG_INFO, "sched_get_priority_std=%d\n",prio_std);
    syslog(LOG_INFO, "sched_get_priority_max=%d\n",prio_max);
    syslog(LOG_INFO, "sched_get_priority_min=%d\n",prio_min);
    syslog(LOG_INFO, "CONFIG_VERSION_MAJOR=%d\n",CONFIG_VERSION_MAJOR);
    syslog(LOG_INFO, "CONFIG_VERSION_MINOR=%d\n",CONFIG_VERSION_MINOR);
    syslog(LOG_INFO, "CONFIG_VERSION_PATCH=%d\n",CONFIG_VERSION_PATCH);
  }
  return 0;
}
