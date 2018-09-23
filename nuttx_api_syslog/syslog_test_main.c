/****************************************************************************
 * examples/nuttx_api_syslog/syslog_test_main.c
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
#include <fcntl.h>
#include <syslog.h>

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
static void send_task_entry(int argc, char * argv[]) {
#if 0
  printf("argc=%d\n",argc);
  for(int i=0;i<argc;i++) {
    printf("argv=%s\n",argv[i]);
  }
#endif
  pid_t myPid = getpid();
  int loop=atoi(argv[2]);
  int wait=atoi(argv[3]);
  printf("%s start PID:%d device=%s loop:%d wait:%d system_timer:%d\n",argv[0],myPid,argv[1],loop,wait,g_system_timer);
  int fd = open(argv[1],O_RDWR);
  if (fd < 0) {
    printf("%s open error %d\n",argv[0],fd);
    return;
  }
  char buffer[30];
  char CR = 0x0d;
  char LF = 0x0a;
//  printf("%s loop=%d wait=%d\n",argv[0],loop,wait);
  for(int i=0;i<loop;i++) {
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"this is %s %c%c",argv[1],CR,LF);
    write(fd,buffer,sizeof(buffer));
    if(wait) sleep(wait);
//      printf("%s i=%d g_system_timer=%d\n",argv[0],i,g_system_timer);
  }
  close(fd);
  printf("%s end PID:%d system_timer:%d\n",argv[0],myPid,g_system_timer);
}

// Task Launcher
static void task_fork(char *name, int priority, char *device, int loop, int wait) {
  int ret;
  UNUSED(ret);
  g_argv[0] = device;
  char wk0[10];
  sprintf(wk0,"%d",loop);
  g_argv[1] = wk0;
  char wk1[10];
  sprintf(wk1,"%d",wait);
  g_argv[2] = wk1;
  g_argv[3] = NULL;
  printf("task_create name:%s priority:%d\n",name, priority);
  ret = task_create(name,priority,STACKSIZE,(main_t)send_task_entry,(FAR char * const *)g_argv);
}

/****************************************************************************
 * syslog_test_main
 ****************************************************************************/

#if defined(BUILD_MODULE)
int main(int argc, FAR char *argv[])
#else
int syslog_test_main(int argc, char *argv[])
#endif
{
  int prio_std = SCHED_PRIORITY_DEFAULT;

  if (strcmp(argv[1],"syslog") == 0) {
    printf("Hello, World!!\n");
    syslog(LOG_ERR,"[LOG_ERR] syslog\n");
    syslog(LOG_WARNING,"[LOG_WARNING] syslog\n");
    syslog(LOG_NOTICE,"[LOG_NOTICE] syslog\n");
    syslog(LOG_INFO,"[LOG_INFO] syslog\n");
    syslog(LOG_DEBUG,"[LOG_DEBUG] syslog\n");
  } else if (strcmp(argv[1],"console") == 0) {
    task_fork("sendTask", prio_std, "/dev/console", 10, 1);
  } else if (strcmp(argv[1],"ttyS0") == 0) {
    task_fork("sendTask", prio_std, "/dev/ttyS0", 10, 1);
  } else {
    printf("SYSLOG Interfaces example\n");
  }
  return 0;
}
