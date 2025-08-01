//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The window of the Great Libary
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Memory leaks repaired in LoadText by Martin G�hmann.
// - Added variable and requirement retriever methods. (Sep 13th 2005 Martin G�hmann)
// - Replaced old concept database by new one. (31-Mar-2007 Martin G�hmann)
// - Fixed terrain database item mismatch. (21-Apr-2007 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "greatlibrarywindow.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_static.h"

#include "pattern.h"
#include "primitives.h"
#include "greatlibrary.h"




#include "World.h"
#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "AdvanceRecord.h"
#include "TerrainImprovementRecord.h"

#include "TerrainRecord.h"
#include "ConceptRecord.h"
#include "GovernmentRecord.h"
#include "prjfile.h"




#include "c3ui.h"
#include "aui_tabgroup.h"
#include "directvideo.h"
#include "CivPaths.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"

#include "aui_ldl.h"

#include "colorset.h"
#include "textutils.h"
#include "debugwindow.h"
#include "soundmanager.h"

#include "UnitRecord.h"
#include "IconRecord.h"
#include "IconRecord.h"
#include "TerrainRecord.h"

#include "wonderutil.h"

#include "text_hasher.h"

#include "ResourceRecord.h"

#include "OrderRecord.h"

#include "stringutils.h"


const int GreatLibraryWindow::GREAT_LIBRARY_PANEL_BLANK = 999;

extern CivPaths						*g_civPaths;
extern sint32						g_ScreenWidth;
extern sint32						g_ScreenHeight;
extern DebugWindow					*g_debugWindow;
extern ProjectFile                  *g_GreatLibPF;
extern SoundManager					*g_soundManager;
extern  C3UI				*g_c3ui;

namespace
{

MBCHAR const         s_libraryWindowBlock[]  = "GreatLibrary";
GreatLibraryWindow * s_libraryWindow         = NULL;

} // namespace

GreatLibraryWindow::GreatLibraryWindow(AUI_ERRCODE * err)
:
    m_window                (NULL),
	m_mode                  (0),
	m_database              (DATABASE_DEFAULT),
	m_techTitle             (NULL),
	m_techTree              (NULL),
	m_techStillShot         (NULL),
    m_techDescriptionGroup  (NULL),
	m_techHistoricalText    (NULL),
	m_techGameplayText      (NULL),
	m_techRequirementsText  (NULL),
	m_techVariablesText     (NULL)
#ifdef __AUI_USE_DIRECTX__
	                              ,
	m_techMovie             (NULL)
#endif // __AUI_USE_DIRECTX__
{
	m_window = (ctp2_Window *) aui_Ldl::BuildHierarchyFromRoot(s_libraryWindowBlock);
	Assert(m_window);
	if (!m_window)
    {
		*err = AUI_ERRCODE_INVALIDPARAM;
	}
}

GreatLibraryWindow::~GreatLibraryWindow()
{
	aui_Ldl::DeleteHierarchyFromRoot(s_libraryWindowBlock);
}

