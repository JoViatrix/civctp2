#
# $Id$
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LIBRARIES = libgsnewdb.a
# This hack shortens include path list,
# but prohibits using a separate build dir
#ctp2_code=..

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

#libgsnewdb_a_LDFLAGS = -static

libgsnewdb_a_SOURCES = \
	lex.dbl.c \
	AdvanceBranchRecord.cpp \
	AdvanceListRecord.cpp \
	AdvanceRecord.cpp \
	AgeCityStyleRecord.cpp \
	AgeRecord.cpp \
	BuildingBuildListRecord.cpp \
	BuildingRecord.cpp \
	BuildListSequenceRecord.cpp \
	CitySizeRecord.cpp \
	CityStyleRecord.cpp \
	CivilisationRecord.cpp \
	ConceptRecord.cpp \
	ConstRecord.cpp \
	CultureRecord.cpp \
	DifficultyRecord.cpp \
	DiplomacyProposalRecord.cpp \
	DiplomacyRecord.cpp \
	DiplomacyThreatRecord.cpp \
	EndGameObjectRecord.cpp \
	FeatRecord.cpp \
	GlobalWarmingRecord.cpp \
	GoalRecord.cpp \
	GovernmentRecord.cpp \
	IconRecord.cpp \
	ImprovementListRecord.cpp \
	LanguageRecord.cpp \
	LeaderRecord.cpp \
	MapIconRecord.cpp \
	MapRecord.cpp \
	OrderRecord.cpp \
	PersonalityRecord.cpp \
	PollutionRecord.cpp \
	PopRecord.cpp \
	ReligionRecord.cpp \
	ResourceRecord.cpp \
	RiskRecord.cpp \
	SoundRecord.cpp \
	SpecialAttackInfoRecord.cpp \
	SpecialEffectRecord.cpp \
	SpriteRecord.cpp \
	StrategyRecord.cpp \
	TerrainImprovementRecord.cpp \
	TerrainRecord.cpp \
	UnitBuildListRecord.cpp \
	UnitpromotionRecord.cpp \
	UnitRecord.cpp \
	WonderBuildListRecord.cpp \
	WonderMovieRecord.cpp \
	WonderRecord.cpp \
	CTPDatabase.cpp \
	CTPRecord.cpp \
	DBLexer.cpp \
	DBLexer.cpp

newdb_CDBTYPES = \
	AdvanceBranchRecord \
	AdvanceListRecord \
	AdvanceRecord \
	AgeCityStyleRecord \
	AgeRecord \
	BuildListSequenceRecord \
	BuildingBuildListRecord \
	BuildingRecord \
	CitySizeRecord \
	CityStyleRecord \
	CivilisationRecord \
	ConceptRecord \
	ConstRecord \
	CultureRecord \
	DifficultyRecord \
	DiplomacyProposalRecord \
	DiplomacyRecord \
	DiplomacyThreatRecord \
	EndGameObjectRecord \
	FeatRecord \
	GlobalWarmingRecord \
	GoalRecord \
	GovernmentRecord \
	IconRecord \
	ImprovementListRecord \
	LanguageRecord \
	LeaderRecord \
	MapIconRecord \
	MapRecord \
	OrderRecord \
	PersonalityRecord \
	PollutionRecord \
	PopRecord \
	ReligionRecord \
	ResourceRecord \
	RiskRecord \
	SoundRecord \
	SpecialAttackInfoRecord \
	SpecialEffectRecord \
	SpriteRecord \
	StrategyRecord \
	TerrainImprovementRecord \
	TerrainRecord \
	UnitBuildListRecord \
	UnitpromotionRecord \
	UnitRecord \
	WonderBuildListRecord \
	WonderMovieRecord \
	WonderRecord

newdb_CDBSRCFILES = \
	$(foreach type,$(newdb_CDBTYPES),$(type).cpp )

newdb_CDBHDRFILES = \
	$(foreach type,$(newdb_CDBTYPES),$(type).h )

newdb_CDBSTAMPFILES = \
	$(foreach type,$(newdb_CDBTYPES),$(type).stamp )

# Rules for generated sources

lex.dbl.c: $(ctp2_code)/gs/newdb/DBLexer.l
	$(LEX) -i -Pdbl -olex.dbl.c $<

CTPDB=$(top_builddir)/ctp2_code/gs/dbgen/ctpdb$(EXEEXT)

$(CTPDB): ../dbgen/Makefile
	$(MAKE) -C ../dbgen

CTPDatabase.cpp: $(newdb_CDBHDRFILES)

