/**
 * @file types.h
 * @author Gibran Fuentes Pineda <gibranfp@turing.iimas.unam.mx>
 * @date 2015
 *
 * @section GPL
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @brief Declaration of general data types and macros.
 */
#ifndef TYPES_H
#define TYPES_H

#define min(a, b) (a) < (b) ? a : b
#define max(a, b) (a) < (b) ? b : a

#define INF 1.7976931348623157e308
#define LARGEST_INT 4294967295 //Largest 32-bit unsigned integer
#define LARGEST_PRIME 4294967291 //Largest 32-bit prime number (2^32-5) 
                                 //[E2LSH Manual, Andoni and Indyk 2005]

#define LARGEST_INT64 18446744073709551615ULL
#define LARGEST_PRIME64 18446744073709551557ULL

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;
#endif
