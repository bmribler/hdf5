/****************************************************************************
 * NCSA HDF                                                                 *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

#ifdef RCSID
static char RcsId[] = "@(#)$Revision$";
#endif

/* $Id$ */

/*
   FILE
       testhdf5.c - HDF5 testing framework main file.

   REMARKS
       General test wrapper for HDF5 base library test programs

   DESIGN
       Each test function should be implemented as function having no
    parameters and returning void (i.e. no return value).  They should be put
    into the list of InitTest() calls in main() below.  Functions which depend
    on other functionality should be placed below the InitTest() call for the
    base functionality testing.
       Each test module should include testhdf5.h and define a unique set of
    names for test files they create.

   BUGS/LIMITATIONS

   EXPORTED ROUTINES/VARIABLES:
       Two variables are exported: num_errs, and Verbosity.

 */

#if defined __MWERKS__
#include <console.h>
#endif

#define MAXNUMOFTESTS 30
#define HDF5_TEST_MASTER

/* Internal Variables */
static int  Index = 0;

/* ANY new test needs to have a prototype in tproto.h */
#include "testhdf5.h"

struct TestStruct
  {
      int         NumErrors;
      char        Description[64];
      int         SkipFlag;
      char        Name[16];
      VOID        (*Call) (void);
  }
         Test[MAXNUMOFTESTS];

static void InitTest(const char *TheName, VOID(*TheCall) (void), const char *TheDescr);
static void usage(void);

static void InitTest(const char *TheName, VOID(*TheCall) (void), const char *TheDescr)
{
    if (Index >= MAXNUMOFTESTS)
      {
          print_func("Uh-oh, too many tests added, increase MAXNUMOFTEST!\n");
          exit(0);
      }     /* end if */
    HDstrcpy(Test[Index].Description, TheDescr);
    HDstrcpy(Test[Index].Name, TheName);
    Test[Index].Call = TheCall;
    Test[Index].NumErrors = -1;
    Test[Index].SkipFlag = 0;
    Index++;
}

static void usage(void)
{
    intn        i;

    print_func("Usage: testhdf5 [-v[erbose] (l[ow]|m[edium]|h[igh]|0-10)] \n");
    print_func("               [-[e]x[clude] name+] \n");
    print_func("               [-o[nly] name+] \n");
    print_func("               [-b[egin] name] \n");
    print_func("               [-s[ummary]]  \n");
    print_func("               [-c[leanoff]]  \n");
    print_func("               [-n[ocaching]]  \n");
    print_func("               [-h[elp]]  \n");
    print_func("\n\n");
    print_func("verbose   controls the amount of information displayed\n");
    print_func("exclude   to exclude tests by name\n");
    print_func("only      to name tests which should be run\n");
    print_func("begin     start at the name of the test givin\n");
    print_func("summary   prints a summary of test results at the end\n");
    print_func("cleanoff  does not delete *.hdf files after execution of tests\n");
    print_func("nocaching do not turn on low-level DD caching\n");
    print_func("help      print out this information\n");
    print_func("\n\n");
    print_func("This program currently tests the following: \n\n");
    print_func("%16s %s\n", "Name", "Description");
    print_func("%16s %s\n", "----", "-----------");
    for (i = 0; i < Index; i++)
        print_func("%16s %s\n", Test[i].Name, Test[i].Description);
    print_func("\n\n");
}   /* end usage() */

/*
 * This routine is designed to provide equivalent functionality to 'printf'
 * and allow easy replacement for environments which don't have stdin/stdout
 * available.  (i.e. Windows & the Mac)
 */
int print_func(const char *format, ...)
{
    va_list arglist;
    int ret_value;

    va_start(arglist, format);
    ret_value=vprintf(format, arglist);
    va_end(arglist);
    return(ret_value);
}

