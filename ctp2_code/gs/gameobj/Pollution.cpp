//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Pollution handling
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
// - Do not trigger disaster warnings when there is no pollution at all.
// - Memory leak repaired.
// - Improved pollution warning recipient handling.
// - Replaced old const database by new one. (5-Aug-2007 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "pollution.h"

#include "Globals.h"

#include "World.h"
#include "player.h"
#include "WonderRecord.h"
#include "civarchive.h"
#include "network.h"
#include "SlicSegment.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "PollutionRecord.h"
#include "profileDB.h"
#include "GameSettings.h"
#include "Cell.h"
#include "ConstRecord.h"
#include "RandGen.h"

#include "installationtree.h"
#include "tiledmap.h"
#include "SelItem.h"

#include "wonderutil.h"
#include "newturncount.h"

#include "GameEventManager.h"

sint32 const    Pollution::ROUNDS_COUNT_IMMEASURABLE    = 9999;

Pollution::Pollution()
{
	sint32	i;

	m_phase = 0;
	m_gwPhase = 0;

	for (i=0; i<k_MAX_GLOBAL_POLLUTION_RECORD_TURNS; i++)
		m_history[i] = 0;

	m_eventTriggered = FALSE;

	m_eventTriggerNextRound = 0;

	m_trend = 0;
	m_next_level = 1;
}

Pollution::Pollution(CivArchive &archive)
{
	Serialize(archive);
}

Pollution::~Pollution()
{
}

void Pollution::Serialize(CivArchive &archive)
{

	CHECKSERIALIZE

#define POLLUTION_MAGIC 0x32675109
	if(archive.IsStoring())
	{
		archive.PerformMagic(POLLUTION_MAGIC) ;
		archive.StoreChunk((uint8 *)&m_eventTriggerNextRound, ((uint8 *)&m_next_level)+sizeof(m_next_level));
	}
	else
	{
		archive.TestMagic(POLLUTION_MAGIC) ;
		archive.LoadChunk((uint8 *)&m_eventTriggerNextRound, ((uint8 *)&m_next_level)+sizeof(m_next_level));
	}
}

//----------------------------------------------------------------------------
//
// Name       : Pollution::WarnPlayers
//
// Description: Warn all players about an imminent pollution disaster.
//
// Parameters : -
//
// Globals    : g_slicEngine    : message display handler
//              g_player        : list of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void Pollution::WarnPlayers()
{
	SlicObject *	so	= new SlicObject("911ImminentFlood");
	SlicSegment *	seg	= g_slicEngine->GetSegment("911ImminentFlood");

	// Start at 1: skip the barbarians.
	for(PLAYER_INDEX i = 1; i < k_MAX_PLAYERS; ++i)
	{
		if(g_player[i]				&&
			!g_player[i]->IsDead()	&&
			!seg->TestLastShown(i, k_ROUNDS_BEFORE_DISASTER)
		  )
		{
			so->AddRecipient(i);
		}
	}

	if (so->GetNumRecipients())
	{
		g_slicEngine->Execute(so);
	}
	else
	{
		delete so;
	}
}

sint32 Pollution::AtTriggerLevel(void)
{
	if(!g_theGameSettings->IsPollution())
		return FALSE;

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(!g_player[i])
			continue;

		if(wonderutil_GetReduceWorldPollution(g_player[i]->GetBuiltWonders()))
		{
			return FALSE;
		}
	}

	const PollutionRecord::Phase* pprec = g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetPhase(m_phase);
	sint32 trigger = pprec->GetPollutionTrigger();

	sint32 pollution = GetGlobalPollutionLevel();

	if(pollution > trigger)
	{
		if(pprec->GetOzoneDisaster())
		{
			// Missing stuff
		}

		if(pprec->GetFloodDisaster())
		{
			if (m_phase < (g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetNumPhase()) / 2)
			{
				// Missing stuff
			}
			else
			{
				// Missing stuff
			}
		}

		if(pprec->GetWarning())
		{
			// Missing stuff
		}

		return TRUE;
	}

	if (m_history[0] <= m_history[1])
	{
		if(pollution > g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetPhase(0)->GetPollutionTrigger()) {
			// Missing stuff
		}
	}
	else if(pollution > (sint32)((double)trigger * 0.80))
	{
		if(m_phase == 0)
		{
			// Missing stuff
		}

		if(pprec->GetOzoneDisaster())
		{
			// Missing stuff
		}

		if(pprec->GetFloodDisaster())
		{
			// Missing stuff
		}
	}
	return FALSE;
}

