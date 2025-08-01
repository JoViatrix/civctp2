/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIO_
#define _TIFFIO_

/*
 * TIFF I/O Library Definitions.
 */
#include "tiff.h"
#include "tiffvers.h"

#ifndef __GNUC__
# define __DLL_IMPORT__  __declspec(dllimport)
# define __DLL_EXPORT__  __declspec(dllexport)
#else
# define __DLL_IMPORT__  __attribute__((dllimport)) extern
# define __DLL_EXPORT__  __attribute__((dllexport)) extern
#endif

#if (defined __WIN32__) || (defined _WIN32)
# ifdef BUILD_LIBTIFF_DLL
#  define LIBTIFF_DLL_IMPEXP     __DLL_EXPORT__
# elif defined(LIBTIFF_STATIC)
#  define LIBTIFF_DLL_IMPEXP
# elif defined (USE_LIBTIFF_DLL)
#  define LIBTIFF_DLL_IMPEXP     __DLL_IMPORT__
# elif defined (USE_LIBTIFF_STATIC)
#  define LIBTIFF_DLL_IMPEXP
# else /* assume USE_LIBTIFF_DLL */
#  define LIBTIFF_DLL_IMPEXP     __DLL_IMPORT__
# endif
#else /* __WIN32__ */
# define LIBTIFF_DLL_IMPEXP
#endif

/*
 * TIFF is defined as an incomplete type to hide the
 * library's internal data structures from clients.
 */
typedef struct tiff TIFF;

/*
 * The following typedefs define the intrinsic size of
 * data types used in the *exported* interfaces.  These
 * definitions depend on the proper definition of types
 * in tiff.h.  Note also that the varargs interface used
 * to pass tag types and values uses the types defined in
 * tiff.h directly.
 *
 * NB: ttag_t is unsigned int and not unsigned short because
 *     ANSI C requires that the type before the ellipsis be a
 *     promoted type (i.e. one of int, unsigned int, pointer,
 *     or double) and because we defined pseudo-tags that are
 *     outside the range of legal Aldus-assigned tags.
 * NB: tsize_t is signed and not unsigned because some functions
 *     return -1.
 * NB: toff_t is not off_t for many reasons; TIFFs max out at
 *     32-bit file offsets, and BigTIFF maxes out at 64-bit
 *     offsets being the most important, and to ensure use of
 *     a consistently unsigned type across architectures.
 *     Prior to libtiff 4.0, this was an unsigned 32 bit type.
 */
/*
 * this is the machine addressing size type, only it's signed, so make it
 * int32_t on 32bit machines, int64_t on 64bit machines
 */
typedef TIFF_SSIZE_T tmsize_t;
#define TIFF_TMSIZE_T_MAX (tmsize_t)(SIZE_MAX >> 1)

typedef uint64_t toff_t; /* file offset */
/* the following are deprecated and should be replaced by their defining
   counterparts */
typedef uint32_t ttag_t;    /* directory tag */
typedef uint32_t tdir_t;    /* directory index */
typedef uint16_t tsample_t; /* sample number */
typedef uint32_t tstrile_t; /* strip or tile number */
typedef tstrile_t tstrip_t; /* strip number */
typedef tstrile_t ttile_t;  /* tile number */
typedef tmsize_t tsize_t;   /* i/o size in bytes */
typedef void *tdata_t;      /* image data ref */

/*
 * On windows you should define USE_WIN32_FILEIO if you are using tif_win32.c
 * or AVOID_WIN32_FILEIO if you are using something else (like tif_unix.c).
 *
 * By default tif_unix.c is assumed.
 */

#if defined(_WIN32)
#if !defined(__CYGWIN) && !defined(AVOID_WIN32_FILEIO) &&                      \
    !defined(USE_WIN32_FILEIO)
#define AVOID_WIN32_FILEIO
#endif
#endif

#if defined(USE_WIN32_FILEIO)
#define VC_EXTRALEAN
#include <windows.h>
#ifdef _WIN32
DECLARE_HANDLE(thandle_t); /* Win32 file handle */
#else
typedef HFILE thandle_t; /* client data handle */
#endif /* _WIN32 */
#else
typedef void *thandle_t; /* client data handle */
#endif /* USE_WIN32_FILEIO */

/*
 * Flags to pass to TIFFPrintDirectory to control
 * printing of data structures that are potentially
 * very large.   Bit-or these flags to enable printing
 * multiple items.
 */
