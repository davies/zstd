/*
    Buffered version of Zstd compression library
    Copyright (C) 2015, Yann Collet.

    BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following disclaimer
    in the documentation and/or other materials provided with the
    distribution.
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    You can contact the author at :
    - zstd source repository : https://github.com/Cyan4973/zstd
    - ztsd public forum : https://groups.google.com/forum/#!forum/lz4c
*/
#ifndef ZSTD0_BUFFERED_H
#define ZSTD0_BUFFERED_H

/* The objects defined into this file should be considered experimental.
 * They are not labelled stable, as their prototype may change in the future.
 * You can use them for tests, provide feedback, or if you can endure risk of future changes.
 */

#if defined (__cplusplus)
extern "C" {
#endif

/* *************************************
*  Includes
***************************************/
#include <stddef.h>   /* size_t */


/* ***************************************************************
*  Compiler specifics
*****************************************************************/
/*!
*  ZSTD0_DLL_EXPORT :
*  Enable exporting of functions when building a Windows DLL
*/
#if defined(_WIN32) && defined(ZSTD0_DLL_EXPORT) && (ZSTD0_DLL_EXPORT==1)
#  define ZSTDLIB_API __declspec(dllexport)
#else
#  define ZSTDLIB_API
#endif


/* *************************************
*  Streaming functions
***************************************/
typedef struct ZBUFF0_CCtx_s ZBUFF0_CCtx;
ZSTDLIB_API ZBUFF0_CCtx* ZBUFF0_createCCtx(void);
ZSTDLIB_API size_t      ZBUFF0_freeCCtx(ZBUFF0_CCtx* cctx);

ZSTDLIB_API size_t ZBUFF0_compressInit(ZBUFF0_CCtx* cctx, int compressionLevel);
ZSTDLIB_API size_t ZBUFF0_compressInitDictionary(ZBUFF0_CCtx* cctx, const void* dict, size_t dictSize, int compressionLevel);

ZSTDLIB_API size_t ZBUFF0_compressContinue(ZBUFF0_CCtx* cctx, void* dst, size_t* dstCapacityPtr, const void* src, size_t* srcSizePtr);
ZSTDLIB_API size_t ZBUFF0_compressFlush(ZBUFF0_CCtx* cctx, void* dst, size_t* dstCapacityPtr);
ZSTDLIB_API size_t ZBUFF0_compressEnd(ZBUFF0_CCtx* cctx, void* dst, size_t* dstCapacityPtr);

/** ************************************************
*  Streaming compression
*
*  A ZBUFF0_CCtx object is required to track streaming operation.
*  Use ZBUFF0_createCCtx() and ZBUFF0_freeCCtx() to create/release resources.
*  ZBUFF0_CCtx objects can be reused multiple times.
*
*  Start by initializing ZBUF_CCtx.
*  Use ZBUFF0_compressInit() to start a new compression operation.
*  Use ZBUFF0_compressInitDictionary() for a compression which requires a dictionary.
*
*  Use ZBUFF0_compressContinue() repetitively to consume input stream.
*  *srcSizePtr and *dstCapacityPtr can be any size.
*  The function will report how many bytes were read or written within *srcSizePtr and *dstCapacityPtr.
*  Note that it may not consume the entire input, in which case it's up to the caller to present again remaining data.
*  The content of @dst will be overwritten (up to *dstCapacityPtr) at each call, so save its content if it matters or change @dst .
*  @return : a hint to preferred nb of bytes to use as input for next function call (it's just a hint, to improve latency)
*            or an error code, which can be tested using ZBUFF0_isError().
*
*  At any moment, it's possible to flush whatever data remains within buffer, using ZBUFF0_compressFlush().
*  The nb of bytes written into @dst will be reported into *dstCapacityPtr.
*  Note that the function cannot output more than *dstCapacityPtr,
*  therefore, some content might still be left into internal buffer if *dstCapacityPtr is too small.
*  @return : nb of bytes still present into internal buffer (0 if it's empty)
*            or an error code, which can be tested using ZBUFF0_isError().
*
*  ZBUFF0_compressEnd() instructs to finish a frame.
*  It will perform a flush and write frame epilogue.
*  The epilogue is required for decoders to consider a frame completed.
*  Similar to ZBUFF0_compressFlush(), it may not be able to output the entire internal buffer content if *dstCapacityPtr is too small.
*  In which case, call again ZBUFF0_compressFlush() to complete the flush.
*  @return : nb of bytes still present into internal buffer (0 if it's empty)
*            or an error code, which can be tested using ZBUFF0_isError().
*
*  Hint : recommended buffer sizes (not compulsory) : ZBUFF0_recommendedCInSize / ZBUFF0_recommendedCOutSize
*  input : ZBUFF0_recommendedCInSize==128 KB block size is the internal unit, it improves latency to use this value (skipped buffering).
*  output : ZBUFF0_recommendedCOutSize==ZSTD0_compressBound(128 KB) + 3 + 3 : ensures it's always possible to write/flush/end a full block. Skip some buffering.
*  By using both, it ensures that input will be entirely consumed, and output will always contain the result, reducing intermediate buffering.
* **************************************************/


typedef struct ZBUFF0_DCtx_s ZBUFF0_DCtx;
ZSTDLIB_API ZBUFF0_DCtx* ZBUFF0_createDCtx(void);
ZSTDLIB_API size_t      ZBUFF0_freeDCtx(ZBUFF0_DCtx* dctx);

ZSTDLIB_API size_t ZBUFF0_decompressInit(ZBUFF0_DCtx* dctx);
ZSTDLIB_API size_t ZBUFF0_decompressInitDictionary(ZBUFF0_DCtx* dctx, const void* dict, size_t dictSize);

ZSTDLIB_API size_t ZBUFF0_decompressContinue(ZBUFF0_DCtx* dctx, void* dst, size_t* dstCapacityPtr, const void* src, size_t* srcSizePtr);

/** ************************************************
*  Streaming decompression
*
*  A ZBUFF0_DCtx object is required to track streaming operation.
*  Use ZBUFF0_createDCtx() and ZBUFF0_freeDCtx() to create/release resources.
*  Use ZBUFF0_decompressInit() to start a new decompression operation,
*   or ZBUFF0_decompressInitDictionary() if decompression requires a dictionary.
*  Note that ZBUFF0_DCtx objects can be reused multiple times.
*
*  Use ZBUFF0_decompressContinue() repetitively to consume your input.
*  *srcSizePtr and *dstCapacityPtr can be any size.
*  The function will report how many bytes were read or written by modifying *srcSizePtr and *dstCapacityPtr.
*  Note that it may not consume the entire input, in which case it's up to the caller to present remaining input again.
*  The content of @dst will be overwritten (up to *dstCapacityPtr) at each function call, so save its content if it matters or change @dst.
*  @return : a hint to preferred nb of bytes to use as input for next function call (it's only a hint, to help latency)
*            or 0 when a frame is completely decoded
*            or an error code, which can be tested using ZBUFF0_isError().
*
*  Hint : recommended buffer sizes (not compulsory) : ZBUFF0_recommendedDInSize / ZBUFF0_recommendedDOutSize
*  output : ZBUFF0_recommendedDOutSize==128 KB block size is the internal unit, it ensures it's always possible to write a full block when it's decoded.
*  input : ZBUFF0_recommendedDInSize==128Kb+3; just follow indications from ZBUFF0_decompressContinue() to minimize latency. It should always be <= 128 KB + 3 .
* **************************************************/


/* *************************************
*  Tool functions
***************************************/
ZSTDLIB_API unsigned ZBUFF0_isError(size_t errorCode);
ZSTDLIB_API const char* ZBUFF0_getErrorName(size_t errorCode);

/** The below functions provide recommended buffer sizes for Compression or Decompression operations.
*   These sizes are just hints, and tend to offer better latency */
ZSTDLIB_API size_t ZBUFF0_recommendedCInSize(void);
ZSTDLIB_API size_t ZBUFF0_recommendedCOutSize(void);
ZSTDLIB_API size_t ZBUFF0_recommendedDInSize(void);
ZSTDLIB_API size_t ZBUFF0_recommendedDOutSize(void);


#if defined (__cplusplus)
}
#endif

#endif  /* ZSTD0_BUFFERED_H */
