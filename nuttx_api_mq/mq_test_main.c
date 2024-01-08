/****************************************************************************
 * examples/nuttx_api_mq/mq_test_main.c
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
#include <mqueue.h>
#include <sched.h>
#include <errno.h>
#include <syslog.h>

#if CONFIG_VERSION_MAJOR >= 12
#define g_system_timer g_system_ticks
#endif

#define MQ1_NAME "foo"
#define MQ2_NAME "bar"
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

// This function searches for prime numbers in the most primitive way possible.
// This is used to gain time.

#if 0
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
#endif


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

  syslog(LOG_INFO, "%s Open Message Queue[%s]\n",argv[0],argv[1]);
  struct mq_attr my_attr;
  mqd_t my_mqfd;
  my_attr.mq_maxmsg  = 20;
  my_attr.mq_msgsize = CONFIG_MQ_MAXMSGSIZE;
  my_attr.mq_flags   = 0;
  my_mqfd = mq_open(argv[1], O_RDONLY|O_CREAT, 0666, &my_attr);
  if (my_mqfd == (mqd_t)-1) {
    syslog(LOG_ERR, "%s: ERROR: mq_open failed\n", argv[0]);
    return;
  }

  char msg_buffer[CONFIG_MQ_MAXMSGSIZE];
  unsigned int prio;
  int nbytes;
  while(1) {
    syslog(LOG_INFO, "%s Wait on Message Queue[%s]\n",argv[0],argv[1]);
    memset(msg_buffer,0,sizeof(msg_buffer));
    nbytes = mq_receive(my_mqfd, msg_buffer, CONFIG_MQ_MAXMSGSIZE, &prio);
    syslog(LOG_INFO, "%s Receive from Message Queue[%s] nbytes=%d prio=%d\n",argv[0],argv[1],nbytes,prio);
    syslog(LOG_INFO, "%s msg_buufer=%s\n",argv[0],msg_buffer);
  }
  mq_close(my_mqfd);
  syslog(LOG_INFO, "%s end PID:%d system_timer:%ld\n",argv[0],myPid,g_system_timer);
}

// Task Launcher
static void task_fork(char *name, int priority, char *mq) {
  int ret;
  UNUSED(ret);
//  char wk0[10];
  g_argv[0] = mq;
  g_argv[1] = NULL;
  syslog(LOG_INFO, "task_create name:%s priorty:%d\n",name, priority);
  ret = task_create(name,priority,STACKSIZE,(main_t)task_entry,(FAR char * const *)g_argv);
}



/****************************************************************************
 * mq_test_main
 ****************************************************************************/

#if defined(BUILD_MODULE)
int main(int argc, FAR char *argv[])
#else
int mq_test_main(int argc, char *argv[])
#endif
{
  int prio_std = SCHED_PRIORITY_DEFAULT;
  struct mq_attr attr;
  mqd_t mqfd;
  char msg_buffer[CONFIG_MQ_MAXMSGSIZE];
  unsigned int prio;
  int status;

  attr.mq_maxmsg  = 20;
  attr.mq_msgsize = CONFIG_MQ_MAXMSGSIZE;
  attr.mq_flags   = 0;
  mqfd = mq_open("mqueu", O_RDONLY|O_CREAT|O_NONBLOCK, 0666, &attr);
  if (mqfd == (mqd_t)-1) {
    syslog(LOG_ERR, "ERROR mq_open failed\n");
  }

  if (strcmp(argv[1],"send") == 0) {
    prio = 42;
    if (argc == 3) prio=atoi(argv[2]);
    //syslog(LOG_INFO, "prio=%d\n",prio);
    memset(msg_buffer,0,sizeof(msg_buffer));
    sprintf(msg_buffer,"g_system_timer=%ld",g_system_timer);
    status = mq_send(mqfd, msg_buffer, CONFIG_MQ_MAXMSGSIZE, prio);
    if (status < 0) {
      syslog(LOG_ERR, "ERROR mq_send failure=%d\n", status);
    }
    mq_close(mqfd);
  } else if (strcmp(argv[1],"recv") == 0) {
    int nbytes;
    while (1) {
      memset(msg_buffer,0,sizeof(msg_buffer));
      nbytes = mq_receive(mqfd, msg_buffer, CONFIG_MQ_MAXMSGSIZE, &prio);
      if (nbytes < 0) break;
      syslog(LOG_INFO, "nbytes=%d prio=%d\n",nbytes,prio);
      syslog(LOG_INFO, "msg_buufer=%s\n",msg_buffer);
    }
    mq_close(mqfd);
  } else if (strcmp(argv[1],"start") == 0) {
    task_fork("myTask1", prio_std, MQ1_NAME);
    task_fork("myTask2", prio_std, MQ2_NAME);
  } else if (strcmp(argv[1],"foo") == 0) {
    mqfd = mq_open(MQ1_NAME, O_WRONLY);
    if (mqfd == (mqd_t)-1) {
      syslog(LOG_ERR, "ERROR mq_open failed\n");
    }
    memset(msg_buffer,0,sizeof(msg_buffer));
    sprintf(msg_buffer,"g_system_timer=%ld",g_system_timer);
    status = mq_send(mqfd, msg_buffer, CONFIG_MQ_MAXMSGSIZE, 47);
    if (status < 0) {
      syslog(LOG_ERR, "ERROR mq_send failure=%d\n", status);
    }
    mq_close(mqfd);
  } else if (strcmp(argv[1],"bar") == 0) {
    mqfd = mq_open(MQ2_NAME, O_WRONLY);
    if (mqfd == (mqd_t)-1) {
      syslog(LOG_ERR, "ERROR mq_open failed\n");
    }
    memset(msg_buffer,0,sizeof(msg_buffer));
    sprintf(msg_buffer,"g_system_timer=%ld",g_system_timer);
    status = mq_send(mqfd, msg_buffer, CONFIG_MQ_MAXMSGSIZE, 47);
    if (status < 0) {
      syslog(LOG_ERR, "ERROR mq_send failure=%d\n", status);
    }
    mq_close(mqfd);
  } else {
    syslog(LOG_INFO, "Named Message Queue Interfaces example\n");
    syslog(LOG_INFO, "CONFIG_VERSION_MAJOR=%d\n",CONFIG_VERSION_MAJOR);
    syslog(LOG_INFO, "CONFIG_VERSION_MINOR=%d\n",CONFIG_VERSION_MINOR);
    syslog(LOG_INFO, "CONFIG_VERSION_PATCH=%d\n",CONFIG_VERSION_PATCH);
    syslog(LOG_INFO, "CONFIG_MQ_MAXMSGSIZE=%d\n",CONFIG_MQ_MAXMSGSIZE);
  }
  return 0;
}
