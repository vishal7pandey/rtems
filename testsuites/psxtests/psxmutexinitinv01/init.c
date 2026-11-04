/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Regression test for RTEMS-1: Verify that a failed pthread_mutex_init()
 * due to an invalid priority ceiling does not modify the target mutex
 * object and that subsequent operations reject it with EINVAL.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

const char rtems_test_name[] = "PSXMUTEXINITINV 1";

/* forward declaration to avoid warnings */
void *POSIX_Init( void *ignored );

static void test_failed_init_leaves_mutex_unmodified( void )
{
  pthread_mutexattr_t attr;
  pthread_mutex_t     mtx;
  int                 sc;
  int                 ceiling;

  memset( &mtx, 0xa5, sizeof( mtx ) );

  sc = pthread_mutexattr_init( &attr );
  rtems_test_assert( sc == 0 );

  sc = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  rtems_test_assert( sc == 0 );

  ceiling = sched_get_priority_max( SCHED_FIFO ) + 1;
  sc = pthread_mutexattr_setprioceiling( &attr, ceiling );
  rtems_test_assert( sc == 0 );

  puts( "Init: pthread_mutex_init - EINVAL (invalid prio ceiling)" );
  sc = pthread_mutex_init( &mtx, &attr );
  rtems_test_assert( sc == EINVAL );

  puts( "Init: pthread_mutex_trylock - EINVAL (uninitialized object)" );
  sc = pthread_mutex_trylock( &mtx );
  rtems_test_assert( sc == EINVAL );

  puts( "Init: pthread_mutex_destroy - EINVAL (uninitialized object)" );
  sc = pthread_mutex_destroy( &mtx );
  rtems_test_assert( sc == EINVAL );

  sc = pthread_mutexattr_destroy( &attr );
  rtems_test_assert( sc == 0 );
}

void *POSIX_Init( void *ignored )
{
  (void) ignored;

  TEST_BEGIN();

  test_failed_init_leaves_mutex_unmodified();

  TEST_END();
  rtems_test_exit( 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
