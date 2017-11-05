/*
    zstd - standard compression library
    Header File for static linking only
    Copyright (C) 2014-2016, Yann Collet.

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
*/
#ifndef ZSTD0_STATIC_H
#define ZSTD0_STATIC_H

/* The objects defined into this file shall be considered experimental.
 * They are not considered stable, as their prototype may change in the future.
 * You can use them for tests, provide feedback, or if you can endure risks of future changes.
 */

#if defined (__cplusplus)
extern "C" {
#endif

/*-*************************************
*  Dependencies
***************************************/
#include "zstd.h"
#include "mem.h"


/*-*************************************
*  Types
***************************************/
#define ZSTD0_WINDOWLOG_MAX 26
#define ZSTD0_WINDOWLOG_MIN 18
#define ZSTD0_WINDOWLOG_ABSOLUTEMIN 11
#define ZSTD0_CONTENTLOG_MAX (ZSTD0_WINDOWLOG_MAX+1)
#define ZSTD0_CONTENTLOG_MIN 4
#define ZSTD0_HASHLOG_MAX 28
#define ZSTD0_HASHLOG_MIN 4
#define ZSTD0_SEARCHLOG_MAX (ZSTD0_CONTENTLOG_MAX-1)
#define ZSTD0_SEARCHLOG_MIN 1
#define ZSTD0_SEARCHLENGTH_MAX 7
#define ZSTD0_SEARCHLENGTH_MIN 4

/** from faster to stronger */
typedef enum { ZSTD0_fast, ZSTD0_greedy, ZSTD0_lazy, ZSTD0_lazy2, ZSTD0_btlazy2 } ZSTD0_strategy;

typedef struct
{
    uint64_t srcSize;       /* optional : tells how much bytes are present in the frame. Use 0 if not known. */
    uint32_t windowLog;     /* largest match distance : larger == more compression, more memory needed during decompression */
    uint32_t contentLog;    /* full search segment : larger == more compression, slower, more memory (useless for fast) */
    uint32_t hashLog;       /* dispatch table : larger == more memory, faster */
    uint32_t searchLog;     /* nb of searches : larger == more compression, slower */
    uint32_t searchLength;  /* size of matches : larger == faster decompression, sometimes less compression */
    ZSTD0_strategy strategy;
} ZSTD0_parameters;


/* *************************************
*  Advanced functions
***************************************/
#define ZSTD0_MAX_CLEVEL 20
ZSTDLIB_API unsigned ZSTD0_maxCLevel (void);

/*! ZSTD0_getParams() :
*   @return ZSTD0_parameters structure for a selected compression level and srcSize.
*   `srcSizeHint` value is optional, select 0 if not known */
ZSTDLIB_API ZSTD0_parameters ZSTD0_getParams(int compressionLevel, U64 srcSizeHint);

/*! ZSTD0_validateParams() :
*   correct params value to remain within authorized range */
ZSTDLIB_API void ZSTD0_validateParams(ZSTD0_parameters* params);

/*! ZSTD0_compress_advanced() :
*   Same as ZSTD0_compress_usingDict(), with fine-tune control of each compression parameter */
ZSTDLIB_API size_t ZSTD0_compress_advanced (ZSTD0_CCtx* ctx,
                                           void* dst, size_t dstCapacity,
                                     const void* src, size_t srcSize,
                                     const void* dict,size_t dictSize,
                                           ZSTD0_parameters params);

/*! ZSTD0_compress_usingPreparedDCtx() :
*   Same as ZSTD0_compress_usingDict, but using a reference context `preparedCCtx`, where dictionary has been loaded.
*   It avoids reloading the dictionary each time.
*   `preparedCCtx` must have been properly initialized using ZSTD0_compressBegin_usingDict() or ZSTD0_compressBegin_advanced().
*   Requires 2 contexts : 1 for reference, which will not be modified, and 1 to run the compression operation */
ZSTDLIB_API size_t ZSTD0_compress_usingPreparedCCtx(
                                           ZSTD0_CCtx* cctx, const ZSTD0_CCtx* preparedCCtx,
                                           void* dst, size_t dstCapacity,
                                     const void* src, size_t srcSize);

/*- Advanced Decompression functions -*/

/*! ZSTD0_decompress_usingPreparedDCtx() :
*   Same as ZSTD0_decompress_usingDict, but using a reference context `preparedDCtx`, where dictionary has been loaded.
*   It avoids reloading the dictionary each time.
*   `preparedDCtx` must have been properly initialized using ZSTD0_decompressBegin_usingDict().
*   Requires 2 contexts : 1 for reference, which will not be modified, and 1 to run the decompression operation */
ZSTDLIB_API size_t ZSTD0_decompress_usingPreparedDCtx(
                                             ZSTD0_DCtx* dctx, const ZSTD0_DCtx* preparedDCtx,
                                             void* dst, size_t dstCapacity,
                                       const void* src, size_t srcSize);


/* **************************************
*  Streaming functions (direct mode)
****************************************/
ZSTDLIB_API size_t ZSTD0_compressBegin(ZSTD0_CCtx* cctx, int compressionLevel);
ZSTDLIB_API size_t ZSTD0_compressBegin_usingDict(ZSTD0_CCtx* cctx, const void* dict,size_t dictSize, int compressionLevel);
ZSTDLIB_API size_t ZSTD0_compressBegin_advanced(ZSTD0_CCtx* cctx, const void* dict,size_t dictSize, ZSTD0_parameters params);
ZSTDLIB_API size_t ZSTD0_copyCCtx(ZSTD0_CCtx* cctx, const ZSTD0_CCtx* preparedCCtx);

ZSTDLIB_API size_t ZSTD0_compressContinue(ZSTD0_CCtx* cctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);
ZSTDLIB_API size_t ZSTD0_compressEnd(ZSTD0_CCtx* cctx, void* dst, size_t dstCapacity);

/*
  Streaming compression, synchronous mode (bufferless)

  A ZSTD0_CCtx object is required to track streaming operations.
  Use ZSTD0_createCCtx() / ZSTD0_freeCCtx() to manage it.
  ZSTD0_CCtx object can be re-used multiple times within successive compression operations.

  Start by initializing a context.
  Use ZSTD0_compressBegin(), or ZSTD0_compressBegin_usingDict() for dictionary compression,
  or ZSTD0_compressBegin_advanced(), for finer parameter control.
  It's also possible to duplicate a reference context which has been initialized, using ZSTD0_copyCCtx()

  Then, consume your input using ZSTD0_compressContinue().
  The interface is synchronous, so all input will be consumed and produce a compressed output.
  You must ensure there is enough space in destination buffer to store compressed data under worst case scenario.
  Worst case evaluation is provided by ZSTD0_compressBound().

  Finish a frame with ZSTD0_compressEnd(), which will write the epilogue.
  Without the epilogue, frames will be considered incomplete by decoder.

  You can then reuse ZSTD0_CCtx to compress some new frame.
*/


ZSTDLIB_API size_t ZSTD0_decompressBegin(ZSTD0_DCtx* dctx);
ZSTDLIB_API size_t ZSTD0_decompressBegin_usingDict(ZSTD0_DCtx* dctx, const void* dict, size_t dictSize);
ZSTDLIB_API void   ZSTD0_copyDCtx(ZSTD0_DCtx* dctx, const ZSTD0_DCtx* preparedDCtx);

ZSTDLIB_API size_t ZSTD0_getFrameParams(ZSTD0_parameters* params, const void* src, size_t srcSize);

ZSTDLIB_API size_t ZSTD0_nextSrcSizeToDecompress(ZSTD0_DCtx* dctx);
ZSTDLIB_API size_t ZSTD0_decompressContinue(ZSTD0_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);

/*
  Streaming decompression, direct mode (bufferless)

  A ZSTD0_DCtx object is required to track streaming operations.
  Use ZSTD0_createDCtx() / ZSTD0_freeDCtx() to manage it.
  A ZSTD0_DCtx object can be re-used multiple times.

  First typical operation is to retrieve frame parameters, using ZSTD0_getFrameParams().
  This operation is independent, and just needs enough input data to properly decode the frame header.
  Objective is to retrieve *params.windowlog, to know minimum amount of memory required during decoding.
  Result : 0 when successful, it means the ZSTD0_parameters structure has been filled.
           >0 : means there is not enough data into src. Provides the expected size to successfully decode header.
           errorCode, which can be tested using ZSTD0_isError()

  Start decompression, with ZSTD0_decompressBegin() or ZSTD0_decompressBegin_usingDict()
  Alternatively, you can copy a prepared context, using ZSTD0_copyDCtx()

  Then use ZSTD0_nextSrcSizeToDecompress() and ZSTD0_decompressContinue() alternatively.
  ZSTD0_nextSrcSizeToDecompress() tells how much bytes to provide as 'srcSize' to ZSTD0_decompressContinue().
  ZSTD0_decompressContinue() requires this exact amount of bytes, or it will fail.
  ZSTD0_decompressContinue() needs previous data blocks during decompression, up to (1 << windowlog).
  They should preferably be located contiguously, prior to current block. Alternatively, a round buffer is also possible.

  @result of ZSTD0_decompressContinue() is the number of bytes regenerated within 'dst'.
  It can be zero, which is not an error; it just means ZSTD0_decompressContinue() has decoded some header.

  A frame is fully decoded when ZSTD0_nextSrcSizeToDecompress() returns zero.
  Context can then be reset to start a new decompression.
*/


/* **************************************
*  Block functions
****************************************/
/*! Block functions produce and decode raw zstd blocks, without frame metadata.
    User will have to save and regenerate necessary information to regenerate data, such as block sizes.

    A few rules to respect :
    - Uncompressed block size must be <= 128 KB
    - Compressing or decompressing requires a context structure
      + Use ZSTD0_createCCtx() and ZSTD0_createDCtx()
    - It is necessary to init context before starting
      + compression : ZSTD0_compressBegin()
      + decompression : ZSTD0_decompressBegin()
      + variants _usingDict() are also allowed
      + copyCCtx() and copyDCtx() work too
    - When a block is considered not compressible enough, ZSTD0_compressBlock() result will be zero.
      In which case, nothing is produced into `dst`.
      + User must test for such outcome and deal directly with uncompressed data
      + ZSTD0_decompressBlock() doesn't accept uncompressed data as input !!
*/

size_t ZSTD0_compressBlock  (ZSTD0_CCtx* cctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);
size_t ZSTD0_decompressBlock(ZSTD0_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);


/* *************************************
*  Error management
***************************************/
#include "error_public.h"
/*! ZSTD0_getErrorCode() :
    convert a `size_t` function result into a `ZSTD0_error_code` enum type,
    which can be used to compare directly with enum list within "error_public.h" */
ZSTD0_ErrorCode ZSTD0_getError(size_t code);


#if defined (__cplusplus)
}
#endif

#endif  /* ZSTD0_STATIC_H */