#define TIFFPRINT_NONE 0x0           /* no extra info */
#define TIFFPRINT_STRIPS 0x1         /* strips/tiles info */
#define TIFFPRINT_CURVES 0x2         /* color/gray response curves */
#define TIFFPRINT_COLORMAP 0x4       /* colormap */
#define TIFFPRINT_JPEGQTABLES 0x100  /* JPEG Q matrices */
#define TIFFPRINT_JPEGACTABLES 0x200 /* JPEG AC tables */
#define TIFFPRINT_JPEGDCTABLES 0x200 /* JPEG DC tables */

/*
 * Colour conversion stuff
 */

/* reference white */
#define D65_X0 (95.0470F)
#define D65_Y0 (100.0F)
#define D65_Z0 (108.8827F)

#define D50_X0 (96.4250F)
#define D50_Y0 (100.0F)
#define D50_Z0 (82.4680F)

/* Structure for holding information about a display device. */

typedef unsigned char TIFFRGBValue; /* 8-bit samples */

typedef struct
{
    float d_mat[3][3]; /* XYZ -> luminance matrix */
    float d_YCR;       /* Light o/p for reference white */
    float d_YCG;
    float d_YCB;
    uint32_t d_Vrwr; /* Pixel values for ref. white */
    uint32_t d_Vrwg;
    uint32_t d_Vrwb;
    float d_Y0R; /* Residual light for black pixel */
    float d_Y0G;
    float d_Y0B;
    float d_gammaR; /* Gamma values for the three guns */
    float d_gammaG;
    float d_gammaB;
} TIFFDisplay;

typedef struct
{                           /* YCbCr->RGB support */
    TIFFRGBValue *clamptab; /* range clamping table */
    int *Cr_r_tab;
    int *Cb_b_tab;
    int32_t *Cr_g_tab;
    int32_t *Cb_g_tab;
    int32_t *Y_tab;
} TIFFYCbCrToRGB;

typedef struct
{              /* CIE Lab 1976->RGB support */
    int range; /* Size of conversion table */
#define CIELABTORGB_TABLE_RANGE 1500
    float rstep, gstep, bstep;
    float X0, Y0, Z0; /* Reference white point */
    TIFFDisplay display;
    float Yr2r[CIELABTORGB_TABLE_RANGE + 1]; /* Conversion of Yr to r */
    float Yg2g[CIELABTORGB_TABLE_RANGE + 1]; /* Conversion of Yg to g */
    float Yb2b[CIELABTORGB_TABLE_RANGE + 1]; /* Conversion of Yb to b */
} TIFFCIELabToRGB;

/*
 * RGBA-style image support.
 */
typedef struct _TIFFRGBAImage TIFFRGBAImage;
/*
 * The image reading and conversion routines invoke
 * ``put routines'' to copy/image/whatever tiles of
 * raw image data.  A default set of routines are
 * provided to convert/copy raw image data to 8-bit
 * packed ABGR format rasters.  Applications can supply
 * alternate routines that unpack the data into a
 * different format or, for example, unpack the data
 * and draw the unpacked raster on the display.
 */
typedef void (*tileContigRoutine)(TIFFRGBAImage *, uint32_t *, uint32_t,
                                  uint32_t, uint32_t, uint32_t, int32_t,
                                  int32_t, unsigned char *);
typedef void (*tileSeparateRoutine)(TIFFRGBAImage *, uint32_t *, uint32_t,
                                    uint32_t, uint32_t, uint32_t, int32_t,
                                    int32_t, unsigned char *, unsigned char *,
                                    unsigned char *, unsigned char *);
/*
 * RGBA-reader state.
 */
struct _TIFFRGBAImage
{
    TIFF *tif;                /* image handle */
    int stoponerr;            /* stop on read error */
    int isContig;             /* data is packed/separate */
    int alpha;                /* type of alpha data present */
    uint32_t width;           /* image width */
    uint32_t height;          /* image height */
    uint16_t bitspersample;   /* image bits/sample */
    uint16_t samplesperpixel; /* image samples/pixel */
    uint16_t orientation;     /* image orientation */
    uint16_t req_orientation; /* requested orientation */
    uint16_t photometric;     /* image photometric interp */
    uint16_t *redcmap;        /* colormap palette */
    uint16_t *greencmap;
    uint16_t *bluecmap;
    /* get image data routine */
    int (*get)(TIFFRGBAImage *, uint32_t *, uint32_t, uint32_t);
    /* put decoded strip/tile */
    union
    {
        void (*any)(TIFFRGBAImage *);
        tileContigRoutine contig;
        tileSeparateRoutine separate;
    } put;
    TIFFRGBValue *Map;       /* sample mapping array */
    uint32_t **BWmap;        /* black&white map */
    uint32_t **PALmap;       /* palette image map */
    TIFFYCbCrToRGB *ycbcr;   /* YCbCr conversion state */
    TIFFCIELabToRGB *cielab; /* CIE L*a*b conversion state */

