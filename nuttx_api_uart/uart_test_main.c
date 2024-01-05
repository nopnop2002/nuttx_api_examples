/****************************************************************************
 * examples/nuttx_api_uart/uart_test_main.c
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
#include <poll.h>

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
  printf("argc=%d\n",argc);
  for(int i=0;i<argc;i++) {
    printf("argv=%s\n",argv[i]);
  }
  pid_t myPid = getpid();
  int loop=atoi(argv[2]);
  int wait=atoi(argv[3]);
  printf("%s start PID:%d device=%s loop:%d wait:%d system_ticks:%ld\n",argv[0],myPid,argv[1],loop,wait,g_system_ticks);
  int fd = open(argv[1],O_RDWR);
  if (fd < 0) {
    printf("%s open error %d\n",argv[0],fd);
    return;
  }
  char buffer[30];
  char CR = 0x0d;
  char LF = 0x0a;
  for(int i=0;i<loop;i++) {
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"this is %s - %d %c%c",argv[1],i,CR,LF);
    write(fd,buffer,sizeof(buffer));
    if(wait) sleep(wait);
  }
  close(fd);
  printf("%s end PID:%d system_ticks:%ld\n",argv[0],myPid,g_system_ticks);
}

#define POLL_TIMEOUT 2000

static void recv_task_entry(int argc, char * argv[]) {
#if 0
  printf("argc=%d\n",argc);
  for(int i=0;i<argc;i++) {
    printf("argv=%s\n",argv[i]);
  }
#endif
  pid_t myPid = getpid();
  int fd = open(argv[1],O_RDWR);
  if (fd < 0) {
    printf("%s open error %d\n",argv[0],fd);
    return;
  }
  struct pollfd fdp;
  fdp.fd = fd;
  fdp.events = POLLIN;
  char buffer[30];
  int pos = 0;
  printf("%s ready to receive from %s\n",argv[0],argv[1]);
  while(1) {
    int ret = poll((struct pollfd *)&fdp, 1, POLL_TIMEOUT);
    if (ret > 0) {
      if ((fdp.revents & POLLIN) != 0) {
        ssize_t len;
        len = read(fd,&buffer[pos],30-pos);
        pos = pos + len;
        if (pos == 30) {
          printf("%s buffer=%s",argv[0],buffer);
          pos = 0;
        }
      }
    }
  }
  close(fd);
  printf("%s end PID:%d system_ticks:%ld\n",argv[0],myPid,g_system_ticks);
}

// Task Launcher
static void send_task_fork(char *name, int priority, char *device, int loop, int wait) {
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

static void recv_task_fork(char *name, int priority, char *device, int loop, int wait) {
  int ret;
  UNUSED(ret);
  g_argv[0] = device;
  g_argv[1] = NULL;
  printf("task_create name:%s priority:%d\n",name, priority);
  ret = task_create(name,priority,STACKSIZE,(main_t)recv_task_entry,(FAR char * const *)g_argv);
}

/****************************************************************************
 * uart_test_main
 ****************************************************************************/

#if defined(BUILD_MODULE)
int main(int argc, FAR char *argv[])
#else
int uart_test_main(int argc, char *argv[])
#endif
{
  int prio_std = SCHED_PRIORITY_DEFAULT;
  char device[20];

  if (strcmp(argv[1],"send") == 0) {
    if (argc == 2) {
      strcpy(device,"/dev/ttyS1");
    }
    if (argc == 3) {
      if (strcmp(argv[2],"0") == 0) {
        strcpy(device,"/dev/ttyS0");
      } else if (strcmp(argv[2],"1") == 0) {
        strcpy(device,"/dev/ttyS1");
      } else if (strcmp(argv[2],"2") == 0) {
        strcpy(device,"/dev/ttyS2");
      } else if (strcmp(argv[2],"3") == 0) {
        strcpy(device,"/dev/ttyS3");
      }
    }
    printf("send to %s\n",device);
    send_task_fork("sendTask", prio_std, device, 10, 1);
  } else if (strcmp(argv[1],"recv") == 0) {
    printf("receive from /dev/ttyS2\n");
    recv_task_fork("recvTask", prio_std, "/dev/ttyS2", 10, 1);
  } else {
    printf("UART Interfaces example\n");
  }
  return 0;
}
