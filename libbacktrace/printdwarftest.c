/* printdwarftest.c -- Print dwarf info cached by libbacktrace.
   Copyright (C) 2018 Free Software Foundation, Inc.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    (1) Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    (2) Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

    (3) The name of the author may not be used to
    endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.  */

/* Add all dwarf.c includes here, to make sure once-included headers are
   included here (before defining backtrace_dwarf_add) rather than at the
   #include "dwarf.c" below.  */

#include "config.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "dwarf2.h"
#include "filenames.h"

#include "backtrace.h"
#include "internal.h"

#include "testlib.h"
#include "backtrace-supported.h"

/* Include dwarf.c, using dwarf.c as header file defining struct dwarf_data.
   Rename the only external function to prevent conflict with dwarf.o.  */

extern int dummy_backtrace_dwarf_add (struct backtrace_state *state,
				      uintptr_t base_address,
				      const unsigned char* dwarf_info,
				      size_t dwarf_info_size,
				      const unsigned char *dwarf_line,
				      size_t dwarf_line_size,
				      const unsigned char *dwarf_abbrev,
				      size_t dwarf_abbrev_size,
				      const unsigned char *dwarf_ranges,
				      size_t dwarf_range_size,
				      const unsigned char *dwarf_str,
				      size_t dwarf_str_size,
				      int is_bigendian,
				      void *fileline_altlink,
				      backtrace_error_callback error_callback,
				      void *data, fileline *fileline_fn,
				      void **fileline_entry);

#define backtrace_dwarf_add dummy_backtrace_dwarf_add
#include "dwarf.c"

static void
error_callback (void *vdata ATTRIBUTE_UNUSED, const char *msg ATTRIBUTE_UNUSED,
		int errnum ATTRIBUTE_UNUSED)
{
}

static int
dummmy_callback (void *data ATTRIBUTE_UNUSED, uintptr_t pc ATTRIBUTE_UNUSED,
		 const char *filename ATTRIBUTE_UNUSED,
		 int lineno ATTRIBUTE_UNUSED,
		 const char *function ATTRIBUTE_UNUSED)
{
  return 0;
}

struct print_callback_data
{
  uintptr_t base_address;
  uintptr_t pc;
  const char *filename;
  int lineno;
  const char *function;
  uintptr_t d1_pc;
  const char *d1_filename;
  int d1_lineno;
  const char *d1_function;
  int depth;
  int stack_printed;
  int skip;
};

static int
print_callback (void *data, uintptr_t pc ATTRIBUTE_UNUSED,
		const char *filename ATTRIBUTE_UNUSED,
		int lineno ATTRIBUTE_UNUSED,
		const char *function ATTRIBUTE_UNUSED)
{
  struct print_callback_data *prev = (struct print_callback_data *)data;

  prev->depth++;
  if (prev->depth == 1)
    {
      if (prev->stack_printed)
	prev->skip = 1;
    }
  else
    {
      if (prev->skip)
	return 0;

      fprintf (stderr, " %s@%d", function, prev->depth);
      fprintf (stderr, " %d", lineno);
      prev->stack_printed = 1;

      return 0;
    }

  if (function == NULL)
    return 0;

  if (prev->pc == 0)
    {
      fprintf (stderr, "%p", (void *)(pc - prev->base_address));
      fprintf (stderr, " %s@%d", function, prev->depth);
      fprintf (stderr, " %s", filename);
      fprintf (stderr, "\n  %d", lineno);
      fprintf (stderr, "@%p", (void *)(pc - prev->base_address));
      goto update;
    }

  if (function != prev->function)
    {
      if (pc != prev->pc)
	{
	  fprintf (stderr, "\n");
	  fprintf (stderr, "%p", (void *)(pc - prev->base_address));
	  fprintf (stderr, " %s@%d", function, prev->depth);
	}
      else
	fprintf (stderr, " %s@%d", function, prev->depth);
    }

  if (filename != prev->filename)
    fprintf (stderr, " %s", filename);

  if (lineno != prev->lineno)
    {
      prev->stack_printed = 0;
      prev->skip = 0;
      fprintf (stderr, "\n  %d", lineno);
      fprintf (stderr, "@%p", (void *)(pc - prev->base_address));
    }

 update:
  prev->pc = pc;
  prev->filename = filename;
  prev->lineno = lineno;
  prev->function = function;
  if (prev->depth == 1)
    {
      prev->d1_pc = pc;
      prev->d1_filename = filename;
      prev->d1_lineno = lineno;
      prev->d1_function = function;
    }
  return 0;
}

static void
print_dwarf_data (struct backtrace_state *state, void *data)
{
  struct dwarf_data *fdata;
  unsigned int i;

  fdata = (struct dwarf_data *)data;

  for (i = 0; i < fdata->addrs_count; ++i)
    {
      struct unit_addrs *addr = &fdata->addrs[i];
      uintptr_t pc;

      struct print_callback_data tdata;
      tdata.base_address = fdata->base_address;
      tdata.pc = 0;
      tdata.stack_printed = 0;
      tdata.skip = 0;
      for (pc = addr->low; pc < addr->high; ++pc)
	{
	  int found;
	  tdata.depth = 0;
	  dwarf_lookup_pc (state, fdata, pc, print_callback, error_callback,
			   &tdata, &found);
	  if (!found)
	    fprintf (stderr, "not found: %p\n", (void *)pc);
	}
      if (tdata.pc != 0)
	fprintf (stderr, "\n");
    }
}

static void
print_dwarf_cache (struct backtrace_state *state)
{
  struct dwarf_data **pp;

  if (state->fileline_data == NULL)
    backtrace_pcinfo (state, (uintptr_t)print_callback, dummmy_callback,
		      NULL, NULL);

  for (pp = (struct dwarf_data **) (void *) &state->fileline_data;
       *pp != NULL;
       pp = &(*pp)->next)
    print_dwarf_data (state, *pp);
}


int
main (int argc ATTRIBUTE_UNUSED, char **argv)
{
  state = backtrace_create_state (argv[0], BACKTRACE_SUPPORTS_THREADS,
				  error_callback_create, NULL);

  print_dwarf_cache ((struct backtrace_state *)state);

  exit (failures ? EXIT_FAILURE : EXIT_SUCCESS);
}