sint32 Pollution::GetNextTrigger()
{
	return g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetPhase(m_phase)->GetPollutionTrigger();
}

sint32 Pollution::GetGlobalPollutionLevel()
{
	sint32 pollution      = m_history[0];
	sint32 gaiaController = 0;

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(!g_player[i]) continue;

		pollution      += g_player[i]->GetPollutionLevel();
		gaiaController += wonderutil_GetReduceWorldPollution(g_player[i]->GetBuiltWonders());
	}

	pollution -= gaiaController;

	return pollution;
}

void Pollution::SetGlobalPollutionLevel(sint32 requiredPollution)
{
	sint32 playerPollution = 0;
	sint32 gaiaController  = 0;

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(g_player[i] != NULL)
		{
			playerPollution += g_player[i]->GetPollutionLevel();
			gaiaController  += wonderutil_GetReduceWorldPollution(g_player[i]->GetBuiltWonders());
		}
	}

	requiredPollution -= playerPollution;
	requiredPollution += gaiaController;

	if (requiredPollution < 0)
		requiredPollution = 0;

	SetHistory(requiredPollution);
}

void Pollution::BeginTurn(void)
{
	if(!g_theGameSettings->IsPollution())
		return;

	if(GetRoundsToNextDisaster() < k_ROUNDS_BEFORE_DISASTER)
	{
		WarnPlayers();
	}

	if(AtTriggerLevel())
	{
		RunNextDesaster();

		m_eventTriggerNextRound = 0;
	}
	else
		m_eventTriggerNextRound-- ;

	if(g_network.IsHost())
	{
		g_network.EnqueuePollution();
	}
}

bool Pollution::RunNextDesaster()
{
	if(m_phase < g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetNumPhase())
	{
		const PollutionRecord::Phase* pprec = g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetPhase(m_phase);

		if(pprec->GetOzoneDisaster())
		{
			g_theWorld->OzoneDepletion();
		}
		if(pprec->GetFloodDisaster())
		{
			g_theWorld->GlobalWarming(m_gwPhase);
			m_gwPhase++;
		}

		GotoNextLevel();
		m_eventTriggered = TRUE;

		return true;
	}
	else
	{
		return false;
	}
}

void Pollution::GotoNextLevel(void)
{
	if(m_phase < g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetNumPhase())
		m_phase++;
}

void Pollution::EndRound()
{
	double	offset,
			slope;

	sint32 pollution = m_history[0];

	memcpy(&m_history[1], &m_history[0], sizeof(m_history)-sizeof(m_history[0]));

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(!g_player[i]) continue;
		pollution += g_player[i]->GetPollutionLevel();
	}

	m_history[0] = pollution;

	m_trend = CalcTrend(m_history, k_MAX_GLOBAL_POLLUTION_RECORD_TURNS, offset, slope);

	if(g_network.IsHost())
	{
		g_network.EnqueuePollution();
	}

	DPRINTF(k_DBG_FIX, ("Global Pollution Level: %ld\n", pollution));
}

sint32 Pollution::CalcTrend(sint32 level[], sint32 numPoints, double &offset, double &slope)
{
	sint32	i;

	double	t,
			sxoss,
			sx=0.0, sy=0.0,
			st2=0.0,
			ss;

	slope=0.0;

	for(i=1; i<=numPoints; i++)
	{
		sx += i;
		sy += level[i];
	}

	ss = numPoints;

	sxoss=sx/ss ;
	for(i = 1; i <= numPoints; i++)
	{
		t      = i-sxoss;
		st2   += t*t;
		slope += t*(level[i]);
	}

	slope /= st2;
	offset =(sy-sx*slope)/ss;

	if (slope<0.0)
		return (k_TREND_DOWNWARD);
	else if (slope>0.0)
		return (k_TREND_UPWARD);

	return (k_TREND_LEVEL);
}

sint32 Pollution::GetTrend(void) const
{
	return (m_trend);
}

