/****************************************************************************
 * examples/nuttx_api_sem/sem_test_main.c
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
#include <semaphore.h>
#include <time.h>
#include <sched.h>
#include <errno.h>
#include <syslog.h>

#if CONFIG_VERSION_MAJOR >= 12
#define g_system_timer g_system_ticks
#endif

#define SEM1_NAME "foo"
#define SEM2_NAME "bar"
#define SEM3_NAME "baz"
#define STACKSIZE 2048
#define PRIORITY SCHED_PRIORITY_DEFAULT

/****************************************************************************
 * Public Functions
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/
static const char *g_argv[5];


/****************************************************************************
 * Private Functions
 ****************************************************************************/

// Task Body
static void task_entry(int argc, char * argv[]) {
  pid_t myPid = getpid();
  syslog(LOG_INFO, "%s start PID:%d system_timer:%ld\n",argv[0],myPid,g_system_timer);
#if 0
  syslog(LOG_INFO, "argc=%d\n",argc);
  for(int i=0;i<argc;i++) {
    syslog(LOG_INFO, "argv=%s\n",argv[i]);
  }
#endif
  syslog(LOG_INFO, "%s Open semaphore[%s]\n",argv[0],argv[1]);

  FAR sem_t *sem;
  int status;
  sem = sem_open(argv[1], 0);
  if (sem == SEM_FAILED) {
    int errcode = errno;
    syslog(LOG_ERR, "%s: ERROR: sem_open(1) failed: %d\n", argv[0],errcode);
    return;
  }

  while(1) {
    syslog(LOG_INFO, "%s Try Lock semaphore [%s]\n",argv[0],argv[1]);
    status = sem_wait(sem);
    if (status < 0) {
      int errcode = errno;
      syslog(LOG_ERR, "%s: ERROR: sem_wait(1) failed: %d\n",  argv[0],errcode);
      return;
    }
    syslog(LOG_INFO, "%s Lock success semaphore [%s]\n",argv[0],argv[1]);
  }
  sem_close(sem);
  syslog(LOG_INFO, "%s end PID:%d system_timer:%ld\n",argv[0],myPid,g_system_timer);
}

// Task Launcher
static void task_fork(char *name, int priority, char *sem) {
  int ret;
  UNUSED(ret);
  g_argv[0] = sem;
  g_argv[1] = NULL;
  syslog(LOG_INFO, "task_create name:%s priorty:%d\n",name, priority);
  ret = task_create(name,priority,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
}



/****************************************************************************
 * sem_test_main
 ****************************************************************************/

#if defined(BUILD_MODULE)
int main(int argc, FAR char *argv[])
#else
int sem_test_main(int argc, char *argv[])
#endif
{
  int prio_std = SCHED_PRIORITY_DEFAULT;
  FAR sem_t *sem1 = NULL;
  FAR sem_t *sem2 = NULL;
  FAR sem_t *sem3 = NULL;

  if (strcmp(argv[1],"start") == 0) {
    sem1 = sem_open(SEM1_NAME, O_CREAT|O_EXCL, 0644, 0);
    if (sem1 == SEM_FAILED) {
      int errcode = errno;
      syslog(LOG_ERR, "ERROR: sem_open(1) failed: %d\n", errcode);
      return 0;
    }
    sem2 = sem_open(SEM2_NAME, O_CREAT|O_EXCL, 0644, 0);
    if (sem2 == SEM_FAILED) {
      int errcode = errno;
      syslog(LOG_ERR, "ERROR: sem_open(1) failed: %d\n", errcode);
      return 0;
    }
    task_fork("myTask1", prio_std, SEM1_NAME);
    task_fork("myTask2", prio_std, SEM2_NAME);
  } else if (strcmp(argv[1],"foo") == 0) {
    sem_post(sem1);
  } else if (strcmp(argv[1],"bar") == 0) {
    sem_post(sem2);
  } else if (strcmp(argv[1],"add") == 0) {
    task_fork("myTask3", prio_std, SEM1_NAME);
  } else if (strcmp(argv[1],"test") == 0) {
    sem3 = sem_open(SEM3_NAME, O_CREAT|O_EXCL, 0644, 2);
    if (sem3 == SEM_FAILED) {
      int errcode = errno;
      syslog(LOG_ERR, "ERROR: sem_open(1) failed: %d\n", errcode);
      return 0;
    }
    int sval;
    struct timespec time;
    int status;
    for(int i=0;i<3;i++) {
      sem_getvalue(sem3, &sval);
      syslog(LOG_INFO, "BEFORE sem value=%d\n",sval);
      clock_gettime(CLOCK_REALTIME, &time);
      time.tv_sec += 2;
      status = sem_timedwait(sem3,&time);
      syslog(LOG_INFO, "sem_timedwait status=%d\n",status);
      sem_getvalue(sem3, &sval);
      syslog(LOG_INFO, "AFTER sem value=%d\n",sval);
    }
    sem_close(sem3);
    sem_unlink(SEM3_NAME);
  } else if (strcmp(argv[1],"test2") == 0) {
    sem3 = sem_open(SEM3_NAME, O_CREAT|O_EXCL, 0644, 2);
    if (sem3 == SEM_FAILED) {
      int errcode = errno;
      syslog(LOG_ERR, "ERROR: sem_open(1) failed: %d\n", errcode);
      return 0;
    }
    int sval;
    while(1) {
      sem_getvalue(sem3, &sval);
      syslog(LOG_INFO, "BEFORE sem value=%d\n",sval);
      sem_wait(sem3);
      sem_getvalue(sem3, &sval);
      syslog(LOG_INFO, "AFTER sem value=%d\n",sval);
    }
    sem_close(sem3);
    sem_unlink(SEM3_NAME);
  } else {
    syslog(LOG_INFO, "Counting Semaphore Interfaces example\n");
    syslog(LOG_INFO, "CONFIG_VERSION_MAJOR=%d\n",CONFIG_VERSION_MAJOR);
    syslog(LOG_INFO, "CONFIG_VERSION_MINOR=%d\n",CONFIG_VERSION_MINOR);
    syslog(LOG_INFO, "CONFIG_VERSION_PATCH=%d\n",CONFIG_VERSION_PATCH);
  }
  return 0;
}
