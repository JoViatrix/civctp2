//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface bitmap font
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _JAPANESE
// - Add provisions for handling SJIS characters.
//
// _MBCS
// _UNICODE
//
// _DEBUG
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//  fixed for japanese by t.s. 2003.12
//
//  fix
//      AUI_ERRCODE aui_BitmapFont::RenderLine
//      aui_BitmapFont::GlyphInfo *aui_BitmapFont::GetGlyphInfo( MBCHAR c )
//      AUI_ERRCODE aui_BitmapFont::DrawString
//      BOOL aui_BitmapFont::TruncateString( MBCHAR *name, sint32 width )
//
//  add
//      aui_BitmapFont::GlyphInfo *aui_BitmapFont::GetGlyphInfo2( MBCHAR *c )
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standardized code (May 21th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_bitmapfont.h"

#include <algorithm>
#include "aui_blitter.h"
#include "aui_surface.h"
#include "aui_pixel.h"
#include "aui_rectangle.h"
#include "aui_ui.h"
#include <locale.h>

#if defined(_JAPANESE)
#include "japanese.h"
#endif

namespace
{
    bool    SUPPORT_MBCS      = false;
    bool    SUPPORT_UNICODE   = false;
}

sint32      aui_BitmapFont::s_bitmapFontRefCount    = 0;
TT_Engine   aui_BitmapFont::s_ttEngine              = { NULL };

aui_BitmapFont::aui_BitmapFont(
	AUI_ERRCODE *retval,
	const MBCHAR *descriptor )
	:
	aui_Base()
{
	*retval = InitCommon( descriptor );
	Assert( AUI_SUCCESS(*retval) );
}


void aui_BitmapFont::AttributesToDescriptor(
	MBCHAR out[ k_AUI_BITMAPFONT_MAXDESCLEN + 1 ],
	const MBCHAR * ttffile,
	sint32 pointSize,
	sint32 bold,
	sint32 italic )
{
	sprintf(
		out,
		"%s|%d|%d|%d",
		ttffile,
		pointSize,
		bold,
		italic );
}


void aui_BitmapFont::DescriptorToAttributes(
	const MBCHAR * in,
	MBCHAR ttffile[ MAX_PATH + 1 ],
	sint32 *pointSize,
	sint32 *bold,
	sint32 *italic )
{
	sscanf(
		in,
		"%[^\t\n|]|%d|%d|%d",
		ttffile,
		pointSize,
		bold,
		italic );
}


AUI_ERRCODE aui_BitmapFont::InitCommon( const MBCHAR *descriptor )
{
	AUI_ERRCODE errcode = SetFilename( descriptor );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	memset( m_ttffile, '\0', sizeof( m_ttffile ) );
	m_pointSize = 0;
	m_bold = 0;
	m_italic = 0;

	DescriptorToAttributes(
		m_descriptor,
		m_ttffile,
		&m_pointSize,
		&m_bold,
		&m_italic );

#if defined(_MBCS)
    Assert(SUPPORT_MBCS);
#endif
#if defined(_UNICODE)
    Assert(SUPPORT_UNICODE);
#endif

	memset( m_glyphs, 0, sizeof( m_glyphs ) );

	m_curOffset = 0;
	m_maxHeight = 0;
	m_baseLine = 0;
	m_lineSkip = 0;
	m_tabSkip = -1;

	m_ttFace.z = NULL;
	m_ttInstance.z = NULL;
	m_ttCharMap.z = NULL;

	memset( &m_ttFaceProperties, 0, sizeof( m_ttFaceProperties ) );
	memset( &m_ttInstanceMetrics, 0, sizeof( m_ttInstanceMetrics ) );

	m_surfaceList = new tech_WLList<aui_Surface *>;
	Assert( m_surfaceList != NULL );
	if ( !m_surfaceList ) return AUI_ERRCODE_MEMALLOCFAILED;

	if ( !s_bitmapFontRefCount++ )
	{
		sint32 error = TT_Init_FreeType(&s_ttEngine);
		Assert( error == 0 );
		if ( error ) return AUI_ERRCODE_HACK;

		static uint8 palette[ 5 ]=
		{
			0,
			86,
			128,
			170,
			255
		};

		error = TT_Set_Raster_Gray_Palette(s_ttEngine, palette );
		Assert( error == 0 );
		if ( error ) return AUI_ERRCODE_HACK;

#ifdef WIN32
		static MBCHAR fontdir[ MAX_PATH + 1 ];
		sint32 last = GetWindowsDirectory( fontdir, MAX_PATH ) - 1;
		if (last > 1)
		{
			if ( fontdir[ last ] == FILE_SEPC )
				fontdir[ last ] = '\0';

			strcat( fontdir, FILE_SEP "fonts" );

			g_ui->GetBitmapFontResource()->AddSearchPath( fontdir );
		}
#endif // WIN32
	}
	return AUI_ERRCODE_OK;
}


aui_BitmapFont::~aui_BitmapFont()
{
	Unload();

	delete m_surfaceList;

	if ( !--s_bitmapFontRefCount )
	{
		if (s_ttEngine.z)
		{
			TT_Done_FreeType(s_ttEngine);
			s_ttEngine.z = NULL;
		}
	}
}


AUI_ERRCODE aui_BitmapFont::SetFilename( const MBCHAR * descriptor )
{
	memset( m_descriptor, '\0', sizeof( m_descriptor ) );

	if ( !descriptor ) return AUI_ERRCODE_INVALIDPARAM;

	strncpy( m_descriptor, descriptor, MAX_PATH );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_BitmapFont::Load( void )
{

	Unload();

	static MBCHAR fullPath[ MAX_PATH + 1 ];
	if ( g_ui->GetBitmapFontResource()->FindFile( fullPath, m_ttffile ) )
		strncpy( m_ttffile, fullPath, MAX_PATH );
	else if (g_civPaths->FindFile(C3DIR_FONTS, m_ttffile, fullPath, TRUE)) // try C3DIR_FONTS if GetBitmapFontResource fails (e.g. is not yet available)
		strncpy( m_ttffile, fullPath, MAX_PATH );

#ifdef __linux__
	sint32 error = TT_Open_Face(s_ttEngine, CI_FixName(m_ttffile), &m_ttFace);
#else
	sint32 error = TT_Open_Face(s_ttEngine, m_ttffile, &m_ttFace);
#endif
	Assert( error == 0 );
	if ( error ) { fprintf(stderr, "Font %s (%s) failled\n", m_ttffile, fullPath); AUI_ERRCODE_HACK;}

	error = TT_Get_Face_Properties( m_ttFace, &m_ttFaceProperties );
	Assert( error == 0 );
	if ( error ) return AUI_ERRCODE_HACK;

	error = TT_New_Instance( m_ttFace, &m_ttInstance );
	Assert( error == 0 );
	if ( error ) return AUI_ERRCODE_HACK;

	uint16 dpi = 96;
	error = TT_Set_Instance_Resolutions( m_ttInstance, dpi, dpi );
	Assert( error == 0 );
	if ( error ) return AUI_ERRCODE_HACK;


	uint16 n = m_ttFaceProperties.num_CharMaps;
	Assert( n != 0 );
	if ( !n ) return AUI_ERRCODE_HACK;

	uint16 i;
	for (i = 0; i < n; i++ )
	{
		uint16 platform, encoding;
		TT_Get_CharMap_ID( m_ttFace, i, &platform, &encoding );
		if ( ( platform == 3 && encoding == 1 )
		||   ( platform == 0 && encoding == 0 ) )
		{
			error = TT_Get_CharMap( m_ttFace, i, &m_ttCharMap );
			Assert( error == 0 );
			if ( error )
				return AUI_ERRCODE_HACK;
			else
				break;
		}
	}

	Assert( i != n );
	if ( i == n ) return AUI_ERRCODE_HACK;

	SetPointSize( m_pointSize );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_BitmapFont::Unload( void )
{
	for ( size_t i = m_surfaceList->L(); i; i-- )
		delete m_surfaceList->RemoveHead();

	memset( m_glyphs, 0, sizeof( m_glyphs ) );

	if ( m_ttFace.z )
	{
		TT_Close_Face( m_ttFace );
		m_ttFace.z = NULL;
		m_ttInstance.z = NULL;
		m_ttCharMap.z = NULL;
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_BitmapFont::SetTTFFile( const MBCHAR * ttffile )
{

	Assert( !IsLoaded() );
	if ( IsLoaded() ) return AUI_ERRCODE_HACK;

	memset( m_ttffile, '\0', sizeof( m_ttffile ) );

	if ( !ttffile ) return AUI_ERRCODE_INVALIDPARAM;

	strncpy( m_ttffile, ttffile, MAX_PATH );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_BitmapFont::SetPointSize( sint32 pointSize )
{

// diabled because SetPointSize needed in linux debug version before m_surfaceList is populated	Assert( !HasCached() );
	if ( HasCached() ) return AUI_ERRCODE_HACK;

	sint32 error = TT_Set_Instance_CharSize( m_ttInstance, pointSize * 64 );
	Assert( error == 0 );
	if ( error ) return AUI_ERRCODE_HACK;

	m_pointSize = pointSize;

	error = TT_Get_Instance_Metrics( m_ttInstance, &m_ttInstanceMetrics );
	Assert( error == 0 );
	if ( error ) return AUI_ERRCODE_HACK;

	sint32 maxAscend = 0;
	sint32 maxDescend = 0;


	for ( uint16 i = 0; i < 256; i++ )
	{
		TT_Glyph ttGlyph;
		error = TT_New_Glyph( m_ttFace, &ttGlyph );
		Assert( error == 0 );
		if ( error ) return AUI_ERRCODE_MEMALLOCFAILED;

		// load glyph of code i to ttGlyph
		error = TT_Load_Glyph(
			m_ttInstance,
			ttGlyph,
			TT_Char_Index( m_ttCharMap, i ),
			TTLOAD_DEFAULT );
		Assert( error == 0 );
		if ( error ) return AUI_ERRCODE_HACK;

		static TT_Glyph_Metrics ttMetrics;
		// extracts the ttGlyph's metrics and copy them to the ttMetrics.
		error = TT_Get_Glyph_Metrics( ttGlyph, &ttMetrics );
		Assert( error == 0 );
		if ( error ) return AUI_ERRCODE_HACK;

		// maxAscend = max ttMetrics.bearingY
		sint32 units = ttMetrics.bearingY;
		if ( units > maxAscend ) maxAscend = units;

		// maxDescend = max ttMetrics.bbox.yMax
		//		- ttMetrics.bbox.yMin - ttMetrics.bearingY
		units = ttMetrics.bbox.yMax - ttMetrics.bbox.yMin - units;
		if ( units > maxDescend ) maxDescend = units;

		TT_Done_Glyph( ttGlyph );
	}

	SetBaseLine( maxAscend / 64 );
	SetMaxHeight( m_baseLine + maxDescend / 64 );
	SetLineSkip( m_maxHeight );

	return AUI_ERRCODE_OK;
}


sint32 aui_BitmapFont::SetBold( sint32 bold )
{

	Assert( !HasCached() );
	if ( HasCached() ) return AUI_ERRCODE_HACK;

	sint32 prevBold = m_bold;
	m_bold = bold;
	return prevBold;
}


sint32 aui_BitmapFont::SetItalic( sint32 italic )
{

	Assert( !HasCached() );
	if ( HasCached() ) return AUI_ERRCODE_HACK;

	sint32 prevItalic = m_italic;
	m_italic = italic;
	return prevItalic;
}


sint32 aui_BitmapFont::SetLineSkip( sint32 lineSkip )
{
	sint32 prevLineSkip = m_lineSkip;
	m_lineSkip = lineSkip;
	return prevLineSkip;
}


sint32 aui_BitmapFont::SetTabSkip( sint32 tabSkip )
{
	sint32 prevTabSkip = m_tabSkip;
	m_tabSkip = tabSkip;
	return prevTabSkip;
}


sint32 aui_BitmapFont::SetMaxHeight( sint32 maxHeight )
{

	Assert( !HasCached() );
	if ( HasCached() ) return AUI_ERRCODE_HACK;

	sint32 prevMaxHeight = m_maxHeight;
	m_maxHeight = maxHeight;
	return prevMaxHeight;
}


sint32 aui_BitmapFont::SetBaseLine( sint32 baseLine )
{

	Assert( !HasCached() );
	if ( HasCached() ) return AUI_ERRCODE_HACK;

	sint32 prevBaseLine = m_baseLine;
	m_baseLine = baseLine;
	return prevBaseLine;
}

aui_BitmapFont::GlyphInfo *aui_BitmapFont::GetGlyphInfo( MBCHAR c )
{
	if ( !IsCached( c ) )
	{
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;

		GlyphInfo *gi = m_glyphs + (sint32)uint8(c);

		if ( c == '\t' )
		{
			GlyphInfo *space = GetGlyphInfo( ' ' );
			Assert( space != NULL );
			if ( !space ) return NULL;

			*gi = *space;
			gi->c = '\t';

			if ( m_tabSkip < 0 ) m_tabSkip = ( gi->advance *= 4 );
			return gi;
		}

		gi->c = c;

		TT_Glyph ttGlyph;
		ttGlyph.z = NULL;
		sint32 error = TT_New_Glyph( m_ttFace, &ttGlyph );
		Assert( error == 0 );
		if ( error ) goto Error;

#if defined(_JAPANESE)
	    // to encode japanese katakana
		// assume uni-code ttf
		// only work on ms-windows
		uint16 c16;
		if ( IS_KATAKANA(c) ) {
			setlocale (LC_ALL,"Japanese");
			wchar_t wc;
			int i = mbtowc( &wc, &c, MB_CUR_MAX );
			c16 = (uint16) wc;
			if ( i < 1 ) {
				c16 = (uint16)uint8(c);
			}
		} else {
			c16 = (uint16)uint8(c);
		}
#endif

		error = TT_Load_Glyph(
			m_ttInstance,
			ttGlyph,
			TT_Char_Index( m_ttCharMap, (uint16)uint8(c) ),
			TTLOAD_DEFAULT );
		Assert( error == 0 );
		if ( error ) goto Error;

		TT_Glyph_Metrics ttMetrics;
		error = TT_Get_Glyph_Metrics( ttGlyph, &ttMetrics );
		Assert( error == 0 );
		if ( error ) goto Error;

		SetRect(
			&gi->bbox,
			(sint32)floor( (double)ttMetrics.bbox.xMin / 64.0 + 0.5 ),
			ttMetrics.bbox.yMin / 64,
			(sint32)floor( (double)ttMetrics.bbox.xMax / 64.0 + 0.5 ),
			ttMetrics.bbox.yMax / 64 );

		gi->bearingX = (sint16)floor( (double)ttMetrics.bearingX / 64.0 + 0.5 );
		gi->bearingY = static_cast<sint16>(-ttMetrics.bearingY / 64);
		gi->advance = (sint16)floor( (double)ttMetrics.advance / 64.0 + 0.5 );

		uint32 nextOffset;
		nextOffset = m_curOffset + gi->bbox.right - (gi->bbox.left < 0 ? gi->bbox.left : 0);

		if ( !m_surfaceList->L()
		||   nextOffset >= k_AUI_BITMAPFONT_SURFACEWIDTH )
		{
			aui_Surface *cache;

			cache = new aui_Surface(
				&errcode,
				k_AUI_BITMAPFONT_SURFACEWIDTH,
				m_maxHeight,
				8 );
			Assert( AUI_NEWOK(cache,errcode) );
			if ( !AUI_NEWOK(cache,errcode) ) goto Error;

			m_surfaceList->AddTail( cache );

			nextOffset -= m_curOffset;
			m_curOffset = 0;
		}

		gi->surface = m_surfaceList->GetTail();

		OffsetRect(
			&gi->bbox,
			m_curOffset - gi->bbox.left,
			m_baseLine - gi->bbox.top - gi->bbox.bottom );

		TT_Raster_Map bitmap;
		memset( &bitmap, 0, sizeof( bitmap ) );
		bitmap.rows = gi->surface->Height();
		bitmap.cols = gi->surface->Pitch();
		bitmap.width = gi->surface->Width() - gi->bbox.left;
		bitmap.flow = TT_Flow_Down;

		bitmap.size = bitmap.rows * bitmap.cols;

		RECT rect;
		SetRect(
			&rect,
			gi->bbox.left,
			0,
			gi->surface->Width(),
			bitmap.rows );

		//	rect of gi->surface is "rocked".
		//	bitmap.bitmap = ( top-left of rect in gi->surface.m_saveBuffer )
		errcode = gi->surface->Lock( &rect, &bitmap.bitmap, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( AUI_SUCCESS(errcode) )
		{
			TT_Outline ttOutline;
			error = TT_Get_Glyph_Outline( ttGlyph, &ttOutline );
			Assert( error == 0 );
			if ( !error )
			{

				TT_Translate_Outline(
					&ttOutline,
					-ttMetrics.bbox.xMin,
					( bitmap.rows - m_baseLine ) * 64 );

				// copy pixcel map of font to gi->surface.m_saveBuffer
				error = TT_Get_Outline_Pixmap(
					s_ttEngine,
					&ttOutline,
					&bitmap );
				Assert( error == 0 );
			}

			//	rect of gi->surface is "unrocked".
			errcode = gi->surface->Unlock( bitmap.bitmap );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) goto Error;
		}

		TT_Done_Glyph( ttGlyph );

		m_curOffset = nextOffset;

		return gi;

	Error:
		if ( ttGlyph.z ) TT_Done_Glyph( ttGlyph );
		return NULL;
	}
	else
		return m_glyphs + (sint32)uint8(c);
}

#if defined(_JAPANESE)
// alternative method for multi-byte c
aui_BitmapFont::GlyphInfo *aui_BitmapFont::GetGlyphInfo( const MBCHAR *pc )
{
	// assume uni-code ttf
	// only work on ms-windows
    setlocale (LC_ALL,"Japanese");
	wchar_t wc;
    int i = mbtowc( &wc, pc, MB_CUR_MAX );
	uint16 c = (uint16) wc;

	if ( !IsCached( c ) )
	{
		AUI_ERRCODE errcode;

		GlyphInfo *gi = m_glyphs + (sint32) c;

		gi->c = *pc;
		gi->c2 = c;

		TT_Glyph ttGlyph;
		ttGlyph.z = NULL;
		sint32 error = TT_New_Glyph( m_ttFace, &ttGlyph );
		Assert( error == 0 );
		if ( error ) goto Error;

		// gliph(c) >> ttGlyph
		error = TT_Load_Glyph(
			m_ttInstance,
			ttGlyph,

			TT_Char_Index( m_ttCharMap, c ),
			TTLOAD_DEFAULT );
		Assert( error == 0 );
		if ( error ) goto Error;

		// metrix of gliph(c) >> ttMetrics
		TT_Glyph_Metrics ttMetrics;
		error = TT_Get_Glyph_Metrics( ttGlyph, &ttMetrics );
		Assert( error == 0 );
		if ( error ) goto Error;

		SetRect(
			&gi->bbox,
			(sint32)floor( (double)ttMetrics.bbox.xMin / 64.0 + 0.5 ),
			ttMetrics.bbox.yMin / 64,
			(sint32)floor( (double)ttMetrics.bbox.xMax / 64.0 + 0.5 ),
			ttMetrics.bbox.yMax / 64 );

		gi->bearingX = (sint16)floor( (double)ttMetrics.bearingX / 64.0 + 0.5 );
		gi->bearingY = -ttMetrics.bearingY / 64;
		gi->advance = (sint16)floor( (double)ttMetrics.advance / 64.0 + 0.5 );

		uint32 nextOffset;
		nextOffset = m_curOffset + gi->bbox.right - gi->bbox.left;

		if ( !m_surfaceList->L()
		||   nextOffset > k_AUI_BITMAPFONT_SURFACEWIDTH )
		{
			aui_Surface *cache;

			cache = new aui_Surface(
				&errcode,
				k_AUI_BITMAPFONT_SURFACEWIDTH,
				m_maxHeight,
				8 );
			Assert( AUI_NEWOK(cache,errcode) );
			if ( !AUI_NEWOK(cache,errcode) ) goto Error;

			m_surfaceList->AddTail( cache );

			nextOffset -= m_curOffset;
			m_curOffset = 0;
		}

		gi->surface = m_surfaceList->GetTail();

		OffsetRect(
			&gi->bbox,
			m_curOffset - gi->bbox.left,
			m_baseLine - gi->bbox.top - gi->bbox.bottom );

		TT_Raster_Map bitmap;
		memset( &bitmap, 0, sizeof( bitmap ) );
		bitmap.rows = gi->surface->Height();
		bitmap.cols = gi->surface->Pitch();
		bitmap.width = gi->surface->Width() - gi->bbox.left;
		bitmap.flow = TT_Flow_Down;

		bitmap.size = bitmap.rows * bitmap.cols;

		RECT rect;
		SetRect(
			&rect,
			gi->bbox.left,
			0,
			gi->surface->Width(),
			bitmap.rows );

		//	rect of gi->surface is "rocked".
		//	bitmap.bitmap = ( top-left of rect in gi->surface.m_saveBuffer )
		errcode = gi->surface->Lock( &rect, &bitmap.bitmap, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( AUI_SUCCESS(errcode) )
		{
			TT_Outline ttOutline;
			error = TT_Get_Glyph_Outline( ttGlyph, &ttOutline );
			Assert( error == 0 );
			if ( !error )
			{

				TT_Translate_Outline(
					&ttOutline,
					-ttMetrics.bbox.xMin,
					( bitmap.rows - m_baseLine ) * 64 );

				// copy pixcel map of font to gi->surface.m_saveBuffer
				error = TT_Get_Outline_Pixmap(
					m_ttEngine,
					&ttOutline,
					&bitmap );
				Assert( error == 0 );
			}

			//	rect of gi->surface is "unrocked".
			errcode = gi->surface->Unlock( bitmap.bitmap );
			Assert( AUI_SUCCESS(errcode) );
			   if ( !AUI_SUCCESS(errcode) ) goto Error;
		}

		TT_Done_Glyph( ttGlyph );

		m_curOffset = nextOffset;

		return gi;

	Error:
		if ( ttGlyph.z ) TT_Done_Glyph( ttGlyph );
		return NULL;
	}
	else
		return m_glyphs + (sint32) c;
}
#endif	// _JAPANESE


AUI_ERRCODE aui_BitmapFont::DrawString(
	aui_Surface *surface,
	RECT *bound,
	RECT *clip,
	const MBCHAR *string,
	uint32 flags,
	COLORREF color,
	sint32 underline,
	size_t selStart,
	size_t selEnd,
	COLORREF highLightColorFG,
	COLORREF highLightColorBG )
{
	AUI_ERRCODE errcode;

	if ( !string ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = g_ui->Secondary();

	RECT localRect = { 0, 0, surface->Width(), surface->Height() };
	if ( !bound )
		bound = &localRect;
	else
	{
		if ( !Rectangle_Clip( bound, &localRect ) )
			return AUI_ERRCODE_OK;
	}

	if ( !clip )
		clip = &localRect;
	else
	{
		if ( !Rectangle_Clip( clip, &localRect ) )
			return AUI_ERRCODE_OK;
	}

	if ( !Rectangle_Intersect( bound, clip ) ) return AUI_ERRCODE_OK;

	const MBCHAR *  ptr     = string;
	const MBCHAR *  start   = ptr;
	size_t          len     =
		std::min<size_t>(strlen(string), k_AUI_BITMAPFONT_MAXSTRLEN);
	const MBCHAR *  stop    = ptr + len;

	const MBCHAR *  selStartPtr = start + (selStart <= selEnd ? selStart : selEnd);
	const MBCHAR *    selEndPtr = start + (selStart <= selEnd ? selEnd   : selStart);

	POINT penPos = { bound->left, bound->top + m_baseLine };

	sint32 numLines = 1;

	if ( flags & k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP )
	{

		// copy string to wrapped
		static MBCHAR wrapped[ k_AUI_BITMAPFONT_MAXSTRLEN + 1 ];
		strncpy( wrapped, string, len );

		MBCHAR *staticPtr = wrapped;
		MBCHAR *staticStop = staticPtr + len;

		// cut and wrap into lines and count them.
		while ( staticPtr != staticStop )
		{
			penPos.x = bound->left;

			errcode = GetLineInfo(
				bound,
				&penPos,
				NULL,
				NULL,
				(const MBCHAR **)&staticPtr,
				(const MBCHAR *)staticStop );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;

			if ( staticPtr != staticStop )
			{
#if !defined(_JAPANESE)
				*(staticPtr - 1) = '\n';
#endif
				numLines++;
			}

			penPos.y += m_lineSkip;
		}

		stop = ( ptr = start = wrapped ) + len;
	}

	if ( flags & k_AUI_BITMAPFONT_DRAWFLAG_VERTCENTER )
	{

		if ( !(flags & k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP) )
		{
			while ( ptr != stop )
				if ( *ptr++ == '\n' )
					numLines++;

			ptr = start;
		}

		penPos.y = m_baseLine +
			( bound->bottom + bound->top - numLines * m_lineSkip ) / 2;
	}
	else
		penPos.y = bound->top + m_baseLine;

	uint32 horizFlags = flags & (k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT | k_AUI_BITMAPFONT_DRAWFLAG_JUSTRIGHT | k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER);


	if ( !horizFlags || (horizFlags & k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT) )
	{
		while ( ptr != stop )
		{
			penPos.x = bound->left;

			errcode = RenderLine
			(
			    surface,
			    bound,
			    clip,
			    &penPos,
			    &ptr,
			    stop,
			    color,
			    underline,
			    selStartPtr,
			    selEndPtr,
			    highLightColorFG,
			    highLightColorBG
#if defined(_JAPANESE)
			  , NULL,
			    NULL,
			    (flags & k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP)
#endif
			);

			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;

			penPos.y += m_lineSkip;
			if(penPos.y > bound->bottom)
				break;
		}
	}
	else if ( horizFlags & k_AUI_BITMAPFONT_DRAWFLAG_JUSTRIGHT )
	{
		while ( ptr != stop )
		{
			penPos.x = 0;

			start = ptr;

			errcode = GetLineInfo(
				NULL,
				&penPos,
				NULL,
				NULL,
				&ptr,
				stop );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;

			penPos.x = bound->right - penPos.x;
			ptr = start;

			errcode = RenderLine
			(
			    surface,
			    bound,
			    clip,
			    &penPos,
			    &ptr,
			    stop,
			    color,
			    underline,
			    selStartPtr,
			    selEndPtr,
			    highLightColorFG,
			    highLightColorBG
#if defined(_JAPANESE)
			  , NULL,
			    NULL,
			   (flags & k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP)
#endif
			);

			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;

			penPos.y += m_lineSkip;
			if(penPos.y > bound->bottom)
				break;
		}
	}
	else if ( horizFlags & k_AUI_BITMAPFONT_DRAWFLAG_JUSTCENTER )
	{
		while ( ptr != stop )
		{
			penPos.x = 0;

			const MBCHAR *start = ptr;

			errcode = GetLineInfo(
				NULL,
				&penPos,
				NULL,
				NULL,
				&ptr,
				stop );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;

			penPos.x = ( bound->right + bound->left - penPos.x ) / 2;
			ptr = start;

			errcode = RenderLine(
				surface,
				bound,
				clip,
				&penPos,
				&ptr,
				stop,
				color,
				underline,
				selStartPtr,
				selEndPtr,
				highLightColorFG,
				highLightColorBG
#if defined(_JAPANESE)
				NULL,
				NULL,
				(flags & k_AUI_BITMAPFONT_DRAWFLAG_WORDWRAP)
#endif
			);

			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;

			penPos.y += m_lineSkip;
			if(penPos.y > bound->bottom)
				break;
		}
	}

	return AUI_ERRCODE_OK;
}


// Draw each single line on the screen from (**start) to (*stop)
AUI_ERRCODE aui_BitmapFont::RenderLine(
	aui_Surface *surface,
	RECT *bound,
	RECT *clip,
	POINT *penPos,
	const MBCHAR **start,
	const MBCHAR *stop,
	COLORREF color,
	sint32 underline,
	const MBCHAR *selStartPtr,
	const MBCHAR *selEndPtr,
	COLORREF highLightColorFG,
	COLORREF highLightColorBG,
	sint32 *ascend,
	sint32 *descend,
	bool wrap,
	bool midWordBreaks,
	bool midWordBreaksOnly )
{
	if ( wrap && !midWordBreaksOnly )
	{
		Assert( bound != NULL );
		if ( !bound ) return AUI_ERRCODE_INVALIDPARAM;
	}

	sint32 tabBase = penPos->x;
	const MBCHAR *lastBreakPtr = NULL;
	sint32 lastBreakPos = 0;

	if ( ascend ) *ascend = 0;
	if ( descend ) *descend = 0;

	const MBCHAR *ptr = *start;
	while ( ptr != stop )
	{
		GlyphInfo *gi = NULL;

		sint32 prevPenPos = penPos->x;

		switch ( *ptr )
		{
		case '\n':
			*start = ptr + 1;
			return AUI_ERRCODE_OK;

		case '\b':
			gi = GetGlyphInfo(' ');
			if (gi)
			{
				penPos->x -= gi->advance;
			}
			break;

		case '\t':

			if ( m_tabSkip > 0 )
			{
				gi = GetGlyphInfo( '\t' );
				Assert( gi != NULL );
				if ( !gi ) return AUI_ERRCODE_HACK;

				sint32 tab = tabBase;
				if ( m_tabSkip > 0 )
				{
					while ( (tab += m_tabSkip) <= penPos->x )
						;

					tabBase = tab;
					gi->advance = sint16(tab - penPos->x);
				}
				else
					gi->advance = 0;
			}

		case ' ':
			if ( wrap && !midWordBreaksOnly )
			{
				lastBreakPtr = ptr;
				lastBreakPos = penPos->x;
			}

		default:
			if ( !gi )
			{
#if !defined(_JAPANESE)
				gi = GetGlyphInfo( *ptr );
#else
				 // japanese sjis-code
				if ( ptr < stop
					&& IS_SJIS_1ST( *ptr )
					&& IS_SJIS_2ND( *(ptr+1) ) ) {
					gi = GetGlyphInfo( ptr );
					ptr++;
					lastBreakPtr = ptr;
					lastBreakPos = penPos->x;
				} else {
					gi = GetGlyphInfo( *ptr );
				}
#endif

				Assert( gi != NULL );
				if ( !gi ) return AUI_ERRCODE_HACK;
			}

			if ( surface )
			{
				bool isSelected = selStartPtr != selEndPtr && selStartPtr <= ptr && ptr < selEndPtr;
				COLORREF actualColor = (isSelected ? highLightColorFG : color);

				if(g_ui->TheBlitter() && isSelected)
				{
					RECT bgRect = *bound;
					bgRect.left = penPos->x;
					bgRect.right = bgRect.left + gi->advance;

					AUI_ERRCODE errcode = g_ui->TheBlitter()->ColorBlt(
						surface,
						&bgRect,
						highLightColorBG,
						0);
				}

				AUI_ERRCODE errcode = RenderGlyph(
					surface,
					clip,
					penPos,
					gi,
					actualColor,
					underline);
				Assert( AUI_SUCCESS(errcode) );
				if ( !AUI_SUCCESS(errcode) ) return errcode;
			}

			if ( ascend )
			{
				*ascend = std::min<sint32>(*ascend, gi->bbox.top - m_baseLine);
			}

			if ( descend )
			{
				*descend = std::max<sint32>(*descend, gi->bbox.bottom - m_baseLine);
			}

			penPos->x += gi->advance;
			break;
		}

#if !defined(_JAPANESE)
		if ( wrap && penPos->x > bound->right )
#else
		if ( wrap && penPos->x > bound->right - 12)
#endif
		{
			if ( lastBreakPtr )
			{
				*start = lastBreakPtr + 1;
				penPos->x = lastBreakPos;
				return AUI_ERRCODE_OK;
			}
			else if ( midWordBreaks || midWordBreaksOnly )
			{
				*start = ptr;
				penPos->x = prevPenPos;
				return AUI_ERRCODE_OK;
			}
		}

		ptr++;
	}

	*start = stop;

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_BitmapFont::GetLineInfo(
	RECT *wrap,
	POINT *penPos,
	sint32 *ascend,
	sint32 *descend,
	const MBCHAR **start,
	const MBCHAR *stop,
	bool midWordBreaks,
	bool midWordBreaksOnly )
{
	return RenderLine(
		NULL,
		wrap,
		wrap,
		penPos,
		start,
		stop,
		0,
		0,
		NULL,
		NULL,
		0,
		0,
		ascend,
		descend,
		wrap != NULL,
		midWordBreaks,
		midWordBreaksOnly );
}




sint32 aui_BitmapFont::GetStringWidth( const MBCHAR * a_String )
{
	Assert(a_String);
	if (!a_String ) return 0;

	POINT penPos = { 0, 0 };

	(void) RenderLine(NULL, NULL, NULL,
	                  &penPos,
	                  &a_String,
	                  a_String + strlen(a_String)
	                 );

	return penPos.x;
}




AUI_ERRCODE aui_BitmapFont::RenderGlyph(
	aui_Surface *destSurf,
	RECT *clipRect,
	POINT *penPos,
	GlyphInfo *gi,
	COLORREF color,
	sint32 underline)
{
	RECT srcRect = gi->bbox;
	RECT destRect = srcRect;
	OffsetRect(
		&destRect,
		penPos->x + gi->bearingX - destRect.left,
		penPos->y + gi->bearingY - destRect.top );

	if ( underline )
	{
		static RECT underlineRect;
		underlineRect.left = destRect.left - gi->bearingX;
		underlineRect.right = underlineRect.left + gi->advance;
		underlineRect.top = destRect.top - gi->bearingY + underline;
		underlineRect.bottom = underlineRect.top + underline;


		if ( Rectangle_Clip( &underlineRect, clipRect ) )
		{
			AUI_ERRCODE errcode = g_ui->TheBlitter()->ColorBlt(
				destSurf,
				&underlineRect,
				color,
				0 );
			Assert( AUI_SUCCESS(errcode) );
			if ( !AUI_SUCCESS(errcode) ) return errcode;
		}
	}

	sint32  leftClip    = std::min<sint32>(0, destRect.left - clipRect->left);
	sint32  topClip     = std::min<sint32>(0, destRect.top - clipRect->top);
	sint32  rightClip   = std::max<sint32>(0, destRect.right - clipRect->right);
	sint32  bottomClip  = std::max<sint32>(0, destRect.bottom - clipRect->bottom);

	destRect.left -= leftClip;
	destRect.top -= topClip;
	destRect.right -= rightClip;
	destRect.bottom -= bottomClip;

	if ( destRect.left >= destRect.right
	||   destRect.top >= destRect.bottom )
		return AUI_ERRCODE_OK;

	srcRect.left -= leftClip;
	srcRect.top -= topClip;
	srcRect.right -= rightClip;
	srcRect.bottom -= bottomClip;

	Assert(16 == destSurf->BitsPerPixel());
	if (16 != destSurf->BitsPerPixel()) return AUI_ERRCODE_INVALIDPARAM;

	return RenderGlyph16(destSurf, &destRect, gi->surface, &srcRect, color);
}




AUI_ERRCODE aui_BitmapFont::RenderGlyph16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	COLORREF color )
{
	AUI_ERRCODE		retval    = AUI_ERRCODE_OK;
	bool			wasLocked = (NULL != destSurf->Buffer());

	uint16			*destBuf;
	AUI_ERRCODE		errcode;

    if (wasLocked)
    {
		destBuf = (uint16 *)(destSurf->Buffer() +
								destRect->left * 2 +
								destRect->top * destSurf->Pitch());
    }
    else
    {
		errcode = destSurf->Lock( destRect, (LPVOID *)&destBuf, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) ) {
			retval = AUI_ERRCODE_SURFACELOCKFAILED;
			return retval;
		}
    }

	uint16 *origDestBuf = destBuf;

	uint8 *srcBuf;
	errcode = srcSurf->Lock( srcRect, (LPVOID *)&srcBuf, 0 );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) )
		retval = AUI_ERRCODE_SURFACELOCKFAILED;
	else
	{
		uint8 *origSrcBuf = srcBuf;

		const sint32 srcPitch = srcSurf->Pitch();
		const sint32 scanWidth = srcRect->right - srcRect->left;
		const sint32 srcDiff = srcPitch - scanWidth;
		const sint32 destDiff = ( destSurf->Pitch() / 2 ) - scanWidth;

		uint8 *stopHorizontal = srcBuf + scanWidth;
		const uint8 *stopVertical = srcBuf +
			srcPitch * ( srcRect->bottom - srcRect->top );

		const uint16 pixelColor = aui_Pixel::Get16BitRGB(
			GetRValue(color),
			GetGValue(color),
			GetBValue(color) );

		if ( destSurf->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555 )
		{
			do
			{
				do
				{
					if ( *srcBuf )
					{
						if ( *srcBuf == 255 )
							*destBuf++ = pixelColor;
						else
							*destBuf++ = aui_Pixel::Blend555(
								pixelColor,
								*destBuf,
								*srcBuf >> 3 );
					}
					else
						destBuf++;
				} while ( ++srcBuf != stopHorizontal );

				stopHorizontal += srcPitch;

				destBuf += destDiff;
			} while ( (srcBuf += srcDiff) != stopVertical );
		}
		else
		{
			do
			{
				do
				{
					if ( *srcBuf )
					{
						if ( *srcBuf == 255 )
							*destBuf++ = pixelColor;
						else
							*destBuf++ = aui_Pixel::Blend565(
								pixelColor,
								*destBuf,
								*srcBuf >> 3 );
					}
					else
						destBuf++;
				} while ( ++srcBuf != stopHorizontal );

				stopHorizontal += srcPitch;

				destBuf += destDiff;
			} while ( (srcBuf += srcDiff) != stopVertical );
		}

		errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) )
			retval = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	if (!wasLocked) {
		destSurf->Unlock( (LPVOID)origDestBuf );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) )
			retval = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return retval;
}

bool aui_BitmapFont::TruncateString( MBCHAR *name, sint32 width )
{
	const MBCHAR *string = name;
	POINT penPos = { 0, 0 };
	RECT wrap = { 0, 0, width, 0 };
	const MBCHAR *stop = string + strlen( string );
	GetLineInfo(
		&wrap,
		&penPos,
		NULL,
		NULL,
		&string,
		stop,
		true,
		true );

	if ( string != stop )
	{
		size_t end = (string - name - 3) > 0 ? string - name - 3 : 0;
		Assert( end > 0 );
		if ( end > 0 )
		{
#if defined(_JAPANESE)
			if( IS_SJIS_2ND( *(name+end) ) ){
				strcpy( name + end + 1, ".." );
			} else {
				strcpy( name + end, "..." );
			}
# else
			strcpy( name + end, "..." );
# endif
		}

		return true;
	}

	return false;
}






#ifdef _DEBUG

void aui_BitmapFont::DumpCachedSurfaces( aui_Surface *destSurf )
{

	if ( !destSurf ) destSurf = g_ui->Secondary();

	Assert( destSurf->BitsPerPixel() == 16 );
	if ( destSurf->BitsPerPixel() != 16 ) return;

	uint16 *destBuf;
	AUI_ERRCODE errcode = destSurf->Lock( NULL, (LPVOID *)&destBuf, 0 );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return;

	uint16 *origDestBuf = destBuf;

	ListPos position = m_surfaceList->GetHeadPosition();
	for ( size_t i = m_surfaceList->L(); i; i-- )
	{
		aui_Surface *srcSurf = m_surfaceList->GetNext( position );

		uint8 *srcBuf;
		errcode = srcSurf->Lock( NULL, (LPVOID *)&srcBuf, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) ) break;

		uint8 *origSrcBuf = srcBuf;

		sint32 srcDiff = srcSurf->Pitch() - srcSurf->Width();
		sint32 destDiff = ( destSurf->Pitch() / 2 ) -
			k_AUI_BITMAPFONT_SURFACEWIDTH;

		for ( sint32 y = m_maxHeight; y; y-- )
		{
			for ( sint32 x = k_AUI_BITMAPFONT_SURFACEWIDTH; x; x-- )
			{
				*destBuf++ =
					aui_Pixel::Get16BitRGB( *srcBuf, *srcBuf, *srcBuf );
				srcBuf++;
			}

			srcBuf += srcDiff;
			destBuf += destDiff;
		}

		errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) ) break;
	}

	destSurf->Unlock( (LPVOID)origDestBuf );
}
#endif