int main(int argc, char *argv[])
{
    int         CLLoop;         /* Command Line Loop */
    int         Loop, Loop1;
    int         Summary = 0;
    int         CleanUp = 1;
    int         Cache = 1;
    uintn       major, minor, release, patch;

#if defined __MWERKS__
    argc = ccommand(&argv);
#endif

#if !(defined MAC || defined __MWERKS__ || defined SYMANTEC_C)
    /* Un-buffer the stdout and stderr */
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);
#endif

    /* Tests are generally arranged from least to most complexity... */
    InitTest("metadata", test_metadata, "Encode/decode metadata code");
    InitTest("file", test_file, "Low-Level File I/O");
    InitTest("heap", test_heap, "Object and Name Heaps");
    InitTest("ohdr", test_ohdr, "Object Headers");
    InitTest("stab", test_stab, "Symbol Tables");

    Verbosity = 4;  /* Default Verbosity is Low */
    H5version(&major, &minor, &release, &patch);

    print_func("\nFor help use: testhdf5 -help\n");
    print_func("Built with HDF5 Library Version: %u.%ur%u, patch %u\n\n", (unsigned) major,
           (unsigned) minor, (unsigned) release, (unsigned)patch);
    for (CLLoop = 1; CLLoop < argc; CLLoop++)
      {
          if ((argc > CLLoop + 1) && ((HDstrcmp(argv[CLLoop], "-verbose") == 0) ||
                                      (HDstrcmp(argv[CLLoop], "-v") == 0)))
            {
                if (argv[CLLoop + 1][0] == 'l')
                    Verbosity = 4;
                else if (argv[CLLoop + 1][0] == 'm')
                    Verbosity = 6;
                else if (argv[CLLoop + 1][0] == 'h')
                    Verbosity = 10;
                else
                    Verbosity = atoi(argv[CLLoop + 1]);
            }   /* end if */
          if ((argc > CLLoop) && ((HDstrcmp(argv[CLLoop], "-summary") == 0) ||
                                  (HDstrcmp(argv[CLLoop], "-s") == 0)))
              Summary = 1;

          if ((argc > CLLoop) && ((HDstrcmp(argv[CLLoop], "-help") == 0) ||
                                  (HDstrcmp(argv[CLLoop], "-h") == 0)))
            {
                usage();
                exit(0);
            }

          if ((argc > CLLoop) && ((HDstrcmp(argv[CLLoop], "-cleanoff") == 0) ||
                                  (HDstrcmp(argv[CLLoop], "-c") == 0)))
              CleanUp = 0;

          if ((argc > CLLoop) && ((HDstrcmp(argv[CLLoop], "-nocache") == 0) ||
                                  (HDstrcmp(argv[CLLoop], "-n") == 0)))
              Cache = 0;

          if ((argc > CLLoop + 1) && ((HDstrcmp(argv[CLLoop], "-exclude") == 0) ||
                                      (HDstrcmp(argv[CLLoop], "-x") == 0)))
            {
                Loop = CLLoop + 1;
                while ((Loop < argc) && (argv[Loop][0] != '-'))
                  {
                      for (Loop1 = 0; Loop1 < Index; Loop1++)
                          if (HDstrcmp(argv[Loop], Test[Loop1].Name) == 0)
                              Test[Loop1].SkipFlag = 1;
                      Loop++;
                  }     /* end while */
            }   /* end if */
          if ((argc > CLLoop + 1) && ((HDstrcmp(argv[CLLoop], "-begin") == 0) ||
                                      (HDstrcmp(argv[CLLoop], "-b") == 0)))
            {
                Loop = CLLoop + 1;
                while ((Loop < argc) && (argv[Loop][0] != '-'))
                  {
                      for (Loop1 = 0; Loop1 < Index; Loop1++)
                        {
                            if (HDstrcmp(argv[Loop], Test[Loop1].Name) != 0)
                                Test[Loop1].SkipFlag = 1;
                            if (HDstrcmp(argv[Loop], Test[Loop1].Name) == 0)
                                Loop1 = Index;
                        }   /* end for */
                      Loop++;
                  }     /* end while */
            }   /* end if */
          if ((argc > CLLoop + 1) && ((HDstrcmp(argv[CLLoop], "-only") == 0) ||
                                      (HDstrcmp(argv[CLLoop], "-o") == 0)))
            {
                for (Loop = 0; Loop < Index; Loop++)
                    Test[Loop].SkipFlag = 1;
                Loop = CLLoop + 1;
                while ((Loop < argc) && (argv[Loop][0] != '-'))
                  {
                      for (Loop1 = 0; Loop1 < Index; Loop1++)
                          if (HDstrcmp(argv[Loop], Test[Loop1].Name) == 0)
                              Test[Loop1].SkipFlag = 0;
                      Loop++;
                  }     /* end while */
            }   /* end if */
      }     /* end for */

#ifdef NOT_YET
    if(Cache) /* turn on caching, unless we were instucted not to */
        Hcache(CACHE_ALL_FILES,TRUE);
#endif /* NOT_YET */

    for (Loop = 0; Loop < Index; Loop++)
      {
          if (Test[Loop].SkipFlag)
            {
                MESSAGE(2, print_func("Skipping -- %s \n", Test[Loop].Description);
                    );
            }
          else
            {
                MESSAGE(2, print_func("Testing  -- %s (%s) \n", Test[Loop].Description,
                                  Test[Loop].Name);
                    );
                MESSAGE(5, print_func("===============================================\n");
                    );
                Test[Loop].NumErrors = num_errs;
                (*Test[Loop].Call) ();
                Test[Loop].NumErrors = num_errs - Test[Loop].NumErrors;
                MESSAGE(5, print_func("===============================================\n");
                    );
                MESSAGE(5, print_func("There were %d errors detected.\n\n", (int) Test[Loop].NumErrors);
                    );
            }   /* end else */
      }     /* end for */

    MESSAGE(2, print_func("\n\n");
        )
        if (num_errs)
        print_func("!!! %d Error(s) were detected !!!\n\n", (int) num_errs);
    else
        print_func("All tests were successful. \n\n");

    if (Summary)
      {
          print_func("Summary of Test Results:\n");
          print_func("Name of Test     Errors Description of Test\n");
          print_func("---------------- ------ --------------------------------------\n");

          for (Loop = 0; Loop < Index; Loop++)
            {
                if (Test[Loop].NumErrors == -1)
                    print_func("%16s %6s %s\n", Test[Loop].Name, "N/A", Test[Loop].Description);
                else
                    print_func("%16s %6d %s\n", Test[Loop].Name, (int) Test[Loop].NumErrors,
                           Test[Loop].Description);
            }   /* end for */
          print_func("\n\n");
      }     /* end if */

    if (CleanUp)
      {
          MESSAGE(2, print_func("\nCleaning Up...\n\n");
              );
#if !(defined DOS386 | defined WIN386)
          system("rm -f *.hdf *.tmp");
#else   /* OLD_WAY */
          remove("*.hdf");
          remove("*.tmp");
#endif  /* OLD_WAY */
      }     /* end if */
    exit(0);
    return (0);
}   /* end main() */
