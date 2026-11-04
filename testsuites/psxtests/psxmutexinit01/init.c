/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Regression test for RTEMS-1: pthread_mutex_init() with invalid
 * priority ceiling must not modify the mutex object. Subsequent
 * operations should return EINVAL.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <string.h>

const char rtems_test_name[] = "PSXMUTEXINIT 1";

static void test_invalid_prio_ceiling_leaves_mutex_unmodified( void )
{
  pthread_mutexattr_t attr;
  pthread_mutex_t     mtx;
  int                 sc;

  /* Fill mutex object with a non-zero pattern */
  memset( &mtx, 0xa5, sizeof( mtx ) );

  sc = pthread_mutexattr_init( &attr );
  rtems_test_assert( sc == 0 );

  sc = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  rtems_test_assert( sc == 0 );

  /* Choose an out-of-range ceiling: max + 1 */
  sc = pthread_mutexattr_setprioceiling(
    &attr,
    sched_get_priority_max( SCHED_FIFO ) + 1
  );
  rtems_test_assert( sc == 0 );

  /* Initialization must fail with EINVAL and leave mtx untouched */
  sc = pthread_mutex_init( &mtx, &attr );
  rtems_test_assert( sc == EINVAL );

  /* Subsequent operations must reject the object with EINVAL */
  sc = pthread_mutex_trylock( &mtx );
  rtems_test_assert( sc == EINVAL );

  sc = pthread_mutex_destroy( &mtx );
  rtems_test_assert( sc == EINVAL );

  sc = pthread_mutexattr_destroy( &attr );
  rtems_test_assert( sc == 0 );
}

void *POSIX_Init( void *ignored )
{
  (void) ignored;

  TEST_BEGIN();

  test_invalid_prio_ceiling_leaves_mutex_unmodified();

  TEST_END();
  rtems_test_exit( 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
