﻿/* Copyright (C) 2002 Jean-Marc Valin 
   File: hexc_table.c
   Codebook for high-band excitation in SB-CELP mode (8000 bps with sign)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

const signed char hexc_table[1024] = {
-24, 21, -20, 5, -5, -7, 14, -10, 
2, -27, 16, -20, 0, -32, 26, 19, 
8, -11, -41, 31, 28, -27, -32, 34, 
42, 34, -17, 22, -10, 13, -29, 18, 
-12, -26, -24, 11, 22, 5, -5, -5, 
54, -68, -43, 57, -25, 24, 4, 4, 
26, -8, -12, -17, 54, 30, -45, 1, 
10, -15, 18, -41, 11, 68, -67, 37, 
-16, -24, -16, 38, -22, 6, -29, 30, 
66, -27, 5, 7, -16, 13, 2, -12, 
-7, -3, -20, 36, 4, -28, 9, 3, 
32, 48, 26, 39, 3, 0, 7, -21, 
-13, 5, -82, -7, 73, -20, 34, -9, 
-5, 1, -1, 10, -5, -10, -1, 9, 
1, -9, 10, 0, -14, 11, -1, -2, 
-1, 11, 20, 96, -81, -22, -12, -9, 
-58, 9, 24, -30, 26, -35, 27, -12, 
13, -18, 56, -59, 15, -7, 23, -15, 
-1, 6, -25, 14, -22, -20, 47, -11, 
16, 2, 38, -23, -19, -30, -9, 40, 
-11, 5, 4, -6, 8, 26, -21, -11, 
127, 4, 1, 6, -9, 2, -7, -2, 
-3, 7, -5, 10, -19, 7, -106, 91, 
-3, 9, -4, 21, -8, 26, -80, 8, 
1, -2, -10, -17, -17, -27, 32, 71, 
6, -29, 11, -23, 54, -38, 29, -22, 
39, 87, -31, -12, -20, 3, -2, -2, 
2, 20, 0, -1, -35, 27, 9, -6, 
-12, 3, -12, -6, 13, 1, 14, -22, 
-59, -15, -17, -25, 13, -7, 7, 3, 
0, 1, -7, 6, -3, 61, -37, -23, 
-23, -29, 38, -31, 27, 1, -8, 2, 
-27, 23, -26, 36, -34, 5, 24, -24, 
-6, 7, 3, -59, 78, -62, 44, -16, 
1, 6, 0, 17, 8, 45, 0, -110, 
6, 14, -2, 32, -77, -56, 62, -3, 
3, -13, 4, -16, 102, -15, -36, -1, 
9, -113, 6, 23, 0, 9, 9, 5, 
-8, -1, -14, 5, -12, 121, -53, -27, 
-8, -9, 22, -13, 3, 2, -3, 1, 
-2, -71, 95, 38, -19, 15, -16, -5, 
71, 10, 2, -32, -13, -5, 15, -1, 
-2, -14, -85, 30, 29, 6, 3, 2, 
0, 0, 0, 0, 0, 0, 0, 0, 
2, -65, -56, -9, 18, 18, 23, -14, 
-2, 0, 12, -29, 26, -12, 1, 2, 
-12, -64, 90, -6, 4, 1, 5, -5, 
-110, -3, -31, 22, -29, 9, 0, 8, 
-40, -5, 21, -5, -5, 13, 10, -18, 
40, 1, 35, -20, 30, -28, 11, -6, 
19, 7, 14, 18, -64, 9, -6, 16, 
51, 68, 8, 16, 12, -8, 0, -9, 
20, -22, 25, 7, -4, -13, 41, -35, 
93, -18, -54, 11, -1, 1, -9, 4, 
-66, 66, -31, 20, -22, 25, -23, 11, 
10, 9, 19, 15, 11, -5, -31, -10, 
-23, -28, -6, -6, -3, -4, 5, 3, 
-28, 22, -11, -42, 25, -25, -16, 41, 
34, 47, -6, 2, 42, -19, -22, 5, 
-39, 32, 6, -35, 22, 17, -30, 8, 
-26, -11, -11, 3, -12, 33, 33, -37, 
21, -1, 6, -4, 3, 0, -5, 5, 
12, -12, 57, 27, -61, -3, 20, -17, 
2, 0, 4, 0, -2, -33, -58, 81, 
-23, 39, -10, -5, 2, 6, -7, 5, 
4, -3, -2, -13, -23, -72, 107, 15, 
-5, 0, -7, -3, -6, 5, -4, 15, 
47, 12, -31, 25, -16, 8, 22, -25, 
-62, -56, -18, 14, 28, 12, 2, -11, 
74, -66, 41, -20, -7, 16, -20, 16, 
-8, 0, -16, 4, -19, 92, 12, -59, 
-14, -39, 49, -25, -16, 23, -27, 19, 
-3, -33, 19, 85, -29, 6, -7, -10, 
16, -7, -12, 1, -6, 2, 4, -2, 
64, 10, -25, 41, -2, -31, 15, 0, 
110, 50, 69, 35, 28, 19, -10, 2, 
-43, -49, -56, -15, -16, 10, 3, 12, 
-1, -8, 1, 26, -12, -1, 7, -11, 
-27, 41, 25, 1, -11, -18, 22, -7, 
-1, -47, -8, 23, -3, -17, -7, 18, 
-125, 59, -5, 3, 18, 1, 2, 3, 
27, -35, 65, -53, 50, -46, 37, -21, 
-28, 7, 14, -37, -5, -5, 12, 5, 
-8, 78, -19, 21, -6, -16, 8, -7, 
5, 2, 7, 2, 10, -6, 12, -60, 
44, 11, -36, -32, 31, 0, 2, -2, 
2, 1, -3, 7, -10, 17, -21, 10, 
6, -2, 19, -2, 59, -38, -86, 38, 
8, -41, -30, -45, -33, 7, 15, 28, 
29, -7, 24, -40, 7, 7, 5, -2, 
9, 24, -23, -18, 6, -29, 30, 2, 
28, 49, -11, -46, 10, 43, -13, -9, 
-1, -3, -7, -7, -17, -6, 97, -33, 
-21, 3, 5, 1, 12, -43, -8, 28, 
7, -43, -7, 17, -20, 19, -1, 2, 
-13, 9, 54, 34, 9, -28, -11, -9, 
-17, 110, -59, 44, -26, 0, 3, -12, 
-47, 73, -34, -43, 38, -33, 16, -5, 
-46, -4, -6, -2, -25, 19, -29, 28, 
-13, 5, 14, 27, -40, -43, 4, 32, 
-13, -2, -35, -4, 112, -42, 9, -12, 
37, -28, 17, 14, -19, 35, -39, 23, 
3, -14, -1, -57, -5, 94, -9, 3, 
-39, 5, 30, -10, -32, 42, -13, -14, 
-97, -63, 30, -9, 1, -7, 12, 5, 
20, 17, -9, -36, -30, 25, 47, -9, 
-15, 12, -22, 98, -8, -50, 15, -27, 
21, -16, -11, 2, 12, -10, 10, -3, 
33, 36, -96, 0, -17, 31, -9, 9, 
3, -20, 13, -11, 8, -4, 10, -10, 
9, 1, 112, -70, -27, 5, -21, 2, 
-57, -3, -29, 10, 19, -21, 21, -10, 
-66, -3, 91, -35, 30, -12, 0, -7, 
59, -28, 26, 2, 14, -18, 1, 1, 
11, 17, 20, -54, -59, 27, 4, 29, 
32, 5, 19, 12, -4, 1, 7, -10, 
5, -2, 10, 0, 23, -5, 28, -104, 
46, 11, 16, 3, 29, 1, -8, -14, 
1, 7, -50, 88, -62, 26, 8, -17, 
-14, 50, 0, 32, -12, -3, -27, 18, 
-8, -5, 8, 3, -20, -11, 37, -12, 
9, 33, 46, -101, -1, -4, 1, 6, 
-1, 28, -42, -15, 16, 5, -1, -2, 
-55, 85, 38, -9, -4, 11, -2, -9, 
-6, 3, -20, -10, -77, 89, 24, -3, 
-104, -57, -26, -31, -20, -6, -9, 14, 
20, -23, 46, -15, -31, 28, 1, -15, 
-2, 6, -2, 31, 45, -76, 23, -25, 
};