AUI_ERRCODE GreatLibraryWindow::Idle ( void )
{
#ifdef __AUI_USE_DIRECTX__
	if (m_techMovie && m_techMovie->Open())
    {
		HRESULT hr = m_techMovie->PlayOne();
		Assert(!FAILED(hr));
		if (FAILED(hr)) {
			m_techMovie->CloseStream();
			delete m_techMovie;
			m_techMovie = NULL;
		}
	}
#endif
	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : LoadText
//
// Description: Load an interpreted text in a user interface item.
//
// Parameters : textbox					: user interface item
//              filename				: name of the text to interpret
//              so						: ?
//
// Globals    : m_great_library_info	: great library database
//
// Returns    : sint32 (bool)			: function result
//
// Remark(s)  : The function result is 1 (true?) when the requested name of
//              the text exist in the database, 0 when it does not exist.
//
//              However, the result is also 1 (illogical) when the passed
//              user interface item is NULL. In that case, this function does
//              nothing.
//
//----------------------------------------------------------------------------
sint32 GreatLibraryWindow::LoadText(ctp2_HyperTextBox *textbox, char *filename, SlicObject &so)
{
    if (textbox)
    {
        char const *    text    =
            GreatLibrary::s_great_library_info->Look_Up_Data(filename);

        if (text == NULL)
        {
		    textbox->SetHyperText(" ");
            return 0;
        }

	    MBCHAR interpreted[k_MAX_GL_ENTRY];
	    stringutils_Interpret(text, so, interpreted, k_MAX_GL_ENTRY);
        textbox->SetHyperText(interpreted);
    }

    return 1;
}

sint32 GreatLibraryWindow::LoadHistoricalText ( SlicObject &so )
{

	if (!strcmp(m_history_file, "NULL"))
		return GREAT_LIBRARY_PANEL_BLANK;

    return(LoadText(m_techHistoricalText, m_history_file, so));
}

sint32 GreatLibraryWindow::LoadGameplayText ( SlicObject &so )
{

	if (!strcmp(m_gameplay_file, "NULL"))
		return GREAT_LIBRARY_PANEL_BLANK;

    return(LoadText(m_techGameplayText, m_gameplay_file, so));
}

sint32 GreatLibraryWindow::LoadRequirementsText ( SlicObject &so )
{
    return(LoadText(m_techRequirementsText, m_requirement_file, so));

}

sint32 GreatLibraryWindow::LoadVariablesText ( SlicObject &so )
{
    return(LoadText(m_techVariablesText, m_variable_file, so));

}

sint32 GreatLibraryWindow::LoadTechMovie ( void )
{
#ifdef __AUI_USE_DIRECTX__
	if (!m_techMovie) return 0;
	if (!strcmp(m_movie_file,"null")) return 0;

	MBCHAR fullPath[_MAX_PATH];
	if (g_civPaths->FindFile(C3DIR_VIDEOS, m_movie_file, fullPath, TRUE)) {

		g_soundManager->ReleaseSoundDriver();

		m_techMovie->OpenStream(fullPath);

		g_soundManager->ReacquireSoundDriver();
	} else {
		return 0;
	}

	return 1;
#else
	return 0;
#endif // __AUI_USE_DIRECTX__
}

sint32 GreatLibraryWindow::LoadTechStill( void )
{
	if ( !m_techStillShot ) return 0;
	if ( !strcmp(m_still_file, "null") ) return 0;

	MBCHAR fullPath[_MAX_PATH];
	if ( g_civPaths->FindFile(C3DIR_PICTURES, m_still_file, fullPath, TRUE) )
    {
		m_techStillShot->SetImage( m_still_file );
	}
	else {
		return 0;
	}

	return 1;
}

void GreatLibraryWindow::PlayTechMovie ( void )
{
#ifdef __AUI_USE_DIRECTX__
	if (m_techMovie)
    {
        m_techMovie->PlayAll();
    }
#endif // __AUI_USE_DIRECTX__
}

sint32 GreatLibraryWindow::SetTechMode ( sint32 theMode, DATABASE theDatabase )
{
	m_mode      = theMode;
	m_database  = theDatabase;

	const IconRecord *  iconRec = NULL;

	switch ( theDatabase )
    {
	case DATABASE_UNITS:
		iconRec = g_theUnitDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_ORDERS:
		iconRec = g_theOrderDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_RESOURCE:
		iconRec = g_theResourceDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_BUILDINGS:
		iconRec = g_theBuildingDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_WONDERS:
		iconRec = g_theWonderDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_ADVANCES:
		iconRec = g_theAdvanceDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_TERRAIN:
		iconRec = g_theTerrainDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_CONCEPTS:
		iconRec = g_theConceptDB->Get(theMode)->GetDefaultIcon();
		break;

	case DATABASE_GOVERNMENTS:
		iconRec = g_theGovernmentDB->Get(theMode)->GetIcon();
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		iconRec = g_theTerrainImprovementDB->Get(theMode)->GetIcon();
		break;

	default:
		BOOL InvalidDatabase = FALSE;
		Assert(InvalidDatabase);
		break;
	}

	if (iconRec)
    {
		snprintf(m_still_file, sizeof(m_still_file), "%s", iconRec->GetFirstFrame());
		snprintf(m_movie_file, sizeof(m_movie_file), "%s", iconRec->GetMovie());
		snprintf(m_gameplay_file, sizeof(m_gameplay_file), "%s", iconRec->GetGameplay());
		snprintf(m_history_file, sizeof(m_history_file), "%s", iconRec->GetHistorical());
		snprintf(m_requirement_file, sizeof(m_requirement_file), "%s", iconRec->GetPrereq());
		snprintf(m_variable_file, sizeof(m_variable_file), "%s", iconRec->GetVari());
	}

	return TRUE;

}


char const * GreatLibraryWindow::GetIconRecText
(
	int database,
	int item,
	uint8 historical
)
{
	IconRecord const *  iconRec;

	switch (database)
    {
    default:
		iconRec = NULL;
        {
            bool InvalidDatabase = false;
            Assert(InvalidDatabase);
        }
		break;

	case DATABASE_UNITS:
		iconRec = g_theUnitDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_ORDERS:
		iconRec = g_theOrderDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_RESOURCE:
		iconRec = g_theResourceDB->Get(item)->GetIcon();
		break;

	case DATABASE_BUILDINGS:
		iconRec = g_theBuildingDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_WONDERS:
		iconRec = g_theWonderDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_ADVANCES:
		iconRec = g_theAdvanceDB->Get(item)->GetIcon();
		break;

    case DATABASE_TERRAIN:
		iconRec = g_theTerrainDB->Get(item)->GetIcon();
		break;

    case DATABASE_CONCEPTS:
		iconRec = g_theConceptDB->Get(item)->GetDefaultIcon();
		break;

	case DATABASE_GOVERNMENTS:
		iconRec = g_theGovernmentDB->Get(item)->GetIcon();
		break;

	case DATABASE_TILE_IMPROVEMENTS:
		iconRec = g_theTerrainImprovementDB->Get(item)->GetIcon();
		break;
	}

	if (iconRec)
	{
		char const * filename;

		switch (historical)
        {
        default:
            filename = NULL;
			Assert(false);

		case 0:
		    filename = iconRec->GetGameplay();
		    break;

		case 1:
		    filename = iconRec->GetHistorical();
			break;

        case 2:
			filename = iconRec->GetPrereq();
			break;

        case 3:
			filename = iconRec->GetVari();
			break;
		}

        if (filename)
        {
    		return GreatLibrary::s_great_library_info->Look_Up_Data(filename);
        }
	}

	return NULL;
}

char const * GreatLibraryWindow::GetGameplayText( int database, int item )
{
	return GetIconRecText( database, item, 0 );
}

char const * GreatLibraryWindow::GetHistoricalText( int database, int item )
{
	return GetIconRecText( database, item, 1 );
}

char const * GreatLibraryWindow::GetRequirementsText( int database, int item )
{
	return GetIconRecText( database, item, 2 );
}

char const * GreatLibraryWindow::GetVariablesText( int database, int item )
{
	return GetIconRecText( database, item, 3 );
}