//----------------------------------------------------------------------------
//
// Name       : Pollution::GetRoundsToNextDisaster
//
// Description: Estimate number of turns to next disaster.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32          : estimated number of turns to next disaster
//
// Remark(s)  : Will return ROUNDS_COUNT_INMEASURABLE when there is no
//              pollution at all.
//
//              m_history[0]    : pollution level this turn
//              m_history[1]    : pollution level previous turn
//
//----------------------------------------------------------------------------
sint32 Pollution::GetRoundsToNextDisaster(void)
{
	// Check if there has been any pollution at all in this turn.
	if((m_history[0] <= 0) || (m_history[0] <= m_history[1]))
	{
		return ROUNDS_COUNT_IMMEASURABLE;
	}

	// Check for a pollution suppressing wonder built by some player.
	for(int i = 0; i < k_MAX_PLAYERS; ++i)
	{
		if(g_player[i] &&
			wonderutil_GetReduceWorldPollution(g_player[i]->GetBuiltWonders())
		  )
		{
			return ROUNDS_COUNT_IMMEASURABLE;
		}
	}

	// Estimate the number of turns until the next disaster.
	sint32 const pollutionDeltaPerTurn	= m_history[0] - m_history[1];
	sint32 const pollutionUntilTrigger	=
		g_thePollutionDB->Get(g_theProfileDB->GetMapSize())->GetPhase(m_phase)->GetPollutionTrigger() - m_history[0];
	return pollutionUntilTrigger / pollutionDeltaPerTurn;
}

void pollution_NukeCell(MapPoint &pos, Cell *cell)
{
	bool CutNPasteCodeIsBad = false;
	Assert(CutNPasteCodeIsBad);
	return;
#if 0 // CtP1?
	if(cell->GetCanDie())
	{
		cell->Kill();

		g_theWorld->CutImprovements(pos);

		if(g_theWorld->GetCell(pos)->GetEnv() & k_BIT_ENV_INSTALLATION)
		{
			DynamicArray<Installation> instArray;
			g_theInstallationTree->GetAt(pos, instArray);
			instArray.KillList();
		}
		g_theWorld->GetCell(pos)->SetEnv(
						 g_theWorld->GetCell(pos)->GetEnv() & ~(k_MASK_ENV_ROAD |
																k_MASK_ENV_IRRIGATION |
																k_MASK_ENV_MINE |
																k_MASK_ENV_INSTALLATION |
																k_MASK_ENV_CANAL_TUNNEL));
		if(g_network.IsHost())
		{
			g_network.Enqueue(g_theWorld->GetCell(pos), pos.x, pos.y);
		}

		cell->CalcTerrainMoveCost();
		MapPoint nonConstPos = pos;

		g_tiledMap->PostProcessTile(nonConstPos, g_theWorld->GetTileInfo(nonConstPos));
		g_tiledMap->TileChanged(nonConstPos);
		MapPoint npos;
		for(WORLD_DIRECTION d = NORTH; d < NOWHERE;
			d = (WORLD_DIRECTION)((sint32)d + 1)) // No better idea of doing it?!
		{
			if(pos.GetNeighborPosition(d, npos))
			{
				g_tiledMap->PostProcessTile(
											npos,
											g_theWorld->GetTileInfo(npos));
				g_tiledMap->TileChanged(npos);
			}
		}
		g_tiledMap->RedrawTile(&pos);
	}
#endif
}

void Pollution::AddNukePollution(const MapPoint &cpos)
{
	if(!g_theWorld->GetCell(cpos)->HasCity())
	{
		MapPoint stupidNonConstMapPoint = cpos;
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_KillTile,
							   GEA_MapPoint, stupidNonConstMapPoint,
							   GEA_End);
	}

	MapPoint pos;
	for (sint32 i = 0; i < g_theConstDB->Get(0)->GetNukeKillsTiles(); i++)
	{
		if(cpos.GetNeighborPosition((WORLD_DIRECTION)g_rand->Next(sint32(NOWHERE)), pos))
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
								   GEV_KillTile,
								   GEA_MapPoint, pos,
								   GEA_End);
		}
	}
	m_history[0]+=g_theConstDB->Get(0)->GetPollutionCausedByNuke();
}

uint32 Pollution::GetPollutionAtRound(const PLAYER_INDEX player, const sint32 round)
{
	Assert(g_player[player] != NULL);
	if (g_player[player] == NULL)
		return 0;

	sint32 current_round = NewTurnCount::GetCurrentRound();

	if (current_round < round)
		return 0;

	if ((current_round - round) > k_MAX_POLLUTION_HISTORY)
		return 0;

	return g_player[player]->GetPollutionHistory()[current_round - round];
}
