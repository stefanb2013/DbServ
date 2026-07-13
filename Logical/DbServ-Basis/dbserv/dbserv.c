#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  DbServ
*  Teil:     Prozessabbild-Aufbau
*  Datei:    dbserv.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.03
*  Datum:    26-07-2004
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Task-Source-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.03 | 26-07-04 | MKrz | Nichtangabe von Item.Name für nicht verwendete Fubinstanzen
*        |          |      | führt zu keinen Logbucheinträgen;
*  01.02 | 15-07-04 | MKrz | Fehlerbeseitigung bei Logbuch-Fehlereintrag;
*  01.01 | 06-07-04 | MKrz | Fehlerbeseitigung Eventblockverwaltung;
*        |          |      | Parserfunktion für Timeout, Port, Client.Anz, App.Anz
*  01.00 | 04-08-03 | MKrz | Datei-Erstellung
*  02.00 | 10-07-26 | BIE  | strnicmp gegen strncasecmp ersetzt
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/
#include <bur\plc.h>		/* RPS-spezifische Typendeklarationen */
#include <sys_lib.h>		/* System-Library */

#include <dataobj.h>
#include <fileio.h>
#include <brsystem.h>

#include <string.h>			/* String-Library */
#include <stdlib.h>			/* String-Library */

#include "platform.h"		/* CPU-Plattform */

#include "bksys.h"			/* Burklimat-Library */

#include "parser.h"			/* Parser-Library */
#include "dbserv.h"			/* BkTcp-Library */

#include <dbslib.h>

//unsigned short itoa(signed long value, unsigned long pString);

/***********************************************************************/
/* Variablendeklaration                                                */
/***********************************************************************/

/**** globale Variablen ************************************************/

/**** lokale Variablen *************************************************/

/* Anzahl Fehlerzaehler fuer ErrStatistics */
/*#define ERRCNTANZ				300*/

/* Struktur fuer Fehler-Statistik */
/*typedef struct {
	unsigned short	LastWarn;
	unsigned short	LastErr;
	unsigned short	Init;
	Timestamp_Typ	LastInit;
	unsigned short	ErrCount[ERRCNTANZ];
} Stats_Typ;*/

_LOCAL_RETAIN Stats_Typ		SYS_Statistic	;

_LOCAL_RETAIN	unsigned short	ParserState	;

_LOCAL_RETAIN	BIT				DbsTypeChanged, DbsPointChanged	;
		void  			*pDA_Data;
_LOCAL_RETAIN	UDINT			SizeDA_Data	;

_LOCAL	DatObjInfo_typ		iDatObjInfo;
_LOCAL	DatObjDelete_typ	iDatObjDelete;
_LOCAL	DatObjCreate_typ	iDatObjCreate;

_LOCAL_RETAIN	unsigned short	CountLines	;
_LOCAL_RETAIN	signed short	State_find_StationTypPoint, State_find_Section, State_find_Data	;
		short			StateGetDpInfo;

		unsigned char	*pParser, *pParserLine, *pParserErr;
		unsigned short	SectionType;
		unsigned char	*pDataArg1, *pDataArg2, *pDataArg3, *pTempStation, *pTempSection, *pTempPoint, *pTempData, *pDbsTypeFileBufferSize, *pDbsTypeFileBufferMode;
		unsigned short	SizeArg1, SizeArg2, SizeArg3 ;

_LOCAL	unsigned short	PortNumber, TimeoutValue;
_LOCAL	long			BufEvents, BufType;

		unsigned char	flagDbsUnit, flagDbsType, flagDbsPoint, flagDbsFub, flag, flagDbsExistPoints, flagDbsNotExistPoints, flagDbsFubInstance;
_LOCAL_RETAIN	unsigned short	maxDbsUnit, maxUsedUnit, DbsUnit, DbsType, maxDbsType, maxUsedType, DbsPoint, DbsItem	;
_LOCAL	unsigned char	DbsMemTypeData, DbsMemTypeEvent;
_LOCAL_RETAIN	unsigned long	memSize, SmemSize, StmpSize	;
		unsigned short	StateTMP_alloc;

_LOCAL_RETAIN	unsigned short	countStationTypPoint, countSection, countItem, countTimeItem	;
_LOCAL	unsigned short	countDefinedType, countDefinedPoint, countDefinedItem, countPortData, countTimeoutData, countEventData, countTypeData, countUnitData, countPointData	;
_LOCAL	unsigned short	countFub, countData, countExistPoints, countNotExistPoints, countNotDefinedItems, countInvalidExistItems, countNotExistItems, countLogMessages, countFubInstance	;
_LOCAL_RETAIN	unsigned short	countEventInstance, countWriteInstance	;

_LOCAL	unsigned short	SizeNameType, SizeNamePoint, SizeNameItem, sizeType, lenItem	;
_LOCAL_RETAIN	unsigned char	sItem, idTypeItem	;

		char			logBuffer[128];
		unsigned short	sMsgMaxLen;
_LOCAL_RETAIN	short			SwVersion	;

		unsigned short	i, j;
		unsigned char	DtIdx;
		unsigned short	DpIdx;

_LOCAL_RETAIN	unsigned char	go	;
_LOCAL_RETAIN	short			AnzWaitCycleDbs	;
		unsigned char	ErrStatisticsYField[EC_MAX / 8 + 1];

_LOCAL	TYPE_DESC		*pTypeDesc, *pWorkTypeDesc, *pInfoTypeDesc;
_LOCAL	ITEM_DESC		*pItemDesc, *pWorkItemDesc, *pInfoItemDesc;
_LOCAL	POINT_DESC		*pPointDesc, *pWorkPointDesc, *pInfoPointDesc;
_LOCAL	FUB_DESC		*pFubInstanceDesc;
_LOCAL_RETAIN	unsigned short	infoType, infoPoint	;
_LOCAL_RETAIN	unsigned char	*pTypeName, *pWorkTypeName, *pPointName, *pWorkPointName, *pWorkItemName, InfoTypeName[16], PointName[64]	;
		unsigned short	lenTypeName, lenPointName;
		unsigned long	len;

_LOCAL	ACCESS_SERVER		*idBrServer,	*pAccessServer;
		ACCESS_UNIT			*pAccessUnit;
		ACCESS_TYPE			*pAccessType,	*pAccessTypeWork;
		ACCESS_POINT		*pAccessPoint,	*pAccessPointWork;
		ACCESS_DATA			*pAccessData,	*pAccessDataWork;
		ACCESS_FUB_INSTANCE	*pAccessFubInstance;

		ACCESS_EVENT_PIPE	*pEventPipe;
		EVENT_PIPE_INSTANCE	*pEventPipeInstance;
		EVENT_PIPE_HEADER	*pEventPipeHeader;
		ACCESS_WRITE_PIPE	*pWritePipe;
		WRITE_PIPE_HEADER	*pWritePipeHeader;

_LOCAL_RETAIN	UINT			stateFubInstance	;
_LOCAL	UDINT			pFubInstance, lenFubInstance	;

/*_LOCAL	FUB_REF			x;*/

_LOCAL_RETAIN	USINT		bValue	;
_LOCAL_RETAIN	RTCtime_typ	RTCTime	;

_LOCAL	FileCreate_typ	iFileCreate;
_LOCAL	DevMemInfo_typ	iDevMemInfo;
_LOCAL	BOOL	filetest;
_LOCAL	USINT	buffer1[32], buffer2[32];

_LOCAL	SysInfo_typ		iSysInfo;

/***********************************************************************/
/* Initialisierungs-Unterprogramm                                      */
/***********************************************************************/

void _INIT init(void)
{
	idBrServer = NULL;
	SwVersion = BK_VERSION_NR;

	SYS_Statistic = SYS_Statistic;
	
	PortNumber = cTCP_DEFAULT_PORT;
	TimeoutValue = cTCP_DEFAULT_TIMEOUT;

	BufEvents = 0;
	BufType = 0;

	ParserState = cCD_PARSER_STATE_DEF_TCP;
	memset(ErrStatisticsYField, 0, sizeof(ErrStatisticsYField));

	AnzWaitCycleDbs = 0;

	countEventInstance = 8;
	countWriteInstance = 4;

	DbsTypeChanged = TRUE;
	DbsPointChanged = TRUE;

	go = TRUE;
}

