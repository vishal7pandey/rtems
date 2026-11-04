/* SPDX-License-Identifier: BSD-2-Clause */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

const char rtems_test_name[] = "PSXMUTEXINITFAIL 1";

static void test_invalid_ceiling_leaves_mutex_unmodified( void )
{
  pthread_mutexattr_t attr;
  pthread_mutex_t     mtx;
  int                 eno;

  memset( &mtx, 0xa5, sizeof( mtx ) );

  eno = pthread_mutexattr_init( &attr );
  rtems_test_assert( eno == 0 );

  eno = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  rtems_test_assert( eno == 0 );

  eno = pthread_mutexattr_setprioceiling(
    &attr,
    sched_get_priority_max( SCHED_FIFO ) + 1
  );
  rtems_test_assert( eno == 0 );

  eno = pthread_mutex_init( &mtx, &attr );
  rtems_test_assert( eno == EINVAL );

  /* The mutex object must be rejected by subsequent operations */
  eno = pthread_mutex_trylock( &mtx );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutexattr_destroy( &attr );
  rtems_test_assert( eno == 0 );
}

void *POSIX_Init( void *arg )
{
  (void) arg;

  TEST_BEGIN();

  test_invalid_ceiling_leaves_mutex_unmodified();

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
