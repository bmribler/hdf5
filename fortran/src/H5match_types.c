/****p* Program/H5match_types
 *
 * NAME
 *  Executable: H5match_types
 *
 * FILE
 *  fortran/src/H5match_types.c
 *
 * PURPOSE
 *  C Program to match C types to Fortran types.
 *  Creates the files H5f90i_gen.h for the C code and
 *  H5fortran_types.f90 for the Fortran code.
 *
 * COPYRIGHT
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 ******
 */

#include <stdio.h>
#include <assert.h>

#include "H5public.h"
/* Include H5Ipublic.h for hid_t type */
#include "H5Ipublic.h"

/* Definitions of which fortran type sizes exist */
#include "H5fort_type_defines.h"

/* File pointers for files */
FILE *c_header;
FILE *fort_header;

#define CFILE "H5f90i_gen.h"
#define FFILE "H5fortran_types.f90"

/* Prototypes for the write routines */
void writeTypedef(const char *c_typedef, const char *c_type, unsigned int size);
void writeTypedefDefault(const char *c_typedef, unsigned int size);
void writeToFiles(const char *c_typedef, const char *fortran_type, const char *c_type, int size,
                  unsigned int kind);

static void
initCfile(void)
{
    fprintf(c_header, "/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n\
 * Copyright by The HDF Group.                                               *\n\
 * Copyright by the Board of Trustees of the University of Illinois.         *\n\
 * All rights reserved.                                                      *\n\
 *                                                                           *\n\
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *\n\
 * terms governing use, modification, and redistribution, is contained in    *\n\
 * the COPYING file, which can be found at the root of the source code       *\n\
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *\n\
 * If you do not have access to either file, you may request a copy from     *\n\
 * help@hdfgroup.org.                                                        *\n\
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */\n\
\n\n\
#ifndef _H5f90i_gen_H\n\
#define _H5f90i_gen_H\n\
\n\
/* This file is automatically generated by H5match_types.c at build time. */\n\
\n\
#include \"H5public.h\"\n\n");
}

static void
initFfile(void)
{
    fprintf(fort_header, "! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * \n\
!   Copyright by The HDF Group.                                               *\n\
!   Copyright by the Board of Trustees of the University of Illinois.         *\n\
!   All rights reserved.                                                      *\n\
!                                                                             *\n\
!   This file is part of HDF5.  The full HDF5 copyright notice, including     *\n\
!   terms governing use, modification, and redistribution, is contained in    *\n\
!   the COPYING file, which can be found at the root of the source code       *\n\
!   distribution tree, or in https://www.hdfgroup.org/licenses.               *\n\
!   If you do not have access to either file, you may request a copy from     *\n\
!   help@hdfgroup.org.                                                        *\n\
! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n\
!\n!\n\
! This file is automatically generated and contains HDF5 Fortran90 type definitions.\n!\n\
       MODULE H5FORTRAN_TYPES\n\
         !\n\
         !  HDF5 integers\n\
         !\n");
}

static void
endCfile(void)
{
    fprintf(c_header, "\n#endif /* _H5f90i_gen_H */\n");
}
static void
endFfile(void)
{
    fprintf(fort_header, "\n        INTEGER(SIZE_T), PARAMETER :: OBJECT_NAMELEN_DEFAULT_F = -1\n\n");
    fprintf(fort_header, "        END MODULE H5FORTRAN_TYPES\n");
}

/* Define a c_int_x type in the C header */
void
writeTypedef(const char *c_typedef, const char *c_type, unsigned int size)
{
    fprintf(c_header, "#define c_%s_%u %s\n", c_typedef, size, c_type);
}

/* Call this function if there is no matching C type for sizes > 1 */
void
writeTypedefDefault(const char *c_typedef, unsigned int size)
{
    assert(size % 2 == 0);
    fprintf(c_header, "typedef struct {c_%s_%u a; c_%s_%u b;} c_%s_%u\n", c_typedef, size / 2, c_typedef,
            size / 2, c_typedef, size);
}

/* Create matching Fortran and C types by writing to both files */
void
writeToFiles(const char *c_typedef, const char *fortran_type, const char *c_type, int size, unsigned int kind)
{
    fprintf(fort_header, "        INTEGER, PARAMETER :: %s = %u\n", fortran_type, kind);
    fprintf(c_header, "typedef c_%s_%d %s;\n", c_typedef, size, c_type);
}