AdvanceBranchRecord.cpp \
AdvanceBranchRecord.h: AdvanceBranchRecord.stamp

AdvanceBranchRecord.stamp: $(ctp2_code)/gs/newdb/advance.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

AdvanceListRecord.cpp \
AdvanceListRecord.h: AdvanceListRecord.stamp

AdvanceListRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		AdvanceRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

AdvanceRecord.cpp \
AdvanceRecord.h: AdvanceRecord.stamp

AdvanceRecord.stamp: $(ctp2_code)/gs/newdb/advance.cdb \
		AgeRecord.h \
		CityStyleRecord.h GovernmentRecord.h \
		FeatRecord.h IconRecord.h ResourceRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

AgeCityStyleRecord.cpp \
AgeCityStyleRecord.h: AgeCityStyleRecord.stamp

AgeCityStyleRecord.stamp: $(ctp2_code)/gs/newdb/citystyle.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

AgeRecord.cpp \
AgeRecord.h: AgeRecord.stamp

AgeRecord.stamp: $(ctp2_code)/gs/newdb/advance.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

BuildingBuildListRecord.cpp \
BuildingBuildListRecord.h: BuildingBuildListRecord.stamp

BuildingBuildListRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		BuildingRecord.h \
		$(CTPDB) 
	$(CTPDB) . < $<
	touch $@

BuildingRecord.cpp \
BuildingRecord.h: BuildingRecord.stamp

BuildingRecord.stamp: $(ctp2_code)/gs/newdb/building.cdb $(CTPDB) \
		CivilisationRecord.h WonderRecord.h \
		TerrainImprovementRecord.h
	$(CTPDB) . < $<
	touch $@

BuildListSequenceRecord.cpp \
BuildListSequenceRecord.h: BuildListSequenceRecord.stamp

BuildListSequenceRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

CitySizeRecord.cpp \
CitySizeRecord.h: CitySizeRecord.stamp

CitySizeRecord.stamp: $(ctp2_code)/gs/newdb/citysize.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

CityStyleRecord.cpp \
CityStyleRecord.h: CityStyleRecord.stamp

CityStyleRecord.stamp: $(ctp2_code)/gs/newdb/citystyle.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

CivilisationRecord.cpp \
CivilisationRecord.h: CivilisationRecord.stamp

CivilisationRecord.stamp: $(ctp2_code)/gs/newdb/Civilisation.cdb $(CTPDB) PersonalityRecord.stamp
	$(CTPDB) . < $<
	touch $@

ConceptRecord.cpp \
ConceptRecord.h: ConceptRecord.stamp

ConceptRecord.stamp: $(ctp2_code)/gs/newdb/Concept.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

ConstRecord.cpp \
ConstRecord.h: ConstRecord.stamp

ConstRecord.stamp: $(ctp2_code)/gs/newdb/Const.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

CultureRecord.cpp \
CultureRecord.h: CultureRecord.stamp

CultureRecord.stamp: $(ctp2_code)/gs/newdb/culture.cdb \
		BuildingRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

DifficultyRecord.cpp \
DifficultyRecord.h: DifficultyRecord.stamp

DifficultyRecord.stamp: $(ctp2_code)/gs/newdb/Difficulty.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

DiplomacyProposalRecord.cpp \
DiplomacyProposalRecord.h: DiplomacyProposalRecord.stamp

DiplomacyProposalRecord.stamp: $(ctp2_code)/gs/newdb/DiplomacyProposal.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

DiplomacyRecord.cpp \
DiplomacyRecord.h: DiplomacyRecord.stamp

DiplomacyRecord.stamp: $(ctp2_code)/gs/newdb/diplomacy.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

DiplomacyThreatRecord.cpp \
DiplomacyThreatRecord.h: DiplomacyThreatRecord.stamp

DiplomacyThreatRecord.stamp: $(ctp2_code)/gs/newdb/DiplomacyThreat.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

EndGameObjectRecord.cpp \
EndGameObjectRecord.h: EndGameObjectRecord.stamp

EndGameObjectRecord.stamp: $(ctp2_code)/gs/newdb/EndGameObject.cdb \
		TerrainImprovementRecord.h \
		WonderRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

FeatRecord.cpp \
FeatRecord.h: FeatRecord.stamp

FeatRecord.stamp: $(ctp2_code)/gs/newdb/feat.cdb \
		BuildingRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

GlobalWarmingRecord.cpp \
GlobalWarmingRecord.h: GlobalWarmingRecord.stamp

GlobalWarmingRecord.stamp: $(ctp2_code)/gs/newdb/GlobalWarming.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

GoalRecord.cpp \
GoalRecord.h: GoalRecord.stamp