/***********************************************************************/
/* zyklisches Hauptprogramm                                            */
/***********************************************************************/
void _CYCLIC cyclic(void)
{
	if (filetest == TRUE) {
		strcpy(buffer1, "Test");
		ultoa(i, buffer2);
		strcat(buffer1, buffer2);

		iFileCreate.enable = 1;
		iFileCreate.pDevice = (unsigned long)"CompactFlash";
		iFileCreate.pFile = (unsigned long)buffer1;
		FileCreate(&iFileCreate);
		if (iFileCreate.status == 0) {
			filetest = FALSE;
			i++;
		}
		else if (iFileCreate.status != 65535) {
			filetest = FALSE;
			i++;
		}
	}

	/* ----------------- */
	/* Start-Verzögerung */
	/* ----------------- */
	if (AnzWaitCycleDbs < ANZ_WAIT_CYCLE_TCP) {
		AnzWaitCycleDbs++;
		return;
	}

	/* ------------- */
	/* Parser - Lauf */
	/* ------------- */
	if (go == TRUE) {

		/* --- */
		/* DFA */
		/* --- */
		/*while (ParserState < cCD_PARSER_STATE_READY) {*/

			/* ------------- */
			/* DFA - Zustand */
			/* ------------- */
			switch (ParserState) {

				/* ============================= */
				/* DFA-Zustand: Tcp-Definitionen */
				/* ============================= */
				case cCD_PARSER_STATE_DEF_TCP:

					iDatObjInfo.enable = TRUE;
					iDatObjInfo.pName = (UDINT)cKW_DBS_DM_DP_LIST;
					DatObjInfo(&iDatObjInfo);
					if (iDatObjInfo.status == 0) {
						pDA_Data = (unsigned char*)iDatObjInfo.pDatObjMem;
						SizeDA_Data = iDatObjInfo.len;

						/* go */

						/* Datenmodul von Anfang an Parsen ... */
						CountLines = 0;
						pParser = (unsigned char*)pDA_Data;

						/* Zähler rücksetzen */
						countPortData = 0;
						countTimeoutData = 0;
						countStationTypPoint = 0;

						/* Parser-Status loeschen */
						State_find_StationTypPoint = 0;
						State_find_Section = 0;
						State_find_Data = 0;

						/* ------ */
						/* Parsen */
						/* ------ */
						while (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {

							/* Suche nach Stations/Typ/Punkt-Beschreibung */
							State_find_StationTypPoint = find_StationTypPoint(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				&pParser, &pParserLine, &pParserErr, &CountLines,
																				&pDataArg1, &SizeArg1,
																				&pDataArg2, &SizeArg2,
																				&SectionType);
							/* keine Daten oder Fehler aufgetreten ? */
							if (State_find_StationTypPoint < 0) {
								break;
							}
							else {
								countStationTypPoint++;
							}

							/* Pointer auf Station/Typ/Punkt */
							switch (SectionType) {

								/* ------------------ */
								/* Stations-Abschnitt */
								/* ------------------ */
								case cCD_STATION:
									countSection = 0;
									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_TCP_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countSection++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										countData = 0;
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < -1) {
												break;
											}
											else if (State_find_Data == -1) {
												if (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {
													pParser++;
													State_find_Data = TRUE;
													continue;
												}
												else {
													break;
												}
											}
											else {
												/* Port-Bezeichner */
												if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
													strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countPortData++;
													PortNumber = atoi(pDataArg3);
												}
												/* Timeout-Bezeichner */
												else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
													strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countTimeoutData++;
													TimeoutValue = atoi(pDataArg3);
												}
												/* unbekannte Zuweisung */
												else {
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									break;
							} /* (SectionType) */
						} /* (pParser < (unsigned char*)pDA_Data + SizeDA_Data) */

						/* ---------------- */
						/* nächster Zustand */
						/* ---------------- */
						ParserState = cCD_PARSER_STATE_DEF_BUF;

					} /* (DA_DataOk == TRUE) */
					else {
						/* Fehler beim Datenmodul */
						ErrStatistics("dbserv", ERRLOG, EC_DA_NOT_FOUND + EC_DBS_BASE, CountLines, ERR_DBS_PRE, ET_DA_NOT_FOUND);
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ============================= */
				/* DFA-Zustand: Buf-Definitionen */
				/* ============================= */
				case cCD_PARSER_STATE_DEF_BUF:

					iDatObjInfo.enable = TRUE;
					iDatObjInfo.pName = (UDINT)cKW_DBS_DM_DP_LIST;
					DatObjInfo(&iDatObjInfo);
					if (iDatObjInfo.status == 0) {
						pDA_Data = (unsigned char*)iDatObjInfo.pDatObjMem;
						SizeDA_Data = iDatObjInfo.len;

						/* go */

						/* Datenmodul von Anfang an Parsen ... */
						CountLines = 0;
						pParser = (unsigned char*)pDA_Data;

						/* Zähler rücksetzen */
						countEventData = 0;
						countTypeData = 0;
						countStationTypPoint = 0;

						/* Parser-Status loeschen */
						State_find_StationTypPoint = 0;
						State_find_Section = 0;
						State_find_Data = 0;

						/* ------ */
						/* Parsen */
						/* ------ */
						while (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {

							/* Suche nach Stations/Typ/Punkt-Beschreibung */
							State_find_StationTypPoint = find_StationTypPoint(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				&pParser, &pParserLine, &pParserErr, &CountLines,
																				&pDataArg1, &SizeArg1,
																				&pDataArg2, &SizeArg2,
																				&SectionType);
							/* keine Daten oder Fehler aufgetreten ? */
							if (State_find_StationTypPoint < 0) {
								break;
							}
							else {
								countStationTypPoint++;
							}

							/* Pointer auf Station/Typ/Punkt */
							switch (SectionType) {

								/* ------------------ */
								/* Stations-Abschnitt */
								/* ------------------ */
								case cCD_STATION:
									countSection = 0;
									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_BUF_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countSection++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										countData = 0;
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < -1) {
												break;
											}
											else if (State_find_Data == -1) {
												if (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {
													pParser++;
													State_find_Data = TRUE;
													continue;
												}
												else {
													break;
												}
											}
											else {
												/* Events-Bezeichner */
												if (strlen(cKW_DBS_BUF_EVENTS) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_BUF_EVENTS, SizeArg1) == 0 &&
													pDataArg2 == NULL &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countEventData++;
													BufEvents = atoi(pDataArg3);
												}
												/* Typ-Bezeichner */
												else if (strlen(cKW_DBS_BUF_TYPE) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_BUF_TYPE, SizeArg2) == 0 &&
													pDataArg2 == NULL &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countTypeData++;
													BufType = atoi(pDataArg3);
												}
												/* unbekannte Zuweisung */
												else {
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									break;
							} /* (SectionType) */
						} /* (pParser < (unsigned char*)pDA_Data + SizeDA_Data) */

						/* ---------------- */
						/* nächster Zustand */
						/* ---------------- */
						ParserState = cCD_PARSER_STATE_COUNT_TYPE;

					} /* (DA_DataOk == TRUE) */
					else {
						/* Fehler beim Datenmodul */
						ErrStatistics("dbserv", ERRLOG, EC_DA_NOT_FOUND + EC_DBS_BASE, CountLines, ERR_DBS_PRE, ET_DA_NOT_FOUND);
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ====================================== */
				/* DFA-Zustand: Zählen der DBS-Datentypen */
				/* ====================================== */
				case cCD_PARSER_STATE_COUNT_TYPE:

					idBrServer = NULL;

					iDatObjInfo.enable = TRUE;
					iDatObjInfo.pName = (UDINT)cKW_DBS_DM_TYP_LIST;
					DatObjInfo(&iDatObjInfo);
					if (iDatObjInfo.status == 0) {
						pDA_Data = (unsigned char*)iDatObjInfo.pDatObjMem;
						SizeDA_Data = iDatObjInfo.len;

						SmemSize = 0, StmpSize = 0;

						/* go */

						/* Datenmodul von Anfang an Parsen ... */
						CountLines = 0;
						pParser = (unsigned char*)pDA_Data;

						/* Zähler rücksetzen */
						countDefinedType = 0;
						countDefinedItem = 0;
						countPortData = 0;
						countTimeoutData = 0;
						countExistPoints = 0;
						countNotExistPoints = 0;
						countStationTypPoint = 0;
						countLogMessages = 0;

						SizeNameType = 0, SizeNameItem = 0;

						/* Parser-Status loeschen */
						State_find_StationTypPoint = 0;
						State_find_Section = 0;
						State_find_Data = 0;

						/* ------ */
						/* Parsen */
						/* ------ */
						while (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {

							/* Suche nach Stations/Typ/Punkt-Beschreibung */
							State_find_StationTypPoint = find_StationTypPoint(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				&pParser, &pParserLine, &pParserErr, &CountLines,
																				&pDataArg1, &SizeArg1,
																				&pDataArg2, &SizeArg2,
																				&SectionType);
							/* keine Daten oder Fehler aufgetreten ? */
							if (State_find_StationTypPoint < 0) {
								break;
							}
							else {
								countStationTypPoint++;
							}

							/* Pointer auf Station/Typ/Punkt */
							switch (SectionType) {

								/* ------------- */
								/* Typ-Abschnitt */
								/* ------------- */
								case cCD_TYP:

									countDefinedType++;
									SizeNameType += SizeArg1 + 1;
									countItem = 0, sizeType = 0;
									countSection = 0;
									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_ALL_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countSection++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										countData = 0;
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < 0) {
												break;
											}
											else {
												/* Typ-Bezeichner */
												if (strlen(cKW_DBS_TYPE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0 &&
													strlen(cKW_DBS_TYPE_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_TYPE_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													if (atoi(pDataArg3) >= 0 && atoi(pDataArg3) <= cMAX_TYPE_DEF) {
														DbsType = atoi(pDataArg3);
														maxDbsType = (maxDbsType > DbsType) ? maxDbsType : DbsType;
													}
													else {
													}
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									break;

								/* -------------- */
								/* Item-Abschnitt */
								/* -------------- */
								case cCD_ITEM:

									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_ALL_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countItem++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										countData = 0;
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < 0) {
												break;
											}
											else {
												/* Typ-Bezeichner */
												if (strlen(cKW_DBS_TYPE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0) {
													/* BOOL */
													if (strlen(cKW_DBS_TYPE_BOOL) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_BOOL, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(BOOL);
													}
													/* USINT */
													else if (strlen(cKW_DBS_TYPE_USINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_USINT, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(USINT);
													}
													/* SINT */
													else if (strlen(cKW_DBS_TYPE_SINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_SINT, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(SINT);
													}
													/* UINT */
													else if (strlen(cKW_DBS_TYPE_UINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_UINT, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(UINT);
													}
													/* INT */
													else if (strlen(cKW_DBS_TYPE_INT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_INT, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(INT);
													}
													/* UDINT */
													else if (strlen(cKW_DBS_TYPE_UDINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_UDINT, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(UDINT);
													}
													/* DINT */
													else if (strlen(cKW_DBS_TYPE_DINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_DINT, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += sizeof(DINT);
													}
													/* STREAM */
													else if (strlen(cKW_DBS_TYPE_STREAM) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_STREAM, SizeArg3) == 0) {
														countDefinedItem++;
														sizeType += 2 * sizeof(UDINT);
													}
													/* sonst */
													else {
														continue;
													}
												}
												/* Zeitstempel-Bezeichner */
												else if (strlen(cKW_DBS_TYPE_TIMESTAMP) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE_TIMESTAMP, SizeArg1) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
												}
												/* Item-Bezeichner */
												else if (strlen(cKW_DBS_ITEM) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_ITEM, SizeArg1) == 0 &&
													strlen(cKW_DBS_ITEM_NAME) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_ITEM_NAME, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0)
												{
													SizeNameItem += SizeArg3 + 1;
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									break;

							} /* (SectionType) */
						} /* (pParser < (unsigned char*)pDA_Data + SizeDA_Data) */

						/* ---------------- */
						/* nächster Zustand */
						/* ---------------- */
						ParserState = cCD_PARSER_STATE_DEF_TYPE;

					} /* (DA_DataOk == TRUE) */
					else {
						/* Fehler beim Datenmodul */
						ErrStatistics("dbserv", ERRLOG, EC_DA_NOT_FOUND + EC_DBS_BASE, CountLines, ERR_DBS_PRE, ET_DA_NOT_FOUND);
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ==================================================================== */
				/* DFA-Zustand: Speicher allokieren und Aufbau der Bäume für Datentypen */
				/* ==================================================================== */
				case cCD_PARSER_STATE_DEF_TYPE:
					memSize = (maxDbsType + 1) * sizeof(TYPE_DESC)
							+ countDefinedItem * sizeof(ITEM_DESC)
							+ SizeNameType
							+ SizeNameItem;
					if (memSize > 0) {
						StateTMP_alloc = TMP_alloc(memSize, (void*)&pTypeDesc);
						if (StateTMP_alloc == 0) {
							SmemSize += 0;
							StmpSize += memSize;
							memset(pTypeDesc, 0, memSize);

							/* initialisieren */
							for (i=0; i<=maxDbsType; i++) {
								pTypeDesc[i].dbsType--;
							}

							pItemDesc = (ITEM_DESC*)(pTypeDesc + (maxDbsType + 1));
							pWorkItemDesc = pItemDesc;
							pWorkTypeName = (unsigned char*)(pItemDesc + countDefinedItem);
							pWorkItemName = pWorkTypeName + SizeNameType;

							/* Datenmodul von Anfang an Parsen ... */
							CountLines = 0;
							pParser = (unsigned char*)pDA_Data;

							/* Zähler rücksetzen */
							countDefinedType = 0;
							countDefinedItem = 0;
							countPortData = 0;
							countTimeoutData = 0;
							countExistPoints = 0;
							countNotExistPoints = 0;
							countStationTypPoint = 0;
							countLogMessages = 0;

							SizeNameType = 0;

							/* Parser-Status loeschen */
							State_find_StationTypPoint = 0;
							State_find_Section = 0;
							State_find_Data = 0;

							/* ------ */
							/* Parsen */
							/* ------ */
							while (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {

								/* Suche nach Stations/Typ/Punkt-Beschreibung */
								State_find_StationTypPoint = find_StationTypPoint(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																					&pParser, &pParserLine, &pParserErr, &CountLines,
																					&pDataArg1, &SizeArg1,
																					&pDataArg2, &SizeArg2,
																					&SectionType);
								/* keine Daten oder Fehler aufgetreten ? */
								if (State_find_StationTypPoint < 0) {
									break;
								}
								else {
									countStationTypPoint++;
								}

								/* Pointer auf Station/Typ/Punkt */
								switch (SectionType) {

									/* ------------- */
									/* Typ-Abschnitt */
									/* ------------- */
									case cCD_TYP:

										countDefinedType++;
										SizeNameType += SizeArg1 + 1;
										pTypeName = pDataArg1, lenTypeName = SizeArg1;
										DbsItem = 0;
										countItem = 0, sizeType = 0;
										countSection = 0;

										pDbsTypeFileBufferSize = NULL, pDbsTypeFileBufferMode = NULL;

										do {
											State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				&pParser, &pParserLine, &pParserErr, &CountLines,
																				&pDataArg1, &SizeArg1,
																				&pDataArg2, &SizeArg2,
																				cKW_ALL_SECTION);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Section < 0) {
												break;
											}
											else {
												countSection++;
											}

											/* Pointer auf Anfang der Sektion stellen */
											if (pDataArg2 != NULL) {
												pParser = pDataArg2;
											}
											else {
												continue;
											}

											/* alle Daten der Sektion bearbeiten */
											countData = 0;
											do {
												/* Daten der Sektion anfordern */
												State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			  &pParser, &pParserLine, &pParserErr, NULL,
																			  &pDataArg1, &SizeArg1,
																			  &pDataArg2, &SizeArg2,
																			  &pDataArg3, &SizeArg3);
												/* keine Daten oder Fehler aufgetreten ? */
												if (State_find_Data < 0) {
													break;
												}
												else {
													/* Typ-Bezeichner */
													if (strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0 && strlen(cKW_DBS_TYPE) == SizeArg1 &&
														strncasecmp(pDataArg2, cKW_DBS_TYPE_NUMBER, SizeArg2) == 0 && strlen(cKW_DBS_TYPE_NUMBER) == SizeArg2 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														DbsType = atoi(pDataArg3);
														/* Typnummer bereits vergeben ? */
														if (pTypeDesc[DbsType].dbsType != DbsType) {
															pTypeDesc[DbsType].dbsType = DbsType;
															/* Typname bereits vergeben ? */
															flag = FALSE;
															for (i=0; i<=maxDbsType; i++) {
																if (pTypeDesc[i].pTypeName != NULL) {
																	if (strlen(pTypeDesc[i].pTypeName) == lenTypeName &&
																		strncmp(pTypeDesc[i].pTypeName, pTypeName, strlen(pTypeDesc[i].pTypeName)) == 0)
																	{
																		flag = TRUE;
																		break;
																	}
																}
															}
															if (flag == FALSE) {
																if (pTypeDesc[DbsType].pTypeName == NULL) {
																	pTypeDesc[DbsType].pTypeName = pWorkTypeName;
																}
																strncpy(pTypeDesc[DbsType].pTypeName, pTypeName, lenTypeName);
																pWorkTypeName = pWorkTypeName + lenTypeName + 1;
															}
															else {
																strcat(strcat(strcpy(logBuffer, (char*)iDatObjInfo.pName), ET_TYPE_ALREADY_EXIST), "\n");
																strcat(strncat(logBuffer, pTypeName, lenTypeName), "\n");
																strncat(logBuffer, pDataArg1, pDataArg3 - pDataArg1 + SizeArg3);
																ErrStatistics("dbserv", ERRLOG, EC_TYPE_ALREADY_EXIST + EC_DBS_BASE, CountLines, ERR_DBS_PRE, logBuffer);
															}
														} /* (pTypeDesc[DbsType].dbsType != DbsType) */
														else {
															/* !!! CHECK !!! */
															strcat(strcat(strcpy(logBuffer, (char*)iDatObjInfo.pName), ET_TYPE_NO_INVALID), "\n");
															strcat(strncat(logBuffer, pTypeName, lenTypeName), "\n");
															strncat(logBuffer, pDataArg1, pDataArg3 - pDataArg1 + SizeArg3);
															ErrStatistics("dbserv", ERRLOG, EC_TYPE_NO_INVALID + EC_DBS_BASE, CountLines, ERR_DBS_PRE, logBuffer);
														}
													}
													/* FileBuffer Grösse */
													else if (strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0 && strlen(cKW_DBS_TYPE) == SizeArg1 &&
														strncasecmp(pDataArg2, cKW_DBS_TYPE_FILE_SIZE, SizeArg2) == 0 && strlen(cKW_DBS_TYPE_FILE_SIZE) == SizeArg2 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														unsigned long	DbsTypeFileBufferSize;
														pDbsTypeFileBufferSize = pDataArg3;

														DbsTypeFileBufferSize = atoi(pDataArg3);
														pTypeDesc[DbsType].SizeFileBuffer = DbsTypeFileBufferSize;
														pTypeDesc[DbsType].FileBuffer = TRUE;
													}
													/* FileBuffer Modus */
													else if (strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0 && strlen(cKW_DBS_TYPE) == SizeArg1 &&
														strncasecmp(pDataArg2, cKW_DBS_TYPE_FILE_MODE, SizeArg2) == 0 && strlen(cKW_DBS_TYPE_FILE_MODE) == SizeArg2 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														unsigned char	DbsTypeFileBufferMode;
														pDbsTypeFileBufferMode = pDataArg3;

														DbsTypeFileBufferMode = atoi(pDataArg3);
														pTypeDesc[DbsType].ModeFileBuffer = DbsTypeFileBufferMode;
													}

													/* Buf-Bezeichner */
													else if (strlen(cKW_DBS_BUF_ATTRIBUTE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_BUF_ATTRIBUTE, SizeArg1) == 0 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														pTypeDesc[DbsType].FileBuffer = atoi(pDataArg3);
													}

												}
											} while (State_find_Data == TRUE);
										} while (State_find_Section == TRUE);

										/* FileBuffer Grösse */
										if (pDbsTypeFileBufferSize != NULL) {
											pTypeDesc[DbsType].SizeFileBuffer = atoi(pDbsTypeFileBufferSize);
											pTypeDesc[DbsType].FileBuffer = TRUE;
										}
										/* FileBuffer Modus */
										if (pDbsTypeFileBufferMode != NULL) {
											pTypeDesc[DbsType].ModeFileBuffer = atoi(pDbsTypeFileBufferMode);
										}
										break;

									/* -------------- */
									/* Item-Abschnitt */
									/* -------------- */
									case cCD_ITEM:

										do {
											State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				&pParser, &pParserLine, &pParserErr, &CountLines,
																				&pDataArg1, &SizeArg1,
																				&pDataArg2, &SizeArg2,
																				cKW_ALL_SECTION);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Section < 0) {
												break;
											}
											else {
												countItem++;
											}

											/* Pointer auf Anfang der Sektion stellen */
											if (pDataArg2 != NULL) {
												pParser = pDataArg2;
											}
											else {
												continue;
											}

											/* alle Daten der Sektion bearbeiten */
											countData = 0;
											do {
												/* Daten der Sektion anfordern */
												State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			  &pParser, &pParserLine, &pParserErr, NULL,
																			  &pDataArg1, &SizeArg1,
																			  &pDataArg2, &SizeArg2,
																			  &pDataArg3, &SizeArg3);
												/* keine Daten oder Fehler aufgetreten ? */
												if (State_find_Data < 0) {
													break;
												}
												else {
													/* Typ-Bezeichner */
													if (strlen(cKW_DBS_TYPE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0) {
														/* BOOL */
														if (strlen(cKW_DBS_TYPE_BOOL) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_BOOL, SizeArg3) == 0) {
															lenItem = sizeof(BOOL);
															sItem = FALSE;
															idTypeItem = cDBS_IDTYPE_BOOL;
														}
														/* USINT */
														else if (strlen(cKW_DBS_TYPE_USINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_USINT, SizeArg3) == 0) {
															lenItem = sizeof(USINT);
															sItem = FALSE;
															idTypeItem = cDBS_IDTYPE_USINT;
														}
														/* SINT */
														else if (strlen(cKW_DBS_TYPE_SINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_SINT, SizeArg3) == 0) {
															lenItem = sizeof(SINT);
															sItem = TRUE;
															idTypeItem = cDBS_IDTYPE_SINT;
														}
														/* UINT */
														else if (strlen(cKW_DBS_TYPE_UINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_UINT, SizeArg3) == 0) {
															lenItem = sizeof(UINT);
															sItem = FALSE;
															idTypeItem = cDBS_IDTYPE_UINT;
														}
														/* INT */
														else if (strlen(cKW_DBS_TYPE_INT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_INT, SizeArg3) == 0) {
															lenItem = sizeof(INT);
															sItem = TRUE;
															idTypeItem = cDBS_IDTYPE_INT;
														}
														/* UDINT */
														else if (strlen(cKW_DBS_TYPE_UDINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_UDINT, SizeArg3) == 0) {
															lenItem = sizeof(UDINT);
															sItem = FALSE;
															idTypeItem = cDBS_IDTYPE_UDINT;
														}
														/* DINT */
														else if (strlen(cKW_DBS_TYPE_DINT) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_DINT, SizeArg3) == 0) {
															lenItem = sizeof(DINT);
															sItem = TRUE;
															idTypeItem = cDBS_IDTYPE_DINT;
														}
														/* STREAM */
														else if (strlen(cKW_DBS_TYPE_STREAM) == SizeArg3 && strncmp(pDataArg3, cKW_DBS_TYPE_STREAM, SizeArg3) == 0) {
															lenItem = 2 * sizeof(UDINT);
															sItem = TRUE;
															idTypeItem = cDBS_IDTYPE_STREAM;
														}
														/* sonst */
														else {
															continue;
														}
														/* Informationen in den Typdescriptor eintragen */
														if (pTypeDesc[DbsType].pItemDesc == NULL) {
															pTypeDesc[DbsType].pItemDesc = pWorkItemDesc;
														}
														pTypeDesc[DbsType].pItemDesc[DbsItem].offItem = sizeType;
														pTypeDesc[DbsType].pItemDesc[DbsItem].lenItem = lenItem;
														pTypeDesc[DbsType].pItemDesc[DbsItem].sItem = sItem;
														pTypeDesc[DbsType].pItemDesc[DbsItem].idTypeItem = idTypeItem;
														sizeType += lenItem;
													}
													/* Zeitstempel-Bezeichner */
													else if (strlen(cKW_DBS_TYPE_TIMESTAMP) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE_TIMESTAMP, SizeArg1) == 0 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														pTypeDesc[DbsType].pItemDesc[DbsItem].tsItem = atoi(pDataArg3);
													}

													/* Buf-Bezeichner */
													else if (strlen(cKW_DBS_BUF_ATTRIBUTE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_BUF_ATTRIBUTE, SizeArg1) == 0 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														pTypeDesc[DbsType].FileBuffer = atoi(pDataArg3);
														pTypeDesc[DbsType].pItemDesc[DbsItem].FileBuffer = atoi(pDataArg3);
													}

													/* Item-Nummer */
													else if (strlen(cKW_DBS_ITEM) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_ITEM, SizeArg1) == 0 &&
														strlen(cKW_DBS_ITEM_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_ITEM_NUMBER, SizeArg2) == 0 &&
														pDataArg3 != NULL && SizeArg3 != 0 &&
														IsANumber(pDataArg3) == TRUE)
													{
														/* Itemnummer richtig ? */
														if (atoi(pDataArg3) + 1 != countItem) {
															/* !!! CHECK !!! */
															strcat(strcat(strcpy(logBuffer, (char*)iDatObjInfo.pName), ET_ITEM_NO_INVALID), "\n");
															strcat(strncat(logBuffer, pTypeName, lenTypeName), "\n");
															strncat(logBuffer, pDataArg1, pDataArg3 - pDataArg1 + SizeArg3);
															ErrStatistics("dbserv", ERRLOG, EC_ITEM_NO_INVALID + EC_DBS_BASE, CountLines, ERR_DBS_PRE, logBuffer);
														}
													}
													/* Item-Bezeichner */
													else if (strlen(cKW_DBS_ITEM) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_ITEM, SizeArg1) == 0 &&
														strlen(cKW_DBS_ITEM_NAME) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_ITEM_NAME, SizeArg2) == 0 &&
														pDataArg3 != NULL && SizeArg3 != 0)
													{
														/* Itemname bereits vergeben ? */
														flag = FALSE;
														for (i=0; i<countItem; i++) {
															if (pTypeDesc[DbsType].pItemDesc[i].pNameItem != NULL) {
																if (strlen(pTypeDesc[DbsType].pItemDesc[i].pNameItem) == SizeArg3 &&
																	strncmp(pTypeDesc[DbsType].pItemDesc[i].pNameItem, pDataArg3, SizeArg3) == 0) {
																	flag = TRUE;
																	break;
																}
															}
														}
														if (flag == FALSE) {
															if (pTypeDesc[DbsType].pItemDesc[DbsItem].pNameItem == NULL) {
																pTypeDesc[DbsType].pItemDesc[DbsItem].pNameItem = pWorkItemName;
															}
															strncpy(pTypeDesc[DbsType].pItemDesc[DbsItem].pNameItem, pDataArg3, SizeArg3);
															pWorkItemName = pWorkItemName + SizeArg3 + 1;
														}
														else {
															pWorkItemName = pWorkItemName + 1;
															/* !!! CHECK !!! */
															strcat(strcat(strcpy(logBuffer, (char*)iDatObjInfo.pName), ET_ITEM_ALREADY_EXIST), "\n");
															strcat(strncat(logBuffer, pTypeName, lenTypeName), "\n");
															strncat(logBuffer, pDataArg1, pDataArg3 - pDataArg1 + SizeArg3);
															ErrStatistics("dbserv", ERRLOG, EC_ITEM_ALREADY_EXIST + EC_DBS_BASE, CountLines, ERR_DBS_PRE, logBuffer);
														}
													}
												}
											} while (State_find_Data == TRUE);
										} while (State_find_Section == TRUE);

										/* Informationen in den Typdescriptor eintragen */
										pTypeDesc[DbsType].sizeType = sizeType;
										pTypeDesc[DbsType].countItem = countItem;
										DbsItem++;
										pWorkItemDesc++;
										break;

								} /* (SectionType) */
							} /* (pParser < (unsigned char*)pDA_Data + SizeDA_Data) */

							/* ---------------- */
							/* nächster Zustand */
							/* ---------------- */
							ParserState = cCD_PARSER_STATE_COMPARE_DO_TYPE;

						} /* (StateTMP_alloc == 0) */
						else {
							/* Fehler bei StateTMP_alloc */
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					} /* (memSize > 0) */
					else {
						/* Fehler bei memSize */
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ============================================== */
				/* DFA-Zustand: Abgleich der Datentypdefinitionen */
				/* ============================================== */
				case cCD_PARSER_STATE_COMPARE_DO_TYPE:
					iDatObjInfo.enable = TRUE;
					iDatObjInfo.pName = (unsigned long)"TypeDesc";
					DatObjInfo(&iDatObjInfo);
					if (iDatObjInfo.status == 0) {
						/* Datenmodul-Alignment beachten */
						if (iDatObjInfo.len >= (maxDbsType + 1) * sizeof(TYPE_DESC) + countDefinedItem * sizeof(ITEM_DESC) &&
							iDatObjInfo.len <  (maxDbsType + 1) * sizeof(TYPE_DESC) + countDefinedItem * sizeof(ITEM_DESC) + sizeof(unsigned long))
						{
							DbsTypeChanged = FALSE;
							pWorkTypeDesc = (TYPE_DESC*)iDatObjInfo.pDatObjMem;
							for (i=0; i<maxDbsType + 1; i++) {
								if (&pWorkTypeDesc[i] >= (TYPE_DESC*)(iDatObjInfo.pDatObjMem + iDatObjInfo.len)) {
									break;
								}
								if (pTypeDesc[i].dbsType			== pWorkTypeDesc[i].dbsType &&
									pTypeDesc[i].sizeType			== pWorkTypeDesc[i].sizeType &&
									pTypeDesc[i].sizeIntType		== pWorkTypeDesc[i].sizeIntType &&
									pTypeDesc[i].countItem			== pWorkTypeDesc[i].countItem &&
									pTypeDesc[i].countTimeItem		== pWorkTypeDesc[i].countTimeItem &&
									pTypeDesc[i].countRelatedPoints	== pWorkTypeDesc[i].countRelatedPoints
								) {
									pWorkItemDesc = pTypeDesc[i].pItemDesc;
									for (j=0; j<pTypeDesc[i].countItem; j++) {
										if (pTypeDesc[i].pItemDesc[j].offItem		== pWorkItemDesc[j].offItem &&
											pTypeDesc[i].pItemDesc[j].lenItem		== pWorkItemDesc[j].lenItem &&
											pTypeDesc[i].pItemDesc[j].sItem			== pWorkItemDesc[j].sItem &&
											pTypeDesc[i].pItemDesc[j].tsItem		== pWorkItemDesc[j].tsItem &&
											pTypeDesc[i].pItemDesc[j].idTypeItem	== pWorkItemDesc[j].idTypeItem
										) {
										}
										else {
											DbsTypeChanged = TRUE;
											break;
										}
									}
									if (DbsTypeChanged == TRUE) {
										break;
									}
								}
								else {
									DbsTypeChanged = TRUE;
									break;
								}
							}
						}
						else {
							DbsTypeChanged = TRUE;
						}
						if (DbsTypeChanged == TRUE) {
							ParserState = cCD_PARSER_STATE_DEL_DO_TYPE;
						}
						else {
							ParserState = cCD_PARSER_STATE_COUNT_DP;
						}
					}
					else if (iDatObjInfo.status == doERR_MODULNOTFOUND) {
						DbsTypeChanged = TRUE;
						ParserState = cCD_PARSER_STATE_CREATE_DO_TYPE;
					}
					else if (iDatObjInfo.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ========================================================= */
				/* DFA-Zustand: Datenobjekt für Datentypdefinitionen löschen */
				/* ========================================================= */
				case cCD_PARSER_STATE_DEL_DO_TYPE:
					iDatObjDelete.enable = TRUE;
					iDatObjDelete.ident = iDatObjInfo.ident;
					DatObjDelete(&iDatObjDelete);
					if (iDatObjDelete.status == 0) {
						ParserState = cCD_PARSER_STATE_CREATE_DO_TYPE;
					}
					else if (iDatObjDelete.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ========================================================= */
				/* DFA-Zustand: Datenobjekt für Datentypdefinitionen anlegen */
				/* ========================================================= */
				case cCD_PARSER_STATE_CREATE_DO_TYPE:
					iDatObjCreate.enable = TRUE;
					iDatObjCreate.grp = 0;
					iDatObjCreate.pName = (unsigned long)"TypeDesc";
					iDatObjCreate.len = (maxDbsType + 1) * sizeof(TYPE_DESC) + countDefinedItem * sizeof(ITEM_DESC);
					iDatObjCreate.MemType = doUSRRAM;
					iDatObjCreate.Option = !doNO_CS;
					iDatObjCreate.pCpyData = (unsigned long)pTypeDesc;
					DatObjCreate(&iDatObjCreate);
					if (iDatObjCreate.status == 0) {
						ParserState = cCD_PARSER_STATE_COUNT_DP;
					}
					else if (iDatObjCreate.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ======================================= */
				/* DFA-Zustand: Zählen der DBS-Datenpunkte */
				/* ======================================= */
				case cCD_PARSER_STATE_COUNT_DP:
					iDatObjInfo.enable = TRUE;
					iDatObjInfo.pName = (UDINT)cKW_DBS_DM_DP_LIST;
					DatObjInfo(&iDatObjInfo);
					if (iDatObjInfo.status == 0) {
						pDA_Data = (unsigned char*)iDatObjInfo.pDatObjMem;
						SizeDA_Data = iDatObjInfo.len;

						/* go */

						/* Datenmodul von Anfang an Parsen ... */
						CountLines = 0;
						pParser = (unsigned char*)pDA_Data;

						/* Zähler rücksetzen */
						countPortData = 0;
						countTimeoutData = 0;
						countDefinedPoint = 0;
						countExistPoints = 0;
						countNotExistPoints = 0;
						countStationTypPoint = 0;
						countLogMessages = 0;
						countFubInstance = 0;

						SizeNamePoint = 0;

						/* Parser-Status loeschen */
						State_find_StationTypPoint = 0;
						State_find_Section = 0;
						State_find_Data = 0;

						/* ------ */
						/* Parsen */
						/* ------ */
						while (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {

							/* Suche nach Stations/Typ/Punkt-Beschreibung */
							State_find_StationTypPoint = find_StationTypPoint(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				&pParser, &pParserLine, &pParserErr, &CountLines,
																				&pDataArg1, &SizeArg1,
																				&pDataArg2, &SizeArg2,
																				&SectionType);
							/* keine Daten oder Fehler aufgetreten ? */
							if (State_find_StationTypPoint < 0) {
								break;
							}
							else {
								countStationTypPoint++;
							}

							/* Pointer auf Station/Typ/Punkt */
							switch (SectionType) {

								/* ------------------ */
								/* Stations-Abschnitt */
								/* ------------------ */
								case cCD_STATION:
									countSection = 0;
									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_ALL_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countSection++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										countData = 0;
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < -1) {
												break;
											}
											else if (State_find_Data == -1) {
												if (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {
													pParser++;
													State_find_Data = TRUE;
													continue;
												}
												else {
													break;
												}
											}
											else {
												/* Port-Bezeichner */
												if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
													strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countPortData++;
												}
												/* Timeout-Bezeichner */
												else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
													strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countTimeoutData++;
												}
												/* Instanz-Anzahl */
												else if (strlen(cKW_DBS_CLIENT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_CLIENT, SizeArg1) == 0 &&
													strlen(cKW_DBS_CLIENT_COUNT) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_CLIENT_COUNT, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countEventInstance = atoi(pDataArg3);
												}
												/* Applikation-Anzahl */
												else if (strlen(cKW_DBS_APP) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_APP, SizeArg1) == 0 &&
													strlen(cKW_DBS_APP_COUNT) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_APP_COUNT, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countWriteInstance = atoi(pDataArg3);
												}
												/* Anlagen-Bezeichner */
												else if (strlen(cKW_DBS_UNIT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_UNIT, SizeArg1) == 0 &&
													strlen(cKW_DBS_UNIT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_UNIT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countUnitData++;
													DbsUnit = atoi(pDataArg3);
													maxDbsUnit = (DbsUnit > maxDbsUnit) ? DbsUnit : maxDbsUnit;
												}
												/* Datenpunkt-Bezeichner */
												else if (strlen(cKW_DBS_POINT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_POINT, SizeArg1) == 0 &&
													strlen(cKW_DBS_POINT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_POINT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countPointData++;
												}
												/* unbekannte Zuweisung */
												else {
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									break;

								/* ------------------- */
								/* Typ/Block-Abschnitt */
								/* ------------------- */
								case cCD_TYP:
									countSection = 0;
									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_TCP_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countSection++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										countData = 0;
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < 0) {
												break;
											}
											else {
												/* Port-Bezeichner */
												if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
													strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countPortData++;
												}
												/* Timeout-Bezeichner */
												else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
													strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countTimeoutData++;
												}
												/* Anlagen-Bezeichner */
												else if (strlen(cKW_DBS_UNIT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_UNIT, SizeArg1) == 0 &&
													strlen(cKW_DBS_UNIT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_UNIT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countUnitData++;
													DbsUnit = atoi(pDataArg3);
													maxDbsUnit = (DbsUnit > maxDbsUnit) ? DbsUnit : maxDbsUnit;
												}
												/* Datenpunkt-Bezeichner */
												else if (strlen(cKW_DBS_POINT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_POINT, SizeArg1) == 0 &&
													strlen(cKW_DBS_POINT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_POINT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countPointData++;
												}
												/* unbekannte Zuweisung */
												else {
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									break;

								/* -------------------- */
								/* Datenpunkt-Abschnitt */
								/* -------------------- */
								case cCD_POINT:

									/* Datenpunkt feststellen */
									StateGetDpInfo = GetDpInfo(pDataArg1, &DtIdx, &DpIdx, NULL);
									StateGetDpInfo = 1;
									flagDbsPoint = FALSE;
									flagDbsType = FALSE;
									flagDbsUnit = FALSE;
									flagDbsFub = FALSE;
									countPointData = 0;
									countSection = 0;
									SizeNamePoint += SizeArg1 + 1;
									do {
										State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																			&pParser, &pParserLine, &pParserErr, &CountLines,
																			&pDataArg1, &SizeArg1,
																			&pDataArg2, &SizeArg2,
																			cKW_ALL_SECTION);
										/* keine Daten oder Fehler aufgetreten ? */
										if (State_find_Section < 0) {
											break;
										}
										else {
											countSection++;
										}

										/* Pointer auf Anfang der Sektion stellen */
										if (pDataArg2 != NULL) {
											pParser = pDataArg2;
										}
										else {
											continue;
										}

										/* alle Daten der Sektion bearbeiten */
										do {
											/* Daten der Sektion anfordern */
											State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																		  &pParser, &pParserLine, &pParserErr, NULL,
																		  &pDataArg1, &SizeArg1,
																		  &pDataArg2, &SizeArg2,
																		  &pDataArg3, &SizeArg3);
											/* keine Daten oder Fehler aufgetreten ? */
											if (State_find_Data < 0) {
												break;
											}
											else {
												/* Port-Bezeichner */
												if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
													strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countPortData++;
												}
												/* Timeout-Bezeichner */
												else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
													strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countTimeoutData++;
												}
												/* Anlagen-Bezeichner */
												else if (strlen(cKW_DBS_UNIT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_UNIT, SizeArg1) == 0 &&
													strlen(cKW_DBS_UNIT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_UNIT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countUnitData++;
													DbsUnit = atoi(pDataArg3);
													maxDbsUnit = (DbsUnit > maxDbsUnit) ? DbsUnit : maxDbsUnit;
													flagDbsUnit = TRUE;
												}
												/* Datentyp-Bezeichner */
												else if (strlen(cKW_DBS_TYPE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0)
												{
													flagDbsType = FALSE;
													for (i=0; i<=maxDbsType; i++) {
														if (pTypeDesc[i].pTypeName != NULL) {
															if (strncmp(pDataArg3, pTypeDesc[i].pTypeName, SizeArg3) == 0 &&
																strlen(pTypeDesc[i].pTypeName) == SizeArg3)
															{
																DbsType = i;
																flagDbsType = TRUE;
																break;
															}
														}
													}
													if (flagDbsType == FALSE) {
													}
													else {
													}
												}
												/* Datenpunkt-Bezeichner */
												else if (strlen(cKW_DBS_POINT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_POINT, SizeArg1) == 0 &&
													strlen(cKW_DBS_POINT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_POINT_NUMBER, SizeArg2) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													DbsPoint = atoi(pDataArg3);
													flagDbsPoint = TRUE;
													if (countPointData == 0) {
														countPointData++;
														if (StateGetDpInfo == 1) {
															countDefinedPoint++;
															countExistPoints++;
														}
														else {
															countNotExistPoints++;
														}
													}
													else {
														/* Warnung: wiederholte Dp-Definition */
													}
												}
												/* Fub-Instanz-Bezeichner */
												else if (strlen(cKW_DBS_FUB) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_FUB, SizeArg1) == 0 &&
													pDataArg3 != NULL && SizeArg3 != 0 &&
													IsANumber(pDataArg3) == TRUE)
												{
													countFub++;
													if (atoi(pDataArg3) == 1) {
														flagDbsFub = TRUE;
													}
												}
												/* unbekannte Zuweisung */
												else {
												}
											}
										} while (State_find_Data == TRUE);
									} while (State_find_Section == TRUE);
									if (flagDbsUnit == TRUE && flagDbsType == TRUE && flagDbsPoint == TRUE && flagDbsFub == TRUE) {
										countFubInstance += pTypeDesc[DbsType].countItem;
									}
									break;
							} /* (SectionType) */
						} /* (pParser < (unsigned char*)pDA_Data + SizeDA_Data) */

						/* ---------------- */
						/* nächster Zustand */
						/* ---------------- */
						ParserState = cCD_PARSER_STATE_DEF_DP;

					} /* (DA_DataOk == TRUE) */
					else {
						/* Fehler beim Datenmodul */
						ErrStatistics("dbserv", ERRLOG, EC_DA_NOT_FOUND + EC_DBS_BASE, CountLines, ERR_DBS_PRE, ET_DA_NOT_FOUND);
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ===================================================================== */
				/* DFA-Zustand: Speicher allokieren und Aufbau der Bäume für Datenpunkte */
				/* ===================================================================== */
				case cCD_PARSER_STATE_DEF_DP:

					maxUsedUnit = maxDbsUnit;
					maxUsedType = maxDbsType;

					/* ----------------------------------------- */
					/* Baum Unit -> Type aufbauen (Wurzelknoten) */
					/* ----------------------------------------- */
					memSize = (maxUsedUnit + 1) * sizeof(ACCESS_UNIT);
					StateTMP_alloc = TMP_alloc(memSize, (void*)&pAccessUnit);
					if (StateTMP_alloc == 0) {
						SmemSize += 0;
						StmpSize += memSize;
						memset(pAccessUnit, 0, memSize);

						memSize = countDefinedPoint * sizeof(POINT_DESC) + SizeNamePoint;
						StateTMP_alloc = TMP_alloc(memSize, (void*)&pPointDesc);
						if (StateTMP_alloc == 0) {
							SmemSize += 0;
							StmpSize += memSize;
							memset(pPointDesc, 0, memSize);
							pWorkPointName = (unsigned char*)(pPointDesc + countDefinedPoint);

							memSize = countFubInstance * sizeof(FUB_DESC);
							StateTMP_alloc = TMP_alloc(memSize, (void*)&pFubInstanceDesc);
							if (StateTMP_alloc == 0) {
								SmemSize += 0;
								StmpSize += memSize;
								memset(pFubInstanceDesc, 0, memSize);

								/* Datenmodul von Anfang an Parsen ... */
								CountLines = 0;
								pParser = (unsigned char*)pDA_Data;

								/* Zähler rücksetzen */
								countPortData = 0;
								countTimeoutData = 0;
								countDefinedPoint = 0;
								countExistPoints = 0;
								countNotExistPoints = 0;

								countNotDefinedItems = 0;
								countInvalidExistItems = 0;
								countNotExistItems = 0;

								countStationTypPoint = 0;
								countLogMessages = 0;
								countFubInstance = 0;

								/* Parser-Status loeschen */
								State_find_StationTypPoint = 0;
								State_find_Section = 0;
								State_find_Data = 0;

								/* ------ */
								/* Parsen */
								/* ------ */
								while (pParser < (unsigned char*)pDA_Data + SizeDA_Data) {

									/* Suche nach Stations/Typ/Punkt-Beschreibung */
									State_find_StationTypPoint = find_StationTypPoint(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																						&pParser, &pParserLine, &pParserErr, &CountLines,
																						&pDataArg1, &SizeArg1,
																						&pDataArg2, &SizeArg2,
																						&SectionType);
									/* keine Daten oder Fehler aufgetreten ? */
									if (State_find_StationTypPoint < 0) {
										break;
									}
									else {
										countStationTypPoint++;
									}

									/* Pointer auf Station/Typ/Punkt */
									switch (SectionType) {

										/* ------------------ */
										/* Stations-Abschnitt */
										/* ------------------ */
										case cCD_STATION:
											countSection = 0;
											do {
												State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																					&pParser, &pParserLine, &pParserErr, &CountLines,
																					&pDataArg1, &SizeArg1,
																					&pDataArg2, &SizeArg2,
																					cKW_TCP_SECTION);
												/* keine Daten oder Fehler aufgetreten ? */
												if (State_find_Section < 0) {
													break;
												}
												else {
													countSection++;
												}

												/* Pointer auf Anfang der Sektion stellen */
												if (pDataArg2 != NULL) {
													pParser = pDataArg2;
												}
												else {
													continue;
												}

												/* alle Daten der Sektion bearbeiten */
												countData = 0;
												do {
													/* Daten der Sektion anfordern */
													State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				  &pParser, &pParserLine, &pParserErr, NULL,
																				  &pDataArg1, &SizeArg1,
																				  &pDataArg2, &SizeArg2,
																				  &pDataArg3, &SizeArg3);
													/* keine Daten oder Fehler aufgetreten ? */
													if (State_find_Data < 0) {
														break;
													}
													else {
														/* Port-Bezeichner */
														if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
															strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countPortData++;
														}
														/* Timeout-Bezeichner */
														else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
															strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countTimeoutData++;
														}
														/* Anlagen-Bezeichner */
														else if (strlen(cKW_DBS_UNIT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_UNIT, SizeArg1) == 0 &&
															strlen(cKW_DBS_UNIT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_UNIT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countUnitData++;
															DbsUnit = atoi(pDataArg3);
															maxDbsUnit = (DbsUnit > maxDbsUnit) ? DbsUnit : maxDbsUnit;
														}
														/* Datenpunkt-Bezeichner */
														else if (strlen(cKW_DBS_POINT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_POINT, SizeArg1) == 0 &&
															strlen(cKW_DBS_POINT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_POINT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countPointData++;
														}
														/* unbekannte Zuweisung */
														else {
														}
													}
												} while (State_find_Data == TRUE);
											} while (State_find_Section == TRUE);
											break;

										/* ------------------- */
										/* Typ/Block-Abschnitt */
										/* ------------------- */
										case cCD_TYP:
											countSection = 0;
											do {
												State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																					&pParser, &pParserLine, &pParserErr, &CountLines,
																					&pDataArg1, &SizeArg1,
																					&pDataArg2, &SizeArg2,
																					cKW_TCP_SECTION);
												/* keine Daten oder Fehler aufgetreten ? */
												if (State_find_Section < 0) {
													break;
												}
												else {
													countSection++;
												}

												/* Pointer auf Anfang der Sektion stellen */
												if (pDataArg2 != NULL) {
													pParser = pDataArg2;
												}
												else {
													continue;
												}

												/* alle Daten der Sektion bearbeiten */
												countData = 0;
												do {
													/* Daten der Sektion anfordern */
													State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				  &pParser, &pParserLine, &pParserErr, NULL,
																				  &pDataArg1, &SizeArg1,
																				  &pDataArg2, &SizeArg2,
																				  &pDataArg3, &SizeArg3);
													/* keine Daten oder Fehler aufgetreten ? */
													if (State_find_Data < 0) {
														break;
													}
													else {
														/* Port-Bezeichner */
														if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
															strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countPortData++;
														}
														/* Timeout-Bezeichner */
														else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
															strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countTimeoutData++;
														}
														/* Anlagen-Bezeichner */
														else if (strlen(cKW_DBS_UNIT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_UNIT, SizeArg1) == 0 &&
															strlen(cKW_DBS_UNIT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_UNIT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countUnitData++;
															DbsUnit = atoi(pDataArg3);
															maxDbsUnit = (DbsUnit > maxDbsUnit) ? DbsUnit : maxDbsUnit;
														}
														/* Datenpunkt-Bezeichner */
														else if (strlen(cKW_DBS_POINT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_POINT, SizeArg1) == 0 &&
															strlen(cKW_DBS_POINT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_POINT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countPointData++;
														}
														/* unbekannte Zuweisung */
														else {
														}
													}
												} while (State_find_Data == TRUE);
											} while (State_find_Section == TRUE);
											break;

										/* -------------------- */
										/* Datenpunkt-Abschnitt */
										/* -------------------- */
										case cCD_POINT:

											/* Datenpunkt feststellen */
											pPointName = pDataArg1, lenPointName = SizeArg1;
											flagDbsPoint = FALSE;
											flagDbsType = FALSE;
											flagDbsUnit = FALSE;
											flagDbsFub = FALSE;
											countPointData = 0;
											countSection = 0;
											do {
												State_find_Section = find_Section(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																					&pParser, &pParserLine, &pParserErr, &CountLines,
																					&pDataArg1, &SizeArg1,
																					&pDataArg2, &SizeArg2,
																					cKW_ALL_SECTION);
												/* keine Daten oder Fehler aufgetreten ? */
												if (State_find_Section < 0) {
													break;
												}
												else {
													countSection++;
												}

												/* Pointer auf Anfang der Sektion stellen */
												if (pDataArg2 != NULL) {
													pParser = pDataArg2;
												}
												else {
													continue;
												}

												/* alle Daten der Sektion bearbeiten */
												do {
													/* Daten der Sektion anfordern */
													State_find_Data = find_Data(pDA_Data, pParser, pDA_Data + SizeDA_Data,
																				  &pParser, &pParserLine, &pParserErr, NULL,
																				  &pDataArg1, &SizeArg1,
																				  &pDataArg2, &SizeArg2,
																				  &pDataArg3, &SizeArg3);
													/* keine Daten oder Fehler aufgetreten ? */
													if (State_find_Data < 0) {
														break;
													}
													else {
														/* Port-Bezeichner */
														if (strlen(cKW_TCP_PORT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_PORT, SizeArg1) == 0 &&
															strlen(cKW_TCP_PORT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_PORT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countPortData++;
														}
														/* Timeout-Bezeichner */
														else if (strlen(cKW_TCP_TIMEOUT) == SizeArg1 && strncasecmp(pDataArg1, cKW_TCP_TIMEOUT, SizeArg1) == 0 &&
															strlen(cKW_TCP_TIMEOUT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_TCP_TIMEOUT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countTimeoutData++;
														}
														/* Anlagen-Bezeichner */
														else if (strlen(cKW_DBS_UNIT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_UNIT, SizeArg1) == 0 &&
															strlen(cKW_DBS_UNIT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_UNIT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countUnitData++;
															if (atoi(pDataArg3) >= 0 && atoi(pDataArg3) <= cMAX_UNIT_DEF) {
																DbsUnit = atoi(pDataArg3);
																maxDbsUnit = (DbsUnit > maxDbsUnit) ? DbsUnit : maxDbsUnit;
																flagDbsUnit = TRUE;
															}
															else {
															}
														}
														/* Datentyp-Bezeichner */
														else if (strlen(cKW_DBS_TYPE) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_TYPE, SizeArg1) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0)
														{
															flagDbsType = FALSE;
															for (i=0; i<=maxDbsType; i++) {
																if (pTypeDesc[i].pTypeName != NULL) {
																	if (strncmp(pDataArg3, pTypeDesc[i].pTypeName, SizeArg3) == 0 &&
																		strlen(pTypeDesc[i].pTypeName) == SizeArg3)
																	{
																		DbsType = i;
																		flagDbsType = TRUE;
																		break;
																	}
																}
															}
															if (flagDbsType == FALSE) {
															}
															else {
															}
														}
														/* Datenpunkt-Bezeichner */
														else if (strlen(cKW_DBS_POINT) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_POINT, SizeArg1) == 0 &&
															strlen(cKW_DBS_POINT_NUMBER) == SizeArg2 && strncasecmp(pDataArg2, cKW_DBS_POINT_NUMBER, SizeArg2) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															if (countPointData == 0) {
																countPointData++;
																if (atoi(pDataArg3) >= 0 && atoi(pDataArg3) <= cMAX_POINT_DEF) {
																	DbsPoint = atoi(pDataArg3);
																	pTempData = pDataArg3;
																	flagDbsPoint = TRUE;
																	/*if (StateGetDpInfo == 1) {
																		countExistPoints++;
																	}
																	else {
																		countNotExistPoints++;
																	}*/
																}
																else {
																}
															}
															else {
																/* Warnung: wiederholte Dp-Definition */
															}
														}
														/* Fub-Instanz-Bezeichner */
														else if (strlen(cKW_DBS_FUB) == SizeArg1 && strncasecmp(pDataArg1, cKW_DBS_FUB, SizeArg1) == 0 &&
															pDataArg3 != NULL && SizeArg3 != 0 &&
															IsANumber(pDataArg3) == TRUE)
														{
															countFub++;
															if (atoi(pDataArg3) == 1) {
																flagDbsFub = TRUE;
															}
														}
														/* unbekannte Zuweisung */
														else {
														}
													}
												} while (State_find_Data == TRUE);
											} while (State_find_Section == TRUE);
											if (flagDbsUnit == TRUE && flagDbsType == TRUE && flagDbsPoint == TRUE) {
												maxUsedUnit = (maxUsedUnit > DbsUnit) ? maxUsedUnit : DbsUnit;
												maxUsedType = (maxUsedType > DbsType) ? maxUsedType : DbsType;

												/* Datenpunkt eindeutig ? */
												flag = FALSE;
												for (i=0; i<countDefinedPoint; i++) {
													if (pPointDesc[i].pPointName != NULL) {
														if (strlen(pPointDesc[i].pPointName) == lenPointName &&
															strncmp(pPointDesc[i].pPointName, pPointName, strlen(pPointDesc[i].pPointName)) == 0)
														{
															flag = TRUE;
															break;
														}
													}
													if (pPointDesc[i].dbsUnit == DbsUnit &&
														pPointDesc[i].dbsType == DbsType &&
														pPointDesc[i].dbsPoint == DbsPoint)
													{
														flag = TRUE;
														break;
													}
												} /* (i=0; i<countDefinedPoint; i++) */
												if (flag == FALSE) {
													if (pPointDesc[countDefinedPoint].pPointName == NULL) {
														pPointDesc[countDefinedPoint].pPointName = pWorkPointName;
													}
													strncpy(pPointDesc[countDefinedPoint].pPointName, pPointName, lenPointName);
													pWorkPointName = pWorkPointName + lenPointName + 1;
												}
												else {
													/* wiederholte Datenpunkt definition */
													/* !!! CHECK !!! */
													strcat(strcat(strcpy(logBuffer, (char*)iDatObjInfo.pName), ET_POINT_ALREADY_EXIST), "\n");
													strcat(strncat(logBuffer, pPointName, lenPointName), "\n");
													strncat(logBuffer, pDataArg1, pDataArg3 - pDataArg1 + SizeArg3);
													ErrStatistics("dbserv", ERRLOG, EC_POINT_ALREADY_EXIST + EC_DBS_BASE, CountLines, ERR_DBS_PRE, logBuffer);
												}

												/* Datenpunkt-Deskriptor aufbauen */
												pPointDesc[countDefinedPoint].dbsUnit  = DbsUnit;
												pPointDesc[countDefinedPoint].dbsType  = DbsType;
												pPointDesc[countDefinedPoint].dbsPoint = DbsPoint;
												countDefinedPoint++;

												/* Wurzelknoten für Datentypen aufbauen */
												pAccessUnit[DbsUnit].countType = (DbsType + 1 > pAccessUnit[DbsUnit].countType) ? DbsType + 1 : pAccessUnit[DbsUnit].countType;

												/* Fub-Instanz */
												if (flagDbsFub == TRUE) {
													flagDbsExistPoints = FALSE;
													flagDbsNotExistPoints = FALSE;
													for (i=0; i<pTypeDesc[DbsType].countItem; i++) {
														strncpy(PointName, pPointName, sizeof(PointName));
														/* Namen über Item-Namen zusammensetzen */
														flagDbsFubInstance = TRUE;
														if (pTypeDesc[DbsType].countItem > 1) {
															if (pTypeDesc[DbsType].pItemDesc[i].pNameItem != NULL) {
																if (sizeof(PointName) - strlen(PointName) > strlen(pTypeDesc[DbsType].pItemDesc[i].pNameItem)) {
																	strcat(PointName, pTypeDesc[DbsType].pItemDesc[i].pNameItem);
																}
															}
															else {
																/* kein Itemname angegeben */
																countNotDefinedItems++;
																flagDbsFubInstance = FALSE;
															}
														}
														stateFubInstance = PV_xgetadr(PointName, &pFubInstance, &lenFubInstance);
														if (flagDbsFubInstance == FALSE) {
															if (stateFubInstance == 0) {
																countInvalidExistItems++;
																if (flagDbsExistPoints == FALSE) {
																	flagDbsExistPoints = TRUE;
																	countExistPoints++;
																}
															}
															else {
																/*countNotExistItems++;
																if (flagDbsNotExistPoints == FALSE) {
																	flagDbsNotExistPoints = TRUE;
																	countNotExistPoints++;
																}*/
															}
														}
														else {
															if (stateFubInstance == 0) {
																if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_BOOL &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == FALSE)
																{
																	((DbsBool_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsBool_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsBool_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsBool_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsBool_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsBool_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsBool_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsBool_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsBool_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_BOOL &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == TRUE)
																{
																	((DbsBoolTs_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsBoolTs_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsBoolTs_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_USINT &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == FALSE)
																{
																	((DbsUsint_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsUsint_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsUsint_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsUsint_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsUsint_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsUsint_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsUsint_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsUsint_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsUsint_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_USINT &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == TRUE)
																{
																	((DbsUsintTs_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsUsintTs_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsUsintTs_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_UINT &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == FALSE)
																{
																	((DbsUint_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsUint_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsUint_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsUint_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsUint_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsUint_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsUint_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsUint_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsUint_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_INT &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == FALSE)
																{
																	((DbsInt_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsInt_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsInt_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsInt_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsInt_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsInt_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsInt_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsInt_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsInt_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_UDINT &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == FALSE)
																{
																	((DbsUdint_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsUdint_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsUdint_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsUdint_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsUdint_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsUdint_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsUdint_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsUdint_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsUdint_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																else if (pTypeDesc[DbsType].pItemDesc[i].idTypeItem == cDBS_IDTYPE_DINT &&
																	pTypeDesc[DbsType].pItemDesc[i].tsItem == FALSE)
																{
																	((DbsDint_typ*)pFubInstance)->SetValue = TRUE;

																	((DbsDint_typ*)pFubInstance)->FubRef.dbsUnit = DbsUnit;
																	((DbsDint_typ*)pFubInstance)->FubRef.dbsDevice = cDBS_DEFAULT_DEVICE;
																	((DbsDint_typ*)pFubInstance)->FubRef.dbsType = DbsType;
																	((DbsDint_typ*)pFubInstance)->FubRef.dbsPoint = DbsPoint;
																	((DbsDint_typ*)pFubInstance)->FubRef.dbsItem = i;
																	((DbsDint_typ*)pFubInstance)->FubRef.countItem = pTypeDesc[DbsType].countItem;
																	((DbsDint_typ*)pFubInstance)->FubRef.pidBrServer = 1;
																	((DbsDint_typ*)pFubInstance)->FubRef.this = pFubInstance;

																	pFubInstanceDesc[countFubInstance].dbsUnit = DbsUnit;
																	pFubInstanceDesc[countFubInstance].dbsType = DbsType;
																	pFubInstanceDesc[countFubInstance].dbsPoint = DbsPoint;
																	pFubInstanceDesc[countFubInstance].dbsItem = i;
																	pFubInstanceDesc[countFubInstance].pFubInstance = (unsigned char*)pFubInstance;
																	countFubInstance++;
																}
																if (flagDbsExistPoints == FALSE) {
																	flagDbsExistPoints = TRUE;
																	countExistPoints++;
																}
															} /* (stateFubInstance == 0) */
															else {
																countNotExistItems++;
																if (flagDbsNotExistPoints == FALSE) {
																	flagDbsNotExistPoints = TRUE;
																	countNotExistPoints++;
																}
															}
														}
													} /* (i=0; i<pTypeDesc[DbsType].countItem; i++) */
												} /* (flagDbsFub == TRUE) */
												else {
													countExistPoints++;
												}
											} /* (flagDbsUnit == TRUE && flagDbsType == TRUE && flagDbsPoint == TRUE) */
											break;
									} /* (SectionType) */
								} /* (pParser < (unsigned char*)pDA_Data + SizeDA_Data) */

								/* ---------------- */
								/* nächster Zustand */
								/* ---------------- */
								ParserState = cCD_PARSER_STATE_COMPARE_DO_POINT;

							} /* (StateTMP_alloc == 0) */
							else {
								/* Fehler bei TMP_alloc */
								ParserState = cCD_PARSER_STATE_ERROR;
							}
						} /* (StateTMP_alloc == 0) */
						else {
							/* Fehler bei TMP_alloc */
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					} /* (StateTMP_alloc == 0) */
					else {
						/* Fehler bei TMP_alloc */
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ================================================ */
				/* DFA-Zustand: Abgleich der Datenpunktdefinitionen */
				/* ================================================ */
				case cCD_PARSER_STATE_COMPARE_DO_POINT:
					iDatObjInfo.enable = TRUE;
					iDatObjInfo.pName = (unsigned long)"DpDesc";
					DatObjInfo(&iDatObjInfo);
					if (iDatObjInfo.status == 0) {
						/* Datenmodul-Alignment beachten */
						if (iDatObjInfo.len >= countDefinedPoint * sizeof(POINT_DESC) &&
							iDatObjInfo.len <  countDefinedPoint * sizeof(POINT_DESC) + sizeof(unsigned long))
						{
							DbsPointChanged = FALSE;
							pWorkPointDesc = (POINT_DESC*)iDatObjInfo.pDatObjMem;
							for (i=0; i<countDefinedPoint; i++) {
								if (&pWorkPointDesc[i] >= (POINT_DESC*)(iDatObjInfo.pDatObjMem + iDatObjInfo.len)) {
									break;
								}
								if (pPointDesc[i].dbsUnit	== pWorkPointDesc[i].dbsUnit &&
									pPointDesc[i].dbsType	== pWorkPointDesc[i].dbsType &&
									pPointDesc[i].dbsPoint	== pWorkPointDesc[i].dbsPoint
								) {
								}
								else {
									DbsPointChanged = TRUE;
									break;
								}
							}
						}
						else {
							DbsPointChanged = TRUE;
						}
						if (DbsPointChanged == TRUE) {
							ParserState = cCD_PARSER_STATE_DEL_DO_POINT;
						}
						else {
							ParserState = cCD_PARSER_STATE_ALLOC_ACCESS_DATA;
						}
					}
					else if (iDatObjInfo.status == doERR_MODULNOTFOUND) {
						DbsPointChanged = TRUE;
						ParserState = cCD_PARSER_STATE_CREATE_DO_POINT;
					}
					else if (iDatObjInfo.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* =========================================================== */
				/* DFA-Zustand: Datenobjekt für Datenpunktdefinitionen löschen */
				/* =========================================================== */
				case cCD_PARSER_STATE_DEL_DO_POINT:
					iDatObjDelete.enable = TRUE;
					iDatObjDelete.ident = iDatObjInfo.ident;
					DatObjDelete(&iDatObjDelete);
					if (iDatObjDelete.status == 0) {
						ParserState = cCD_PARSER_STATE_CREATE_DO_POINT;
					}
					else if (iDatObjDelete.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* =========================================================== */
				/* DFA-Zustand: Datenobjekt für Datenpunktdefinitionen anlegen */
				/* =========================================================== */
				case cCD_PARSER_STATE_CREATE_DO_POINT:
					iDatObjCreate.enable = TRUE;
					iDatObjCreate.grp = 0;
					iDatObjCreate.pName = (unsigned long)"DpDesc";
					iDatObjCreate.len = countDefinedPoint * sizeof(POINT_DESC);
					iDatObjCreate.MemType = doUSRRAM;
					iDatObjCreate.Option = !doNO_CS;
					iDatObjCreate.pCpyData = (unsigned long)pPointDesc;
					DatObjCreate(&iDatObjCreate);
					if (iDatObjCreate.status == 0) {
						ParserState = cCD_PARSER_STATE_ALLOC_ACCESS_DATA;
					}
					else if (iDatObjCreate.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ======================================= */
				/* DFA-Zustand: Zählen der DBS-Datenpunkte */
				/* ======================================= */
				case cCD_PARSER_STATE_ALLOC_ACCESS_DATA:

					/* --------------------------------------- */
					/* Speicher für Datentypzugriff allokieren */
					/* --------------------------------------- */
					memSize = 0;
					for (i=0; i<=maxDbsUnit; i++) {
						if (pAccessUnit[i].countType > 0) {
							memSize += pAccessUnit[i].countType;
						}
					}
					memSize *= sizeof(ACCESS_TYPE);
					StateTMP_alloc = TMP_alloc(memSize, (void*)&pAccessType);
					if (StateTMP_alloc == 0) {
						SmemSize += 0;
						StmpSize += memSize;
						memset(pAccessType, 0, memSize);

						/* ------------------------------------------ */
						/* Speicher für Anlagenzugriff initialisieren */
						/* ------------------------------------------ */
						pAccessTypeWork = pAccessType;
						for (i=0; i<=maxDbsUnit; i++) {
							if (pAccessUnit[i].countType > 0) {
								pAccessUnit[i].pAccessType = pAccessTypeWork;
								pAccessTypeWork += pAccessUnit[i].countType;
							}
						}

						/* ------------------------------------------- */
						/* Speicher für Datentypzugriff initialisieren */
						/* ------------------------------------------- */
						for (i=0; i<countDefinedPoint; i++) {
							DbsUnit = pPointDesc[i].dbsUnit;
							DbsType = pPointDesc[i].dbsType;
							DbsPoint = pPointDesc[i].dbsPoint;
							pAccessUnit[DbsUnit].pAccessType[DbsType].countPoint = (DbsPoint + 1 > pAccessUnit[DbsUnit].pAccessType[DbsType].countPoint) ? DbsPoint + 1 : pAccessUnit[DbsUnit].pAccessType[DbsType].countPoint;
							pTypeDesc[DbsType].countRelatedPoints++;
						}

						/* ----------------------------------------- */
						/* Speicher für Datenpunktzugriff allokieren */
						/* ----------------------------------------- */
						memSize = 0;
						for (i=0; i<=maxDbsUnit; i++) {
							if (pAccessUnit[i].pAccessType != NULL) {
								for (j=0; j<pAccessUnit[i].countType; j++) {
									memSize += pAccessUnit[i].pAccessType[j].countPoint;
								}
							}
						}
						memSize *= sizeof(ACCESS_POINT);
						StateTMP_alloc = TMP_alloc(memSize, (void*)&pAccessPoint);
						if (StateTMP_alloc == 0) {
							SmemSize += 0;
							StmpSize += memSize;
							memset(pAccessPoint, 0, memSize);

							/* ------------------------------------------- */
							/* Speicher für Datentypzugriff initialisieren */
							/* ------------------------------------------- */
							pAccessPointWork = pAccessPoint;
							for (i=0; i<=maxDbsUnit; i++) {
								for (j=0; j<pAccessUnit[i].countType; j++) {
									if (pAccessUnit[i].pAccessType[j].countPoint > 0) {
										pAccessUnit[i].pAccessType[j].pAccessPoint = pAccessPointWork;
										pAccessPointWork = pAccessPointWork + pAccessUnit[i].pAccessType[j].countPoint;
									}
								}
							}

							/* --------------------------------------------- */
							/* Speicher für Datenpunktzugriff initialisieren */
							/* --------------------------------------------- */
							pAccessDataWork = NULL;
							for (i=0; i<countDefinedPoint; i++) {
								DbsUnit = pPointDesc[i].dbsUnit;
								DbsType = pPointDesc[i].dbsType;
								DbsPoint = pPointDesc[i].dbsPoint;
								countItem = pTypeDesc[DbsType].countItem;
								countTimeItem = pTypeDesc[DbsType].countTimeItem;

								pAccessUnit[DbsUnit].pAccessType[DbsType].pAccessPoint[DbsPoint].pAccessData = pAccessDataWork;
								pAccessTypeWork = &pAccessUnit[DbsUnit].pAccessType[DbsType];
								len = 0;

								pAccessTypeWork->offExtern = len;
								pAccessTypeWork->lenExtern = pTypeDesc[DbsType].sizeType;
								len = pAccessTypeWork->lenExtern;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offIntern = pAccessTypeWork->offExtern + len;
								pAccessTypeWork->lenIntern = pTypeDesc[DbsType].sizeIntType;
								len = pAccessTypeWork->lenIntern;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offiExtern = pAccessTypeWork->offIntern + len;
								pAccessTypeWork->leniExtern = pTypeDesc[DbsType].sizeType;
								pAccessTypeWork->leniExternAlign = (pAccessTypeWork->leniExtern > 1 && pAccessTypeWork->leniExtern % 2 != 0) ? pAccessTypeWork->leniExtern + 1 : pAccessTypeWork->leniExtern;
								pAccessTypeWork->leniExternNext = pAccessTypeWork->leniExternAlign;
								len = countEventInstance * pAccessTypeWork->leniExternAlign;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offValid = pAccessTypeWork->offiExtern + len;
								pAccessTypeWork->lenValid = sizeof(ACCESS_ITEM_VALID);
								pAccessTypeWork->lenValidAlign = (pAccessTypeWork->lenValid > 1 && pAccessTypeWork->lenValid % 2 != 0) ? pAccessTypeWork->lenValid + 1 : pAccessTypeWork->lenValid;
								len = countItem * pAccessTypeWork->lenValidAlign;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offTime = pAccessTypeWork->offValid + len;
								pAccessTypeWork->lenTime = sizeof(RTCtime_typ);
								pAccessTypeWork->lenTimeAlign = (pAccessTypeWork->lenTime > 1 && pAccessTypeWork->lenTime % 2 != 0) ? pAccessTypeWork->lenTime + 1 : pAccessTypeWork->lenTime;
								len = countItem * pAccessTypeWork->lenTimeAlign;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offiTime = pAccessTypeWork->offTime + len;
								pAccessTypeWork->leniTime = sizeof(RTCtime_typ);
								pAccessTypeWork->leniTimeAlign = (pAccessTypeWork->leniTime > 1 && pAccessTypeWork->leniTime % 2 != 0) ? pAccessTypeWork->leniTime + 1 : pAccessTypeWork->leniTime;
								pAccessTypeWork->leniTimeNext = countItem * pAccessTypeWork->leniTimeAlign;
								len = countEventInstance * pAccessTypeWork->leniTimeNext;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offWritelink = pAccessTypeWork->offiTime + len;
								pAccessTypeWork->lenWritelink = sizeof(ACCESS_ITEM_WRITE_LINK);
								pAccessTypeWork->lenWritelinkAlign = (pAccessTypeWork->lenWritelink > 1 && pAccessTypeWork->lenWritelink % 2 != 0) ? pAccessTypeWork->lenWritelink + 1 : pAccessTypeWork->lenWritelink;
								len = countItem * pAccessTypeWork->lenWritelinkAlign;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offWritehandle = pAccessTypeWork->offWritelink + len;
								pAccessTypeWork->lenWritehandle = sizeof(ACCESS_ITEM_WRITE_HANDLE);
								pAccessTypeWork->lenWritehandleAlign = (pAccessTypeWork->lenWritehandle > 1 && pAccessTypeWork->lenWritehandle % 2 != 0) ? pAccessTypeWork->lenWritehandle + 1 : pAccessTypeWork->lenWritehandle;
								len = countItem * pAccessTypeWork->lenWritehandleAlign;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offiEventlink = pAccessTypeWork->offWritehandle + len;
								pAccessTypeWork->leniEventlink = sizeof(ACCESS_ITEM_EVENT_LINK);
								pAccessTypeWork->leniEventlinkAlign = (pAccessTypeWork->leniEventlink > 1 && pAccessTypeWork->leniEventlink % 2 != 0) ? pAccessTypeWork->leniEventlink + 1 : pAccessTypeWork->leniEventlink;
								pAccessTypeWork->leniEventlinkNext = countItem * pAccessTypeWork->leniEventlinkAlign;
								len = countEventInstance * pAccessTypeWork->leniEventlinkNext;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offiReleasestream = pAccessTypeWork->offiEventlink + len;
								pAccessTypeWork->leniReleasestream = sizeof(ACCESS_ITEM_RELEASE);
								pAccessTypeWork->leniReleasestreamAlign = (pAccessTypeWork->leniReleasestream > 1 && pAccessTypeWork->leniReleasestream % 2 != 0) ? pAccessTypeWork->leniReleasestream + 1 : pAccessTypeWork->leniReleasestream;
								pAccessTypeWork->leniReleasestreamNext = countItem * pAccessTypeWork->leniReleasestreamAlign;
								len = countEventInstance * pAccessTypeWork->leniReleasestreamNext;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offiHyst = pAccessTypeWork->offiReleasestream + len;
								pAccessTypeWork->leniHyst = pTypeDesc[DbsType].sizeType;
								pAccessTypeWork->leniHystAlign = (pAccessTypeWork->leniHyst > 1 && pAccessTypeWork->leniHyst % 2 != 0) ? pAccessTypeWork->leniHyst + 1 : pAccessTypeWork->leniHyst;
								pAccessTypeWork->leniHystNext = pAccessTypeWork->leniHystAlign;
								len = countEventInstance * pAccessTypeWork->leniHystNext;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offiEvent = pAccessTypeWork->offiHyst + len;
								pAccessTypeWork->leniEvent = sizeof(ACCESS_ITEM_EVENT_STATE);
								pAccessTypeWork->leniEventAlign = (pAccessTypeWork->leniEvent > 1 && pAccessTypeWork->leniEvent % 2 != 0) ? pAccessTypeWork->leniEvent + 1 : pAccessTypeWork->leniEvent;
								pAccessTypeWork->leniEventNext = countItem * pAccessTypeWork->leniEventAlign;
								len = countEventInstance * pAccessTypeWork->leniEventNext;
								if (len % 2 != 0) len++;

								pAccessTypeWork->offFubInstance = pAccessTypeWork->offiEvent + len;
								pAccessTypeWork->lenFubInstance = sizeof(ACCESS_FUB_INSTANCE);
								pAccessTypeWork->lenFubInstanceAlign = (pAccessTypeWork->lenFubInstance > 1 && pAccessTypeWork->lenFubInstance % 2 != 0) ? pAccessTypeWork->lenFubInstance + 1 : pAccessTypeWork->lenFubInstance;
								len = countItem * pAccessTypeWork->lenFubInstanceAlign;
								if (len % 2 != 0) len++;

								pAccessDataWork = (ACCESS_DATA*)((unsigned char*)pAccessDataWork + pAccessTypeWork->offFubInstance + len);
							} /* (i=0; i<countDefinedPoint; i++) */

							/* ---------------- */
							/* nächster Zustand */
							/* ---------------- */
							ParserState = cCD_PARSER_STATE_COMPARE_DO_DATA;

						} /* (StateTMP_alloc == 0) */
						else {
							/* Fehler bei TMP_alloc */
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					} /* (StateTMP_alloc == 0) */
					else {
						/* Fehler bei TMP_alloc */
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ==================================== */
				/* DFA-Zustand: Datenobjekt vorhanden ? */
				/* ==================================== */
				case cCD_PARSER_STATE_COMPARE_DO_DATA:
					memSize = (unsigned long)pAccessDataWork;
					if (DbsMemTypeData == 0) {
						StateTMP_alloc = TMP_alloc(memSize, (void*)&pAccessData);
						if (StateTMP_alloc == 0) {
							SmemSize += 0;
							StmpSize += memSize;
							memset(pAccessData, 0, memSize);
							ParserState = cCD_PARSER_STATE_WORK_DATA;
						}
						else {
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					}
					else {
						iDatObjInfo.enable = TRUE;
						iDatObjInfo.pName = (unsigned long)"dbsData";
						DatObjInfo(&iDatObjInfo);
						if (iDatObjInfo.status == 0) {
							/* Datenmodul-Alignment beachten */
							if (iDatObjInfo.len >= memSize &&
								iDatObjInfo.len <  memSize + sizeof(unsigned long) &&
								DbsTypeChanged  == FALSE &&
								DbsPointChanged == FALSE)
							{
								pAccessData = (ACCESS_DATA*)iDatObjInfo.pDatObjMem;
								ParserState = cCD_PARSER_STATE_WORK_DATA;
							}
							else {
								ParserState = cCD_PARSER_STATE_DEL_DO_DATA;
							}
						}
						else if (iDatObjInfo.status == doERR_MODULNOTFOUND) {
							ParserState = cCD_PARSER_STATE_CREATE_DO_DATA;
						}
						else if (iDatObjInfo.status != ERR_FUB_BUSY) {
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					}
					break;

				/* ================================ */
				/* DFA-Zustand: Datenobjekt löschen */
				/* ================================ */
				case cCD_PARSER_STATE_DEL_DO_DATA:
					iDatObjDelete.enable = TRUE;
					iDatObjDelete.ident = iDatObjInfo.ident;
					DatObjDelete(&iDatObjDelete);
					if (iDatObjDelete.status == 0) {
						ParserState = cCD_PARSER_STATE_CREATE_DO_DATA;
					}
					else if (iDatObjDelete.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ==================================== */
				/* DFA-Zustand: Datenobjekt neu anlegen */
				/* ==================================== */
				case cCD_PARSER_STATE_CREATE_DO_DATA:
					iDatObjCreate.enable = TRUE;
					iDatObjCreate.grp = 0;
					iDatObjCreate.pName = (unsigned long)"dbsData";
					iDatObjCreate.len = memSize;
					iDatObjCreate.MemType = doUSRRAM;
					iDatObjCreate.Option = doNO_CS;
					iDatObjCreate.pCpyData = NULL;
					DatObjCreate(&iDatObjCreate);
					if (iDatObjCreate.status == 0) {
						pAccessData = (ACCESS_DATA*)iDatObjCreate.pDatObjMem;
						SmemSize += 0;
						StmpSize += memSize;
						memset(pAccessData, 0, memSize);
						ParserState = cCD_PARSER_STATE_WORK_DATA;
					}
					else if (iDatObjCreate.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ======================================= */
				/* DFA-Zustand: Zählen der DBS-Datenpunkte */
				/* ======================================= */
				case cCD_PARSER_STATE_WORK_DATA:

					/* ------------------------------ */
					/* Pointer auf Datenpunkte setzen */
					/* ------------------------------ */
					for (i=0; i<countDefinedPoint; i++) {
						DbsUnit = pPointDesc[i].dbsUnit;
						DbsType = pPointDesc[i].dbsType;
						DbsPoint = pPointDesc[i].dbsPoint;
						pAccessUnit[DbsUnit].pAccessType[DbsType].pAccessPoint[DbsPoint].pAccessData += (unsigned long)pAccessData;
					}

					/* ---------------------- */
					/* Fubinstanzen eintragen */
					/* ---------------------- */
					for (i=0; i<countFubInstance; i++) {
						DbsUnit = pFubInstanceDesc[i].dbsUnit;
						DbsType = pFubInstanceDesc[i].dbsType;
						DbsPoint = pFubInstanceDesc[i].dbsPoint;
						DbsItem = pFubInstanceDesc[i].dbsItem;
						pAccessTypeWork  = &pAccessUnit[DbsUnit].pAccessType[DbsType];
						pAccessPointWork = &pAccessUnit[DbsUnit].pAccessType[DbsType].pAccessPoint[DbsPoint];
						pAccessFubInstance = (USINT**)(pAccessPointWork->pAccessData + pAccessTypeWork->offFubInstance + DbsItem * pAccessTypeWork->lenFubInstanceAlign);
						*pAccessFubInstance = pFubInstanceDesc[i].pFubInstance;
					}

					/* ---------------- */
					/* nächster Zustand */
					/* ---------------- */
					ParserState = cCD_PARSER_STATE_COMPARE_DO_EVENT;
					break;

				/* ==================================== */
				/* DFA-Zustand: Datenobjekt vorhanden ? */
				/* ==================================== */
				case cCD_PARSER_STATE_COMPARE_DO_EVENT:
					memSize = (maxDbsType + 1) * sizeof(ACCESS_EVENT_PIPE);
					for (i=0; i<=maxDbsType; i++) {
						if (pTypeDesc[i].pItemDesc != NULL && pTypeDesc[i].countRelatedPoints > 0) {
							memSize += countEventInstance * sizeof(EVENT_PIPE_INSTANCE);
							memSize += 	3 * countEventInstance *
										(
											sizeof(EVENT_PIPE_HEADER) +
											(pTypeDesc[i].countRelatedPoints + 1)
											*
											(
												pTypeDesc[i].countItem * sizeof(EVENT_PIPE_DATA) +
												pTypeDesc[i].sizeType +
												pTypeDesc[i].countTimeItem * sizeof(RTCtime_typ)
											)
										);
						}
					}
					if (DbsMemTypeEvent == 0) {
						StateTMP_alloc = TMP_alloc(memSize, (void*)&pEventPipe);
						if (StateTMP_alloc == 0) {
							SmemSize += 0;
							StmpSize += memSize;
							memset(pEventPipe, 0, memSize);
							ParserState = cCD_PARSER_STATE_WORK_EVENT;
						}
						else {
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					}
					else {
						iDatObjInfo.enable = TRUE;
						iDatObjInfo.pName = (unsigned long)"dbsEvent";
						DatObjInfo(&iDatObjInfo);
						if (iDatObjInfo.status == 0) {
							/* Datenmodul-Alignment beachten */
							if (iDatObjInfo.len >= memSize &&
								iDatObjInfo.len <  memSize + sizeof(unsigned long) &&
								DbsTypeChanged  == FALSE &&
								DbsPointChanged == FALSE)
							{
								pEventPipe = (ACCESS_EVENT_PIPE*)iDatObjInfo.pDatObjMem;
								ParserState = cCD_PARSER_STATE_WORK_EVENT;
							}
							else {
								ParserState = cCD_PARSER_STATE_DEL_DO_EVENT;
							}
						}
						else if (iDatObjInfo.status == doERR_MODULNOTFOUND) {
							ParserState = cCD_PARSER_STATE_CREATE_DO_EVENT;
						}
						else if (iDatObjInfo.status != ERR_FUB_BUSY) {
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					}
					break;

				/* ================================ */
				/* DFA-Zustand: Datenobjekt löschen */
				/* ================================ */
				case cCD_PARSER_STATE_DEL_DO_EVENT:
					iDatObjDelete.enable = TRUE;
					iDatObjDelete.ident = iDatObjInfo.ident;
					DatObjDelete(&iDatObjDelete);
					if (iDatObjDelete.status == 0) {
						ParserState = cCD_PARSER_STATE_CREATE_DO_EVENT;
					}
					else if (iDatObjDelete.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ==================================== */
				/* DFA-Zustand: Datenobjekt neu anlegen */
				/* ==================================== */
				case cCD_PARSER_STATE_CREATE_DO_EVENT:
					iDatObjCreate.enable = TRUE;
					iDatObjCreate.grp = 0;
					iDatObjCreate.pName = (unsigned long)"dbsEvent";
					iDatObjCreate.len = memSize;
					iDatObjCreate.MemType = doUSRRAM;
					iDatObjCreate.Option = doNO_CS;
					iDatObjCreate.pCpyData = NULL;
					DatObjCreate(&iDatObjCreate);
					if (iDatObjCreate.status == 0) {
						pEventPipe = (ACCESS_EVENT_PIPE*)iDatObjCreate.pDatObjMem;
						SmemSize += 0;
						StmpSize += memSize;
						memset(pEventPipe, 0, memSize);
						ParserState = cCD_PARSER_STATE_GET_DEVMEMINFO;
					}
					else if (iDatObjCreate.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ==================================== */
				/* DFA-Zustand: Datenobjekt neu anlegen */
				/* ==================================== */
				case cCD_PARSER_STATE_GET_DEVMEMINFO:
					iDevMemInfo.enable = TRUE;
					iDevMemInfo.pDevice = (UDINT)"CompactFlash";
					DevMemInfo(&iDevMemInfo);
					if (iDevMemInfo.status == 0) {
						ParserState = cCD_PARSER_STATE_WORK_EVENT;
					}
					else if (iDevMemInfo.status != ERR_FUB_BUSY) {
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

				/* ==================================== */
				/* DFA-Zustand: Datenobjekt neu anlegen */
				/* ==================================== */
				case cCD_PARSER_STATE_WORK_EVENT:

					/* ---------- */
					/* Event-Pipe */
					/* ---------- */
					memSize = (maxDbsType + 1) * sizeof(ACCESS_EVENT_PIPE);
					for (i=0; i<=maxDbsType; i++) {
						if (pTypeDesc[i].pItemDesc != NULL && pTypeDesc[i].countRelatedPoints > 0) {
							memSize += countEventInstance * sizeof(EVENT_PIPE_INSTANCE);
							memSize += 	3 *	countEventInstance *
										(
											sizeof(EVENT_PIPE_HEADER) +
											(pTypeDesc[i].countRelatedPoints + 1)
											*
											(
												pTypeDesc[i].countItem * sizeof(EVENT_PIPE_DATA) +
												pTypeDesc[i].sizeType +
												pTypeDesc[i].countTimeItem * sizeof(RTCtime_typ)
											)
										);
						}
					} /* (i=0; i<=maxDbsType; i++) */

					StateTMP_alloc = TMP_alloc(memSize, (void*)&pEventPipe);
					if (StateTMP_alloc == 0) {
						SmemSize += 0;
						StmpSize += memSize;
						memset(pEventPipe, 0, memSize);
						pEventPipeInstance = (EVENT_PIPE_INSTANCE*)(pEventPipe + (maxDbsType + 1));

						pEventPipeHeader = (EVENT_PIPE_HEADER*)pEventPipeInstance;
						for (i=0; i<=maxDbsType; i++) {
							if (pTypeDesc[i].pItemDesc != NULL && pTypeDesc[i].countRelatedPoints > 0) {
							//	(USINT*)pEventPipeHeader += countEventInstance * sizeof(EVENT_PIPE_INSTANCE);
/*BIE*/						//	pEventPipeHeader = pEventPipeHeader + countEventInstance * sizeof(EVENT_PIPE_INSTANCE);
								pEventPipeHeader = (EVENT_PIPE_HEADER*)((USINT*)pEventPipeHeader + countEventInstance * sizeof(EVENT_PIPE_INSTANCE));
							}
						}

						for (i=0; i<=maxDbsType; i++) {
							if (pTypeDesc[i].pItemDesc != NULL && pTypeDesc[i].countRelatedPoints > 0) {
								if (pTypeDesc[i].FileBuffer == TRUE) {
									pTypeDesc[i].EventFileBuffer = BufEvents;
									pTypeDesc[i].SizeFileBuffer  = pTypeDesc[i].EventFileBuffer * (sizeof(EVENT_PIPE_DATA) + sizeof(long));
									pTypeDesc[i].ModeFileBuffer  = BufType;
									pEventPipe[i].WriteToFile    = (pTypeDesc[i].SizeFileBuffer > 0) ? TRUE : FALSE;
									pEventPipe[i].SizeFileBuffer = pTypeDesc[i].SizeFileBuffer;
									pEventPipe[i].ModeFileBuffer = pTypeDesc[i].ModeFileBuffer;
								}
								pEventPipe[i].pEventPipeInstance = pEventPipeInstance;

								for (j=0; j<countEventInstance; j++) {
									pEventPipeInstance->pEventPipeHeaderWrite[0] = pEventPipeHeader;
									pEventPipeHeader->pipe_entry = pTypeDesc[i].countRelatedPoints + 1;
									pEventPipeHeader->pipe_size  = pEventPipeHeader->pipe_entry * (	pTypeDesc[i].countItem * sizeof(EVENT_PIPE_DATA) +
																									pTypeDesc[i].sizeType +
																									pTypeDesc[i].countTimeItem * sizeof(RTCtime_typ));
									pEventPipeHeader->pPipeData  = (unsigned char*)&pEventPipeHeader[1];
									pEventPipeHeader->iPipeRead  = 0;
									pEventPipeHeader->iPipeWrite = 0;
									pEventPipeHeader = (EVENT_PIPE_HEADER*)((unsigned char*)pEventPipeHeader->pPipeData + pEventPipeHeader->pipe_size);

									pEventPipeInstance->pEventPipeHeaderWrite[1] = pEventPipeHeader;
									pEventPipeHeader->pipe_entry = pTypeDesc[i].countRelatedPoints + 1;
									pEventPipeHeader->pipe_size  = pEventPipeHeader->pipe_entry * (	pTypeDesc[i].countItem * sizeof(EVENT_PIPE_DATA) +
																									pTypeDesc[i].sizeType +
																									pTypeDesc[i].countTimeItem * sizeof(RTCtime_typ));
									pEventPipeHeader->pPipeData  = (unsigned char*)&pEventPipeHeader[1];
									pEventPipeHeader->iPipeRead  = 0;
									pEventPipeHeader->iPipeWrite = 0;
									pEventPipeHeader = (EVENT_PIPE_HEADER*)((unsigned char*)pEventPipeHeader->pPipeData + pEventPipeHeader->pipe_size);

									pEventPipeInstance->pEventPipeHeaderRead = pEventPipeHeader;
									pEventPipeHeader->pipe_entry = pTypeDesc[i].countRelatedPoints + 1;
									pEventPipeHeader->pipe_size  = pEventPipeHeader->pipe_entry * (	pTypeDesc[i].countItem * sizeof(EVENT_PIPE_DATA) +
																									pTypeDesc[i].sizeType +
																									pTypeDesc[i].countTimeItem * sizeof(RTCtime_typ));
									pEventPipeHeader->pPipeData  = (unsigned char*)&pEventPipeHeader[1];
									pEventPipeHeader->iPipeRead  = 0;
									pEventPipeHeader->iPipeWrite = 0;
									pEventPipeHeader = (EVENT_PIPE_HEADER*)((unsigned char*)pEventPipeHeader->pPipeData + pEventPipeHeader->pipe_size);

									pEventPipeInstance++;
								} /* (j=0; j<countEventInstance; j++) */
							} /* (pTypeDesc[i].pItemDesc != NULL && pTypeDesc[i].countRelatedPoints > 0) */
						} /* (i=0; i<=maxDbsType; i++) */

						/* ---------- */
						/* Write-Pipe */
						/* ---------- */
						memSize = sizeof(ACCESS_WRITE_PIPE) + countWriteInstance * (sizeof(WRITE_PIPE_HEADER) + 64 * (sizeof(WRITE_PIPE_DATA) + sizeof(UDINT)));
						StateTMP_alloc = TMP_alloc(memSize, (void*)&pWritePipe);
						if (StateTMP_alloc == 0) {
							SmemSize += 0;
							StmpSize += memSize;
							memset(pWritePipe, 0, memSize);
							pWritePipeHeader = (WRITE_PIPE_HEADER*)&pWritePipe[1];
							pWritePipe->pWritePipeHeader = pWritePipeHeader;
							for (j=0; j<countWriteInstance; j++) {
								pWritePipeHeader->pipe_entry = 64;
								pWritePipeHeader->pipe_size = pWritePipeHeader->pipe_entry * (sizeof(WRITE_PIPE_DATA) + sizeof(UDINT));
								pWritePipeHeader->pPipeData = (unsigned char*)&pWritePipeHeader[1];
								pWritePipeHeader->iPipeRead = 0;
								pWritePipeHeader->iPipeWrite = 0;
								if (j == 0) {
									pWritePipe->length_pipe_next = sizeof(WRITE_PIPE_HEADER) + pWritePipeHeader->pipe_size;
								}
								pWritePipeHeader = (WRITE_PIPE_HEADER*)((unsigned char*)pWritePipeHeader + pWritePipe->length_pipe_next);
							} /* (j=0; j<countWriteInstance; j++) */

							/* ------------------------------------- */
							/* Speicher für Serverzugriff allokieren */
							/* ------------------------------------- */
							memSize = sizeof(ACCESS_SERVER);
							StateTMP_alloc = TMP_alloc(memSize, (void*)&pAccessServer);
							if (StateTMP_alloc == 0) {
								SmemSize += 0;
								StmpSize += memSize;
								memset(pAccessServer, 0, memSize);

								pAccessServer->countEventInstance = countEventInstance;
								pAccessServer->countWriteInstance = countWriteInstance;
								pAccessServer->countType = maxDbsType + 1;
								pAccessServer->countUnit = maxDbsUnit + 1;
								pAccessServer->pAccessUnit = pAccessUnit;
								pAccessServer->pAccessTypeDesc = pTypeDesc;
								pAccessServer->pAccessPointDesc = pPointDesc;
								pAccessServer->noDevice = cDBS_DEFAULT_DEVICE;
								pAccessServer->pUnitEventState = 0;
								pAccessServer->pAccessEventPipe = pEventPipe;
								pAccessServer->pAccessWritePipe = pWritePipe;
								pAccessServer->PortNumber = PortNumber;
								pAccessServer->TimeoutValue = TimeoutValue;

								idBrServer = pAccessServer;
							}

							/* --------------- */
							/* Logbuch-Ausgabe */
							/* --------------- */
							ultoa((unsigned long) SmemSize / 1024, strcpy(logBuffer, ET_MEM_ALLOC_NEED) + strlen(ET_MEM_ALLOC_NEED));
							ultoa((unsigned long)(SmemSize % 1024 + 50) / 100, strcat(logBuffer,".") + strlen(logBuffer));
							ultoa((unsigned long) StmpSize / 1024, strcat(logBuffer, ET_TMP_ALLOC_NEED) + strlen(logBuffer));
							ultoa((unsigned long)(StmpSize % 1024 + 50) / 100, strcat(logBuffer,".") + strlen(logBuffer));
							strcat(logBuffer, "kB");
							ErrStatistics("dbserv", ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);

							ultoa((unsigned long)countExistPoints, strcpy(logBuffer, ET_DPS_FOUND) + strlen(ET_DPS_FOUND));
							ultoa((unsigned long)countNotExistPoints, strcat(logBuffer, ET_DPS_NOT_FOUND) + strlen(logBuffer));
							ultoa((unsigned long)countNotExistItems, strcat(logBuffer, ET_ITEM_NOT_FOUND) + strlen(logBuffer));
							if (countInvalidExistItems > 0) {
								strcat(logBuffer, "\n");
								ultoa((unsigned long)countInvalidExistItems, strcat(logBuffer, ET_ITEM_INVALID_FOUND) + strlen(logBuffer));
							}
							ErrStatistics("dbserv", ERRLOG, 0, countNotExistPoints, ERR_DBS_PRE, logBuffer);
							ultoa((unsigned long)BufEvents, strcpy(logBuffer, ET_BUF_EVENTS) + strlen(ET_BUF_EVENTS));
							ErrStatistics("dbserv", ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);

							/* Berechnung der Speichergrösse für File */
							memSize = (maxDbsType + 1) * sizeof(ACCESS_EVENT_PIPE);
							for (i=0; i<=maxDbsType; i++) {
								if (pTypeDesc[i].pItemDesc != NULL && pTypeDesc[i].countRelatedPoints > 0) {
									memSize += countEventInstance * sizeof(EVENT_PIPE_INSTANCE);
									memSize += countEventInstance *
												(
													sizeof(EVENT_PIPE_HEADER) +
													BufEvents *
													(
														pTypeDesc[i].countItem * sizeof(EVENT_PIPE_DATA) +
														pTypeDesc[i].sizeType +
														pTypeDesc[i].countTimeItem * sizeof(RTCtime_typ)
													)
												);
								}
							}
							ultoa((unsigned long)memSize, strcpy(logBuffer, ET_MEM_EVENTS) + strlen(ET_MEM_EVENTS));
							strcat(logBuffer, "kB");
							ErrStatistics("dbserv", ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);

							strcat(strcpy(logBuffer, BK_VERSION_TEXT), ET_TCP_READY);
							ErrStatistics("dbserv", ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);

							/* ---------------- */
							/* nächster Zustand */
							/* ---------------- */
							ParserState = cCD_PARSER_STATE_READY;

						} /* (StateTMP_alloc == 0) */
						else {
							/* Fehler bei TMP_alloc */
							ParserState = cCD_PARSER_STATE_ERROR;
						}
					} /* (StateTMP_alloc == 0) */
					else {
						/* Fehler bei TMP_alloc */
						ParserState = cCD_PARSER_STATE_ERROR;
					}
					break;

			} /* (ParserState) */

			/* ========================================= */
			/* Parser eventuell in Fehlerzustand bringen */
			/* ========================================= */
			/*if (State_find_StationTypPoint < -1 || State_find_Section < -1 || State_find_Data < -1) {*/
			if (State_find_StationTypPoint < -1 || State_find_Section < -1) {
				ParserState = cCD_PARSER_STATE_ERROR;
			}

			/* ======================================== */
			/* Parser in naechsten Zustand schalten ... */
			/* ======================================== */
			switch (ParserState)
			{
/*
				case cCD_PARSER_STATE_DEF_TCP:
					ParserState = cCD_PARSER_STATE_COUNT_TYPE;
					break;

				case cCD_PARSER_STATE_COUNT_TYPE:
					ParserState = cCD_PARSER_STATE_DEF_TYPE;
					break;

				case cCD_PARSER_STATE_DEF_TYPE:
					ParserState = cCD_PARSER_STATE_COMPARE_DO_TYPE;
					break;

				case cCD_PARSER_STATE_COUNT_DP:
					ParserState = cCD_PARSER_STATE_DEF_DP;
					break;

				case cCD_PARSER_STATE_DEF_DP:
					ParserState = cCD_PARSER_STATE_COMPARE_DO_POINT;
					break;

				case cCD_PARSER_STATE_ALLOC_MEM:
					ParserState = cCD_PARSER_STATE_READY;

					ultoa((unsigned long) SmemSize / 1024, strcpy(logBuffer, ET_MEM_ALLOC_NEED) + strlen(ET_MEM_ALLOC_NEED));
					ultoa((unsigned long)(SmemSize % 1024 + 50) / 100, strcat(logBuffer,".") + strlen(logBuffer));
					ultoa((unsigned long) StmpSize / 1024, strcat(logBuffer, ET_TMP_ALLOC_NEED) + strlen(logBuffer));
					ultoa((unsigned long)(StmpSize % 1024 + 50) / 100, strcat(logBuffer,".") + strlen(logBuffer));
					strcat(logBuffer, "kB");
					ErrStatistics(ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);

					ultoa((unsigned long)countExistPoints, strcpy(logBuffer, ET_DPS_FOUND) + strlen(ET_DPS_FOUND));
					ultoa((unsigned long)countNotExistPoints, strcat(logBuffer, ET_DPS_NOT_FOUND) + strlen(logBuffer));
					ultoa((unsigned long)countNotExistItems, strcat(logBuffer, ET_ITEM_NOT_FOUND) + strlen(logBuffer));
					if (countInvalidExistItems > 0) {
						strcat(logBuffer, "\n");
						ultoa((unsigned long)countInvalidExistItems, strcat(logBuffer, ET_ITEM_INVALID_FOUND) + strlen(logBuffer));
					}
					ErrStatistics(ERRLOG, 0, countNotExistPoints, ERR_DBS_PRE, logBuffer);

					strcat(strcpy(logBuffer, BK_VERSION_TEXT), ET_TCP_READY);
					ErrStatistics(ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);
					break;
*/
				case cCD_PARSER_STATE_READY:
					CopyPipeToFile(idBrServer);
					break;

				case cCD_PARSER_STATE_ERROR:
					ParserState = cCD_PARSER_STATE_ERROR_READY;
					/* Fehlermeldung */
					if (State_find_StationTypPoint < -1 || State_find_Section < -1 || State_find_Data < -1) {
						strcat(strcpy(logBuffer, (char*)iDatObjInfo.pName), ET_PARSER_STATE);
						if (State_find_StationTypPoint < -1) {
							ultoa(abs(State_find_StationTypPoint), logBuffer + strlen(logBuffer));
						}
						if (State_find_Section < -1) {
							ultoa(abs(State_find_Section), logBuffer + strlen(logBuffer));
						}
						if (State_find_Data < -1) {
							ultoa(abs(State_find_Data), logBuffer + strlen(logBuffer));
						}
						strcat(logBuffer, "\n" );
						sMsgMaxLen = ERRTEXT_MAXLEN - strlen(ERR_DBS_PRE) - 2;
						if (strlen( pParserLine ) > sMsgMaxLen) {
							if (pParserErr - pParserLine > sMsgMaxLen) {
								pParserLine = pParserErr - (sMsgMaxLen * 3) / 4;
							}
						}
						strncat(logBuffer, pParserLine, sMsgMaxLen);
						ErrStatistics("dbserv", ERRLOG, EC_DBS_BASE + EC_PARSER_STATE, CountLines, ERR_DBS_PRE, logBuffer);
					}
					ultoa((unsigned long) SmemSize / 1024, strcpy(logBuffer, ET_MEM_ALLOC_NEED) + strlen(ET_MEM_ALLOC_NEED));
					ultoa((unsigned long)(SmemSize % 1024 + 50) / 100, strcat(logBuffer,".") + strlen(logBuffer));
					ultoa((unsigned long) StmpSize / 1024, strcat(logBuffer, ET_TMP_ALLOC_NEED) + strlen(logBuffer));
					ultoa((unsigned long)(StmpSize % 1024 + 50) / 100, strcat(logBuffer,".") + strlen(logBuffer));
					strcat(logBuffer, "kB");
					ErrStatistics("dbserv", ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);

					ultoa((unsigned long)countExistPoints, strcpy(logBuffer, ET_DPS_FOUND) + strlen(ET_DPS_FOUND));
					ultoa((unsigned long)countNotExistPoints, strcat(logBuffer, ET_DPS_NOT_FOUND) + strlen(logBuffer));
					ultoa((unsigned long)countNotExistItems, strcat(logBuffer, ET_ITEM_NOT_FOUND) + strlen(logBuffer));
					if (countInvalidExistItems > 0) {
						strcat(logBuffer, "\n");
						ultoa((unsigned long)countInvalidExistItems, strcat(logBuffer, ET_ITEM_INVALID_FOUND) + strlen(logBuffer));
					}
					ErrStatistics("dbserv", ERRLOG, 0, countNotExistPoints, ERR_DBS_PRE, logBuffer);

					strcat(strcpy(logBuffer, BK_VERSION_TEXT), ET_TCP_NOT_READY);
					ErrStatistics("dbserv", ERRLOG, 0, 0, ERR_DBS_PRE, logBuffer);
					break;

				case cCD_PARSER_STATE_ERROR_READY:
					break;

/*
				default:
					ParserState = cCD_PARSER_STATE_ERROR;
					break;
*/

			} /* (ParserState) */
		/*}  (ParserState < cCD_PARSER_STATE_READY) */
	} /* (go == TRUE) */

	/* ------------ */
	/* Testroutinen */
	/* ------------ */
	if (ParserState == cCD_PARSER_STATE_READY) {

		iSysInfo.enable = TRUE;
		SysInfo(&iSysInfo);
		idBrServer->TickCount = iSysInfo.tick_count;

		/* Datentypen */
		pInfoTypeDesc = &pTypeDesc[infoType];
		pInfoItemDesc = pInfoTypeDesc->pItemDesc;
		if (pInfoTypeDesc->pTypeName != NULL) {
			strncpy(InfoTypeName, pInfoTypeDesc->pTypeName, sizeof(InfoTypeName));
		}

		/* Datenpunkte */
		pInfoPointDesc = &pPointDesc[infoPoint];

		SetValueItem (idBrServer,
						1, 1, 33, 0, 0,
						&bValue, sizeof(bValue),
						RTCTime, 0, 1);
	}
} /* _CYCLIC cyclic(void) */

/***********************************************************************/
/* Tcp-Funktionen                                                      */
/***********************************************************************/

unsigned char IsANumber(unsigned char *pData) {

	unsigned char	flag;
	unsigned short	i;

#define	cMAX_CHARACTERS	16


	flag = FALSE;
	if (pData != NULL) {
		for (i=0; i<cMAX_CHARACTERS; i++) {
			if (pData[i] == 0) {
				break;
			}
			else if (pData[i] == ' ' && flag == TRUE) {
				break;
			}
			else if (pData[i]>='0' && pData[i]<='9') {
				flag = TRUE;
			}
			else {
				break;
			}
		}
		if (i == cMAX_CHARACTERS) {
			flag = FALSE;
		}
	}
	return flag;
}

#ifdef x
/*
	ltoa  converts a signed long  value into ASCII Format
	ultoa converts a unsigned long  value into ASCII Format

	both returns the length of the converted string.
*/

int ultoa(unsigned long value, char *buffer) {

	int string_len=0, i, j;
	char swap;


	do {
		buffer[string_len++]=(value % 10) + '0'; /*calculate next digit*/
	} while((value = value/10)); /*while there are more digits*/

	/*swap digits*/
	i=0;
	j=string_len-1;

	while (i<j) {
		swap = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = swap;
		i++; j--;
	}
	buffer[string_len]=0; /*NULL termination of string*/
	return string_len;    /*Return lenght of string*/
}

int ltoa(long value, char *buffer) {

	int string_len=0, neg=0, i, j;
	char swap;
	unsigned long absval;  /*we have to use unsigned long because, with signed long
	                         0x80000000 would not work.*/

	if (value<0) {
		absval = -value;
		neg=1;
		buffer[0]='-';
		string_len=1;
	}
	else {
		absval = value;
	}
	do {
		buffer[string_len++]=(absval % 10) + '0'; /*calculate next digit*/
	} while((absval = absval/10)); /*while there are more digits*/

	/*swap digits*/
	i=neg;  /*begin at pos 1 if negative*/
	j=string_len-1;

	while (i<j) {
		swap = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = swap;
		i++; j--;
	}
	buffer[string_len]=0; /*NULL termination of string*/
	return string_len;    /*Return lenght of string*/
}
#endif
/*============================= Ende der Datei ==============================*/



