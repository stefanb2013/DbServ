#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  I/O-Treiber
*  Teil:     Parserfunktionen
*  Datei:    iodrv_parse.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    05-04-2006
*  Autor:    WAI
*------------------------------------------------------------------------------
*  Funktion:
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 05-04-06 | WAI  | Datei-Erstellung
*  02.00 | 10-07-26 | BIE  | strnicmp gegen strncasecmp ersetzt
*        |          |      |
******************************************************************************/
#include <bur\plc.h>			/* RPS-spezifische Typendeklarationen */
#include <string.h>
#include <dvframe.h>
#include <sys_lib.h>
#include <asbrstr.h>
#include <brsystem.h>
#include "iodrv.h"
#include "iodrv_parse.h"
#include "parser.h"


/**********************************************************
*		Konfiguration aus Datenmodul auslesen
**********************************************************/
void iodrv_parser()
{
	plcbit				BlockFound,DPFound,TypFound,ItemFound;
	IODRV_DP_Info_typ		*pIODRV_DP_Info;
	unsigned char ANr[20];

	memset(&IODRV_ParserInfo,0,sizeof(IODRV_ParserInfo));
	IODRV_ParserInfo.ErrorMsg = 1;									/* Freigabe: Fehlermeldungen ins Logbuch eintragen */

	/*--------------------------------------------------*/
	/* Info ueber Datenmodul dplist und typlist besorgen*/
	/*--------------------------------------------------*/

	DOI_dplist.enable	= 1;
	DOI_dplist.pName	= (unsigned long) DATA_OBJEKT_DPLIST;
	DatObjInfo((DatObjInfo_typ*) &DOI_dplist);

	DOI_typlist.enable	= 1;
	DOI_typlist.pName	= (unsigned long) DATA_OBJEKT_TYPLIST;
	DatObjInfo((DatObjInfo_typ*) &DOI_typlist);

	/*--------------------------------------------------*/
	/* Fehlermeldung: Datenmodul dplist nicht vorhanden */
	/*--------------------------------------------------*/
	if(DOI_typlist.status) {
		iodrv_parser_err(PARS_ERR_DPLIST_NOT_FOUND, "Datenmodul fehlt:", (unsigned char *) DOI_dplist.pName, 0, 0, 0, 0, 0, 0);
	}

	/*--------------------------------------------------*/
	/* Fehlermeldung: Datenmodul typlist nicht vorhanden */
	/*--------------------------------------------------*/
	if(DOI_typlist.status) {
		iodrv_parser_err(PARS_ERR_TYPLIST_NOT_FOUND, "Datenmodul fehlt:", (unsigned char *) DOI_typlist.pName, 0, 0, 0, 0, 0, 0);
	}

	/*--------------------------------------------------*/
	/* Globale Parameter lesen							*/
	/*--------------------------------------------------*/
	if(!IODRV_ParserInfo.error) {

		iodrv_parser_dplist_clear();									/* Parsen von Beginn an */

		memset(&IODRV_GlobalData,0,sizeof(IODRV_GlobalData));			/* Parameterstruktur loeschen */

		iodrv_parser_global();											/* Parameter parsen */
	}

	/*------------------------------------------------------*/
	/* Anzahl Datenpunkte und Items ermitteln				*/
	/*------------------------------------------------------*/

	if(!IODRV_ParserInfo.error) {

		IODRV_ParserInfo.ErrorMsg = 0;															/* Keine Eintrage ins Logbuch */
		iodrv_parser_dplist_clear();															/* dplist von Beginn an Parsen */
		memset(&IODRV_BlockData,0,sizeof(IODRV_BlockData));										/* Info ueber Blockparameter loeschen */

		do {
			memset(&IODRV_DPData,0,sizeof(IODRV_DPData));											/* Info ueber Datenpunkt loeschen */
			copyItemData((IODRV_ItemData_typ *) &IODRV_DPData,(IODRV_ItemData_typ *) &IODRV_BlockData);	/* Blockparameter uebernehmen */

			BlockFound	= 0;
			DPFound		= 0;

			iodrv_parser_dpinfo((plcbit *) &BlockFound,(plcbit *) &DPFound);						/* Nächsten Datenpunkt für IODRV-Treiber suchen */

			if(DPFound) {																	/* Datenpunkt für IODRV-Treiber gefunden */
				IODRV_ParserInfo.NrOfDataPoints++;											/* Anzahl Datenpunkte pro Linie zählen */
				if(IODRV_DPData.Typ.defined) {												/* Ist zu dem Datenpunkt der Datentyp definiert ? */
					iodrv_parser_typlist_clear();											/* typlist von Beginn an Parsen */
					TypFound = iodrv_parser_dptype(IODRV_DPData.Typ.name, IODRV_DPData.Typ.size); /* Datentyp in typlist suchen */
					if(TypFound) {															/* Datentyp wurde in typlist gefunden */
						/* Anzahl Items des Datenpunktes ermitteln */
						do {
							memset(&IODRV_ItemData,0,sizeof(IODRV_ItemData));				/* Struktur für ITEM-Parameter loeschen */
							ItemFound = iodrv_parser_iteminfo();							/* Item in typlist suchen */
							if(ItemFound) {													/* Item wurde gefunden */
								copyItemData((IODRV_ItemData_typ *) &IODRV_ItemData,(IODRV_ItemData_typ *) &IODRV_DPData);
								IODRV_ParserInfo.NrOfItems++;								/* Anzahl der Items ermitteln */
							}
						} while(ItemFound);

					} /* if(TypFound) */
				} /* if(IODRV_DPData.Typ.defined) */
			} /* if(DPFound) */
		} while(DPFound || BlockFound);
	} /* if(!IODRV_ParserInfo.error) */


	/*------------------------------------------------------*/
	/* Speicher fuer alle Items allokieren					*/
	/*------------------------------------------------------*/

	if(!IODRV_ParserInfo.error) {
		IODRV_ParserInfo.ErrorMsg	= 1;					/* Freigabe: Fehlermeldungen ins Logbuch eintragen */
		IODRV_ParserInfo.memSize = IODRV_ParserInfo.NrOfItems * sizeof(IODRV_DP_Info_typ);
		IODRV_ParserInfo.allocStatus = TMP_alloc(IODRV_ParserInfo.memSize, (void**) &IODRV_ParserInfo.memAdr); 	/* allocate new memory from RAM*/
		if(IODRV_ParserInfo.allocStatus) {
			iodrv_parser_err(PARS_ERR_MEM_ALLOC, "Fehler TMP_alloc:", 0, "Groesse:", IODRV_ParserInfo.memSize, "Status:", (long) IODRV_ParserInfo.allocStatus, 0, 0);
		}
		else {
			memset((void *) IODRV_ParserInfo.memAdr,0,IODRV_ParserInfo.memSize);
		}
	}

	/*-----------------------------------------------------------------------*/
	/* Iteminformationen auslesen und in Speicher eintragen     */
	/*-----------------------------------------------------------------------*/

	if(!IODRV_ParserInfo.error) {

		iodrv_parser_dplist_clear();																/* dplist von Beginn an Parsen */
		memset(&IODRV_BlockData,0,sizeof(IODRV_BlockData));											/* Info ueber Blockparameter loeschen */

		do {
			memset(&IODRV_DPData,0,sizeof(IODRV_DPData));											/* Info ueber Datenpunkt loeschen */
			copyItemData((IODRV_ItemData_typ *) &IODRV_DPData,(IODRV_ItemData_typ *) &IODRV_BlockData);	/* Blockparameter uebernehmen */
			BlockFound	= 0;
			DPFound		= 0;
			iodrv_parser_dpinfo((plcbit *) &BlockFound,(plcbit *) &DPFound);						/* Nächsten Datenpunkt für IODRV-Treiber suchen */
			if(DPFound) {																		/* Datenpunkt für IODRV-Treiber gefunden */
				if(iodrv_parser_dpinfo_ok((IODRV_ItemData_typ *) &IODRV_DPData)) {					/* Alle notwendigen Angaben vorhanden und korrekt ? */
																								/* Zugehoerigen Linienindex suchen */
					iodrv_parser_typlist_clear();												/* typlist von Beginn an Parsen */
					TypFound = iodrv_parser_dptype(IODRV_DPData.Typ.name, IODRV_DPData.Typ.size); /* Datentyp in typlist suchen */
					if(TypFound) {															/* Datentyp wurde in typlist gefunden */
						/* Anzahl Items des Datenpunktes ermitteln */
						do {
							memset(&IODRV_ItemData,0,sizeof(IODRV_ItemData));					/* Struktur für ITEM-Parameter loeschen */
																								/* Parameter des Datenpunktes auf Item übertragen */
							copyItemData((IODRV_ItemData_typ *) &IODRV_ItemData,(IODRV_ItemData_typ *) &IODRV_DPData);

							ItemFound = iodrv_parser_iteminfo();								/* Item in typlist suchen */
							if(ItemFound) {													/* Item wurde gefunden */


																							/* Pruefen, ob Parameter vollstaendig und korrekt sind */
								if(iodrv_parser_iteminfo_ok((IODRV_ItemData_typ *) &IODRV_ItemData)) {
									pIODRV_DP_Info = (IODRV_DP_Info_typ *) (IODRV_ParserInfo.memAdr);
									pIODRV_DP_Info += IODRV_ParserInfo.ActItemIdx;

																						/* Daten in Speicher eintragen */
									pIODRV_DP_Info->AnlagenNr	= (UINT)	IODRV_ItemData.AnlagenNr.value;
									pIODRV_DP_Info->DPNr		= (UINT)	IODRV_ItemData.DPNr.value;
									pIODRV_DP_Info->DPTypNr		= (UINT)	IODRV_ItemData.DPTypNr.value;
									pIODRV_DP_Info->DPName		= (USINT *)	IODRV_ItemData.DPName.name;
									pIODRV_DP_Info->DPNameLen	= (UINT)	IODRV_ItemData.DPName.size;
									pIODRV_DP_Info->ItemNr		= (UINT)	IODRV_ItemData.ItemNr.value;
									pIODRV_DP_Info->ItemTypNr	= (USINT)	IODRV_ItemData.ItemTypNr.value;
									pIODRV_DP_Info->ItemName	= (USINT *)	IODRV_ItemData.ItemName.name;
									pIODRV_DP_Info->ItemNameLen	= (UINT)	IODRV_ItemData.ItemName.size;
									pIODRV_DP_Info->Invers		= (USINT)	IODRV_ItemData.Invers.value;
									pIODRV_DP_Info->Quitt		= (USINT)	IODRV_ItemData.Quitt.value;
									pIODRV_DP_Info->Dir			= (USINT)	IODRV_ItemData.Dir.value;

									IODRV_ParserInfo.ActItemIdx++;
								} /* if(!error) */
							} /* if(!error) */
						} while(ItemFound);
					} /* if(TypFound) */
				} /* if(iodrv_parser_dpinfo_ok())*/
			} /* if(DPFound) */
		} while(DPFound || BlockFound);
	} /* if(!IODRV_ParserInfo.error) */

	strcpy(MsgTxt,ERR_MSG_PRE_STRG);
	strcat(MsgTxt," Parser beendet");
	ERRxwarning( 0, bin_to_bcd_8(0),MsgTxt);

	strcpy(MsgTxt,ERR_MSG_PRE_STRG);
	strcat(MsgTxt," Anzahl Meldungen ");
	brsitoa(IODRV_ParserInfo.NrOfLogEntrys, (UDINT) &ANr);
	strcat(MsgTxt,ANr);
	ERRxwarning( 0, bin_to_bcd_8((unsigned long) IODRV_ParserInfo.NrOfLogEntrys), MsgTxt);

}