int
main(void)
{
    int      FoundIntSize[4];
    unsigned FoundIntSizeKind[4];
    int      FoundRealSize[3];
    unsigned FoundRealSizeKind[3];
    int      i, j, flag;
    char     chrA[20], chrB[20];
    int      H5_C_HAS_REAL_NATIVE_16;

    /* Open target files */
    c_header    = fopen(CFILE, "w");
    fort_header = fopen(FFILE, "w");

    /* Default is C has 16 byte float */
    H5_C_HAS_REAL_NATIVE_16 = 1;

    /* Write copyright, boilerplate to both files */
    initCfile();
    initFfile();

    /* First, define c_int_x */

#if defined H5_FORTRAN_HAS_INTEGER_1_KIND
    if (sizeof(long long) == 1)
        writeTypedef("int", "long long", 1);
    else if (sizeof(long) == 1)
        writeTypedef("int", "long", 1);
    else if (sizeof(int) == 1)
        writeTypedef("int", "int", 1);
    else if (sizeof(short) == 1)
        writeTypedef("int", "short", 1);
    else
        writeTypedef("int", "char", 1);
    /* Actually, char is not necessarily one byte.
     * But if char isn't, then nothing is, so this
     * is as close as we can get. */
    if (sizeof(size_t) == 1)
        writeTypedef("size_t", "size_t", 1);
    if (sizeof(hsize_t) == 1)
        writeTypedef("hsize_t", "hsize_t", 1);
#endif /*H5_FORTRAN_HAS_INTEGER_1_KIND*/

#if defined H5_FORTRAN_HAS_INTEGER_2_KIND
    if (sizeof(long long) == 2)
        writeTypedef("int", "long long", 2);
    else if (sizeof(long) == 2)
        writeTypedef("int", "long", 2);
    else if (sizeof(int) == 2)
        writeTypedef("int", "int", 2);
    else if (sizeof(short) == 2)
        writeTypedef("int", "short", 2);
    else
        writeTypedefDefault("int", 2);

    if (sizeof(size_t) == 2)
        writeTypedef("size_t", "size_t", 2);
    if (sizeof(hsize_t) == 2)
        writeTypedef("hsize_t", "hsize_t", 2);
#endif /*H5_FORTRAN_HAS_INTEGER_2_KIND*/

#if defined H5_FORTRAN_HAS_INTEGER_4_KIND
    if (sizeof(long long) == 4)
        writeTypedef("int", "long long", 4);
    else if (sizeof(long) == 4)
        writeTypedef("int", "long", 4);
    else if (sizeof(int) == 4)
        writeTypedef("int", "int", 4);
    else if (sizeof(short) == 4)
        writeTypedef("int", "short", 4);
    else
        writeTypedefDefault("int", 4);

    if (sizeof(size_t) == 4)
        writeTypedef("size_t", "size_t", 4);
    if (sizeof(hsize_t) == 4)
        writeTypedef("hsize_t", "hsize_t", 4);

#endif /*H5_FORTRAN_HAS_INTEGER_4_KIND*/

#if defined H5_FORTRAN_HAS_INTEGER_8_KIND
    if (sizeof(long long) == 8)
        writeTypedef("int", "long long", 8);
    else if (sizeof(long) == 8)
        writeTypedef("int", "long", 8);
    else if (sizeof(int) == 8)
        writeTypedef("int", "int", 8);
    else if (sizeof(short) == 8)
        writeTypedef("int", "short", 8);
    else
        writeTypedefDefault("int", 8);

    if (sizeof(size_t) == 8)
        writeTypedef("size_t", "size_t", 8);
    if (sizeof(hsize_t) == 8)
        writeTypedef("hsize_t", "hsize_t", 8);

#endif /*H5_FORTRAN_HAS_INTEGER_8_KIND*/

        /* Define c_float_x */

#if defined H5_FORTRAN_HAS_REAL_NATIVE_4_KIND || defined H5_FORTRAN_HAS_REAL_4_KIND
    if (sizeof(long double) == 4)
        writeTypedef("float", "long double", 4);
    else if (sizeof(double) == 4)
        writeTypedef("float", "double", 4);
    else if (sizeof(float) == 4)
        writeTypedef("float", "float", 4);
    else {
        printf("Fortran REAL is 4 bytes, no corresponding C floating type\n");
        printf("Quitting....\n");
        return -1;
    }
#endif /*H5_FORTRAN_HAS_REAL_NATIVE_4_KIND*/

#if defined H5_FORTRAN_HAS_REAL_NATIVE_8_KIND || defined H5_FORTRAN_HAS_REAL_8_KIND
    if (sizeof(long double) == 8)
        writeTypedef("float", "long double", 8);
    else if (sizeof(double) == 8)
        writeTypedef("float", "double", 8);
    else if (sizeof(float) == 8)
        writeTypedef("float", "float", 8);
    else {
        printf("Fortran REAL is 16 bytes, no corresponding C floating type\n");
        printf("Quitting....\n");
        return -1;
    }
#endif /*H5_FORTRAN_HAS_REAL_NATIVE_8_KIND*/

#if defined H5_FORTRAN_HAS_REAL_NATIVE_16_KIND || defined H5_FORTRAN_HAS_REAL_16_KIND
    if (sizeof(long double) == 16)
        writeTypedef("float", "long double", 16);
    else if (sizeof(double) == 16)
        writeTypedef("float", "double", 16);
    else if (sizeof(float) == 16)
        writeTypedef("float", "float", 16);
    else /*C has no 16 byte float so disable it in Fortran*/
    {
        printf("warning: Fortran REAL is 16 bytes, no corresponding C floating type\n");
        printf("         Disabling Fortran 16 byte REALs\n");
        H5_C_HAS_REAL_NATIVE_16 = 0;
    }
#endif /*H5_FORTRAN_HAS_REAL_NATIVE_16_KIND*/

    /* Now begin defining fortran types. */
    fprintf(c_header, "\n");
    /* haddr_t */
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_HADDR_T >= 8
    writeToFiles("int", "HADDR_T", "haddr_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_4_KIND && H5_SIZEOF_HADDR_T >= 4
    writeToFiles("int", "HADDR_T", "haddr_t_f", 4, H5_FORTRAN_HAS_INTEGER_4_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_2_KIND && H5_SIZEOF_HADDR_T >= 2
    writeToFiles("int", "HADDR_T", "haddr_t_f", 2, H5_FORTRAN_HAS_INTEGER_2_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_1_KIND && H5_SIZEOF_HADDR_T >= 1
    writeToFiles("int", "HADDR_T", "haddr_t_f", 1, H5_FORTRAN_HAS_INTEGER_1_KIND);
#else
    /* Error: couldn't find a size for haddr_t */
    return -1;
#endif

    /* hsize_t */
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_HSIZE_T >= 8
    writeToFiles("hsize_t", "HSIZE_T", "hsize_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_4_KIND && H5_SIZEOF_HSIZE_T >= 4
    writeToFiles("hsize_t", "HSIZE_T", "hsize_t_f", 4, H5_FORTRAN_HAS_INTEGER_4_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_2_KIND && H5_SIZEOF_HSIZE_T >= 2
    writeToFiles("hsize_t", "HSIZE_T", "hsize_t_f", 2, H5_FORTRAN_HAS_INTEGER_2_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_1_KIND && H5_SIZEOF_HSIZE_T >= 1
    writeToFiles("hsize_t", "HSIZE_T", "hsize_t_f", 1, H5_FORTRAN_HAS_INTEGER_1_KIND);
#else
    /* Error: couldn't find a size for hsize_t */
    return -1;
#endif

    /* hssize_t */
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_HSSIZE_T >= 8
    writeToFiles("int", "HSSIZE_T", "hssize_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_4_KIND && H5_SIZEOF_HSSIZE_T >= 4
    writeToFiles("int", "HSSIZE_T", "hssize_t_f", 4, H5_FORTRAN_HAS_INTEGER_4_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_2_KIND && H5_SIZEOF_HSSIZE_T >= 2
    writeToFiles("int", "HSSIZE_T", "hssize_t_f", 2, H5_FORTRAN_HAS_INTEGER_2_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_1_KIND && H5_SIZEOF_HSSIZE_T >= 1
    writeToFiles("int", "HSSIZE_T", "hssize_t_f", 1, H5_FORTRAN_HAS_INTEGER_1_KIND);
#else
    /* Error: couldn't find a size for hssize_t */
    return -1;
#endif

    /* off_t */
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_OFF_T >= 8
    writeToFiles("int", "OFF_T", "off_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_4_KIND && H5_SIZEOF_OFF_T >= 4
    writeToFiles("int", "OFF_T", "off_t_f", 4, H5_FORTRAN_HAS_INTEGER_4_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_2_KIND && H5_SIZEOF_OFF_T >= 2
    writeToFiles("int", "OFF_T", "off_t_f", 2, H5_FORTRAN_HAS_INTEGER_2_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_1_KIND && H5_SIZEOF_OFF_T >= 1
    writeToFiles("int", "OFF_T", "off_t_f", 1, H5_FORTRAN_HAS_INTEGER_1_KIND);
#else
    /* Error: couldn't find a size for off_t */
    return -1;
#endif

    /* size_t */
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_SIZE_T >= 8
    writeToFiles("size_t", "SIZE_T", "size_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_4_KIND && H5_SIZEOF_SIZE_T >= 4
    writeToFiles("size_t", "SIZE_T", "size_t_f", 4, H5_FORTRAN_HAS_INTEGER_4_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_2_KIND && H5_SIZEOF_SIZE_T >= 2
    writeToFiles("size_t", "SIZE_T", "size_t_f", 2, H5_FORTRAN_HAS_INTEGER_2_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_1_KIND && H5_SIZEOF_SIZE_T >= 1
    writeToFiles("size_t", "SIZE_T", "size_t_f", 1, H5_FORTRAN_HAS_INTEGER_1_KIND);
#else
    /* Error: couldn't find a size for size_t */
    return -1;
#endif

    /* int */
#if defined H5_FORTRAN_HAS_NATIVE_8_KIND
    writeToFiles("int", "Fortran_INTEGER", "int_f", 8, H5_FORTRAN_HAS_NATIVE_8_KIND);
#elif defined H5_FORTRAN_HAS_NATIVE_4_KIND
    writeToFiles("int", "Fortran_INTEGER", "int_f", 4, H5_FORTRAN_HAS_NATIVE_4_KIND);
#elif defined H5_FORTRAN_HAS_NATIVE_2_KIND
    writeToFiles("int", "Fortran_INTEGER", "int_f", 2, H5_FORTRAN_HAS_NATIVE_2_KIND);
#elif defined H5_FORTRAN_HAS_NATIVE_1_KIND
    writeToFiles("int", "Fortran_INTEGER", "int_f", 1, H5_FORTRAN_HAS_NATIVE_1_KIND);
#else
    /* Error: couldn't find a size for int */
    return -1;
#endif

    /* int_1, int_2, int_4, int_8 */

    /* Defined different KINDs of integers:                       */
    /* if the integer kind is not available then we assign        */
    /* it a value of the next larger one, but if the next         */
    /* higher one is not available we assigned it the next lowest */

    FoundIntSize[0] = -1;
    FoundIntSize[1] = -2;
    FoundIntSize[2] = -4;
    FoundIntSize[3] = -8;

#if defined H5_FORTRAN_HAS_INTEGER_1_KIND
    FoundIntSize[0]     = 1;
    FoundIntSizeKind[0] = H5_FORTRAN_HAS_INTEGER_1_KIND;
#endif
#if defined H5_FORTRAN_HAS_INTEGER_2_KIND
    FoundIntSize[1]     = 2;
    FoundIntSizeKind[1] = H5_FORTRAN_HAS_INTEGER_2_KIND;
#endif
#if defined H5_FORTRAN_HAS_INTEGER_4_KIND
    FoundIntSize[2]     = 4;
    FoundIntSizeKind[2] = H5_FORTRAN_HAS_INTEGER_4_KIND;
#endif
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND
    FoundIntSize[3]     = 8;
    FoundIntSizeKind[3] = H5_FORTRAN_HAS_INTEGER_8_KIND;
#endif

    for (i = 0; i < 4; i++) {
        if (FoundIntSize[i] > 0) /* Found the integer type */
        {
            sprintf(chrA, "Fortran_INTEGER_%d", FoundIntSize[i]);
            sprintf(chrB, "int_%d_f", FoundIntSize[i]);
            writeToFiles("int", chrA, chrB, FoundIntSize[i], FoundIntSizeKind[i]);
        }
        else /* Did not find the integer type */
        {
            flag = 0;                   /* flag indicating if found the next highest */
            for (j = i + 1; j < 4; j++) /* search for next highest */
            {
                if (FoundIntSize[j] > 0) /* Found the next highest */
                {
                    sprintf(chrA, "Fortran_INTEGER_%d", (-1) * FoundIntSize[i]);
                    sprintf(chrB, "int_%d_f", (-1) * FoundIntSize[i]);
                    writeToFiles("int", chrA, chrB, FoundIntSize[j], FoundIntSizeKind[j]);
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) /* No higher one found, so find next lowest */
            {
                for (j = 2; j > -1; j--) /* Search for next lowest */
                {
                    if (FoundIntSize[j] > 0) /* Found the next lowest */
                    {
                        sprintf(chrA, "Fortran_INTEGER_%d", (-1) * FoundIntSize[i]);
                        sprintf(chrB, "int_%d_f", (-1) * FoundIntSize[i]);
                        writeToFiles("int", chrA, chrB, FoundIntSize[j], FoundIntSizeKind[j]);
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag == 0) /* No higher or lower one found, indicating an error */
                return -1;
        }
    }

    /* real_4, real_8, real_16 */

    /* Defined different KINDs of reals:                          */
    /* if the REAL kind is not available then we assign           */
    /* it a value of the next larger one, but if the next         */
    /* higher one is not available we assigned it the next lowest */

    FoundRealSize[0] = -4;
    FoundRealSize[1] = -8;
    FoundRealSize[2] = -16;

#if defined H5_FORTRAN_HAS_REAL_4_KIND
    FoundRealSize[0]     = 4;
    FoundRealSizeKind[0] = H5_FORTRAN_HAS_REAL_4_KIND;
#endif
#if defined H5_FORTRAN_HAS_REAL_8_KIND
    FoundRealSize[1]     = 8;
    FoundRealSizeKind[1] = H5_FORTRAN_HAS_REAL_8_KIND;
#endif

#if defined H5_FORTRAN_HAS_REAL_16_KIND
    if (H5_C_HAS_REAL_NATIVE_16 != 0) {
        FoundRealSize[2]     = 16;
        FoundRealSizeKind[2] = H5_FORTRAN_HAS_REAL_16_KIND;
    }
#endif

    for (i = 0; i < 3; i++) {
        if (FoundRealSize[i] > 0) /* Found the real type */
        {
            sprintf(chrA, "Fortran_REAL_%d", FoundRealSize[i]);
            sprintf(chrB, "real_%d_f", FoundRealSize[i]);
            writeToFiles("float", chrA, chrB, FoundRealSize[i], FoundRealSizeKind[i]);
        }
        else /* Did not find the real type */
        {
            flag = 0;                   /* flag indicating if found the next highest */
            for (j = i + 1; j < 3; j++) /* search for next highest */
            {
                if (FoundRealSize[j] > 0) /* Found the next highest */
                {
                    sprintf(chrA, "Fortran_REAL_%d", (-1) * FoundRealSize[i]);
                    sprintf(chrB, "real_%d_f", (-1) * FoundRealSize[i]);
                    if (FoundRealSize[j] > 4) {
                        writeToFiles("float", chrA, chrB, FoundRealSize[j], FoundRealSizeKind[j]);
                        flag = 1;
                    }
                    /*   else { */
                    /* 		    writeToFiles("float", chrA, chrB, FoundRealSize[j]); */
                    /* 		  } */
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) /* No higher one found, so find next lowest */
            {
                for (j = 1; j > -1; j--) /* Search for next lowest */
                {
                    if (FoundRealSize[j] > 0) /* Found the next lowest */
                    {
                        sprintf(chrA, "Fortran_REAL_%d", (-1) * FoundRealSize[i]);
                        sprintf(chrB, "real_%d_f", (-1) * FoundRealSize[i]);
                        if (FoundRealSize[j] > 4)
                            writeToFiles("float", chrA, chrB, FoundRealSize[j], FoundRealSizeKind[j]);
                        /*  else { */
                        /* 			writeToFiles("float", chrA, chrB, FoundRealSize[j]); */
                        /* 		      } */
                        flag = 1;
                        break;
                    }
                }
            }
            if (flag == 0) /* No higher or lower one found, indicating an error */
                return -1;
        }
    }

    /* hid_t */
#if defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_HID_T >= 8
    writeToFiles("int", "HID_T", "hid_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_4_KIND && H5_SIZEOF_HID_T >= 4
    writeToFiles("int", "HID_T", "hid_t_f", 4, H5_FORTRAN_HAS_INTEGER_4_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_2_KIND && H5_SIZEOF_HID_T >= 2
    writeToFiles("int", "HID_T", "hid_t_f", 2, H5_FORTRAN_HAS_INTEGER_2_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_1_KIND && H5_SIZEOF_HID_T >= 1
    writeToFiles("int", "HID_T", "hid_t_f", 1, H5_FORTRAN_HAS_INTEGER_1_KIND);
#elif defined H5_FORTRAN_HAS_INTEGER_8_KIND && H5_SIZEOF_HID_T >= 4
    writeToFiles("int", "HID_T", "hid_t_f", 8, H5_FORTRAN_HAS_INTEGER_8_KIND);
#else
    /* Error: couldn't find a size for hid_t */
    return -1;
#endif

    /* real_f */
#if defined H5_FORTRAN_HAS_REAL_NATIVE_16_KIND
    if (H5_C_HAS_REAL_NATIVE_16 != 0) {
        writeToFiles("float", "Fortran_REAL", "real_f", 16, H5_FORTRAN_HAS_REAL_NATIVE_16_KIND);
    }
#elif defined H5_FORTRAN_HAS_REAL_NATIVE_8_KIND
    writeToFiles("float", "Fortran_REAL", "real_f", 8, H5_FORTRAN_HAS_REAL_NATIVE_8_KIND);
#elif defined H5_FORTRAN_HAS_REAL_NATIVE_4_KIND
    writeToFiles("float", "Fortran_REAL", "real_f", 4, H5_FORTRAN_HAS_REAL_NATIVE_4_KIND);
#else
    /* Error: couldn't find a size for real_f */
    return -1;
#endif

    /* double_f */
#if defined H5_FORTRAN_HAS_DOUBLE_NATIVE_16_KIND
    if (H5_C_HAS_REAL_NATIVE_16 != 0) { /* Check if C has 16 byte floats */
        writeToFiles("float", "Fortran_DOUBLE", "double_f", 16, H5_FORTRAN_HAS_DOUBLE_NATIVE_16_KIND);
    }
    else {
#if defined H5_FORTRAN_HAS_REAL_NATIVE_8_KIND /* Fall back to 8 byte floats */
        writeToFiles("float", "Fortran_DOUBLE", "double_f", 8, H5_FORTRAN_HAS_REAL_NATIVE_8_KIND);
    }
#elif defined H5_FORTRAN_HAS_REAL_NATIVE_4_KIND /* Fall back to 4 byte floats */
        writeToFiles("float", "Fortran_DOUBLE", "double_f", 4, H5_FORTRAN_HAS_REAL_NATIVE_4_KIND);
    }
#else
        /* Error: couldn't find a size for double_f when fortran has 16 byte reals */
        return -1;
    }
#endif

#elif defined H5_FORTRAN_HAS_DOUBLE_NATIVE_8_KIND
    writeToFiles("float", "Fortran_DOUBLE", "double_f", 8, H5_FORTRAN_HAS_DOUBLE_NATIVE_8_KIND);
#else
    /* Error: couldn't find a size for real_f */
    return -1;
#endif

    /* Need the buffer size for the fortran derive type 'hdset_reg_ref_t_f03'
     * in order to be interoperable with C's structure, the C buffer size
     * H5R_DSET_REG_REF_BUF_SIZE is (sizeof(haddr_t)+4)
     */

    fprintf(fort_header, "        INTEGER, PARAMETER :: H5R_DSET_REG_REF_BUF_SIZE_F = %u\n",
            H5_SIZEOF_HADDR_T + 4);

    /* Close files */
    endCfile();
    endFfile();
    fclose(c_header);
    fclose(fort_header);
    return 0;
}
