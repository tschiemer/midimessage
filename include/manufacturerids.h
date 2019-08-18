//
// MIDI SysEx Message Manufacturer Id List
//
// Generated using 'make manufacturerids'
//
// @date  2019-08-18T10:23:06+00:00
// @url https://www.midi.org/specifications-old/item/manufacturer-id-numbers
//

#ifndef MIDIMESSAGE_MANUFACTURERIDS_H
#define MIDIMESSAGE_MANUFACTURERIDS_H

namespace MidiMessage {

    typedef enum {

		 ManufacturerIdUsedforIDExtensions 	 = 0x00000000 /* --- / [Used for ID Extensions]  */,
		 ManufacturerIdTimeWarnerInteractive 	 = 0x00000001 /* --- / Time/Warner Interactive  */,
		 ManufacturerIdAdvancedGravisCompTechLtd 	 = 0x00000002 /* rescinded / Advanced Gravis Comp. Tech Ltd.  */,
		 ManufacturerIdMediaVision 	 = 0x00000003 /* rescinded / Media Vision  */,
		 ManufacturerIdDornesResearchGroup 	 = 0x00000004 /* rescinded / Dornes Research Group  */,
		 ManufacturerIdKMuse 	 = 0x00000005 /* rescinded / K-Muse  */,
		 ManufacturerIdStypher 	 = 0x00000006 /* rescinded / Stypher  */,
		 ManufacturerIdDigitalMusicCorp 	 = 0x00000007 /* --- / Digital Music Corp.  */,
		 ManufacturerIdIOTASystems 	 = 0x00000008 /* --- / IOTA Systems  */,
		 ManufacturerIdNewEnglandDigital 	 = 0x00000009 /* --- / New England Digital  */,
		 ManufacturerIdArtisyn 	 = 0x0000000A /* --- / Artisyn  */,
		 ManufacturerIdIVLTechnologiesLtd 	 = 0x0000000B /* --- / IVL Technologies Ltd.  */,
		 ManufacturerIdSouthernMusicSystems 	 = 0x0000000C /* --- / Southern Music Systems  */,
		 ManufacturerIdLakeButlerSoundCompany 	 = 0x0000000D /* --- / Lake Butler Sound Company  */,
		 ManufacturerIdAlesisStudioElectronics 	 = 0x0000000E /* --- / Alesis Studio Electronics  */,
		 ManufacturerIdSoundCreation 	 = 0x0000000F /* rescinded / Sound Creation  */,
		 ManufacturerIdDODElectronicsCorp 	 = 0x00000010 /* --- / DOD Electronics Corp.  */,
		 ManufacturerIdStuderEditech 	 = 0x00000011 /* --- / Studer-Editech  */,
		 ManufacturerIdSonus 	 = 0x00000012 /* rescinded / Sonus  */,
		 ManufacturerIdTemporalAcuityProducts 	 = 0x00000013 /* rescinded / Temporal Acuity Products  */,
		 ManufacturerIdPerfectFretworks 	 = 0x00000014 /* --- / Perfect Fretworks  */,
		 ManufacturerIdKATInc 	 = 0x00000015 /* --- / KAT Inc.  */,
		 ManufacturerIdOpcodeSystems 	 = 0x00000016 /* --- / Opcode Systems  */,
		 ManufacturerIdRaneCorporation 	 = 0x00000017 /* --- / Rane Corporation  */,
		 ManufacturerIdAnadiElectronique 	 = 0x00000018 /* --- / Anadi Electronique  */,
		 ManufacturerIdKMX 	 = 0x00000019 /* --- / KMX  */,
		 ManufacturerIdAllenampHeathBrenell 	 = 0x0000001A /* --- / Allen &amp; Heath Brenell  */,
		 ManufacturerIdPeaveyElectronics 	 = 0x0000001B /* --- / Peavey Electronics  */,
		 ManufacturerIdSystems 	 = 0x0000001C /* --- / 360 Systems  */,
		 ManufacturerIdSpectrumDesignandDevelopment 	 = 0x0000001D /* --- / Spectrum Design and Development  */,
		 ManufacturerIdMarquisMusic 	 = 0x0000001E /* --- / Marquis Music  */,
		 ManufacturerIdZetaSystems 	 = 0x0000001F /* --- / Zeta Systems  */,
		 ManufacturerIdAxxesBrianParsonett 	 = 0x00000020 /* --- / Axxes (Brian Parsonett)  */,
		 ManufacturerIdOrban 	 = 0x00000021 /* --- / Orban  */,
		 ManufacturerIdIndianValleyMfg 	 = 0x00000022 /* rescinded / Indian Valley Mfg.  */,
		 ManufacturerIdTriton 	 = 0x00000023 /* rescinded / Triton  */,
		 ManufacturerIdKTI 	 = 0x00000024 /* --- / KTI  */,
		 ManufacturerIdBreakawayTechnologies 	 = 0x00000025 /* --- / Breakaway Technologies  */,
		 ManufacturerIdLepreconCAEInc 	 = 0x00000026 /* --- / Leprecon / CAE Inc.  */,
		 ManufacturerIdHarrisonSystemsInc 	 = 0x00000027 /* rescinded / Harrison Systems Inc.  */,
		 ManufacturerIdFutureLabMarkKuo 	 = 0x00000028 /* rescinded / Future Lab/Mark Kuo  */,
		 ManufacturerIdRocktronCorporation 	 = 0x00000029 /* --- / Rocktron Corporation  */,
		 ManufacturerIdPianoDisc 	 = 0x0000002A /* --- / PianoDisc  */,
		 ManufacturerIdCannonResearchGroup 	 = 0x0000002B /* --- / Cannon Research Group  */,
		 ManufacturerIdReserved 	 = 0x0000002C /* --- / Reserved  */,
		 ManufacturerIdRodgersInstrumentLLC 	 = 0x0000002D /* --- / Rodgers Instrument LLC  */,
		 ManufacturerIdBlueSkyLogic 	 = 0x0000002E /* --- / Blue Sky Logic  */,
		 ManufacturerIdEncoreElectronics 	 = 0x0000002F /* --- / Encore Electronics  */,
		 ManufacturerIdUptown 	 = 0x00000030 /* --- / Uptown  */,
		 ManufacturerIdVoce 	 = 0x00000031 /* --- / Voce  */,
		 ManufacturerIdCTIAudioIncMusicallyIntelDevs 	 = 0x00000032 /* --- / CTI Audio, Inc. (Musically Intel. Devs.)  */,
		 ManufacturerIdSIncorporated 	 = 0x00000033 /* --- / S3 Incorporated  */,
		 ManufacturerIdBroderbundRedOrb 	 = 0x00000034 /* --- / Broderbund / Red Orb  */,
		 ManufacturerIdAllenOrganCo 	 = 0x00000035 /* --- / Allen Organ Co.  */,
		 ManufacturerIdReserved 	 = 0x00000036 /* --- / Reserved  */,
		 ManufacturerIdMusicQuest 	 = 0x00000037 /* rescinded / Music Quest  */,
		 ManufacturerIdAphex 	 = 0x00000038 /* --- / Aphex  */,
		 ManufacturerIdGallienKrueger 	 = 0x00000039 /* --- / Gallien Krueger  */,
		 ManufacturerIdIBM 	 = 0x0000003A /* --- / IBM  */,
		 ManufacturerIdMarkOfTheUnicorn 	 = 0x0000003B /* current / Mark Of The Unicorn http://www.motu.com */,
		 ManufacturerIdHotzCorporation 	 = 0x0000003C /* --- / Hotz Corporation  */,
		 ManufacturerIdETALighting 	 = 0x0000003D /* --- / ETA Lighting  */,
		 ManufacturerIdNSICorporation 	 = 0x0000003E /* --- / NSI Corporation  */,
		 ManufacturerIdAdLibInc 	 = 0x0000003F /* --- / Ad Lib, Inc.  */,
		 ManufacturerIdRichmondSoundDesign 	 = 0x00000040 /* --- / Richmond Sound Design  */,
		 ManufacturerIdMicrosoft 	 = 0x00000041 /* current / Microsoft https://www.microsoft.com */,
		 ManufacturerIdMindscapeSoftwareToolworks 	 = 0x00000042 /* --- / Mindscape (Software Toolworks)  */,
		 ManufacturerIdRussJonesMarketingNiche 	 = 0x00000043 /* --- / Russ Jones Marketing / Niche  */,
		 ManufacturerIdIntone 	 = 0x00000044 /* --- / Intone  */,
		 ManufacturerIdAdvancedRemoteTechnologies 	 = 0x00000045 /* rescinded / Advanced Remote Technologies  */,
		 ManufacturerIdWhiteInstruments 	 = 0x00000046 /* rescinded / White Instruments  */,
		 ManufacturerIdGTElectronicsGrooveTubes 	 = 0x00000047 /* --- / GT Electronics/Groove Tubes  */,
		 ManufacturerIdPacificResearchampEngineering 	 = 0x00000048 /* rescinded / Pacific Research &amp; Engineering  */,
		 ManufacturerIdTimelineVistaInc 	 = 0x00000049 /* --- / Timeline Vista, Inc.  */,
		 ManufacturerIdMesaBoogieLtd 	 = 0x0000004A /* --- / Mesa Boogie Ltd.  */,
		 ManufacturerIdFSLI 	 = 0x0000004B /* rescinded / FSLI  */,
		 ManufacturerIdSequoiaDevelopmentGroup 	 = 0x0000004C /* --- / Sequoia Development Group  */,
		 ManufacturerIdStudioElectronics 	 = 0x0000004D /* --- / Studio Electronics  */,
		 ManufacturerIdEuphonixInc 	 = 0x0000004E /* --- / Euphonix, Inc  */,
		 ManufacturerIdInterMIDIInc 	 = 0x0000004F /* --- / InterMIDI, Inc.  */,
		 ManufacturerIdMIDISolutionsInc 	 = 0x00000050 /* --- / MIDI Solutions Inc.  */,
		 ManufacturerIdDOCompany 	 = 0x00000051 /* --- / 3DO Company  */,
		 ManufacturerIdLightwaveResearchHighEndSystems 	 = 0x00000052 /* --- / Lightwave Research / High End Systems  */,
		 ManufacturerIdMicroWCorporation 	 = 0x00000053 /* --- / Micro-W Corporation  */,
		 ManufacturerIdSpectralSynthesisInc 	 = 0x00000054 /* --- / Spectral Synthesis, Inc.  */,
		 ManufacturerIdLoneWolf 	 = 0x00000055 /* --- / Lone Wolf  */,
		 ManufacturerIdStudioTechnologiesInc 	 = 0x00000056 /* --- / Studio Technologies Inc.  */,
		 ManufacturerIdPetersonElectroMusicalProductInc 	 = 0x00000057 /* --- / Peterson Electro-Musical Product, Inc.  */,
		 ManufacturerIdAtariCorporation 	 = 0x00000058 /* --- / Atari Corporation  */,
		 ManufacturerIdMarionSystemsCorporation 	 = 0x00000059 /* --- / Marion Systems Corporation  */,
		 ManufacturerIdDesignEvent 	 = 0x0000005A /* --- / Design Event  */,
		 ManufacturerIdWinjammerSoftwareLtd 	 = 0x0000005B /* --- / Winjammer Software Ltd.  */,
		 ManufacturerIdATampTBellLaboratories 	 = 0x0000005C /* --- / AT&amp;T Bell Laboratories  */,
		 ManufacturerIdReserved 	 = 0x0000005D /* --- / Reserved  */,
		 ManufacturerIdSymetrix 	 = 0x0000005E /* --- / Symetrix  */,
		 ManufacturerIdMIDItheWorld 	 = 0x0000005F /* --- / MIDI the World  */,
		 ManufacturerIdSpatializer 	 = 0x00000060 /* --- / Spatializer  */,
		 ManufacturerIdMicrosNMIDI 	 = 0x00000061 /* --- / Micros 'N MIDI  */,
		 ManufacturerIdAccordiansInternational 	 = 0x00000062 /* --- / Accordians International  */,
		 ManufacturerIdEuPhonicsnowCom 	 = 0x00000063 /* --- / EuPhonics (now 3Com)  */,
		 ManufacturerIdMusonix 	 = 0x00000064 /* --- / Musonix  */,
		 ManufacturerIdTurtleBeachSystemsVoyetra 	 = 0x00000065 /* --- / Turtle Beach Systems (Voyetra)  */,
		 ManufacturerIdLoudTechnologiesMackie 	 = 0x00000066 /* --- / Loud Technologies / Mackie  */,
		 ManufacturerIdCompuserve 	 = 0x00000067 /* --- / Compuserve  */,
		 ManufacturerIdBECTechnologies 	 = 0x00000068 /* --- / BEC Technologies  */,
		 ManufacturerIdQRSMusicInc 	 = 0x00000069 /* --- / QRS Music Inc  */,
		 ManufacturerIdPGMusic 	 = 0x0000006A /* --- / P.G. Music  */,
		 ManufacturerIdSierraSemiconductor 	 = 0x0000006B /* --- / Sierra Semiconductor  */,
		 ManufacturerIdEpiGraf 	 = 0x0000006C /* --- / EpiGraf  */,
		 ManufacturerIdElectronicsDiversifiedInc 	 = 0x0000006D /* --- / Electronics Diversified Inc  */,
		 ManufacturerIdTune 	 = 0x0000006E /* --- / Tune 1000  */,
		 ManufacturerIdAdvancedMicroDevices 	 = 0x0000006F /* --- / Advanced Micro Devices  */,
		 ManufacturerIdMediamation 	 = 0x00000070 /* --- / Mediamation  */,
		 ManufacturerIdSabineMusicalMfgCoInc 	 = 0x00000071 /* --- / Sabine Musical Mfg. Co. Inc.  */,
		 ManufacturerIdWoogLabs 	 = 0x00000072 /* --- / Woog Labs  */,
		 ManufacturerIdMicropolisCorp 	 = 0x00000073 /* --- / Micropolis Corp  */,
		 ManufacturerIdTaHorngMusicalInstrument 	 = 0x00000074 /* --- / Ta Horng Musical Instrument  */,
		 ManufacturerIdeTekLabsForteTech 	 = 0x00000075 /* --- / e-Tek Labs (Forte Tech)  */,
		 ManufacturerIdElectroVoice 	 = 0x00000076 /* --- / Electro-Voice  */,
		 ManufacturerIdMidisoftCorporation 	 = 0x00000077 /* --- / Midisoft Corporation  */,
		 ManufacturerIdQSoundLabs 	 = 0x00000078 /* Relinquished / QSound Labs  */,
		 ManufacturerIdWestrex 	 = 0x00000079 /* --- / Westrex  */,
		 ManufacturerIdNvidia 	 = 0x0000007A /* --- / Nvidia  */,
		 ManufacturerIdESSTechnology 	 = 0x0000007B /* --- / ESS Technology  */,
		 ManufacturerIdMediaTrixPeripherals 	 = 0x0000007C /* --- / Media Trix Peripherals  */,
		 ManufacturerIdBrooktreeCorp 	 = 0x0000007D /* --- / Brooktree Corp  */,
		 ManufacturerIdOtariCorp 	 = 0x0000007E /* --- / Otari Corp  */,
		 ManufacturerIdKeyElectronicsInc 	 = 0x0000007F /* --- / Key Electronics, Inc.  */,
		 ManufacturerIdShureIncorporated 	 = 0x00000100 /* --- / Shure Incorporated  */,
		 ManufacturerIdAuraSound 	 = 0x00000101 /* --- / AuraSound  */,
		 ManufacturerIdCrystalSemiconductor 	 = 0x00000102 /* --- / Crystal Semiconductor  */,
		 ManufacturerIdConexantRockwell 	 = 0x00000103 /* --- / Conexant (Rockwell)  */,
		 ManufacturerIdSiliconGraphics 	 = 0x00000104 /* --- / Silicon Graphics  */,
		 ManufacturerIdMAudioMidiman 	 = 0x00000105 /* --- / M-Audio (Midiman)  */,
		 ManufacturerIdPreSonus 	 = 0x00000106 /* --- / PreSonus  */,
		 ManufacturerIdTopazEnterprises 	 = 0x00000108 /* --- / Topaz Enterprises  */,
		 ManufacturerIdCastLighting 	 = 0x00000109 /* --- / Cast Lighting  */,
		 ManufacturerIdMicrosoftConsumerDivision 	 = 0x0000010A /* --- / Microsoft Consumer Division  */,
		 ManufacturerIdSonicFoundry 	 = 0x0000010B /* --- / Sonic Foundry  */,
		 ManufacturerIdBeatnikInc 	 = 0x0000010D /* out of business / Beatnik Inc  */,
		 ManufacturerIdVanKoeveringCompany 	 = 0x0000010E /* --- / Van Koevering Company  */,
		 ManufacturerIdAltechSystems 	 = 0x0000010F /* --- / Altech Systems  */,
		 ManufacturerIdVLSITechnology 	 = 0x00000111 /* --- / VLSI Technology  */,
		 ManufacturerIdChromaticResearch 	 = 0x00000112 /* --- / Chromatic Research  */,
		 ManufacturerIdSapphire 	 = 0x00000113 /* --- / Sapphire  */,
		 ManufacturerIdIDRC 	 = 0x00000114 /* --- / IDRC  */,
		 ManufacturerIdJustonicTuning 	 = 0x00000115 /* --- / Justonic Tuning  */,
		 ManufacturerIdTorCompResearchInc 	 = 0x00000116 /* --- / TorComp Research Inc.  */,
		 ManufacturerIdNewtekInc 	 = 0x00000117 /* --- / Newtek Inc.  */,
		 ManufacturerIdSoundSculpture 	 = 0x00000118 /* --- / Sound Sculpture  */,
		 ManufacturerIdDigitalHarmonyPAVO 	 = 0x0000011A /* --- / Digital Harmony (PAVO)  */,
		 ManufacturerIdInVisionInteractive 	 = 0x0000011B /* --- / InVision Interactive  */,
		 ManufacturerIdTSquareDesign 	 = 0x0000011C /* --- / T-Square Design  */,
		 ManufacturerIdNemesysMusicTechnology 	 = 0x0000011D /* --- / Nemesys Music Technology  */,
		 ManufacturerIdDBXProfessionalHarmanIntl 	 = 0x0000011E /* --- / DBX Professional (Harman Intl)  */,
		 ManufacturerIdSyndyneCorporation 	 = 0x0000011F /* --- / Syndyne Corporation  */,
		 ManufacturerIdBitheadz 	 = 0x00000120 /* --- / Bitheadz  */,
		 ManufacturerIdNationalSemiconductor 	 = 0x00000123 /* --- / National Semiconductor  */,
		 ManufacturerIdBoomTheoryAdinolfiAlternativePercussion 	 = 0x00000124 /* --- / Boom Theory / Adinolfi Alternative Percussion  */,
		 ManufacturerIdVirtualDSPCorporation 	 = 0x00000125 /* --- / Virtual DSP Corporation  */,
		 ManufacturerIdAntaresSystems 	 = 0x00000126 /* --- / Antares Systems  */,
		 ManufacturerIdAngelSoftware 	 = 0x00000127 /* --- / Angel Software  */,
		 ManufacturerIdStLouisMusic 	 = 0x00000128 /* --- / St Louis Music  */,
		 ManufacturerIdAshleyAudioInc 	 = 0x0000012A /* --- / Ashley Audio Inc.  */,
		 ManufacturerIdVariLiteInc 	 = 0x0000012B /* --- / Vari-Lite Inc.  */,
		 ManufacturerIdSummitAudioInc 	 = 0x0000012C /* --- / Summit Audio Inc.  */,
		 ManufacturerIdAurealSemiconductorInc 	 = 0x0000012D /* --- / Aureal Semiconductor Inc.  */,
		 ManufacturerIdSeaSoundLLC 	 = 0x0000012E /* --- / SeaSound LLC  */,
		 ManufacturerIdUSRobotics 	 = 0x0000012F /* --- / U.S. Robotics  */,
		 ManufacturerIdAurisisResearch 	 = 0x00000130 /* --- / Aurisis Research  */,
		 ManufacturerIdNearfieldResearch 	 = 0x00000131 /* --- / Nearfield Research  */,
		 ManufacturerIdFMInc 	 = 0x00000132 /* --- / FM7 Inc  */,
		 ManufacturerIdSwivelSystems 	 = 0x00000133 /* --- / Swivel Systems  */,
		 ManufacturerIdMidiLiteCastleStudiosProductions 	 = 0x00000135 /* --- / MidiLite (Castle Studios Productions)  */,
		 ManufacturerIdRadikalTechnologies 	 = 0x00000136 /* --- / Radikal Technologies  */,
		 ManufacturerIdSonicNetworkInc 	 = 0x0000013A /* relinquished / Sonic Network Inc  */,
		 ManufacturerIdNumarkIndustries 	 = 0x0000013F /* current / Numark Industries http://www.numark.com */,
		 ManufacturerIdFrontierDesignGroupLLC 	 = 0x00000140 /* relinquished / Frontier Design Group, LLC  */,
		 ManufacturerIdRecordareLLC 	 = 0x00000141 /* relinquished / Recordare LLC  */,
		 ManufacturerIdVoyagerSoundInc 	 = 0x00000143 /* out of business / Voyager Sound Inc.  */,
		 ManufacturerIdMixmeisterTechnology 	 = 0x00000146 /* relinquished / Mixmeister Technology  */,
		 ManufacturerIdNotationSoftware 	 = 0x00000147 /* relinquished / Notation Software  */,
		 ManufacturerIdMercurialCommunications 	 = 0x00000148 /* relinquished / Mercurial Communications  */,
		 ManufacturerIdWaveArts 	 = 0x00000149 /* relinquished / Wave Arts  */,
		 ManufacturerIdElectronicTheatreControls 	 = 0x00000150 /* current / Electronic Theatre Controls http://www.etcconnect.com */,
		 ManufacturerIdMobileer 	 = 0x00000152 /* relinquished / Mobileer  */,
		 ManufacturerIdLynxStudioTechnologyInc 	 = 0x00000154 /* current / Lynx Studio Technology Inc. http://www.lynxstudio.com */,
		 ManufacturerIdDamageControlEngineeringLLC 	 = 0x00000155 /* current / Damage Control Engineering LLC http://www.damagecontrolusa.com */,
		 ManufacturerIdBrooksampForsmanDesignsLLCDrumLite 	 = 0x00000157 /* out of business / Brooks &amp; Forsman Designs LLC / DrumLite  */,
		 ManufacturerIdInfiniteResponse 	 = 0x00000158 /* out of business / Infinite Response  */,
		 ManufacturerIdGarritanCorp 	 = 0x00000159 /* relinquished / Garritan Corp  */,
		 ManufacturerIdCustomSolutionsSoftware 	 = 0x0000015C /* relinquished / Custom Solutions Software  */,
		 ManufacturerIdSonarcanaLLCHighlyLiquid 	 = 0x0000015D /* relinquished / Sonarcana LLC / Highly Liquid  */,
		 ManufacturerIdLividInstruments 	 = 0x00000161 /* current / Livid Instruments https://www.lividinstruments.com */,
		 ManufacturerIdFirstActMedia 	 = 0x00000162 /* relinquished / First Act / 745 Media  */,
		 ManufacturerIdPygraphicsInc 	 = 0x00000163 /* current / Pygraphics, Inc. http://www.pyware.com */,
		 ManufacturerIdPanadigmInnovationsLtd 	 = 0x00000164 /* relinquished / Panadigm Innovations Ltd  */,
		 ManufacturerIdAvedisZildjianCo 	 = 0x00000165 /* current / Avedis Zildjian Co https://zildjian.com/gen16-overview */,
		 ManufacturerIdSourceAudioLLC 	 = 0x0000016C /* current / Source Audio LLC http://www.sourceaudio.net */,
		 ManufacturerIdErnieBallMusicMan 	 = 0x0000016D /* relinquished / Ernie Ball / Music Man  */,
		 ManufacturerIdKilpatrickAudio 	 = 0x00000172 /* current / Kilpatrick Audio http://www.kilpatrickaudio.com */,
		 ManufacturerIdFractalAudio 	 = 0x00000174 /* current / Fractal Audio  */,
		 ManufacturerIdNetLogicMicrosystems 	 = 0x00000175 /* relinquished / NetLogic Microsystems  */,
		 ManufacturerIdMusicComputing 	 = 0x00000176 /* relinquished / Music Computing  */,
		 ManufacturerIdZenphSoundInnovations 	 = 0x00000178 /* relinquished / Zenph Sound Innovations  */,
		 ManufacturerIdSynclavierDigital 	 = 0x00000204 /* relinquished / Synclavier Digital  */,
		 ManufacturerIdJSTechnologies 	 = 0x00000207 /* current / JS Technologies http://www.suhr.com */,
		 ManufacturerIdKeithRobertMurray 	 = 0x0000020C /* current / Keith Robert Murray http://www.keithrobertmurray.com */,
		 ManufacturerIdISPTechnologies 	 = 0x0000020E /* current / ISP Technologies http://www.isptechnologies.com */,
		 ManufacturerIdMerisLLC 	 = 0x00000210 /* current / Meris LLC http://www.meris.us */,
		 ManufacturerIdHiZLabs 	 = 0x00000212 /* current / Hi-Z Labs http://hizlabs.com */,
		 ManufacturerIdIntellijelDesignsInc 	 = 0x00000214 /* current / Intellijel Designs Inc. http://www.intellijel.com */,
		 ManufacturerIdDisasterAreaDesignsLLC 	 = 0x00000217 /* current / Disaster Area Designs LLC http://www.disasterareadesigns.com */,
		 ManufacturerIdCarterDuncanCorp 	 = 0x00000219 /* current / Carter Duncan Corp http://www.seymourduncan.com */,
		 ManufacturerIdEssentialTechnology 	 = 0x0000021A /* current / Essential Technology http://www.opus-two.ca */,
		 ManufacturerIdCantuxResearchLLC 	 = 0x0000021B /* current / Cantux Research LLC http://www.cantux.com */,
		 ManufacturerIdHummelTechnologies 	 = 0x0000021C /* current / Hummel Technologies  */,
		 ManufacturerIdSenselInc 	 = 0x0000021D /* current / Sensel Inc http://www.sensel.com */,
		 ManufacturerIdDBMLGroup 	 = 0x0000021E /* relinquished / DBML Group  */,
		 ManufacturerIdMesaBoogie 	 = 0x00000220 /* current / Mesa Boogie http://www.mesaboogie.com */,
		 ManufacturerIdEffigyLabs 	 = 0x00000221 /* current / Effigy Labs http://www.effigylabs.com */,
		 ManufacturerIdRedPandaLLC 	 = 0x00000223 /* current / Red Panda LLC http://redpandalab.com */,
		 ManufacturerIdJamboxxInc 	 = 0x00000225 /* current / Jamboxx Inc. http://www.jamboxx.com */,
		 ManufacturerIdElectroHarmonixnbsp 	 = 0x00000226 /* current / Electro-Harmonix&nbsp; https://www.ehx.com/ */,
		 ManufacturerIdRnDInc 	 = 0x00000227 /* current / RnD64 Inc http://www.rnd64.com */,
		 ManufacturerIdKaomInc 	 = 0x00000229 /* current / Kaom Inc. http://www.stepaudio.net */,
		 ManufacturerIdHallowellEMC 	 = 0x0000022A /* current / Hallowell EMC http://www.hallowell.com */,
		 ManufacturerIdSoundDevicesLLC 	 = 0x0000022B /* current / Sound Devices, LLC http://www.sounddevices.com */,
		 ManufacturerIdSpectrasonicsInc 	 = 0x0000022C /* current / Spectrasonics, Inc https://www.spectrasonics.net */,
		 ManufacturerIdSecondSoundLLC 	 = 0x0000022D /* current / Second Sound, LLC https://www.secondsound.com */,
		 ManufacturerIdeoHorn 	 = 0x0000022E /* &nbsp;current / 8eo (Horn) https://horn.co */,
		 ManufacturerIdDreamSAS 	 = 0x00002000 /* --- / Dream SAS  */,
		 ManufacturerIdStrandLighting 	 = 0x00002001 /* --- / Strand Lighting  */,
		 ManufacturerIdAmekDivofHarmanIndustries 	 = 0x00002002 /* --- / Amek Div of Harman Industries  */,
		 ManufacturerIdCasaDiRisparmioDiLoreto 	 = 0x00002003 /* rescinded / Casa Di Risparmio Di Loreto  */,
		 ManufacturerIdBhmelectronicGmbH 	 = 0x00002004 /* --- / Böhm electronic GmbH  */,
		 ManufacturerIdSyntecDigitalAudio 	 = 0x00002005 /* rescinded / Syntec Digital Audio  */,
		 ManufacturerIdTridentAudioDevelopments 	 = 0x00002006 /* --- / Trident Audio Developments  */,
		 ManufacturerIdRealWorldStudio 	 = 0x00002007 /* --- / Real World Studio  */,
		 ManufacturerIdEvolutionSynthesisLtd 	 = 0x00002008 /* rescinded / Evolution Synthesis, Ltd  */,
		 ManufacturerIdYesTechnology 	 = 0x00002009 /* --- / Yes Technology  */,
		 ManufacturerIdAudiomatica 	 = 0x0000200A /* --- / Audiomatica  */,
		 ManufacturerIdBontempiSpASigma 	 = 0x0000200B /* --- / Bontempi SpA (Sigma)  */,
		 ManufacturerIdFBTElettronicaSpA 	 = 0x0000200C /* --- / F.B.T. Elettronica SpA  */,
		 ManufacturerIdMidiTempGmbH 	 = 0x0000200D /* --- / MidiTemp GmbH  */,
		 ManufacturerIdLAAudioLarkingAudio 	 = 0x0000200E /* --- / LA Audio (Larking Audio)  */,
		 ManufacturerIdZeroLightingLimited 	 = 0x0000200F /* --- / Zero 88 Lighting Limited  */,
		 ManufacturerIdMiconAudioElectronicsGmbH 	 = 0x00002010 /* --- / Micon Audio Electronics GmbH  */,
		 ManufacturerIdForefrontTechnology 	 = 0x00002011 /* --- / Forefront Technology  */,
		 ManufacturerIdStudioAudioandVideoLtd 	 = 0x00002012 /* rescinded / Studio Audio and Video Ltd.  */,
		 ManufacturerIdKentonElectronics 	 = 0x00002013 /* --- / Kenton Electronics  */,
		 ManufacturerIdCelcoElectrosonic 	 = 0x00002014 /* rescinded / Celco/ Electrosonic  */,
		 ManufacturerIdADB 	 = 0x00002015 /* --- / ADB  */,
		 ManufacturerIdMarshallProductsLimited 	 = 0x00002016 /* --- / Marshall Products Limited  */,
		 ManufacturerIdDDA 	 = 0x00002017 /* --- / DDA  */,
		 ManufacturerIdBSSAudioLtd 	 = 0x00002018 /* --- / BSS Audio Ltd.  */,
		 ManufacturerIdMALightingTechnology 	 = 0x00002019 /* --- / MA Lighting Technology  */,
		 ManufacturerIdFatarSRLcoMusicIndustries 	 = 0x0000201A /* --- / Fatar SRL c/o Music Industries  */,
		 ManufacturerIdQSCAudioProductsInc 	 = 0x0000201B /* rescinded / QSC Audio Products Inc.  */,
		 ManufacturerIdArtisanClasicOrganInc 	 = 0x0000201C /* --- / Artisan Clasic Organ Inc.  */,
		 ManufacturerIdOrlaSpa 	 = 0x0000201D /* --- / Orla Spa  */,
		 ManufacturerIdPinnacleAudioKlarkTeknikPLC 	 = 0x0000201E /* --- / Pinnacle Audio (Klark Teknik PLC)  */,
		 ManufacturerIdTCElectronics 	 = 0x0000201F /* --- / TC Electronics  */,
		 ManufacturerIdDoepferMusikelektronikGmbH 	 = 0x00002020 /* --- / Doepfer Musikelektronik GmbH  */,
		 ManufacturerIdCreativeATCEmu 	 = 0x00002021 /* --- / Creative ATC / E-mu  */,
		 ManufacturerIdSeyddoMinami 	 = 0x00002022 /* --- / Seyddo/Minami  */,
		 ManufacturerIdLGElectronicsGoldstar 	 = 0x00002023 /* --- / LG Electronics (Goldstar)  */,
		 ManufacturerIdMidisoftsasdiMCimaampC 	 = 0x00002024 /* --- / Midisoft sas di M.Cima &amp; C  */,
		 ManufacturerIdSamickMusicalInstCoLtd 	 = 0x00002025 /* --- / Samick Musical Inst. Co. Ltd.  */,
		 ManufacturerIdPennyandGilesBowthorpePLC 	 = 0x00002026 /* --- / Penny and Giles (Bowthorpe PLC)  */,
		 ManufacturerIdAcornComputer 	 = 0x00002027 /* --- / Acorn Computer  */,
		 ManufacturerIdLSCElectronicsPtyLtd 	 = 0x00002028 /* --- / LSC Electronics Pty. Ltd.  */,
		 ManufacturerIdFocusriteNovation 	 = 0x00002029 /* current / Focusrite/Novation https://novationmusic.com */,
		 ManufacturerIdSamkyungMechatronics 	 = 0x0000202A /* --- / Samkyung Mechatronics  */,
		 ManufacturerIdCharlieLabSRL 	 = 0x0000202C /* --- / Charlie Lab SRL  */,
		 ManufacturerIdBlueChipMusicTechnology 	 = 0x0000202D /* --- / Blue Chip Music Technology  */,
		 ManufacturerIdBEEOHCorp 	 = 0x0000202E /* --- / BEE OH Corp  */,
		 ManufacturerIdLGSemiconAmerica 	 = 0x0000202F /* --- / LG Semicon America  */,
		 ManufacturerIdTESI 	 = 0x00002030 /* --- / TESI  */,
		 ManufacturerIdEMAGIC 	 = 0x00002031 /* --- / EMAGIC  */,
		 ManufacturerIdBehringerGmbH 	 = 0x00002032 /* --- / Behringer GmbH  */,
		 ManufacturerIdSynoptic 	 = 0x00002034 /* --- / Synoptic  */,
		 ManufacturerIdHanmesoft 	 = 0x00002035 /* --- / Hanmesoft  */,
		 ManufacturerIdTerratecElectronicGmbH 	 = 0x00002036 /* --- / Terratec Electronic GmbH  */,
		 ManufacturerIdProelSpA 	 = 0x00002037 /* --- / Proel SpA  */,
		 ManufacturerIdIBKMIDI 	 = 0x00002038 /* --- / IBK MIDI  */,
		 ManufacturerIdIRCAM 	 = 0x00002039 /* --- / IRCAM  */,
		 ManufacturerIdRedSoundSystemsLtd 	 = 0x0000203B /* --- / Red Sound Systems Ltd  */,
		 ManufacturerIdElektronESIAB 	 = 0x0000203C /* --- / Elektron ESI AB  */,
		 ManufacturerIdSintefexAudio 	 = 0x0000203D /* --- / Sintefex Audio  */,
		 ManufacturerIdMAMMusicandMore 	 = 0x0000203E /* --- / MAM (Music and More)  */,
		 ManufacturerIdAmsaroGmbH 	 = 0x0000203F /* --- / Amsaro GmbH  */,
		 ManufacturerIdModeMachinesTouchedBySoundGmbH 	 = 0x00002041 /* --- / Mode Machines (Touched By Sound GmbH)  */,
		 ManufacturerIdPhilReesMusicTech 	 = 0x00002043 /* --- / Phil Rees Music Tech  */,
		 ManufacturerIdKlavisTechnologies 	 = 0x00002047 /* relinquished / Klavis Technologies  */,
		 ManufacturerIdAlgorithmix 	 = 0x00002049 /* relinquished / Algorithmix  */,
		 ManufacturerIdProfessionalAudioCompany 	 = 0x0000204B /* relinquished / Professional Audio Company  */,
		 ManufacturerIdNewWaveLabsMadWaves 	 = 0x0000204C /* relinquished / NewWave Labs (MadWaves)  */,
		 ManufacturerIdNokia 	 = 0x0000204E /* relinquished / Nokia  */,
		 ManufacturerIdWaveIdea 	 = 0x0000204F /* relinquished / Wave Idea  */,
		 ManufacturerIdHartmannGmbH 	 = 0x00002050 /* relinquished / Hartmann GmbH  */,
		 ManufacturerIdComeTech 	 = 0x00002058 /* --- / 1 Come Tech  */,
		 ManufacturerIdDolbyAustraliaLake 	 = 0x0000205A /* relinquished / Dolby Australia (Lake)  */,
		 ManufacturerIdCinetixMedienundInterfaceGmbH 	 = 0x0000205D /* relinquished / Cinetix Medien und Interface GmbH  */,
		 ManufacturerIdAampGSoluzioniDigitali 	 = 0x0000205E /* relinquished / A&amp;G Soluzioni Digitali  */,
		 ManufacturerIdSequentixGmbH 	 = 0x0000205F /* current / Sequentix GmbH http://www.sequentix.com */,
		 ManufacturerIdMedialon 	 = 0x00002065 /* relinquished / Medialon  */,
		 ManufacturerIdWavesAudioLtd 	 = 0x00002066 /* relinquished / Waves Audio Ltd  */,
		 ManufacturerIdDaFact 	 = 0x00002068 /* relinquished / Da Fact  */,
		 ManufacturerIdElbyDesigns 	 = 0x00002069 /* current / Elby Designs http://www.elby-designs.com */,
		 ManufacturerIdArturia 	 = 0x0000206B /* current / Arturia http://www.arturia.com */,
		 ManufacturerIdCThruMusic 	 = 0x0000206D /* out of business / C-Thru Music  */,
		 ManufacturerIdYaHorngElectronicCoLTD 	 = 0x0000206E /* current / Ya Horng Electronic Co LTD http://www.yahorng.com/products_4573-english.html */,
		 ManufacturerIdOTOMachines 	 = 0x00002070 /* current / OTO Machines http://www.otomachines.com */,
		 ManufacturerIdMiTechnologiesGmbH 	 = 0x00002073 /* current / M3i Technologies GmbH http://www.m3i-technologies.com */,
		 ManufacturerIdProstageSL 	 = 0x00002075 /* relinquished / Prostage SL  */,
		 ManufacturerIdHanpinElectronCoLtd 	 = 0x00002079 /* relinquished / Hanpin Electron Co Ltd  */,
		 ManufacturerIdMIDIhardwareRSowa 	 = 0x0000207A /* current / "MIDI-hardware" R.Sowa http://www.midi-hardware.com */,
		 ManufacturerIdMisaDigitalTechnologiesLtd 	 = 0x0000207D /* relinquished / Misa Digital Technologies Ltd  */,
		 ManufacturerIdLimex 	 = 0x00002100 /* current / Limex http://www.limex.eu/en/ */,
		 ManufacturerIdMutableInstruments 	 = 0x00002102 /* current / Mutable Instruments http://mutable-instruments.net/ */,
		 ManufacturerIdPreSonusSoftwareLtd 	 = 0x00002103 /* relinquished / PreSonus Software Ltd  */,
		 ManufacturerIdIngenicowasXiring 	 = 0x00002104 /* relinquished / Ingenico (was Xiring)  */,
		 ManufacturerIdNativeInstruments 	 = 0x00002109 /* current / Native Instruments http://www.native-instruments.de */,
		 ManufacturerIdPloytecGmbH 	 = 0x0000210D /* current / Ploytec GmbH http://www.ploytec.com */,
		 ManufacturerIdSurfinKangarooStudio 	 = 0x0000210E /* current / Surfin Kangaroo Studio http://www.surfin-kangaroo-studio.de */,
		 ManufacturerIdPandaAudioLtd 	 = 0x00002111 /* current / Panda-Audio Ltd http://www.panda-audio.com */,
		 ManufacturerIdBauMSoftware 	 = 0x00002112 /* current / BauM Software http://www.baum-software.ch */,
		 ManufacturerIdKiwitechnicsLtd 	 = 0x00002116 /* current / Kiwitechnics Ltd http://www.kiwitechnics.com */,
		 ManufacturerIdRobPapen 	 = 0x00002117 /* relinquished / Rob Papen http://www.robpapen.com */,
		 ManufacturerIdModorMusic 	 = 0x0000211C /* current / Modor Music http://www.modormusic.com */,
		 ManufacturerIdAbleton 	 = 0x0000211D /* current / Ableton http://www.ableton.com */,
		 ManufacturerIdDtronics 	 = 0x0000211E /* current / Dtronics http://www.dtronics.nl */,
		 ManufacturerIdZAQAudio 	 = 0x0000211F /* current / ZAQ Audio http://zaqaudio.com */,
		 ManufacturerIdMuabaobaoEducationTechnologyCoLtd 	 = 0x00002120 /* out of business / Muabaobao Education Technology Co Ltd  */,
		 ManufacturerIdFluxEffects 	 = 0x00002121 /* current / Flux Effects http://fluxeffects.com */,
		 ManufacturerIdRetrokits 	 = 0x00002123 /* current / Retrokits https://www.retrokits.com/ */,
		 ManufacturerIdChangshaHotoneAudioCoLtd 	 = 0x00002125 /* current / Changsha Hotone Audio Co Ltd http://www.hotoneaudio.com */,
		 ManufacturerIdExpressiveE 	 = 0x00002126 /* current / Expressive E http://www.expressivee.com */,
		 ManufacturerIdExpertSleepersLtd 	 = 0x00002127 /* current / Expert Sleepers Ltd http://www.expert-sleepers.co.uk */,
		 ManufacturerIdHornbergResearchGbR 	 = 0x00002129 /* current / Hornberg Research GbR http://www.hornberg-research.de */,
		 ManufacturerIdAudiofront 	 = 0x0000212B /* current / Audiofront http://audiofront.net */,
		 ManufacturerIdAudioModeling 	 = 0x0000212D /* current / Audio Modeling http://www.audiomodeling.com */,
		 ManufacturerIdCBechsteinDigitalGmbH 	 = 0x0000212E /* current / C. Bechstein Digital GmbH http://www.bechstein-digital.com */,
		 ManufacturerIdMotasElectronicsLtd 	 = 0x0000212F /* current / Motas Electronics Ltd http://www.motas-synth.uk */,
		 ManufacturerIdAODYOSAS 	 = 0x00002133 /* current / AODYO SAS https://www.aodyo.com */,
		 ManufacturerIdPianoforceSRO 	 = 0x00002134 /* current / Pianoforce S.R.O https://www.pianoforce.eu */,
		 ManufacturerIdDreadboxPC 	 = 0x00002135 /* current / Dreadbox P.C. https://www.dreadbox-fx.com */,
		 ManufacturerIdTouchKeysInstrumentsLtd 	 = 0x00002136 /* current&nbsp; / TouchKeys Instruments Ltd https://touchkeys.co.uk */,
		 ManufacturerIdTheGigrigLtd 	 = 0x00002137 /* current / The Gigrig Ltd https://www.thegigrig.com */,
		 ManufacturerIdALMCo 	 = 0x00002138 /* current / ALM Co http://busycircuits.com */,
		 ManufacturerIdCHSoundDesign 	 = 0x00002139 /* current / CH Sound Design http://www.chsounddesign.com */,
		 ManufacturerIdBeatBars 	 = 0x0000213A /* current / Beat Bars http://www.beatbars.com */,
		 ManufacturerIdBlokas 	 = 0x0000213B /* current / Blokas https://blokas.io */,
		 ManufacturerIdGEWAMusicGmbH 	 = 0x0000213C /* current / GEWA Music GmbH https://gewamusic.com */,
		 ManufacturerIddadamachines 	 = 0x0000213D /* current / dadamachines https://dadamachines.com */,
		 ManufacturerIdAugmentedInstrumentsLtdBela 	 = 0x0000213E /* current / Augmented Instruments Ltd (Bela) https://bela.io */,
		 ManufacturerIdSupercriticalLtd 	 = 0x0000213F /* current / Supercritical Ltd https://supercriticalsynthesizers.com */,
		 ManufacturerIdGenkiInstruments 	 = 0x00002140 /* current&nbsp; / Genki Instruments https://genkiinstruments.com */,
		 ManufacturerIdMarienbergDevicesGermany 	 = 0x00002141 /* current / Marienberg Devices Germany https://marienbergdevices.de */,
		 ManufacturerIdSupperwareLtd 	 = 0x00002142 /* current&nbsp; / Supperware Ltd https://supperware.net */,
		 ManufacturerIdPending 	 = 0x00002143 /* current / Pending  */,
		 ManufacturerIdSequentialCircuits 	 = 0x00010000 /* current / Sequential Circuits https://www.sequential.com */,
		 ManufacturerIdIDP 	 = 0x00020000 /* --- / IDP  */,
		 ManufacturerIdVoyetraTurtleBeachInc 	 = 0x00030000 /* --- / Voyetra Turtle Beach, Inc.  */,
		 ManufacturerIdMoogMusic 	 = 0x00040000 /* current / Moog Music https://www.moogmusic.com */,
		 ManufacturerIdPassportDesigns 	 = 0x00050000 /* --- / Passport Designs  */,
		 ManufacturerIdLexiconInc 	 = 0x00060000 /* --- / Lexicon Inc.  */,
		 ManufacturerIdKurzweilYoungChang 	 = 0x00070000 /* current / Kurzweil / Young Chang https://kurzweil.com */,
		 ManufacturerIdFender 	 = 0x00080000 /* --- / Fender  */,
		 ManufacturerIdMIDI 	 = 0x00090000 /* current / MIDI9 https://www.midi9.com */,
		 ManufacturerIdAKGAcoustics 	 = 0x000A0000 /* --- / AKG Acoustics  */,
		 ManufacturerIdVoyceMusic 	 = 0x000B0000 /* --- / Voyce Music  */,
		 ManufacturerIdWaveFrameTimeline 	 = 0x000C0000 /* --- / WaveFrame (Timeline)  */,
		 ManufacturerIdADASignalProcessorsInc 	 = 0x000D0000 /* --- / ADA Signal Processors, Inc.  */,
		 ManufacturerIdGarfieldElectronics 	 = 0x000E0000 /* --- / Garfield Electronics  */,
		 ManufacturerIdEnsoniq 	 = 0x000F0000 /* --- / Ensoniq  */,
		 ManufacturerIdOberheimGibsonLabs 	 = 0x00100000 /* --- / Oberheim / Gibson Labs  */,
		 ManufacturerIdApple 	 = 0x00110000 /* current / Apple https://www.apple.com */,
		 ManufacturerIdGreyMatterResponse 	 = 0x00120000 /* --- / Grey Matter Response  */,
		 ManufacturerIdDigidesignInc 	 = 0x00130000 /* --- / Digidesign Inc.  */,
		 ManufacturerIdPalmtreeInstruments 	 = 0x00140000 /* --- / Palmtree Instruments  */,
		 ManufacturerIdJLCooperElectronics 	 = 0x00150000 /* --- / JLCooper Electronics  */,
		 ManufacturerIdLowreyOrganCompany 	 = 0x00160000 /* --- / Lowrey Organ Company  */,
		 ManufacturerIdAdamsSmith 	 = 0x00170000 /* --- / Adams-Smith  */,
		 ManufacturerIdEmu 	 = 0x00180000 /* --- / E-mu  */,
		 ManufacturerIdHarmonySystems 	 = 0x00190000 /* --- / Harmony Systems  */,
		 ManufacturerIdART 	 = 0x001A0000 /* --- / ART  */,
		 ManufacturerIdBaldwin 	 = 0x001B0000 /* --- / Baldwin  */,
		 ManufacturerIdEventide 	 = 0x001C0000 /* --- / Eventide  */,
		 ManufacturerIdInventronics 	 = 0x001D0000 /* --- / Inventronics  */,
		 ManufacturerIdKeyConcepts 	 = 0x001E0000 /* rescinded / Key Concepts  */,
		 ManufacturerIdClarity 	 = 0x001F0000 /* --- / Clarity  */,
		 ManufacturerIdPassac 	 = 0x00200000 /* --- / Passac  */,
		 ManufacturerIdProelLabsSIEL 	 = 0x00210000 /* --- / Proel Labs (SIEL)  */,
		 ManufacturerIdSynthaxeUK 	 = 0x00220000 /* --- / Synthaxe (UK)  */,
		 ManufacturerIdStepp 	 = 0x00230000 /* rescinded / Stepp  */,
		 ManufacturerIdHohner 	 = 0x00240000 /* --- / Hohner  */,
		 ManufacturerIdTwister 	 = 0x00250000 /* --- / Twister  */,
		 ManufacturerIdKetronsrl 	 = 0x00260000 /* current / Ketron s.r.l. https://www.ketron.it */,
		 ManufacturerIdJellinghausMS 	 = 0x00270000 /* --- / Jellinghaus MS  */,
		 ManufacturerIdSouthworthMusicSystems 	 = 0x00280000 /* --- / Southworth Music Systems  */,
		 ManufacturerIdPPGGermany 	 = 0x00290000 /* --- / PPG (Germany)  */,
		 ManufacturerIdJEN 	 = 0x002A0000 /* --- / JEN  */,
		 ManufacturerIdSolidStateLogicOrganSystems 	 = 0x002B0000 /* --- / Solid State Logic Organ Systems  */,
		 ManufacturerIdAudioVeritriebPStruven 	 = 0x002C0000 /* --- / Audio Veritrieb-P. Struven  */,
		 ManufacturerIdNeve 	 = 0x002D0000 /* rescinded / Neve  */,
		 ManufacturerIdSoundtracsLtd 	 = 0x002E0000 /* rescinded / Soundtracs Ltd.  */,
		 ManufacturerIdElka 	 = 0x002F0000 /* --- / Elka  */,
		 ManufacturerIdDynacord 	 = 0x00300000 /* --- / Dynacord  */,
		 ManufacturerIdDrawmer 	 = 0x00320000 /* rescinded / Drawmer  */,
		 ManufacturerIdClaviaDigitalInstruments 	 = 0x00330000 /* --- / Clavia Digital Instruments  */,
		 ManufacturerIdAudioArchitecture 	 = 0x00340000 /* --- / Audio Architecture  */,
		 ManufacturerIdGeneralmusicCorpSpA 	 = 0x00350000 /* --- / Generalmusic Corp SpA  */,
		 ManufacturerIdCheetahMarketing 	 = 0x00360000 /* rescinded / Cheetah Marketing  */,
		 ManufacturerIdCTM 	 = 0x00370000 /* rescinded / C.T.M.  */,
		 ManufacturerIdSimmonsUK 	 = 0x00380000 /* rescinded / Simmons UK  */,
		 ManufacturerIdSoundcraftElectronics 	 = 0x00390000 /* --- / Soundcraft Electronics  */,
		 ManufacturerIdSteinbergMediaTechnologiesGmbH 	 = 0x003A0000 /* current / Steinberg Media Technologies GmbH https://steinberg.de */,
		 ManufacturerIdWersiGmbh 	 = 0x003B0000 /* --- / Wersi Gmbh  */,
		 ManufacturerIdAVABNiethammerAB 	 = 0x003C0000 /* --- / AVAB Niethammer AB  */,
		 ManufacturerIdDigigram 	 = 0x003D0000 /* --- / Digigram  */,
		 ManufacturerIdWaldorfElectronicsGmbH 	 = 0x003E0000 /* --- / Waldorf Electronics GmbH  */,
		 ManufacturerIdQuasimidi 	 = 0x003F0000 /* --- / Quasimidi  */

    } ManufacturerId_t;

}

#endif //MIDIMESSAGE_MANUFACTURERIDS_H