/**********************************************************
*		Globale Parameter aus Datennmodul lesen
**********************************************************/
void iodrv_parser_global()
{

	BOOL			dataReady;

		/* ----------------------------------------- */
	 	/* Globale Eintraege fuer IODRV-Treiber parsen */
		/* ----------------------------------------- */

		/* Parser-Status loeschen */
		State_find_StationTypPoint	= 0;
		State_find_Section			= 0;
		State_find_Data				= 0;

		while (PaDPList.pParser < (unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len) {

			/* Suche nach Stations/Typ/Punkt-Beschreibung */
			State_find_StationTypPoint = find_StationTypPoint (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
																(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
																&PaDPList.pParser, &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
																&PaDPList.pDataArg1, &PaDPList.SizeArg1,
																&PaDPList.pDataArg2, &PaDPList.SizeArg2,
																&PaDPList.SectionType);
			/* keine Daten oder Fehler aufgetreten ? */
			if (State_find_StationTypPoint < 0) {
				break;
			}

			/* Pointer auf Station/Typ/Punkt */
			switch (PaDPList.SectionType) {

				/* ------------------ */
				/* Stations-Abschnitt */
				/* ------------------ */
				case cCD_STATION:
					do {
						State_find_Section = find_Section (
							(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
															(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
															&PaDPList.pParser, &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
															&PaDPList.pDataArg1, &PaDPList.SizeArg1,
															&PaDPList.pDataArg2, &PaDPList.SizeArg2,
															KW_IODRV_SECTION);

						/* keine Daten oder Fehler aufgetreten ? */
						if (State_find_Section < 0) {
							break;
						}

						/* Pointer auf Anfang der Sektion stellen */
						if (PaDPList.pDataArg2 != NULL) {
							PaDPList.pParser = PaDPList.pDataArg2;
						}
						else {
							continue;
						}

						/* alle Daten der Sektion bearbeiten */
						dataReady	= 0;
						do {
							/* Daten der Sektion anfordern */
							State_find_Data = find_Data (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
															(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
															  &PaDPList.pParser, &PaDPList.pParserLine, &PaDPList.pParserErr, NULL,
															  &PaDPList.pDataArg1, &PaDPList.SizeArg1,
															  &PaDPList.pDataArg2, &PaDPList.SizeArg2,
															  &PaDPList.pDataArg3, &PaDPList.SizeArg3);

							/* Zeilenende erreicht */
							if (State_find_Data == -1) {
								PaDPList.pParser++;
							}
							/* Daten vorhanden */
							else if (State_find_Data == TRUE) {
								/* noch keine globalen Daten vorhanden */
								if (0) {
								}
								/* Unbekannter Eintrag */
								else {
									iodrv_parser_warn(PARS_ERR_UNKNOWN_GLOBAL_ENTRY,"Eintrag unbekannt:", PaDPList.pParserLine, " / Zeile =", (long) PaDPList.CountLines, 0, 0, 0, 0);
								}
							}

							/* Endebedinung der Schleife: Keine Daten mehr vorhanden, oder Ende des Datenmodules erreicht */
							if((State_find_Data!=TRUE) && (State_find_Data!=-1))					dataReady = 1;
							if(PaDPList.pParser >= (unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len)	dataReady = 1;
						} while (!dataReady);
					} while (State_find_Section == TRUE);
					break;
				case cCD_TYP:
					break;
				case cCD_POINT:
					break;
			}
		}
}

/*****************************************************************
*		Informationen ueber einen Datenpunkt ermitteln
******************************************************************/
void iodrv_parser_dpinfo(plcbit *pBlockFound, plcbit *pDPFound)
{

		*pDPFound	= 0;
		*pBlockFound= 0;

		/* -------------------------------------------- */
	 	/* Datenpunkt Eintraege fuer IODRV-Treiber parsen */
		/* -------------------------------------------- */

		/* Parser-Status loeschen */
		State_find_StationTypPoint	= 0;
		State_find_Section			= 0;
		State_find_Data				= 0;

		while (!*pDPFound && !*pBlockFound && (PaDPList.pParser < (unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len)) {

			/* Suche nach Stations/Typ/Punkt-Beschreibung */
			State_find_StationTypPoint = find_StationTypPoint (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
																(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
																&PaDPList.pParser, &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
																&PaDPList.pDataArg1, &PaDPList.SizeArg1,
																&PaDPList.pDataArg2, &PaDPList.SizeArg2,
																&PaDPList.SectionType);
			/* keine Daten oder Fehler aufgetreten ? */
			if (State_find_StationTypPoint < 0) {
				break;
			}

			/* Pointer auf Station/Typ/Punkt */
			switch (PaDPList.SectionType) {
				/* ------------------ */
				/* Datentyp-Abschnitt */
				/* ------------------ */
				case cCD_POINT:

					/* Parserposition zwischenspeichern */
					memcpy(&PaDPListSave,&PaDPList,sizeof(PaDPListSave));

					pPointName = PaDPList.pDataArg1, lenPointName = PaDPList.SizeArg1;

					/* Suchen nach Sektion "IODRV" */
					do {
						State_find_Section = find_Section (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
															(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
															&PaDPList.pParser, 	 &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
															&PaDPList.pDataArg1, &PaDPList.SizeArg1,
															&PaDPList.pDataArg2, &PaDPList.SizeArg2,
															KW_IODRV_SECTION);

						/* keine Daten oder Fehler aufgetreten ? */
						if (State_find_Section < 0) {
							break;
						}

						/* Pointer auf Anfang der Sektion stellen */
						if (PaDPList.pDataArg2 != NULL) {
							PaDPList.pParser = PaDPList.pDataArg2;
						}
						else {
							continue;
						}

						/* alle Daten der Sektion bearbeiten */

						IODRV_DPData.DPName.name = pPointName;
						IODRV_DPData.DPName.size = lenPointName;
						IODRV_DPData.DPName.defined = 1;

						*pDPFound		= 1;
						iodrv_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
										(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
										(Parser_typ *) &PaDPList,
										(IODRV_ItemData_typ *) &IODRV_DPData,
										IODRV_SECTION,DP_PART);

					} while (State_find_Section == TRUE);

					/* Parserposition rückholen */
					memcpy(&PaDPList,&PaDPListSave,sizeof(PaDPList));

					/* Suchen nach Sektion "ALL" */
					do {
						State_find_Section = find_Section (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
															(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
															&PaDPList.pParser, &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
															&PaDPList.pDataArg1, &PaDPList.SizeArg1,
															&PaDPList.pDataArg2, &PaDPList.SizeArg2,
															KW_ALL_SECTION);

						/* keine Daten oder Fehler aufgetreten ? */
						if (State_find_Section < 0) {
							break;
						}

						/* Pointer auf Anfang der Sektion stellen */
						if (PaDPList.pDataArg2 != NULL) {
							PaDPList.pParser = PaDPList.pDataArg2;
						}
						else {
							continue;
						}

						/* alle Daten der Sektion bearbeiten */
						iodrv_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
										(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
										(Parser_typ *) &PaDPList,
										(IODRV_ItemData_typ *) &IODRV_DPData,
										ALL_SECTION,DP_PART);

					} while (State_find_Section == TRUE);
					break;

				/* ------------------ */
				/* Block-Abschnitt    */
				/* ------------------ */
				case cCD_TYP:
					if(strncasecmp(PaDPList.pDataArg1, KW_BLOCK_SECTION, PaDPList.SizeArg1) == 0 && strlen(KW_BLOCK_SECTION) == PaDPList.SizeArg1 ) {
						*pBlockFound = 1;

						/* Parserposition zwischenspeichern */
						memcpy(&PaDPListSave,&PaDPList,sizeof(PaDPListSave));

						/* Suchen nach Sektion "IODRV" */
						do {
							State_find_Section = find_Section (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
																(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
																&PaDPList.pParser, 	 &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
																&PaDPList.pDataArg1, &PaDPList.SizeArg1,
																&PaDPList.pDataArg2, &PaDPList.SizeArg2,
																KW_IODRV_SECTION);

							/* keine Daten oder Fehler aufgetreten ? */
							if (State_find_Section < 0) {
								break;
							}

							/* Pointer auf Anfang der Sektion stellen */
							if (PaDPList.pDataArg2 != NULL) {
								PaDPList.pParser = PaDPList.pDataArg2;
							}
							else {
								continue;
							}

							/* alle Daten der Sektion bearbeiten */

							iodrv_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
											(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
											(Parser_typ *) &PaDPList,
											(IODRV_ItemData_typ *) &IODRV_BlockData,
											IODRV_SECTION,DP_PART);

						} while (State_find_Section == TRUE);

						/* Parserposition rückholen */
						memcpy(&PaDPList,&PaDPListSave,sizeof(PaDPList));

						/* Suchen nach Sektion "ALL" */
						do {
							State_find_Section = find_Section (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
																(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
																&PaDPList.pParser, &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
																&PaDPList.pDataArg1, &PaDPList.SizeArg1,
																&PaDPList.pDataArg2, &PaDPList.SizeArg2,
																KW_ALL_SECTION);

							/* keine Daten oder Fehler aufgetreten ? */
							if (State_find_Section < 0) {
								break;
							}

							/* Pointer auf Anfang der Sektion stellen */
							if (PaDPList.pDataArg2 != NULL) {
								PaDPList.pParser = PaDPList.pDataArg2;
							}
							else {
								continue;
							}

							/* alle Daten der Sektion bearbeiten */
							iodrv_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
											(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
											(Parser_typ *) &PaDPList,
											(IODRV_ItemData_typ *) &IODRV_BlockData,
											ALL_SECTION,DP_PART);

						} while (State_find_Section == TRUE);
					}
					break;

			}
		}
}

/*****************************************************************
*		Datentyp in typlist suchen
******************************************************************/
plcbit iodrv_parser_dptype(unsigned char *pName, unsigned short size)
{

		BOOL	TypFound=0;

		/* -------------------------------------------- */
	 	/* Datenpunkt Eintraege fuer IODRV-Treiber parsen */
		/* -------------------------------------------- */


		/* Parser-Status loeschen */
		State_find_StationTypPoint	= 0;
		State_find_Section			= 0;
		State_find_Data				= 0;

		while (!TypFound && (PaTypList.pParser < (unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len) && (State_find_StationTypPoint>=0)) {

			/* Suche nach Stations/Typ/Punkt-Beschreibung */
			State_find_StationTypPoint = find_StationTypPoint (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
																(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
																&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
																&PaTypList.pDataArg1, &PaTypList.SizeArg1,
																&PaTypList.pDataArg2, &PaTypList.SizeArg2,
																&PaTypList.SectionType);
			/* keine Daten oder Fehler aufgetreten ? */
			if (State_find_StationTypPoint < 0) {
				break;
			}

			if(PaTypList.SectionType==cCD_TYP) {
				if((PaTypList.SizeArg1==size) && (strncasecmp(PaTypList.pDataArg1, pName, PaTypList.SizeArg1) == 0)) {
					TypFound = 1;

					/* Parserposition zwischenspeichern */
					memcpy(&PaTypListSave,&PaTypList,sizeof(PaTypListSave));

					/* Suchen nach Sektion "IODRV" */
					State_find_Section = find_Section (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
														(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
														&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
														&PaTypList.pDataArg1, &PaTypList.SizeArg1,
														&PaTypList.pDataArg2, &PaTypList.SizeArg2,
														KW_IODRV_SECTION);

					/* Section gefunden ? Daten parsen */
					if (State_find_Section == TRUE) {

						/* Pointer auf Anfang der Sektion stellen */
						if (PaTypList.pDataArg2 != NULL) {
							PaTypList.pParser = PaTypList.pDataArg2;
						}
						else {
							continue;
						}

						iodrv_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
										(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
										(Parser_typ *) &PaTypList,
										(IODRV_ItemData_typ *) &IODRV_DPData,
										IODRV_SECTION,DP_PART);
					}
					/* Parserposition rückkopieren */
					memcpy(&PaTypList,&PaTypListSave,sizeof(PaTypList));

					/* Suchen nach Sektion "ALL" */
					State_find_Section = find_Section (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
														(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
														&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
														&PaTypList.pDataArg1, &PaTypList.SizeArg1,
														&PaTypList.pDataArg2, &PaTypList.SizeArg2,
														KW_ALL_SECTION);

					/* Section gefunden ? Daten parsen */
					if (State_find_Section == TRUE) {

						/* Pointer auf Anfang der Sektion stellen */
						if (PaTypList.pDataArg2 != NULL) {
							PaTypList.pParser = PaTypList.pDataArg2;
						}
						else {
							continue;
						}

						iodrv_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
										(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
										(Parser_typ *) &PaTypList,
										(IODRV_ItemData_typ *) &IODRV_DPData,
										ALL_SECTION,DP_PART);
					}
					/* Parserposition rückkopieren */
					memcpy(&PaTypList,&PaTypListSave,sizeof(PaTypList));

				}
			}
		}
	return(TypFound);
}

/*****************************************************************
*		Informationen ueber ein Item ermitteln
******************************************************************/
plcbit iodrv_parser_iteminfo()
{

		BOOL	ItemFound=0;

		/* -------------------------------------------- */
	 	/* Datenpunkt Eintraege fuer IODRV-Treiber parsen */
		/* -------------------------------------------- */


		/* Parser-Status loeschen */
		State_find_StationTypPoint	= 0;
		State_find_Section			= 0;
		State_find_Data				= 0;

		while (!ItemFound && (PaTypList.pParser < (unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len)) {

			/* Suche nach Stations/Typ/Punkt-Beschreibung */
			State_find_StationTypPoint = find_StationTypPoint (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
																(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
																&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
																&PaTypList.pDataArg1, &PaTypList.SizeArg1,
																&PaTypList.pDataArg2, &PaTypList.SizeArg2,
																&PaTypList.SectionType);
			/* keine Daten oder Fehler aufgetreten ? */
			if (State_find_StationTypPoint < 0) {
				break;
			}

			if(PaTypList.SectionType==cCD_ITEM) {

				/* Parserposition zwischenspeichern */
				memcpy(&PaTypListSave,&PaTypList,sizeof(PaTypListSave));

				/* Suchen nach Sektion "IODRV" */
				State_find_Section = find_Section (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
													(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
													&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
													&PaTypList.pDataArg1, &PaTypList.SizeArg1,
													&PaTypList.pDataArg2, &PaTypList.SizeArg2,
													KW_IODRV_SECTION);

				/* keine Daten oder Fehler aufgetreten ? */
				if (State_find_Section < 0) {
					break;
				}

				/* Pointer auf Anfang der Sektion stellen */
				if (PaTypList.pDataArg2 != NULL) {
					PaTypList.pParser = PaTypList.pDataArg2;
				}
				else {
					continue;
				}

				ItemFound	= 1;

				iodrv_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
								(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
								(Parser_typ *) &PaTypList,
								(IODRV_ItemData_typ *) &IODRV_ItemData,
								IODRV_SECTION,ITEM_PART);


				/* Parserposition rückkopieren */
				memcpy(&PaTypList,&PaTypListSave,sizeof(PaTypList));


				/* Suchen nach Sektion "ALL" */
				State_find_Section = find_Section (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
													(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
													&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
													&PaTypList.pDataArg1, &PaTypList.SizeArg1,
													&PaTypList.pDataArg2, &PaTypList.SizeArg2,
													KW_ALL_SECTION);

				/* keine Daten oder Fehler aufgetreten ? */

				if (State_find_Section>0) {

					/* Pointer auf Anfang der Sektion stellen */
					if (PaTypList.pDataArg2 != NULL) {
						PaTypList.pParser = PaTypList.pDataArg2;
					}
					else {
						continue;
					}

					iodrv_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
									(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
									(Parser_typ *) &PaTypList,
									(IODRV_ItemData_typ *) &IODRV_ItemData,
									ALL_SECTION,ITEM_PART);

				}
			}
			else {
				break;
			}

		}

	return(ItemFound);
}

/*****************************************************************
*	Datenbereich einer Sektion parsen
******************************************************************/
void iodrv_parser_data(unsigned char *pSrcStart,unsigned char *pSrcEnd, Parser_typ *pPa, IODRV_ItemData_typ *pData, unsigned char SectionInfo, unsigned char PartInfo) {

	plcbit			dataReady = 0;
	plcbit			dataFound = 0;
	unsigned char	Arg3IsANumber;
	long			Arg3Value;

	do {
		/* Daten der Sektion anfordern */
		State_find_Data = find_Data (	pSrcStart, pPa->pParser, pSrcEnd,
										  &pPa->pParser, &pPa->pParserLine, &pPa->pParserErr, NULL,
										  &pPa->pDataArg1, &pPa->SizeArg1,
										  &pPa->pDataArg2, &pPa->SizeArg2,
										  &pPa->pDataArg3, &pPa->SizeArg3);

		/* Zeilenende erreicht */
		if (State_find_Data == -1) {
			pPa->pParser++;
		}
		/* Daten vorhanden */
		else if (State_find_Data == TRUE) {

			dataFound = 0;

			/* Pruefen, ob Argument 3 einer Zahl entspricht */
			if(pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {
				Arg3IsANumber	= IsANumber(pPa->pDataArg3);
				if(Arg3IsANumber==TRUE)	Arg3Value	= brsatoi((UDINT) pPa->pDataArg3);
			}
			else {
				Arg3IsANumber = FALSE;
			}

			/* Schlüsselworte pruefen, die nur im allgemeinen Datenpunktbereich vorkommen */
			if(PartInfo==DP_PART) {
				/* Anlagennummer */
				if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_ANL, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_ANL) == pPa->SizeArg1 &&
					strncasecmp(pPa->pDataArg2, KW_DP_ALL_ANL_NR, pPa->SizeArg2) == 0 && strlen(KW_DP_ALL_ANL_NR) == pPa->SizeArg2 &&
					Arg3IsANumber  == TRUE) {

						pData->AnlagenNr.value		= Arg3Value;
						pData->AnlagenNr.defined	= 1;
						dataFound					= 1;
				}
				/* Datenpunkt Nummer */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_DP, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_DP) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DP_ALL_DP_NR, pPa->SizeArg2) == 0 && strlen(KW_DP_ALL_DP_NR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {
						/* Nummer der Anlage */
						pData->DPNr.value			= Arg3Value;
						pData->DPNr.defined			= 1;
						dataFound					= 1;
				}
				/* Datentyp */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_TYP, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_TYP) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						pData->Typ.name				= pPa->pDataArg3;
						pData->Typ.size				= pPa->SizeArg3;
						pData->Typ.defined			= 1;
						dataFound					= 1;
				}
				/* Datentyp Item */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_ITEM, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_ITEM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DP_ALL_ITEM_TYP, pPa->SizeArg2) == 0 && strlen(KW_DP_ALL_ITEM_TYP) == pPa->SizeArg2 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						pData->ItemTyp.name			= pPa->pDataArg3;
						pData->ItemTyp.size			= pPa->SizeArg3;
						pData->ItemTyp.defined		= 1;
						dataFound					= 1;
				}
				/* Item Nummer */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_ITEM, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_ITEM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DP_ALL_ITEM_NR, pPa->SizeArg2) == 0 && strlen(KW_DP_ALL_ITEM_NR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->ItemNr.value			= Arg3Value;
						pData->ItemNr.defined		= 1;
						dataFound					= 1;
				}
				/* Item Name */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_ITEM, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_ITEM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DP_ALL_ITEM_NAME, pPa->SizeArg2) == 0 && strlen(KW_DP_ALL_ITEM_NAME) == pPa->SizeArg2 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						pData->ItemName.name		= pPa->pDataArg3;
						pData->ItemName.size		= pPa->SizeArg3;
						pData->ItemName.defined		= 1;
						dataFound					= 1;
				}
				/* invers */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_INV, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_INV) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber  == TRUE) {

						/* invers */
						pData->Invers.value			= Arg3Value;
						pData->Invers.defined		= 1;
						dataFound					= 1;
				}
				/* Quitt */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_QUITT, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_QUITT) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->Quitt.value			= Arg3Value;
						pData->Quitt.defined		= 1;
						if(Arg3Value)	{
							/* bei Quittierpflichtigem Item: Attr = ReadWrite */
							pData->Dir.value			= cKW_READWRITE;
							pData->Dir.defined			= 1;
						}
						dataFound					= 1;
				}
				/* Dir (als Zahl 0/1/2)*/
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_DIR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_DIR) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->Dir.value			= Arg3Value;
						pData->Dir.defined			= 1;
						dataFound					= 1;
				}
				/* Dir (als Text R/W/RW)*/
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_DIR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_DIR) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						if(!strncasecmp(pPa->pDataArg3,KW_ITEM_IODRV_DIR_R,pPa->SizeArg3))	{
							pData->Dir.value			= cKW_READ;
							pData->Dir.defined			= 1;
							dataFound					= 1;
						}
						else if(!strncasecmp(pPa->pDataArg3,KW_ITEM_IODRV_DIR_W,pPa->SizeArg3))	{
							pData->Dir.value			= cKW_WRITE;
							pData->Dir.defined			= 1;
							dataFound					= 1;
						}
						else if(!strncasecmp(pPa->pDataArg3,KW_ITEM_IODRV_DIR_RW,pPa->SizeArg3))	{
							pData->Dir.value			= cKW_READWRITE;
							pData->Dir.defined			= 1;
							dataFound					= 1;
						}
				}
				/* Datentyp Nummer */
				else if (strncasecmp(pPa->pDataArg1, KW_DPTYP_ALL_TYP, pPa->SizeArg1) == 0 && strlen(KW_DPTYP_ALL_TYP) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DPTYP_ALL_TYP_NR, pPa->SizeArg2) == 0 && strlen(KW_DPTYP_ALL_TYP_NR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->DPTypNr.value		= Arg3Value;
						pData->DPTypNr.defined		= 1;
						dataFound					= 1;
				}
			}

			if(PartInfo==ITEM_PART) {
				/* Datentyp Item */
				if (strncasecmp(pPa->pDataArg1, KW_ITEM_ALL_ITEM_TYP, pPa->SizeArg1) == 0 && strlen(KW_ITEM_ALL_ITEM_TYP) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						pData->ItemTyp.name			= pPa->pDataArg3;
						pData->ItemTyp.size			= pPa->SizeArg3;
						pData->ItemTyp.defined		= 1;
						dataFound					= 1;
				}
				/* Item Nummer */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_ALL_ITEM, pPa->SizeArg1) == 0 && strlen(KW_ITEM_ALL_ITEM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_ALL_ITEM_NR, pPa->SizeArg2) == 0 && strlen(KW_ITEM_ALL_ITEM_NR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->ItemNr.value			= Arg3Value;
						pData->ItemNr.defined		= 1;
						dataFound					= 1;
				}
				/* Item Name */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_ALL_ITEM, pPa->SizeArg1) == 0 && strlen(KW_DP_ALL_ITEM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DP_ALL_ITEM_NAME, pPa->SizeArg2) == 0 && strlen(KW_DP_ALL_ITEM_NAME) == pPa->SizeArg2 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						pData->ItemName.name		= pPa->pDataArg3;
						pData->ItemName.size		= pPa->SizeArg3;
						pData->ItemName.defined		= 1;
						dataFound					= 1;
				}
			}

			if(((PartInfo==ITEM_PART) || (PartInfo==DP_PART)) && (!dataFound)) {
				/* invers */
				if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_INV, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_INV) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber  == TRUE) {

						/* invers */
						pData->Invers.value			= Arg3Value;
						pData->Invers.defined		= 1;
						dataFound					= 1;
				}
				/* Quitt */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_QUITT, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_QUITT) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->Quitt.value			= Arg3Value;
						pData->Quitt.defined		= 1;
						if(Arg3Value)	{
							/* bei Quittierpflichtigem Item: Attr = ReadWrite */
							pData->Dir.value			= cKW_READWRITE;
							pData->Dir.defined			= 1;
						}
						dataFound					= 1;
				}
				/* Dir (als Zahl 0/1/2)*/
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_DIR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_DIR) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->Dir.value			= Arg3Value;
						pData->Dir.defined			= 1;
						dataFound					= 1;
				}
				/* Dir (als Text R/W/RW)*/
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IODRV_DIR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IODRV_DIR) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						pPa->pDataArg3 != NULL && pPa->SizeArg3 != 0) {

						if(!strncasecmp(pPa->pDataArg3,KW_ITEM_IODRV_DIR_R,pPa->SizeArg3))	{
							pData->Dir.value			= cKW_READ;
							pData->Dir.defined			= 1;
							dataFound					= 1;
						}
						else if(!strncasecmp(pPa->pDataArg3,KW_ITEM_IODRV_DIR_W,pPa->SizeArg3))	{
							pData->Dir.value			= cKW_WRITE;
							pData->Dir.defined			= 1;
							dataFound					= 1;
						}
						else if(!strncasecmp(pPa->pDataArg3,KW_ITEM_IODRV_DIR_RW,pPa->SizeArg3))	{
							pData->Dir.value			= cKW_READWRITE;
							pData->Dir.defined			= 1;
							dataFound					= 1;
						}
				}
			}

			/* Unbekannter Eintrag */
			if((!dataFound) && (SectionInfo==IODRV_SECTION)) {
				iodrv_parser_warn(PARS_ERR_UNKNOWN_ITEM_ENTRY,"Unbekannter Eintrag im Item-Bereich:", pPa->pParserLine, " / Zeile =", (long) pPa->CountLines, 0, 0, 0, 0);
			}
		}

		/* Endebedinung der Schleife: Keine Daten mehr vorhanden, oder Ende des Datenmodules erreicht */
		if((State_find_Data!=TRUE) && (State_find_Data!=-1))	dataReady = 1;
		if(pPa->pParser >= pSrcEnd)	dataReady = 1;
	} while (!dataReady);
}

