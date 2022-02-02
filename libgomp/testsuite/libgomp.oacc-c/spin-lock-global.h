#define XSTR(S) STR (S)
#define STR(S) #S

#define PRINTF(...)				\
  {						\
    printf (__VA_ARGS__);			\
    fflush (NULL);				\
  }

#define DO_PRAGMA(x) _Pragma (#x)

#ifndef SPIN_CNT_MAX
/* Define to have limited-spin spinlock.
   Ensures that the program will terminate.  */
#define SPIN_CNT_MAX 0x8000U
#endif

#define TEST_1(N, LOCKVAR, VERIFY, N_GANGS, N_WORKERS)			\
  assert (N % N_GANGS == 0);						\
									\
  DO_PRAGMA (acc parallel						\
	     num_gangs(N_GANGS)						\
	     num_workers(N_WORKERS)					\
	     copy (lock_cnt)						\
	     copy (spin_cnt_max_hit)					\
	     present (LOCKVAR))						\
  {									\
    TYPE unlocked = (TYPE)0;						\
    TYPE locked = ~unlocked;						\
									\
    LOCKVAR = unlocked;							\
									\
    unsigned int n_gangs						\
      = __builtin_goacc_parlevel_size (GOMP_DIM_GANG);			\
									\
    DO_PRAGMA (acc loop worker)						\
      for (unsigned int i = 0; i < N / n_gangs; i++)			\
	{								\
	  TYPE res;							\
									\
	  unsigned int spin_cnt = 0;					\
	  while (1)							\
	    {								\
	      res = __atomic_exchange_n (&LOCKVAR, locked,		\
					 MEMMODEL_ACQUIRE);		\
	      if (res == locked)					\
		{							\
		  if (SPIN_CNT_MAX > 0)					\
		    {							\
		      spin_cnt++;					\
		      if (spin_cnt == SPIN_CNT_MAX)			\
			{						\
			  if (VERIFY)					\
			    __atomic_fetch_add (&spin_cnt_max_hit, 1,	\
						MEMMODEL_RELAXED);	\
			  break;					\
			}						\
		    }							\
		  continue;						\
									\
		}							\
	      else							\
		{							\
		  if (res != unlocked)					\
		    __builtin_abort ();					\
									\
		  if (VERIFY)						\
		    __atomic_fetch_add (&lock_cnt, 1,			\
					MEMMODEL_RELAXED);		\
									\
		  __atomic_store_n (&LOCKVAR, unlocked,			\
				    MEMMODEL_RELEASE);			\
		  break;						\
		}							\
	    }								\
	}								\
  }

#define TEST(N, LOCKVAR, VERIFY, N_GANGS, N_WORKERS)			\
  {									\
    spin_cnt_max_hit = 0;						\
									\
    if (VERIFY)								\
      lock_cnt = 0;							\
									\
    PRINTF ("%s - verify=%u - lock=%s - gangs=%u - workers=%u ... ",	\
	    XSTR (TYPE), VERIFY, STR(LOCKVAR), N_GANGS, N_WORKERS);	\
    TEST_1 (N, LOCKVAR, VERIFY, N_GANGS, N_WORKERS);			\
    PRINTF ("done\n");							\
									\
    if (VERIFY && SPIN_CNT_MAX)						\
      PRINTF ("spin_cnt_max_hit: %llu\n", spin_cnt_max_hit);		\
									\
    if (VERIFY && (lock_cnt + spin_cnt_max_hit != N))			\
      {									\
	PRINTF ("lock_cnt: %llu\n", lock_cnt);				\
	PRINTF ("lock_cnt + spin_cnt_max_hit: %llu\n",			\
		lock_cnt + spin_cnt_max_hit);				\
	PRINTF ("N: %u\n", N);						\
	__builtin_abort ();						\
      }									\
  }

/* Uses .global addressing on nvptx.  */
TYPE LOCKVAR1;
#pragma acc declare create (LOCKVAR1)

void
TESTS (unsigned int n)
{
  unsigned long long int lock_cnt;
  unsigned long long int spin_cnt_max_hit;

  /* Uses generic addressing on nvptx.  */
  TYPE LOCKVAR2;
#pragma acc declare create (LOCKVAR2)

#define N_GANGS 1
#define N_WORKERS 8
#define VERIFY 0
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#define VERIFY 1
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#undef N_WORKERS
#undef N_GANGS

#define N_GANGS 2
#define N_WORKERS 4
#define VERIFY 0
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#define VERIFY 1
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#undef N_WORKERS
#undef N_GANGS

#define N_GANGS 4
#define N_WORKERS 2
#define VERIFY 0
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#define VERIFY 1
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#undef N_WORKERS
#undef N_GANGS

#define N_GANGS 8
#define N_WORKERS 1
#define VERIFY 0
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#define VERIFY 1
  TEST (n, LOCKVAR1, VERIFY, N_GANGS, N_WORKERS);
  TEST (n, LOCKVAR2, VERIFY, N_GANGS, N_WORKERS);
#undef VERIFY
#undef N_WORKERS
#undef N_GANGS
}
