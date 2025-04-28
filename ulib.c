#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

// MODIFIED CODE ---------------------------------------------------------->
// locking functions- not system calls, but user defined functions that are required to protect state while using threads
void Lock_Init(Lock *mutex)
{
  mutex->Is_Locked = 0; // initialize a lock by setting it to `0` (unlocked)
}

void Lock_Acquire(Lock *mutex)
{
  // (basic spin lock)
  __sync_synchronize(); // __sync_synchronize() is a memory barrier- ensures memory operations are not reordered
  while (xchg(&(mutex->Is_Locked), 1) != 0)
    ; // does an atomic exchange- if Is_Locked was `0`, it becomes `1` and you succeed. If it was already `1`, you busy wait until it becomes 0.

  return;
}

void Lock_Release(Lock *mutex)
{
  // release the lock (exit the critical section)
  asm volatile("movl $0, %0" : "+m"(mutex->Is_Locked) :);
}

int thread_create(void (*worker)(void *, void *), void *arg1, void *arg2)
{
  // create a child stack- allocating one page (4096 bytes)
  void *Child_Stack = malloc(4096);

  // create a thread using the clone() kernel function
  int Thread_id = clone(worker, arg1, arg2, Child_Stack);
  return Thread_id;
}
int REQUEST(int Resource_ID)
{
  // call placeholder kernel function
  return requestresource(Resource_ID);
}
int RELEASE(int Resource_ID)
{
  // call placeholder kernel function
  return releaseresource(Resource_ID);
}
int WRITE(int Resource_ID, void *buffer, int offset, int size)
{
  // call placeholder kernel function
  return writeresource(Resource_ID, buffer, offset, size);
}
int READ(int Resource_ID, int offset, int size, void *buffer)
{
  // call placeholder kernel function
  return readresource(Resource_ID, offset, size, buffer);
}
int thread_join(int thread_id)
{
  // call join() kernel function
  return join(thread_id);
}

// MODIFIED CODE ---------------------------------------------------------->

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while ((*s++ = *t++) != 0)
    ;
  return os;
}

int strcmp(const char *p, const char *q)
{
  while (*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint strlen(const char *s)
{
  int n;

  for (n = 0; s[n]; n++)
    ;
  return n;
}

void *
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char *
strchr(const char *s, char c)
{
  for (; *s; s++)
    if (*s == c)
      return (char *)s;
  return 0;
}

char *
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for (i = 0; i + 1 < max;)
  {
    cc = read(0, &c, 1);
    if (cc < 1)
      break;
    buf[i++] = c;
    if (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if (fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int atoi(const char *s)
{
  int n;

  n = 0;
  while ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  return n;
}

void *
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while (n-- > 0)
    *dst++ = *src++;
  return vdst;
}