GoalRecord.stamp: $(ctp2_code)/gs/newdb/goal.cdb \
		OrderRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

GovernmentRecord.cpp \
GovernmentRecord.h: GovernmentRecord.stamp

GovernmentRecord.stamp: $(ctp2_code)/gs/newdb/government.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

IconRecord.cpp \
IconRecord.h: IconRecord.stamp

IconRecord.stamp: $(ctp2_code)/gs/newdb/icon.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

ImprovementListRecord.cpp \
ImprovementListRecord.h: ImprovementListRecord.stamp

ImprovementListRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		TerrainImprovementRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

LanguageRecord.cpp \
LanguageRecord.h: LanguageRecord.stamp

LanguageRecord.stamp: $(ctp2_code)/gs/newdb/Language.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

LeaderRecord.cpp \
LeaderRecord.h: LeaderRecord.stamp

LeaderRecord.stamp: $(ctp2_code)/gs/newdb/Leader.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

MapIconRecord.cpp \
MapIconRecord.h: MapIconRecord.stamp

MapIconRecord.stamp: $(ctp2_code)/gs/newdb/Map.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

MapRecord.cpp \
MapRecord.h: MapRecord.stamp

MapRecord.stamp: $(ctp2_code)/gs/newdb/Map.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

OrderRecord.cpp \
OrderRecord.h: OrderRecord.stamp

OrderRecord.stamp: $(ctp2_code)/gs/newdb/order.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

PersonalityRecord.cpp \
PersonalityRecord.h: PersonalityRecord.stamp

PersonalityRecord.stamp: $(ctp2_code)/gs/newdb/personality.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@


PollutionRecord.cpp \
PollutionRecord.h: PollutionRecord.stamp

PollutionRecord.stamp: $(ctp2_code)/gs/newdb/Pollution.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

PopRecord.cpp \
PopRecord.h: PopRecord.stamp

PopRecord.stamp: $(ctp2_code)/gs/newdb/pop.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

ReligionRecord.cpp \
ReligionRecord.h: ReligionRecord.stamp

ReligionRecord.stamp: $(ctp2_code)/gs/newdb/religion.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

ResourceRecord.cpp \
ResourceRecord.h: ResourceRecord.stamp

ResourceRecord.stamp: $(ctp2_code)/gs/newdb/terrain.cdb \
		SoundRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

RiskRecord.cpp \
RiskRecord.h: RiskRecord.stamp

RiskRecord.stamp: $(ctp2_code)/gs/newdb/Risk.cdb \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

SoundRecord.cpp \
SoundRecord.h: SoundRecord.stamp

SoundRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

SpecialAttackInfoRecord.cpp \
SpecialAttackInfoRecord.h: SpecialAttackInfoRecord.stamp

SpecialAttackInfoRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

SpecialEffectRecord.cpp \
SpecialEffectRecord.h: SpecialEffectRecord.stamp

SpecialEffectRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

SpriteRecord.cpp \
SpriteRecord.h: SpriteRecord.stamp

SpriteRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

StrategyRecord.cpp \
StrategyRecord.h: StrategyRecord.stamp

StrategyRecord.stamp: $(ctp2_code)/gs/newdb/strategy.cdb \
		ConstRecord.h GoalRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

TerrainImprovementRecord.cpp \
TerrainImprovementRecord.h: TerrainImprovementRecord.stamp

TerrainImprovementRecord.stamp: $(ctp2_code)/gs/newdb/TerrainImprovement.cdb \
		TerrainRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

TerrainRecord.cpp \
TerrainRecord.h: TerrainRecord.stamp

TerrainRecord.stamp: $(ctp2_code)/gs/newdb/terrain.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

UnitBuildListRecord.cpp \
UnitBuildListRecord.h: UnitBuildListRecord.stamp

UnitBuildListRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

UnitpromotionRecord.cpp \
UnitpromotionRecord.h: UnitpromotionRecord.stamp

UnitpromotionRecord.stamp: $(ctp2_code)/gs/newdb/unitpromotion.cdb \
		BuildingRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

UnitRecord.cpp \
UnitRecord.h: UnitRecord.stamp

UnitRecord.stamp: $(ctp2_code)/gs/newdb/unit.cdb \
		TerrainImprovementRecord.h \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@

WonderBuildListRecord.cpp \
WonderBuildListRecord.h: WonderBuildListRecord.stamp

WonderBuildListRecord.stamp: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

WonderMovieRecord.cpp \
WonderMovieRecord.h: WonderMovieRecord.stamp