    uint8_t *UaToAa; /* Unassociated alpha to associated alpha conversion LUT */
    uint8_t *Bitdepth16To8; /* LUT for conversion from 16bit to 8bit values */

    int row_offset;
    int col_offset;
};

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */
#define TIFFGetR(abgr) ((abgr)&0xff)
#define TIFFGetG(abgr) (((abgr) >> 8) & 0xff)
#define TIFFGetB(abgr) (((abgr) >> 16) & 0xff)
#define TIFFGetA(abgr) (((abgr) >> 24) & 0xff)

/*
 * A CODEC is a software package that implements decoding,
 * encoding, or decoding+encoding of a compression algorithm.
 * The library provides a collection of builtin codecs.
 * More codecs may be registered through calls to the library
 * and/or the builtin implementations may be overridden.
 */
typedef int (*TIFFInitMethod)(TIFF *, int);
typedef struct
{
    char *name;
    uint16_t scheme;
    TIFFInitMethod init;
} TIFFCodec;

typedef struct
{
    uint32_t uNum;
    uint32_t uDenom;
} TIFFRational_t;

#include <stdarg.h>
#include <stdio.h>

/* share internal LogLuv conversion routines? */
#ifndef LOGLUV_PUBLIC
#define LOGLUV_PUBLIC 1
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(__attribute__)
#define TIFF_ATTRIBUTE(x) __attribute__(x)
#else
#define TIFF_ATTRIBUTE(x) /*nothing*/
#endif

