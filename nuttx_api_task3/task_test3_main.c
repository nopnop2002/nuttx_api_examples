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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

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
      printf(" Prime %d: %d\n", local_count, number);
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
  printf("%s start PID:%d loop:%d wait:%d system_timer:%d\n",argv[0],myPid,loop,wait,g_system_timer);
#if 0
  printf("argc=%d\n",argc);
  for(int i=0;i<argc;i++) {
    printf("argv=%s\n",argv[i]);
  }
#endif
  int count;
  int last;
//  printf("%s loop=%d wait=%d\n",argv[0],loop,wait);
  for(int i=0;i<loop;i++) {
    if(wait) {
//      printf("%s i=%d g_system_timer=%d\n",argv[0],i,g_system_timer);
      sleep(wait);
    } else {
      get_primes(&count, &last);
    }
  }
  printf("%s end PID:%d system_timer:%d\n",argv[0],myPid,g_system_timer);
}

#if 0
// Task Launcher
static void task_fork(char *name, int priority, int loop, int wait) {
  int ret;
  UNUSED(ret);
  char wk0[10];
  sprintf(wk0,"%d",loop);
  g_argv[0] = wk0;
  char wk1[10];
  sprintf(wk1,"%d",wait);
  g_argv[1] = wk1;
  g_argv[2] = NULL;
  printf("task_create name:%s priority:%d\n",name, priority);
  ret = task_create(name,priority,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
}
#endif



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
    ret = task_init((FAR struct tcb_s *)tcb1,"myTask1",prio_std,stack1,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
    pid1 = tcb1->cmn.pid;
    ret = task_init((FAR struct tcb_s *)tcb2,"myTask2",prio_std,stack2,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
    pid2 = tcb2->cmn.pid;
    printf("pid1=%d pid2=%d\n",pid1,pid2);
    sched_getparam(pid1,&param1);
    sched_getparam(pid2,&param2);
  } else if (strcmp(argv[1],"activate") == 0) {
    task_activate((FAR struct tcb_s *)tcb1);
    task_activate((FAR struct tcb_s *)tcb2);
  } else if (strcmp(argv[1],"rr") == 0) {
    sched_setscheduler(pid1,SCHED_RR,&param1);
    sched_setscheduler(pid2,SCHED_RR,&param2);
  } else {
    printf("Task Scheduling Interfaces example\n");
    printf("sched_get_priority_max=%d\n",prio_max);
    printf("sched_get_priority_min=%d\n",prio_min);
  }
  return 0;
}