WonderMovieRecord.stamp: $(ctp2_code)/gs/newdb/wonder.cdb $(CTPDB)
	$(CTPDB) . < $<
	touch $@

WonderRecord.cpp \
WonderRecord.h: WonderRecord.stamp BuildingRecord.stamp FeatRecord.stamp

WonderRecord.stamp: $(ctp2_code)/gs/newdb/wonder.cdb \
		$(CTPDB)
	$(CTPDB) . < $<
	touch $@


AM_CXXFLAGS=\
	$(SDL_CFLAGS)

AM_CPPFLAGS=\
	$(CTP2_NOWIN32_INC) \
	$(DIRECTX_INCLUDES) \
	$(DXMEDIA_INCLUDES) \
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ctp/ctp2_utils \
	-I$(ctp2_code)/ctp/ctp2_rsrc \
	$(CTP2_WIN32_RSRC_INC) \
	-I$(ctp2_code)/ctp/debugtools \
		$(INCORDER_ctp2_code__libs__freetype__lib) \
		$(INCORDER_ctp2_code__libs__freetype__utils) \
		$(INCORDER_ctp2_code__libs__freetype__win32) \
	-I$(ctp2_code)/ui/interface \
	-I$(ctp2_code)/ui/netshell \
	-I$(ctp2_code)/robot/utility \
	-I$(ctp2_code)/robot/pathing \
	-I$(ctp2_code)/robot/aibackdoor \
	$(CTP2_WIN32_FREETYPE_INC) \
	-I$(ctp2_code)/gfx/spritesys \
	-I$(ctp2_code)/gfx/tilesys \
	-I$(ctp2_code)/gfx/gfx_utils \
	-I$(ctp2_code)/gs/database \
	-I$(ctp2_code)/gs/fileio \
	-I$(ctp2_code)/gs/gameobj \
	-I$(ctp2_code)/gs/utility \
	-I$(ctp2_code)/gs/world \
	-I$(ctp2_code)/net/io \
	-I$(ctp2_code)/net/general \
	-I$(ctp2_code)/ui/aui_utils \
	-I$(ctp2_code)/ui/aui_sdl \
	-I$(ctp2_code)/ui/aui_directx \
	-I$(ctp2_code)/ui/aui_ctp2 \
	-I$(ctp2_code)/ui/aui_common \
		$(INCORDER_ctp2_code__libs__tiff) \
		$(CTP2_ANET_INC) \
	-I$(ctp2_code)/mm \
	-I$(ctp2_code)/robotcom/backdoor \
	-I$(ctp2_code)/gs/slic \
	-I$(ctp2_code)/gfx/layers \
	-I$(ctp2_code)/mapgen \
	-I$(ctp2_code)/ui/freetype \
	-I$(ctp2_code)/sound \
	-I$(ctp2_code)/GameWatch/gamewatch \
	-I$(ctp2_code)/GameWatch/gwciv \
	$(CTP2_WIN32_MILES_INC) \
	-I$(ctp2_code)/ctp/fingerprint \
	$(CTP2_WIN32_ZLIB_INC) \
	-I$(ctp2_code)/ui/slic_debug \
	-I$(ctp2_code)/gs/outcom \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ui/aui_common \
	-I$(ctp2_code)/ui/ldl \
		$(INCORDER_ctp2_code__libs__dxmedia__classes__base) \
		$(INCORDER_ctp2_code__libs__dxmedia__include) \
		$(INCORDER_ctp2_code__libs__directx__include) \
	-I$(ctp2_code)/gs/events \
	-I$(top_builddir)/ctp2_code/gs/newdb \
	-I$(ctp2_code)/gs/newdb \
	-I$(ctp2_code)/ai/diplomacy \
	-I$(ctp2_code)/ai/mapanalysis \
	-I$(ctp2_code)/ai/strategy/scheduler \
	-I$(ctp2_code)/ai/strategy/agents \
	-I$(ctp2_code)/ai/strategy/goals \
	-I$(ctp2_code)/ai/strategy/squads \
	-I$(ctp2_code)/ai \
	-I$(ctp2_code)/ai/citymanagement

MOSTLYCLEANFILES=\
	$(newdb_CDBSRCFILES) \
	$(newdb_CDBHDRFILES) \
	$(newdb_CDBSTAMPFILES) \
	$(foreach src,$(newdb_CDBSRCFILES),$(src).old $(src).new ) \
	$(foreach hdr,$(newdb_CDBHDRFILES),$(hdr).old $(hdr).new ) \
	$(foreach stamp,$(newdb_CDBSTAMPFILES),$(stamp).old $(stamp).new )
