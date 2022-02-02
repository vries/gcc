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
#define SPIN_CNT_MAX 0x20000U
#endif

#define TEST_1(N, LOCKREF)						\
  DO_PRAGMA (acc parallel						\
	     num_gangs(1)						\
	     num_workers(N_WORKERS)					\
	     copy (lock_cnt)						\
	     copy (spin_cnt_max_hit))					\
  {									\
    TYPE unlocked = (TYPE)0;						\
    TYPE locked = ~unlocked;						\
    TYPE lock;								\
    TYPE *volatile lock_ptr = &lock;					\
    unsigned long long int lock_cnt_1;					\
    unsigned long long int spin_cnt_max_hit_1;				\
									\
    if (VERIFY)								\
      {									\
	lock_cnt_1 = 0;							\
									\
	if (SPIN_CNT_MAX)						\
	  spin_cnt_max_hit_1 = 0;					\
      }									\
									\
    *(LOCKREF) = unlocked;						\
									\
    DO_PRAGMA (acc loop worker)						\
      for (unsigned int i = 0; i < N; i++)				\
	{								\
	  TYPE res;							\
									\
	  unsigned int spin_cnt = 0;					\
	  while (1)							\
	    {								\
	      res = __atomic_exchange_n (LOCKREF, locked,		\
					 MEMMODEL_ACQUIRE);		\
	      if (res == locked)					\
		{							\
		  if (SPIN_CNT_MAX > 0)					\
		    {							\
		      spin_cnt++;					\
		      if (spin_cnt == SPIN_CNT_MAX)			\
			{						\
			  if (VERIFY)					\
			    __atomic_fetch_add (&spin_cnt_max_hit_1, 1,	\
						MEMMODEL_RELAXED);	\
			  break;					\
			}						\
		    }							\
		  continue;						\
		}							\
	      else							\
		{							\
		  if (res != unlocked)					\
		    __builtin_abort ();					\
									\
		  if (VERIFY)						\
		    __atomic_fetch_add (&lock_cnt_1, 1,			\
					MEMMODEL_RELAXED);		\
									\
		  __atomic_store_n (LOCKREF, unlocked,			\
				    MEMMODEL_RELEASE);			\
									\
		  break;						\
		}							\
	    }								\
	}								\
									\
    if (VERIFY)								\
      {									\
	lock_cnt += lock_cnt_1;						\
									\
	if (SPIN_CNT_MAX)						\
	  spin_cnt_max_hit += spin_cnt_max_hit_1;			\
      }									\
  }

#define TEST(N, LOCKREF)					\
  {								\
    spin_cnt_max_hit = 0;					\
								\
    if (VERIFY)							\
      lock_cnt = 0;						\
								\
    PRINTF ("%s - verify=%u - LOCKREF=%s ... ",			\
	    XSTR (TYPE), VERIFY, #LOCKREF);			\
    TEST_1 (N, LOCKREF);					\
    PRINTF ("done\n");						\
								\
    if (VERIFY && SPIN_CNT_MAX)					\
      PRINTF ("spin_cnt_max_hit: %llu\n", spin_cnt_max_hit);	\
								\
    if (VERIFY && (lock_cnt + spin_cnt_max_hit != N))		\
      {								\
	PRINTF ("lock_cnt: %llu\n", lock_cnt);			\
	PRINTF ("lock_cnt + spin_cnt_max_hit: %llu\n",		\
		lock_cnt + spin_cnt_max_hit);			\
	PRINTF ("N: %u\n", N);					\
	__builtin_abort ();					\
      }								\
  }

void
TESTS (unsigned int n)
{
  unsigned long long int lock_cnt;
  unsigned long long int spin_cnt_max_hit;

#define N_WORKERS 8

#define VERIFY 0
  TEST (n, &lock);
  TEST (n, lock_ptr);
#undef VERIFY

#define VERIFY 1
  TEST (n, &lock);
  TEST (n, lock_ptr);
#undef VERIFY
}