#if defined(c_plusplus) || defined(__cplusplus)
extern "C"
{
#endif
    typedef void (*TIFFErrorHandler)(const char *, const char *, va_list);
    typedef void (*TIFFErrorHandlerExt)(thandle_t, const char *, const char *,
                                        va_list);
    typedef int (*TIFFErrorHandlerExtR)(TIFF *, void *user_data, const char *,
                                        const char *, va_list);
    typedef tmsize_t (*TIFFReadWriteProc)(thandle_t, void *, tmsize_t);
    typedef toff_t (*TIFFSeekProc)(thandle_t, toff_t, int);
    typedef int (*TIFFCloseProc)(thandle_t);
    typedef toff_t (*TIFFSizeProc)(thandle_t);
    typedef int (*TIFFMapFileProc)(thandle_t, void **base, toff_t *size);
    typedef void (*TIFFUnmapFileProc)(thandle_t, void *base, toff_t size);
    typedef void (*TIFFExtendProc)(TIFF *);

    LIBTIFF_DLL_IMPEXP const char *TIFFGetVersion(void);

    LIBTIFF_DLL_IMPEXP const TIFFCodec *TIFFFindCODEC(uint16_t);
    LIBTIFF_DLL_IMPEXP TIFFCodec *TIFFRegisterCODEC(uint16_t, const char *, TIFFInitMethod);
    LIBTIFF_DLL_IMPEXP void TIFFUnRegisterCODEC(TIFFCodec *);
    LIBTIFF_DLL_IMPEXP int TIFFIsCODECConfigured(uint16_t);
    LIBTIFF_DLL_IMPEXP TIFFCodec *TIFFGetConfiguredCODECs(void);

    /*
     * Auxiliary functions.
     */
#ifndef TIFF_DO_NOT_USE_NON_EXT_ALLOC_FUNCTIONS
    LIBTIFF_DLL_IMPEXP void *_TIFFmalloc(tmsize_t s);
    LIBTIFF_DLL_IMPEXP void *_TIFFcalloc(tmsize_t nmemb, tmsize_t siz);
    LIBTIFF_DLL_IMPEXP void *_TIFFrealloc(void *p, tmsize_t s);
    LIBTIFF_DLL_IMPEXP void _TIFFfree(void *p);
#endif
    LIBTIFF_DLL_IMPEXP void _TIFFmemset(void *p, int v, tmsize_t c);
    LIBTIFF_DLL_IMPEXP void _TIFFmemcpy(void *d, const void *s, tmsize_t c);
    LIBTIFF_DLL_IMPEXP int _TIFFmemcmp(const void *p1, const void *p2, tmsize_t c);

    /*
    ** Stuff, related to tag handling and creating custom tags.
    */
    LIBTIFF_DLL_IMPEXP int TIFFGetTagListCount(TIFF *);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFGetTagListEntry(TIFF *, int tag_index);

#define TIFF_ANY TIFF_NOTYPE /* for field descriptor searching */
#define TIFF_VARIABLE -1     /* marker for variable length tags */
#define TIFF_SPP -2          /* marker for SamplesPerPixel tags */
#define TIFF_VARIABLE2 -3    /* marker for uint32_t var-length tags */

#define FIELD_CUSTOM 65

    typedef struct _TIFFField TIFFField;
    typedef struct _TIFFFieldArray TIFFFieldArray;

    LIBTIFF_DLL_IMPEXP const TIFFField *TIFFFindField(TIFF *, uint32_t, TIFFDataType);
    LIBTIFF_DLL_IMPEXP const TIFFField *TIFFFieldWithTag(TIFF *, uint32_t);
    LIBTIFF_DLL_IMPEXP const TIFFField *TIFFFieldWithName(TIFF *, const char *);

    LIBTIFF_DLL_IMPEXP uint32_t TIFFFieldTag(const TIFFField *);
    LIBTIFF_DLL_IMPEXP const char *TIFFFieldName(const TIFFField *);
    LIBTIFF_DLL_IMPEXP TIFFDataType TIFFFieldDataType(const TIFFField *);
    LIBTIFF_DLL_IMPEXP int TIFFFieldPassCount(const TIFFField *);
    LIBTIFF_DLL_IMPEXP int TIFFFieldReadCount(const TIFFField *);
    LIBTIFF_DLL_IMPEXP int TIFFFieldWriteCount(const TIFFField *);
    LIBTIFF_DLL_IMPEXP int
    TIFFFieldSetGetSize(const TIFFField *); /* returns internal storage size of
                                               TIFFSetGetFieldType in bytes. */
    LIBTIFF_DLL_IMPEXP int TIFFFieldSetGetCountSize(
        const TIFFField *); /* returns size of count parameter 0=none,
                               2=uint16_t, 4=uint32_t */
    LIBTIFF_DLL_IMPEXP int TIFFFieldIsAnonymous(const TIFFField *);

    typedef int (*TIFFVSetMethod)(TIFF *, uint32_t, va_list);
    typedef int (*TIFFVGetMethod)(TIFF *, uint32_t, va_list);
    typedef void (*TIFFPrintMethod)(TIFF *, FILE *, long);

    typedef struct
    {
        TIFFVSetMethod vsetfield; /* tag set routine */
        TIFFVGetMethod vgetfield; /* tag get routine */
        TIFFPrintMethod printdir; /* directory print routine */
    } TIFFTagMethods;

    LIBTIFF_DLL_IMPEXP TIFFTagMethods *TIFFAccessTagMethods(TIFF *);
    LIBTIFF_DLL_IMPEXP void *TIFFGetClientInfo(TIFF *, const char *);
    LIBTIFF_DLL_IMPEXP void TIFFSetClientInfo(TIFF *, void *, const char *);

    LIBTIFF_DLL_IMPEXP void TIFFCleanup(TIFF *tif);
    LIBTIFF_DLL_IMPEXP void TIFFClose(TIFF *tif);
    LIBTIFF_DLL_IMPEXP int TIFFFlush(TIFF *tif);
    LIBTIFF_DLL_IMPEXP int TIFFFlushData(TIFF *tif);
    LIBTIFF_DLL_IMPEXP int TIFFGetField(TIFF *tif, uint32_t tag, ...);
    LIBTIFF_DLL_IMPEXP int TIFFVGetField(TIFF *tif, uint32_t tag, va_list ap);
    LIBTIFF_DLL_IMPEXP int TIFFGetFieldDefaulted(TIFF *tif, uint32_t tag, ...);
    LIBTIFF_DLL_IMPEXP int TIFFVGetFieldDefaulted(TIFF *tif, uint32_t tag, va_list ap);
    LIBTIFF_DLL_IMPEXP int TIFFReadDirectory(TIFF *tif);
    LIBTIFF_DLL_IMPEXP int TIFFReadCustomDirectory(TIFF *tif, toff_t diroff,
                                       const TIFFFieldArray *infoarray);
    LIBTIFF_DLL_IMPEXP int TIFFReadEXIFDirectory(TIFF *tif, toff_t diroff);
    LIBTIFF_DLL_IMPEXP int TIFFReadGPSDirectory(TIFF *tif, toff_t diroff);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFScanlineSize64(TIFF *tif);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFScanlineSize(TIFF *tif);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFRasterScanlineSize64(TIFF *tif);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFRasterScanlineSize(TIFF *tif);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFStripSize64(TIFF *tif);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFStripSize(TIFF *tif);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFRawStripSize64(TIFF *tif, uint32_t strip);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFRawStripSize(TIFF *tif, uint32_t strip);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFVStripSize64(TIFF *tif, uint32_t nrows);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFVStripSize(TIFF *tif, uint32_t nrows);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFTileRowSize64(TIFF *tif);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFTileRowSize(TIFF *tif);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFTileSize64(TIFF *tif);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFTileSize(TIFF *tif);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFVTileSize64(TIFF *tif, uint32_t nrows);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFVTileSize(TIFF *tif, uint32_t nrows);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFDefaultStripSize(TIFF *tif, uint32_t request);
    LIBTIFF_DLL_IMPEXP void TIFFDefaultTileSize(TIFF *, uint32_t *, uint32_t *);
    LIBTIFF_DLL_IMPEXP int TIFFFileno(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFSetFileno(TIFF *, int);
    LIBTIFF_DLL_IMPEXP thandle_t TIFFClientdata(TIFF *);
    LIBTIFF_DLL_IMPEXP thandle_t TIFFSetClientdata(TIFF *, thandle_t);
    LIBTIFF_DLL_IMPEXP int TIFFGetMode(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFSetMode(TIFF *, int);
    LIBTIFF_DLL_IMPEXP int TIFFIsTiled(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFIsByteSwapped(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFIsUpSampled(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFIsMSB2LSB(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFIsBigEndian(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFIsBigTIFF(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFReadWriteProc TIFFGetReadProc(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFReadWriteProc TIFFGetWriteProc(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFSeekProc TIFFGetSeekProc(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFCloseProc TIFFGetCloseProc(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFSizeProc TIFFGetSizeProc(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFMapFileProc TIFFGetMapFileProc(TIFF *);
    LIBTIFF_DLL_IMPEXP TIFFUnmapFileProc TIFFGetUnmapFileProc(TIFF *);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFCurrentRow(TIFF *);
    LIBTIFF_DLL_IMPEXP tdir_t TIFFCurrentDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP tdir_t TIFFNumberOfDirectories(TIFF *);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFCurrentDirOffset(TIFF *);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFCurrentStrip(TIFF *);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFCurrentTile(TIFF *tif);
    LIBTIFF_DLL_IMPEXP int TIFFReadBufferSetup(TIFF *tif, void *bp, tmsize_t size);
    LIBTIFF_DLL_IMPEXP int TIFFWriteBufferSetup(TIFF *tif, void *bp, tmsize_t size);
    LIBTIFF_DLL_IMPEXP int TIFFSetupStrips(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFWriteCheck(TIFF *, int, const char *);
    LIBTIFF_DLL_IMPEXP void TIFFFreeDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFCreateDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFCreateCustomDirectory(TIFF *, const TIFFFieldArray *);
    LIBTIFF_DLL_IMPEXP int TIFFCreateEXIFDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFCreateGPSDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFLastDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFSetDirectory(TIFF *, tdir_t);
    LIBTIFF_DLL_IMPEXP int TIFFSetSubDirectory(TIFF *, uint64_t);
    LIBTIFF_DLL_IMPEXP int TIFFUnlinkDirectory(TIFF *, tdir_t);
    LIBTIFF_DLL_IMPEXP int TIFFSetField(TIFF *, uint32_t, ...);
    LIBTIFF_DLL_IMPEXP int TIFFVSetField(TIFF *, uint32_t, va_list);
    LIBTIFF_DLL_IMPEXP int TIFFUnsetField(TIFF *, uint32_t);
    LIBTIFF_DLL_IMPEXP int TIFFWriteDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFWriteCustomDirectory(TIFF *, uint64_t *);
    LIBTIFF_DLL_IMPEXP int TIFFCheckpointDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFRewriteDirectory(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFDeferStrileArrayWriting(TIFF *);
    LIBTIFF_DLL_IMPEXP int TIFFForceStrileArrayWriting(TIFF *);

#if defined(c_plusplus) || defined(__cplusplus)
    LIBTIFF_DLL_IMPEXP void TIFFPrintDirectory(TIFF *, FILE *, long = 0);
    LIBTIFF_DLL_IMPEXP int TIFFReadScanline(TIFF *tif, void *buf, uint32_t row,
                                uint16_t sample = 0);
    LIBTIFF_DLL_IMPEXP int TIFFWriteScanline(TIFF *tif, void *buf, uint32_t row,
                                 uint16_t sample = 0);
    LIBTIFF_DLL_IMPEXP int TIFFReadRGBAImage(TIFF *, uint32_t, uint32_t, uint32_t *,
                                 int = 0);
    LIBTIFF_DLL_IMPEXP int TIFFReadRGBAImageOriented(TIFF *, uint32_t, uint32_t, uint32_t *,
                                         int = ORIENTATION_BOTLEFT, int = 0);
#else
LIBTIFF_DLL_IMPEXP void TIFFPrintDirectory(TIFF *, FILE *, long);
LIBTIFF_DLL_IMPEXP int TIFFReadScanline(TIFF *tif, void *buf, uint32_t row,
                            uint16_t sample);
LIBTIFF_DLL_IMPEXP int TIFFWriteScanline(TIFF *tif, void *buf, uint32_t row,
                             uint16_t sample);
LIBTIFF_DLL_IMPEXP int TIFFReadRGBAImage(TIFF *, uint32_t, uint32_t, uint32_t *, int);
LIBTIFF_DLL_IMPEXP int TIFFReadRGBAImageOriented(TIFF *, uint32_t, uint32_t, uint32_t *,
                                     int, int);
#endif

    LIBTIFF_DLL_IMPEXP int TIFFReadRGBAStrip(TIFF *, uint32_t, uint32_t *);
    LIBTIFF_DLL_IMPEXP int TIFFReadRGBATile(TIFF *, uint32_t, uint32_t, uint32_t *);
    LIBTIFF_DLL_IMPEXP int TIFFReadRGBAStripExt(TIFF *, uint32_t, uint32_t *,
                                    int stop_on_error);
    LIBTIFF_DLL_IMPEXP int TIFFReadRGBATileExt(TIFF *, uint32_t, uint32_t, uint32_t *,
                                   int stop_on_error);
    LIBTIFF_DLL_IMPEXP int TIFFRGBAImageOK(TIFF *, char[1024]);
    LIBTIFF_DLL_IMPEXP int TIFFRGBAImageBegin(TIFFRGBAImage *, TIFF *, int, char[1024]);
    LIBTIFF_DLL_IMPEXP int TIFFRGBAImageGet(TIFFRGBAImage *, uint32_t *, uint32_t,
                                uint32_t);
    LIBTIFF_DLL_IMPEXP void TIFFRGBAImageEnd(TIFFRGBAImage *);

    LIBTIFF_DLL_IMPEXP const char *TIFFFileName(TIFF *);
    LIBTIFF_DLL_IMPEXP const char *TIFFSetFileName(TIFF *, const char *);
    LIBTIFF_DLL_IMPEXP void TIFFError(const char *, const char *, ...)
        TIFF_ATTRIBUTE((__format__(__printf__, 2, 3)));
    LIBTIFF_DLL_IMPEXP void TIFFErrorExt(thandle_t, const char *, const char *, ...)
        TIFF_ATTRIBUTE((__format__(__printf__, 3, 4)));
    LIBTIFF_DLL_IMPEXP void TIFFWarning(const char *, const char *, ...)
        TIFF_ATTRIBUTE((__format__(__printf__, 2, 3)));
    LIBTIFF_DLL_IMPEXP void TIFFWarningExt(thandle_t, const char *, const char *, ...)
        TIFF_ATTRIBUTE((__format__(__printf__, 3, 4)));
    LIBTIFF_DLL_IMPEXP TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler);
    LIBTIFF_DLL_IMPEXP TIFFErrorHandlerExt TIFFSetErrorHandlerExt(TIFFErrorHandlerExt);
    LIBTIFF_DLL_IMPEXP TIFFErrorHandler TIFFSetWarningHandler(TIFFErrorHandler);
    LIBTIFF_DLL_IMPEXP TIFFErrorHandlerExt TIFFSetWarningHandlerExt(TIFFErrorHandlerExt);

    LIBTIFF_DLL_IMPEXP void TIFFWarningExtR(TIFF *, const char *, const char *, ...)
        TIFF_ATTRIBUTE((__format__(__printf__, 3, 4)));
    LIBTIFF_DLL_IMPEXP void TIFFErrorExtR(TIFF *, const char *, const char *, ...)
        TIFF_ATTRIBUTE((__format__(__printf__, 3, 4)));

    typedef struct TIFFOpenOptions TIFFOpenOptions;
    LIBTIFF_DLL_IMPEXP TIFFOpenOptions *TIFFOpenOptionsAlloc(void);
    LIBTIFF_DLL_IMPEXP void TIFFOpenOptionsFree(TIFFOpenOptions *);
    LIBTIFF_DLL_IMPEXP void
    TIFFOpenOptionsSetMaxSingleMemAlloc(TIFFOpenOptions *opts,
                                        tmsize_t max_single_mem_alloc);
    LIBTIFF_DLL_IMPEXP void
    TIFFOpenOptionsSetMaxCumulatedMemAlloc(TIFFOpenOptions *opts,
                                           tmsize_t max_cumulated_mem_alloc);
    LIBTIFF_DLL_IMPEXP void
    TIFFOpenOptionsSetErrorHandlerExtR(TIFFOpenOptions *opts,
                                       TIFFErrorHandlerExtR handler,
                                       void *errorhandler_user_data);
    LIBTIFF_DLL_IMPEXP void
    TIFFOpenOptionsSetWarningHandlerExtR(TIFFOpenOptions *opts,
                                         TIFFErrorHandlerExtR handler,
                                         void *warnhandler_user_data);

    LIBTIFF_DLL_IMPEXP TIFF *TIFFOpen(const char *, const char *);
    LIBTIFF_DLL_IMPEXP TIFF *TIFFOpenExt(const char *, const char *, TIFFOpenOptions *opts);
#ifdef _WIN32
    LIBTIFF_DLL_IMPEXP TIFF *TIFFOpenW(const wchar_t *, const char *);
    LIBTIFF_DLL_IMPEXP TIFF *TIFFOpenWExt(const wchar_t *, const char *,
                              TIFFOpenOptions *opts);
#endif /* _WIN32 */
    LIBTIFF_DLL_IMPEXP TIFF *TIFFFdOpen(int, const char *, const char *);
    LIBTIFF_DLL_IMPEXP TIFF *TIFFFdOpenExt(int, const char *, const char *,
                               TIFFOpenOptions *opts);
    LIBTIFF_DLL_IMPEXP TIFF *TIFFClientOpen(const char *, const char *, thandle_t,
                                TIFFReadWriteProc, TIFFReadWriteProc,
                                TIFFSeekProc, TIFFCloseProc, TIFFSizeProc,
                                TIFFMapFileProc, TIFFUnmapFileProc);
    LIBTIFF_DLL_IMPEXP TIFF *TIFFClientOpenExt(const char *, const char *, thandle_t,
                                   TIFFReadWriteProc, TIFFReadWriteProc,
                                   TIFFSeekProc, TIFFCloseProc, TIFFSizeProc,
                                   TIFFMapFileProc, TIFFUnmapFileProc,
                                   TIFFOpenOptions *opts);
    LIBTIFF_DLL_IMPEXP TIFFExtendProc TIFFSetTagExtender(TIFFExtendProc);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFComputeTile(TIFF *tif, uint32_t x, uint32_t y,
                                    uint32_t z, uint16_t s);
    LIBTIFF_DLL_IMPEXP int TIFFCheckTile(TIFF *tif, uint32_t x, uint32_t y, uint32_t z,
                             uint16_t s);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFNumberOfTiles(TIFF *);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFReadTile(TIFF *tif, void *buf, uint32_t x, uint32_t y,
                                 uint32_t z, uint16_t s);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFWriteTile(TIFF *tif, void *buf, uint32_t x, uint32_t y,
                                  uint32_t z, uint16_t s);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFComputeStrip(TIFF *, uint32_t, uint16_t);
    LIBTIFF_DLL_IMPEXP uint32_t TIFFNumberOfStrips(TIFF *);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFReadEncodedStrip(TIFF *tif, uint32_t strip, void *buf,
                                         tmsize_t size);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFReadRawStrip(TIFF *tif, uint32_t strip, void *buf,
                                     tmsize_t size);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFReadEncodedTile(TIFF *tif, uint32_t tile, void *buf,
                                        tmsize_t size);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFReadRawTile(TIFF *tif, uint32_t tile, void *buf,
                                    tmsize_t size);
    LIBTIFF_DLL_IMPEXP int TIFFReadFromUserBuffer(TIFF *tif, uint32_t strile, void *inbuf,
                                      tmsize_t insize, void *outbuf,
                                      tmsize_t outsize);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFWriteEncodedStrip(TIFF *tif, uint32_t strip, void *data,
                                          tmsize_t cc);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFWriteRawStrip(TIFF *tif, uint32_t strip, void *data,
                                      tmsize_t cc);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFWriteEncodedTile(TIFF *tif, uint32_t tile, void *data,
                                         tmsize_t cc);
    LIBTIFF_DLL_IMPEXP tmsize_t TIFFWriteRawTile(TIFF *tif, uint32_t tile, void *data,
                                     tmsize_t cc);
    LIBTIFF_DLL_IMPEXP int TIFFDataWidth(
        TIFFDataType); /* table of tag datatype widths within TIFF file. */
    LIBTIFF_DLL_IMPEXP void TIFFSetWriteOffset(TIFF *tif, toff_t off);
    LIBTIFF_DLL_IMPEXP void TIFFSwabShort(uint16_t *);
    LIBTIFF_DLL_IMPEXP void TIFFSwabLong(uint32_t *);
    LIBTIFF_DLL_IMPEXP void TIFFSwabLong8(uint64_t *);
    LIBTIFF_DLL_IMPEXP void TIFFSwabFloat(float *);
    LIBTIFF_DLL_IMPEXP void TIFFSwabDouble(double *);
    LIBTIFF_DLL_IMPEXP void TIFFSwabArrayOfShort(uint16_t *wp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP void TIFFSwabArrayOfTriples(uint8_t *tp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP void TIFFSwabArrayOfLong(uint32_t *lp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP void TIFFSwabArrayOfLong8(uint64_t *lp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP void TIFFSwabArrayOfFloat(float *fp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP void TIFFSwabArrayOfDouble(double *dp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP void TIFFReverseBits(uint8_t *cp, tmsize_t n);
    LIBTIFF_DLL_IMPEXP const unsigned char *TIFFGetBitRevTable(int);

    LIBTIFF_DLL_IMPEXP uint64_t TIFFGetStrileOffset(TIFF *tif, uint32_t strile);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFGetStrileByteCount(TIFF *tif, uint32_t strile);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFGetStrileOffsetWithErr(TIFF *tif, uint32_t strile,
                                               int *pbErr);
    LIBTIFF_DLL_IMPEXP uint64_t TIFFGetStrileByteCountWithErr(TIFF *tif, uint32_t strile,
                                                  int *pbErr);

#ifdef LOGLUV_PUBLIC
#define U_NEU 0.210526316
#define V_NEU 0.473684211
#define UVSCALE 410.
    LIBTIFF_DLL_IMPEXP double LogL16toY(int);
    LIBTIFF_DLL_IMPEXP double LogL10toY(int);
    LIBTIFF_DLL_IMPEXP void XYZtoRGB24(float *, uint8_t *);
    LIBTIFF_DLL_IMPEXP int uv_decode(double *, double *, int);
    LIBTIFF_DLL_IMPEXP void LogLuv24toXYZ(uint32_t, float *);
    LIBTIFF_DLL_IMPEXP void LogLuv32toXYZ(uint32_t, float *);
#if defined(c_plusplus) || defined(__cplusplus)
    LIBTIFF_DLL_IMPEXP int LogL16fromY(double, int = SGILOGENCODE_NODITHER);
    LIBTIFF_DLL_IMPEXP int LogL10fromY(double, int = SGILOGENCODE_NODITHER);
    LIBTIFF_DLL_IMPEXP int uv_encode(double, double, int = SGILOGENCODE_NODITHER);
    LIBTIFF_DLL_IMPEXP uint32_t LogLuv24fromXYZ(float *, int = SGILOGENCODE_NODITHER);
    LIBTIFF_DLL_IMPEXP uint32_t LogLuv32fromXYZ(float *, int = SGILOGENCODE_NODITHER);
#else
    LIBTIFF_DLL_IMPEXP int LogL16fromY(double, int);
    LIBTIFF_DLL_IMPEXP int LogL10fromY(double, int);
    LIBTIFF_DLL_IMPEXP int uv_encode(double, double, int);
    LIBTIFF_DLL_IMPEXP uint32_t LogLuv24fromXYZ(float *, int);
    LIBTIFF_DLL_IMPEXP uint32_t LogLuv32fromXYZ(float *, int);
#endif
#endif /* LOGLUV_PUBLIC */

    LIBTIFF_DLL_IMPEXP int TIFFCIELabToRGBInit(TIFFCIELabToRGB *, const TIFFDisplay *,
                                   float *);
    LIBTIFF_DLL_IMPEXP void TIFFCIELabToXYZ(TIFFCIELabToRGB *, uint32_t, int32_t, int32_t,
                                float *, float *, float *);
    LIBTIFF_DLL_IMPEXP void TIFFXYZToRGB(TIFFCIELabToRGB *, float, float, float, uint32_t *,
                             uint32_t *, uint32_t *);

    LIBTIFF_DLL_IMPEXP int TIFFYCbCrToRGBInit(TIFFYCbCrToRGB *, float *, float *);
    LIBTIFF_DLL_IMPEXP void TIFFYCbCrtoRGB(TIFFYCbCrToRGB *, uint32_t, int32_t, int32_t,
                               uint32_t *, uint32_t *, uint32_t *);

    /****************************************************************************
     *               O B S O L E T E D    I N T E R F A C E S
     *
     * Don't use this stuff in your applications, it may be removed in the
     *future libtiff versions.
     ****************************************************************************/
    typedef struct
    {
        ttag_t field_tag;               /* field's tag */
        short field_readcount;          /* read count/TIFF_VARIABLE/TIFF_SPP */
        short field_writecount;         /* write count/TIFF_VARIABLE */
        TIFFDataType field_type;        /* type of associated data */
        unsigned short field_bit;       /* bit in fieldsset bit vector */
        unsigned char field_oktochange; /* if true, can change while writing */
        unsigned char field_passcount;  /* if true, pass dir count on set */
        char *field_name;               /* ASCII name */
    } TIFFFieldInfo;

    LIBTIFF_DLL_IMPEXP int TIFFMergeFieldInfo(TIFF *, const TIFFFieldInfo[], uint32_t);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif /* _TIFFIO_ */