/*****************************************************************
*	Pruefen, ob alle Angaben eines Datenpunktes vorhanden sind
******************************************************************/
plcbit iodrv_parser_dpinfo_ok(IODRV_ItemData_typ *pData)
{
	plcbit ok;

	ok = 1;
	if(!pData->AnlagenNr.defined) {
		iodrv_parser_warn(PARS_ERR_ANLNR_NOT_DEFINED,"DP Anlage.Nr fehlt. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}
	if(!pData->DPNr.defined) {
		iodrv_parser_warn(PARS_ERR_DPNR_NOT_DEFINED,"DP DP.Nr fehlt. DPTyp: ", pData->Typ.name , 0, 0, 0, 0, 0, 0);
		ok = 0;
	}
	if(!pData->Typ.defined) {
		iodrv_parser_warn(PARS_ERR_TYP_NOT_DEFINED,"DP Typbezeichnung fehlt. DPTyp: ", 0, "DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}

	return(ok);
}

/*****************************************************************
*	Pruefen, ob alle Angaben eines Items vorhanden sind
******************************************************************/
plcbit iodrv_parser_iteminfo_ok(IODRV_ItemData_typ *pData)
{

	plcbit	ok;

	ok = 1;
	/* Datentyp Nummer definiert ? */
	if(!pData->DPTypNr.defined) {
		iodrv_parser_warn(PARS_ERR_DPTYP_NR_NOT_DEFINED,"Typ.Nr fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}

	/* Item Nummer definiert ? */
	if(!pData->ItemNr.defined) {
		iodrv_parser_warn(PARS_ERR_ITEM_NR_NOT_DEFINED,"Item.Nr fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}


	/* Item Typ definiert ? */
	if(!pData->ItemTyp.defined) {
		iodrv_parser_warn(PARS_ERR_ITEM_TYP_NOT_DEFINED,"ITEM Datentyp fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
		ok = 0;
	}
	else {
		pData->ItemTypNr.defined = 0;
		/* Item Datentyp gültig ? */
		if(strncasecmp(pData->ItemTyp.name, BOOL_TYP, pData->ItemTyp.size) == 0 && strlen(BOOL_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= BOOL_IDX;
		}
		if(strncasecmp(pData->ItemTyp.name, USINT_TYP, pData->ItemTyp.size) == 0 && strlen(USINT_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= USINT_IDX;
		}
		if(strncasecmp(pData->ItemTyp.name, SINT_TYP, pData->ItemTyp.size) == 0 && strlen(SINT_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= SINT_IDX;
		}
		if(strncasecmp(pData->ItemTyp.name, UINT_TYP, pData->ItemTyp.size) == 0 && strlen(UINT_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= UINT_IDX;
		}
		if(strncasecmp(pData->ItemTyp.name, INT_TYP, pData->ItemTyp.size) == 0 && strlen(INT_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= INT_IDX;
		}
		if(strncasecmp(pData->ItemTyp.name, UDINT_TYP, pData->ItemTyp.size) == 0 && strlen(UDINT_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= UDINT_IDX;
		}
		if(strncasecmp(pData->ItemTyp.name, DINT_TYP, pData->ItemTyp.size) == 0 && strlen(DINT_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= DINT_IDX;
		}
		if(!pData->ItemTypNr.defined) {
			iodrv_parser_warn(PARS_ERR_ITEM_TYP_UNKNOWN,"ITEM Datentyp unbekannt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
			ok = 0;
		}
	}
	/* Item Name definiert ? */
	if(!pData->ItemName.defined) {
		iodrv_parser_warn(PARS_ERR_ITEM_NAME_NOT_DEFINED,"ITEM Itemname fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
		ok = 0;
	}

	/* Invers definiert ? */
	if(!pData->Invers.defined) {
		iodrv_parser_warn(PARS_ERR_IODRV_INV_NOT_DEFINED,"ITEM Invers fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
		ok = 0;
	}

	/* Quitt definiert ? */
	if(!pData->Quitt.defined) {
		iodrv_parser_warn(PARS_ERR_IODRV_QUITT_NOT_DEFINED,"ITEM Quitt fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
		ok = 0;
	}

	return(ok);
}


/***************************************************************************
*		Zeiger auf Startadresse Datenmodul stellen, Zeilenzaehler loeschen
****************************************************************************/
void copyItemData(IODRV_ItemData_typ *pItemDest, IODRV_ItemData_typ *pItemSrc)
{
	if(pItemSrc->AnlagenNr.defined)		memcpy(&pItemDest->AnlagenNr,	&pItemSrc->AnlagenNr,	sizeof(pItemDest->AnlagenNr));
	if(pItemSrc->DPNr.defined)			memcpy(&pItemDest->DPNr,		&pItemSrc->DPNr,		sizeof(pItemDest->DPNr));
	if(pItemSrc->DPTypNr.defined)		memcpy(&pItemDest->DPTypNr,		&pItemSrc->DPTypNr,		sizeof(pItemDest->DPTypNr));
	if(pItemSrc->DPName.defined)		memcpy(&pItemDest->DPName,		&pItemSrc->DPName,		sizeof(pItemDest->DPName));
	if(pItemSrc->Typ.defined)			memcpy(&pItemDest->Typ,			&pItemSrc->Typ,			sizeof(pItemDest->Typ));
	if(pItemSrc->ItemNr.defined)		memcpy(&pItemDest->ItemNr,		&pItemSrc->ItemNr,		sizeof(pItemDest->ItemNr));
	if(pItemSrc->ItemTyp.defined)		memcpy(&pItemDest->ItemTyp,		&pItemSrc->ItemTyp,		sizeof(pItemDest->ItemTyp));
	if(pItemSrc->ItemName.defined)		memcpy(&pItemDest->ItemName,	&pItemSrc->ItemName,	sizeof(pItemDest->ItemName));
	if(pItemSrc->Invers.defined)		memcpy(&pItemDest->Invers,		&pItemSrc->Invers,		sizeof(pItemDest->Invers));
	if(pItemSrc->Quitt.defined)			memcpy(&pItemDest->Quitt,		&pItemSrc->Quitt,		sizeof(pItemDest->Quitt));
	if(pItemSrc->Dir.defined)			memcpy(&pItemDest->Dir,			&pItemSrc->Dir,			sizeof(pItemDest->Dir));
}


/***************************************************************************
*		Zeiger auf Startadresse Datenmodul stellen, Zeilenzaehler loeschen
****************************************************************************/
void iodrv_parser_dplist_clear()
{

	/* Datenmodul von Anfang an Parsen .. */
	PaDPList.pParser		= (unsigned char*) DOI_dplist.pDatObjMem;
	PaDPList.CountLines		= 0;
}

/***************************************************************************
*		Zeiger auf Startadresse Datenmodul stellen, Zeilenzaehler loeschen
****************************************************************************/
void iodrv_parser_typlist_clear()
{

	/* Datenmodul von Anfang an Parsen ... */
	PaTypList.pParser		= (unsigned char*) DOI_typlist.pDatObjMem;
	PaTypList.CountLines	= 0;
}
/**********************************************************
*		Fehlermeldung in Log-Buch eintragen
**********************************************************/
void iodrv_parser_err(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3)
{
	unsigned char ANr[20];
	unsigned long ErrorInfo=0;

	memset(MsgTxt,0,sizeof(MsgTxt));
	if(IODRV_ParserInfo.ErrorMsg) {
		IODRV_ParserInfo.error = errorNr;
		strcpy(MsgTxt," ");
		if(pTxtInfo>0) {
			strcat(MsgTxt,pTxtInfo);
			strcat(MsgTxt," ");
		}
		if(pTxt0>0) {
			strcat(MsgTxt,pTxt0);
			strcat(MsgTxt," ");
		}
		if(pTxt1>0) {
			strcat(MsgTxt,pTxt1);
			strcat(MsgTxt," ");
			brsitoa(info1, (UDINT) &ANr);
			strcat(MsgTxt,ANr);
			strcat(MsgTxt," ");
		}
		if(pTxt2>0) {
			strcat(MsgTxt,pTxt2);
			strcat(MsgTxt," ");
			brsitoa(info2, (UDINT) &ANr);
			strcat(MsgTxt,ANr);
			strcat(MsgTxt," ");
		}
		if(pTxt3>0) {
			strcat(MsgTxt,pTxt3);
			strcat(MsgTxt," ");
			brsitoa(info3, (UDINT) &ANr);
			strcat(MsgTxt,ANr);
			strcat(MsgTxt," ");
		}
		if(IODRV_ParserInfo.NrOfLogEntrys>MAX_ERROR_MSG_LOG) {
			ErrStatistics("iodrv", ERR, errorNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		else {
			ErrStatistics("iodrv", ERRLOG, errorNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		IODRV_ParserInfo.NrOfLogEntrys++;
	}
}

/**********************************************************
*		Warnungsmeldung in Log-Buch eintragen
**********************************************************/
void iodrv_parser_warn(unsigned short warnNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3)
{
	unsigned char ANr[20];
	unsigned long ErrorInfo=0;

	memset(MsgTxt,0,sizeof(MsgTxt));
	if(IODRV_ParserInfo.ErrorMsg) {
		IODRV_ParserInfo.warning = warnNr;
		strcpy(MsgTxt," ");
		if(pTxtInfo>0) {
			strcat(MsgTxt,pTxtInfo);
			strcat(MsgTxt," ");
		}
		if(pTxt0>0) {
			strcat(MsgTxt,pTxt0);
			strcat(MsgTxt," ");
		}
		if(pTxt1>0) {
			strcat(MsgTxt,pTxt1);
			strcat(MsgTxt," ");
			brsitoa(info1, (UDINT) &ANr);
			strcat(MsgTxt,ANr);
			strcat(MsgTxt," ");
		}
		if(pTxt2>0) {
			strcat(MsgTxt,pTxt2);
			strcat(MsgTxt," ");
			brsitoa(info2, (UDINT) &ANr);
			strcat(MsgTxt,ANr);
			strcat(MsgTxt," ");
		}
		if(pTxt3>0) {
			strcat(MsgTxt,pTxt3);
			strcat(MsgTxt," ");
			brsitoa(info3, (UDINT) &ANr);
			strcat(MsgTxt,ANr);
			strcat(MsgTxt," ");
		}
		if(IODRV_ParserInfo.NrOfLogEntrys>MAX_ERROR_MSG_LOG) {
			ErrStatistics("iodrv", ERR, warnNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		else {
			ErrStatistics("iodrv", ERRLOG, warnNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		IODRV_ParserInfo.NrOfLogEntrys++;
	}
}

/**********************************************************
*		Speicherdiagnose
**********************************************************/
void iodrv_mem_diagnose()
{
	IODRV_DP_Info_typ *pDPInfo;
	unsigned short	n,i;
	plcbit			found;


	for(i=0;i<ITEM_DIAGNOSE_SIZE;i++) {
		if(IODRV_ItemDiagnose[i].showData) {
			IODRV_ItemDiagnose[i].showData	= 0;
			IODRV_ItemDiagnose[i].DataValid	= 0;

			found = 0;
			pDPInfo = (IODRV_DP_Info_typ *) (IODRV_ParserInfo.memAdr);
			for(n = 0; n < IODRV_ParserInfo.NrOfItems; n++, pDPInfo++) {
				if(IODRV_ItemDiagnose[i].AnlagenNr 	!= pDPInfo->AnlagenNr)	continue;
				if(IODRV_ItemDiagnose[i].DPTypNr 	!= pDPInfo->DPTypNr)	continue;
				if(IODRV_ItemDiagnose[i].DPNr 		!= pDPInfo->DPNr)	continue;
				if(IODRV_ItemDiagnose[i].ItemNr 	!= pDPInfo->ItemNr)		continue;
				found = 1;
				IODRV_ItemDiagnose[i].ItemIdx = n;
				break;
			}

			if(found) {
				IODRV_ItemDiagnose[i].pItemData		 = (IODRV_DP_Info_typ *)  (IODRV_ParserInfo.memAdr);
				IODRV_ItemDiagnose[i].pItemData		+= IODRV_ItemDiagnose[i].ItemIdx;
				IODRV_ItemDiagnose[i].DataValid	 = 1;
				if(i==0) {
					pItemData0Diagnose = IODRV_ItemDiagnose[i].pItemData;
				}
				else {
					pItemData1Diagnose = IODRV_ItemDiagnose[i].pItemData;
				}
			}
		}
	}
}

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

/*** eof *****************************************************************************/


