#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Parserfunktionen
*  Datei:    iec__103_parse.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    26-11-2003
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion:
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 26-11-03 | KOB  | Datei-Erstellung
*  02.00 | 10-07-26 | BIE  | strnicmp gegen strncasecmp ersetzt
*        |          |      |
******************************************************************************/
#include <bur\plc.h>			/* RPS-spezifische Typendeklarationen */
#include <string.h>
#include <dvframe.h>
#include <asbrstr.h>
#include <stdlib.h>
#include <sys_lib.h>
#include <brsystem.h>
#include "iec60870_5_103.h"
#include "iec__103_parse.h"
#include "parser.h"


/**********************************************************
*		Konfiguration aus Datenmodul auslesen
**********************************************************/
void iec_parser()
{
	unsigned short 		i,error,DeviceIdx;
	plcbit				BlockFound,DPFound,TypFound,ItemFound,NewDevice;
	IEC_DP_Info_typ		*pIEC_DP_Info;
	IEC_Normierung_typ	*pIEC_Normierung;
	unsigned char ANr[20];

	memset(&IEC_ParserInfo,0,sizeof(IEC_ParserInfo));
	IEC_ParserInfo.ErrorMsg = 1;									/* Freigabe: Fehlermeldungen ins Logbuch eintragen */

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
		iec_parser_err(PARS_ERR_DPLIST_NOT_FOUND, "Datenmodul fehlt:", (unsigned char *) DOI_dplist.pName, 0, 0, 0, 0, 0, 0);
	}

	/*--------------------------------------------------*/
	/* Fehlermeldung: Datenmodul typlist nicht vorhanden */
	/*--------------------------------------------------*/
	if(DOI_typlist.status) {
		iec_parser_err(PARS_ERR_TYPLIST_NOT_FOUND, "Datenmodul fehlt:", (unsigned char *) DOI_typlist.pName, 0, 0, 0, 0, 0, 0);
	}

	/*--------------------------------------------------*/
	/* Globale Parameter lesen							*/
	/*--------------------------------------------------*/
	if(!IEC_ParserInfo.error) {

		iec_parser_dplist_clear();									/* Parsen von Beginn an */

		memset(&IEC_GlobalData,0,sizeof(IEC_GlobalData));			/* Parameterstruktur loeschen */

		iec_parser_global();										/* Parameter parsen */

		if(IEC_ParserInfo.NrOfLines==0) {							/* Parameter auswerten */
			iec_parser_err(PARS_ERR_NR_OF_LINES_ZERO, "Liniendefinition fehlt:", "Parser abgebrochen", 0, 0, 0, 0, 0, 0);
		}
		else {
			/* Sind fuer alle angegebenen Linien Schnittstellenbezeichnung und -mode und Konfiguration deklariert worden ? */
			for(i=0;i<IEC_ParserInfo.NrOfLines;i++) {
				if(!IEC_GlobalData.Line[i].Interface.defined)	{
					IEC_ParserInfo.Line[i].error = PARS_ERR_LINE_IF_NOT_DEFINED;
					iec_parser_warn(IEC_ParserInfo.Line[i].error, "Schnittstelle nicht deklariert", 0, "fuer Linie:", IEC_GlobalData.Line[i].LineNr.value, 0, 0, 0, 0);
				}
				else if(!IEC_GlobalData.Line[i].InterfaceMode.defined) {
						IEC_ParserInfo.Line[i].error = PARS_ERR_LINE_MODE_NOT_DEFINED;
						iec_parser_warn(IEC_ParserInfo.Line[i].error, "Schnittstellenmodus nicht deklariert", 0, "fuer Linie:", IEC_GlobalData.Line[i].LineNr.value, 0, 0, 0, 0);
				}
			}
		}
	}

	/*------------------------------------------------------*/
	/* Anzahl Datenpunkte und Items ermitteln				*/
	/*------------------------------------------------------*/

	if(!IEC_ParserInfo.error) {

		IEC_ParserInfo.ErrorMsg = 0;															/* Keine Eintrage ins Logbuch */
		iec_parser_dplist_clear();																/* dplist von Beginn an Parsen */
		memset(&IEC_BlockData,0,sizeof(IEC_BlockData));											/* Info ueber Blockparameter loeschen */

		do {
			memset(&IEC_DPData,0,sizeof(IEC_DPData));											/* Info ueber Datenpunkt loeschen */
			copyItemData((IEC_ItemData_typ *) &IEC_DPData,(IEC_ItemData_typ *) &IEC_BlockData);	/* Blockparameter uebernehmen */

			BlockFound	= 0;
			DPFound		= 0;

			iec_parser_dpinfo((plcbit *) &BlockFound,(plcbit *) &DPFound);						/* Nächsten Datenpunkt für IEC-Treiber suchen */

			if(DPFound) {																		/* Datenpunkt für IEC-Treiber gefunden */
																								/* Zugehoerigen Linienindex suchen */
				error	= get_line_idx((IEC_ItemData_typ *) &IEC_DPData, (IEC_GlobalData_typ *) &IEC_GlobalData, (unsigned short *) &IEC_ParserInfo.LineIdx);
				if(!error) {
					IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].NrOfDataPoints++;				/* Anzahl Datenpunkte pro Linie zählen */
					if(IEC_DPData.Typ.defined) {												/* Ist zu dem Datenpunkt der Datentyp definiert ? */
						iec_parser_typlist_clear();												/* typlist von Beginn an Parsen */
						TypFound = iec_parser_dptype(IEC_DPData.Typ.name, IEC_DPData.Typ.size); /* Datentyp in typlist suchen */
						if(TypFound) {															/* Datentyp wurde in typlist gefunden */
							/* Anzahl Items des Datenpunktes ermitteln */
							do {
								memset(&IEC_ItemData,0,sizeof(IEC_ItemData));					/* Struktur für ITEM-Parameter loeschen */
								ItemFound = iec_parser_iteminfo();								/* Item in typlist suchen */
								if(ItemFound) {													/* Item wurde gefunden */
									copyItemData((IEC_ItemData_typ *) &IEC_ItemData,(IEC_ItemData_typ *) &IEC_DPData);
									IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].NrOfItems++;
																								/* Normierung fuer Analogwert angegeben ? */
									if(normierung_defined((IEC_ItemData_typ *) &IEC_ItemData)) {
										IEC_ParserInfo.NrOfNormValues++;						/* Anzahl Normierungseinträge zählen */
									}
								}
							} while(ItemFound);

						} /* if(TypFound) */
					} /* if(IEC_DPData.Typ.defined) */
				} /* if(!error) */
			} /* if(DPFound) */
		} while(DPFound || BlockFound);
	} /* if(!IEC_ParserInfo.error) */


	/*------------------------------------------------------*/
	/* Speicher fuer alle Items allokieren					*/
	/*------------------------------------------------------*/

	if(!IEC_ParserInfo.error) {
		IEC_ParserInfo.ErrorMsg	= 1;					/* Freigabe: Fehlermeldungen ins Logbuch eintragen */
		IEC_ParserInfo.memSize	= IEC_ParserInfo.NrOfNormValues * sizeof(IEC_Normierung_typ);
		for(i=0;i<IEC_ParserInfo.NrOfLines;i++) {
			IEC_ParserInfo.Line[i].memSize = IEC_ParserInfo.Line[i].NrOfItems * sizeof(IEC_DP_Info_typ);
			IEC_ParserInfo.memSize += IEC_ParserInfo.Line[i].memSize;
			if(IEC_ParserInfo.Line[i].NrOfItems==0) {
				IEC_ParserInfo.Line[i].error = PARS_ERR_NR_OF_DP_ZERO;
				iec_parser_warn(IEC_ParserInfo.Line[i].error, "Keine Datenpunkte", 0, "fuer Linie:", IEC_GlobalData.Line[i].LineNr.value, 0, 0, 0, 0);
			}
		}
		IEC_ParserInfo.allocStatus = TMP_alloc(IEC_ParserInfo.memSize, (void**) &IEC_ParserInfo.memAdr); 	/* allocate new memory from RAM*/
		if(IEC_ParserInfo.allocStatus) {
			iec_parser_err(PARS_ERR_MEM_ALLOC, "Fehler TMP_alloc:", 0, "Groesse:", IEC_ParserInfo.memSize, "Status:", (long) IEC_ParserInfo.allocStatus, 0, 0);
		}
		else {
			memset((void *) IEC_ParserInfo.memAdr,0,IEC_ParserInfo.memSize);
			IEC_ParserInfo.Line[0].memAdr = IEC_ParserInfo.memAdr + IEC_ParserInfo.NrOfNormValues * sizeof(IEC_Normierung_typ);
			for(i=1;i<IEC_ParserInfo.NrOfLines;i++) {
				IEC_ParserInfo.Line[i].memAdr = IEC_ParserInfo.Line[i-1].memAdr + IEC_ParserInfo.Line[i-1].memSize;
			}
		}
	}

	/*-----------------------------------------------------------------------*/
	/* Iteminformationen auslesen und in Speicher eintragen     */
	/*-----------------------------------------------------------------------*/

	if(!IEC_ParserInfo.error) {

		iec_parser_dplist_clear();																/* dplist von Beginn an Parsen */
		memset(&IEC_BlockData,0,sizeof(IEC_BlockData));											/* Info ueber Blockparameter loeschen */

		do {
			memset(&IEC_DPData,0,sizeof(IEC_DPData));											/* Info ueber Datenpunkt loeschen */
			copyItemData((IEC_ItemData_typ *) &IEC_DPData,(IEC_ItemData_typ *) &IEC_BlockData);	/* Blockparameter uebernehmen */
			BlockFound	= 0;
			DPFound		= 0;
			iec_parser_dpinfo((plcbit *) &BlockFound,(plcbit *) &DPFound);						/* Nächsten Datenpunkt für IEC-Treiber suchen */
			if(DPFound) {																		/* Datenpunkt für IEC-Treiber gefunden */
				if(iec_parser_dpinfo_ok((IEC_ItemData_typ *) &IEC_DPData)) {					/* Alle notwendigen Angaben vorhanden und korrekt ? */
																								/* Zugehoerigen Linienindex suchen */
					error	= get_line_idx((IEC_ItemData_typ *) &IEC_DPData, (IEC_GlobalData_typ *) &IEC_GlobalData, (unsigned short *) &IEC_ParserInfo.LineIdx);

					if(!error) {																/* Deviceindex in IEC-Treiberstruktur ermitteln */
						error = get_device_idx(IEC_ParserInfo.LineIdx,IEC_DPData.DeviceAdr.value, &DeviceIdx, &NewDevice);
					}
					if(!error) {
						iec_parser_typlist_clear();												/* typlist von Beginn an Parsen */
						TypFound = iec_parser_dptype(IEC_DPData.Typ.name, IEC_DPData.Typ.size); /* Datentyp in typlist suchen */
						if(TypFound) {															/* Datentyp wurde in typlist gefunden */
							/* Anzahl Items des Datenpunktes ermitteln */
							do {
								memset(&IEC_ItemData,0,sizeof(IEC_ItemData));					/* Struktur für ITEM-Parameter loeschen */

								IEC_ItemData.Mode.defined	= 1;								/* Defaultwerte setzen */
								IEC_ItemData.Mode.value		= DEFAULT_MODE;

								ItemFound = iec_parser_iteminfo();								/* Item in typlist suchen */
								if(ItemFound) {													/* Item wurde gefunden */

																								/* Parameter des Datenpunktes auf Item übertragen */
									copyItemData((IEC_ItemData_typ *) &IEC_ItemData,(IEC_ItemData_typ *) &IEC_DPData);

																								/* Pruefen, ob Parameter vollstaendig und korrekt sind */
									if(iec_parser_iteminfo_ok((IEC_ItemData_typ *) &IEC_ItemData)) {
										pIEC_DP_Info = (IEC_DP_Info_typ *) (IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].memAdr);
										pIEC_DP_Info += IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].ActItemIdx;

																							/* Daten in Speicher eintragen */
										pIEC_DP_Info->DeviceAdr			= (USINT)	IEC_ItemData.DeviceAdr.value;
										pIEC_DP_Info->IECType			= (USINT)	IEC_ItemData.IECType.value;
										pIEC_DP_Info->IECInfoNr			= (USINT)	IEC_ItemData.IECInfoNr.value;
										pIEC_DP_Info->IECAnalogNr		= (USINT)	IEC_ItemData.IECAnalogNr.value;
										pIEC_DP_Info->MRuc				= (USINT)	IEC_ItemData.MRuc.value;
										pIEC_DP_Info->MRucInfo			= (USINT)	IEC_ItemData.MRucInfo.value;
										pIEC_DP_Info->MRucType			= (USINT)	IEC_ItemData.MRucType.value;
										pIEC_DP_Info->Mode				= (USINT)	IEC_ItemData.Mode.value;
										pIEC_DP_Info->DPNr				= (UINT)	IEC_ItemData.DPNr.value;
										pIEC_DP_Info->DPTypNr			= (UINT)	IEC_ItemData.DPTypNr.value;
										pIEC_DP_Info->ItemNr			= (UINT)	IEC_ItemData.ItemNr.value;
										pIEC_DP_Info->ItemTypNr			= (USINT)	IEC_ItemData.ItemTypNr.value;
										pIEC_DP_Info->Invalid			= (USINT)	IEC_ItemData.Invalid.value;
										pIEC_DP_Info->Overflow			= (USINT)	IEC_ItemData.Overflow.value;
										pIEC_DP_Info->KopplungsStoerung	= (USINT)	IEC_ItemData.KopplungsStoerung.value;
										pIEC_DP_Info->TimeSynchError	= (USINT)	IEC_ItemData.TimeSynchError.value;


										if(NewDevice==TRUE) {
											NewDevice = FALSE;
											stANL[IEC_ParserInfo.LineIdx].DeviceActiv[DeviceIdx].DPInfoFirstEntry	= (UINT) IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].ActItemIdx;
										}
										stANL[IEC_ParserInfo.LineIdx].DeviceActiv[DeviceIdx].DPInfoLastEntry		= (UINT) IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].ActItemIdx;

										IEC_ParserInfo.Line[IEC_ParserInfo.LineIdx].ActItemIdx++;
																							/* Normierungswerte speichern */
																							/* Normierung fuer Analogwert angegeben ? */
										if(normierung_defined((IEC_ItemData_typ *) &IEC_ItemData)) {

																							/* Normierungsinformation zu Datenpunktinformation */
											pIEC_DP_Info->Normierung	= 1;
											pIEC_DP_Info->NormierungIdx	= IEC_ParserInfo.ActNormIdx;

																							/* Zeiger auf Zielspeicher stellen */
											pIEC_Normierung = (IEC_Normierung_typ *) (IEC_ParserInfo.memAdr);
											pIEC_Normierung += IEC_ParserInfo.ActNormIdx;

											pIEC_Normierung->x1	= IEC_ItemData.RawMin.value;
											pIEC_Normierung->x2	= IEC_ItemData.RawMax.value;
											pIEC_Normierung->y1	= IEC_ItemData.NormMin.value;
											pIEC_Normierung->y2	= IEC_ItemData.NormMax.value;

											IEC_ParserInfo.ActNormIdx++;
										}
																							/* Daten in IEC-Treiber Struktur eintragen */
										stANL[IEC_ParserInfo.LineIdx].DeviceTAB[DeviceIdx].Adresse			= (USINT) IEC_ItemData.DeviceAdr.value;
										stANL[IEC_ParserInfo.LineIdx].DeviceTAB[DeviceIdx].Aktiv			= 1;
										stANL[IEC_ParserInfo.LineIdx].DeviceTAB[DeviceIdx].TimeSynch		= (USINT) IEC_ItemData.TSynch.value;
										stANL[IEC_ParserInfo.LineIdx].DeviceTAB[DeviceIdx].AnlagenNr		= (UINT) IEC_ItemData.AnlagenNr.value;


										/* Datenpunktinformationen fuer Stoerschrieb */

										if(IEC_ItemData.ItemTypNr.value	== STREAM_IDX) {
											stANL[IEC_ParserInfo.LineIdx].DeviceActiv[DeviceIdx].pFaultDPInfo			= pIEC_DP_Info;
											if(IEC_ItemData.IECType.defined) {
												stANL[IEC_ParserInfo.LineIdx].DeviceActiv[DeviceIdx].FaultFunctionType	= (USINT) IEC_ItemData.IECType.value;
											}
										}
									} /* if(!error) */
								} /* if(!error) */
							} while(ItemFound);
						} /* if(TypFound) */
					} /* if(!error) */
				} /* if(iec_parser_dpinfo_ok())*/
			} /* if(DPFound) */
		} while(DPFound || BlockFound);
	} /* if(!IEC_ParserInfo.error) */

	memset(MsgTxt,0,sizeof(MsgTxt));
	strcpy(MsgTxt,ERR_MSG_PRE_STRG);
	strcat(MsgTxt,": IEC-Parser beendet");
	ERRxwarning( 0, bin_to_bcd_8(0), MsgTxt);

	memset(MsgTxt,0,sizeof(MsgTxt));
	strcpy(MsgTxt,ERR_MSG_PRE_STRG);
	strcat(MsgTxt,": Anzahl Meldungen ");
	brsitoa(IEC_ParserInfo.NrOfLogEntrys, (UDINT) &ANr);
	strcat(MsgTxt,ANr);
	ERRxwarning( 0, bin_to_bcd_8((unsigned long) IEC_ParserInfo.NrOfLogEntrys), MsgTxt);

}

/**********************************************************
*		Globale Parameter aus Datennmodul lesen
**********************************************************/
void iec_parser_global()
{

	BOOL			found;
	BOOL			dataReady;
	unsigned short	i;
	unsigned short	size;

		/* ----------------------------------------- */
	 	/* Globale Eintraege fuer IEC-Treiber parsen */
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
															KW_IEC_SECTION);

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

								/* Linie */
								if (strncasecmp(PaDPList.pDataArg1, KW_IEC_LINE, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_LINE) == PaDPList.SizeArg1 &&
									PaDPList.SizeArg2 == 0 &&
									PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
									IsANumber(PaDPList.pDataArg3) == TRUE) {

									if(IEC_ParserInfo.NrOfLines<MAX_LINES) {

										/* Nummer der Linie */
										IEC_ParserInfo.LineNrAct = brsatoi((UDINT) PaDPList.pDataArg3);
										/* Wurde Linie bereits parametriert ? */
										found = 0;
										for(i=0;i<IEC_ParserInfo.NrOfLines;i++) {
											if(IEC_GlobalData.Line[i].LineNr.value==IEC_ParserInfo.LineNrAct) {
												found					= 1;
												IEC_ParserInfo.LineIdx	= i;
											}
										}
										if(!found) {
											IEC_ParserInfo.LineIdx	= IEC_ParserInfo.NrOfLines;
											IEC_ParserInfo.NrOfLines++;
										}

										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].LineNr.value	 = IEC_ParserInfo.LineNrAct;
										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].LineNr.defined= 1;

										IEC_ParserInfo.LineNrValid = 1;
									}
									else {
										/* Fehler: Anzahl moeglicher Linien ueberschritten */
										iec_parser_warn(PARS_ERR_TO_MUCH_LINES, "Zuviele Linien (max. 6):", 0, "Zeile:", PaDPList.CountLines, "Liniennummer", (long)brsatoi((UDINT) PaDPList.pDataArg3), 0, 0);
									}
								}
								/* Interface */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_LINE_IF, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_LINE_IF) == PaDPList.SizeArg1 &&
									PaDPList.SizeArg2 == 0 &&
									PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0) {
									if(IEC_ParserInfo.LineNrValid) {

										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].Interface.name		= PaDPList.pDataArg3;
										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].Interface.size		= PaDPList.SizeArg3;
										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].Interface.defined	= 1;

										strcpy(stComIni[IEC_ParserInfo.LineIdx].StringDevice,PaDPList.pDataArg3);
										size = sizeof(stComIni[IEC_ParserInfo.LineIdx].StringDevice);

										if(PaDPList.SizeArg3<size) {
											stComIni[IEC_ParserInfo.LineIdx].StringDevice[PaDPList.SizeArg3] = 0;
										}
										else {
											stComIni[IEC_ParserInfo.LineIdx].StringDevice[size-1] = 0;
										}
									}
									else {
										/* Fehler: Liniennummer nicht definiert */
										iec_parser_warn(PARS_ERR_LINE_UNKNOWN, "Keine Linie deklariert:", PaDPList.pDataArg3, "Zeile:", PaDPList.CountLines, 0, 0, 0, 0);
									}
								}
								/* Freigabe Interface Reset */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_LINE_IF, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_LINE_IF) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_IF_RESET, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_IF_RESET) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.EnableComReset.value	= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.EnableComReset.defined 	= 1;

										if((IEC_GlobalData.EnableComReset.value<IF_RESET_MIN) || (IEC_GlobalData.EnableComReset.value>IF_RESET_MAX)) {
											iec_parser_warn(PARS_ERR_IF_RESET_NOT_VALID, "Wert ungueltig:", 0, "IF.Reset =", IEC_GlobalData.EnableComReset.value, "Min = ", IF_RESET_MIN, "Max =", IF_RESET_MAX);
										}
										else {
											IEC_Config.EnableComReset = IEC_GlobalData.EnableComReset.value;
										}
								}
								/* Mode Interface */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_LINE_MODE, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_LINE_MODE) == PaDPList.SizeArg1 &&
										PaDPList.SizeArg2 == 0 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0) {

									if(IEC_ParserInfo.LineNrValid) {
										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].InterfaceMode.name		= PaDPList.pDataArg3;
										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].InterfaceMode.size		= PaDPList.SizeArg3;
										IEC_GlobalData.Line[IEC_ParserInfo.LineIdx].InterfaceMode.defined	= 1;

										strcpy(stComIni[IEC_ParserInfo.LineIdx].StringMode,PaDPList.pDataArg3);
										size = sizeof(stComIni[IEC_ParserInfo.LineIdx].StringMode);

										if(PaDPList.SizeArg3<size) {
											stComIni[IEC_ParserInfo.LineIdx].StringMode[PaDPList.SizeArg3] = 0;
										}
										else {
											stComIni[IEC_ParserInfo.LineIdx].StringMode[size-1] = 0;
										}
									}
									else {
										/* Fehler: Liniennummer nicht definiert */
										iec_parser_warn(PARS_ERR_LINE_UNKNOWN, "Keine Line deklariert:", PaDPList.pDataArg3, "Zeile:", PaDPList.CountLines, 0, 0, 0, 0);
									}
								}
								/*  Timeout Telegrammempfang */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_TIMEOUT, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_TIMEOUT) == PaDPList.SizeArg1 &&
										PaDPList.SizeArg2 == 0 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.TimeOut.value	= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.TimeOut.defined 	= 1;

										if((IEC_GlobalData.TimeOut.value<TIMEOUT_MIN) || (IEC_GlobalData.TimeOut.value>TIMEOUT_MAX)) {
											iec_parser_warn(PARS_ERR_LINE_UNKNOWN, "Wert ungueltig:", 0, "Timeout =", IEC_GlobalData.TimeOut.value, "Min = ", TIMEOUT_MIN, "Max =", TIMEOUT_MAX);
										}
										else {
											IEC_Config.TimeOut = IEC_GlobalData.TimeOut.value;
										}

								}
								/*  Wiederholungen bei Telegrammfehlern */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_RETRY, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_RETRY) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_RETRY_ANZ, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_RETRY_ANZ) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.RetryAnz.value	= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.RetryAnz.defined	= 1;

										if((IEC_GlobalData.RetryAnz.value<RETRY_ANZ_MIN) || (IEC_GlobalData.RetryAnz.value>RETRY_ANZ_MAX)) {
											iec_parser_warn(PARS_ERR_RETRY_ANZ_NOT_VALID, "Wert ungueltig:", 0, "Retry.Anz =", IEC_GlobalData.RetryAnz.value, "Min = ", RETRY_ANZ_MIN, "Max =", RETRY_ANZ_MAX);
										}
										else {
											IEC_Config.RetryAnz = IEC_GlobalData.RetryAnz.value;
										}
								}
								/*  Anzahl Anmeldeversuche */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_CONN, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_CONN) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_CONN_ANZ, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_CONN_ANZ) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.ConnAnz.value	= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.ConnAnz.defined	= 1;

										if((IEC_GlobalData.ConnAnz.value<CONN_ANZ_MIN) || (IEC_GlobalData.ConnAnz.value>CONN_ANZ_MAX)) {
											iec_parser_warn(PARS_ERR_CONN_ANZ_NOT_VALID, "Wert ungueltig:", 0, "Conn.Anz =", IEC_GlobalData.ConnAnz.value, "Min = ", CONN_ANZ_MIN, "Max =", CONN_ANZ_MAX);
										}
										else {
											IEC_Config.ConnAnz = IEC_GlobalData.ConnAnz.value;
										}
								}
								/*  Wartezeit für Anmeldeversuch */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_WAIT, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_WAIT) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_WAIT_INTV, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_WAIT_INTV) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.WaitIntv.value	= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.WaitIntv.defined	= 1;

										if((IEC_GlobalData.WaitIntv.value<WAIT_INTV_MIN) || (IEC_GlobalData.WaitIntv.value>WAIT_INTV_MAX)) {
											iec_parser_warn(PARS_ERR_WAIT_INTV_NOT_VALID, "Wert ungueltig:", 0, "Wait.Intv =", IEC_GlobalData.WaitIntv.value, "Min = ", WAIT_INTV_MIN, "Max =", WAIT_INTV_MAX);
										}
										else {
											IEC_Config.WaitIntv = IEC_GlobalData.WaitIntv.value;
										}
								}
								/*  Zeitsynchronisation */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_TIME, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_TIME) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_TIME_TS, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_TIME_TS) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.TimeTS.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.TimeTS.defined	= 1;

										if((IEC_GlobalData.TimeTS.value<TIME_TS_MIN) || (IEC_GlobalData.TimeTS.value>TIME_TS_MAX)) {
											iec_parser_warn(PARS_ERR_TIME_TS_NOT_VALID, "Wert ungueltig:", 0, "Time.TS =", IEC_GlobalData.TimeTS.value, "Min = ", TIME_TS_MIN, "Max =", TIME_TS_MAX);
										}
										else {
											IEC_Config.TimeTS = IEC_GlobalData.TimeTS.value;
										}
								}
								/*  Abtastzeit Class 2 Telegramme */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_TIME, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_TIME) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_TIME_CLASS2, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_TIME_CLASS2) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.TimeClass2Poll.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.TimeClass2Poll.defined	= 1;

										if((IEC_GlobalData.TimeClass2Poll.value<TIME_CLASS2_MIN) || (IEC_GlobalData.TimeClass2Poll.value>TIME_CLASS2_MAX)) {
											iec_parser_warn(PARS_ERR_TIME_CLASS2_NOT_VALID, "Wert ungueltig:", 0, "Time.Class2 =", IEC_GlobalData.TimeClass2Poll.value, "Min = ", TIME_CLASS2_MIN, "Max =", TIME_CLASS2_MAX);
										}
										else {
											IEC_Config.TimeClass2Poll = IEC_GlobalData.TimeClass2Poll.value;
										}
								}
								/*  Abtastzeit hoch, Class 2 Telegramme */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_TIME, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_TIME) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_TIME_CLASS2F, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_TIME_CLASS2F) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.TimeClass2PollFast.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.TimeClass2PollFast.defined	= 1;

										if((IEC_GlobalData.TimeClass2PollFast.value<TIME_CLASS2F_MIN) || (IEC_GlobalData.TimeClass2PollFast.value>TIME_CLASS2F_MAX)) {
											iec_parser_warn(PARS_ERR_TIME_CLASS2F_NOT_VALID, "Wert ungueltig:", 0, "Time.Class2F =", IEC_GlobalData.TimeClass2PollFast.value, "Min = ", TIME_CLASS2F_MIN, "Max =", TIME_CLASS2F_MAX);
										}
										else {
											IEC_Config.TimeClass2PollFast = IEC_GlobalData.TimeClass2PollFast.value;
										}
								}
								/*  Freigabe Übertragung von Störschrieben */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_DD, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_DD) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_DD_ENABLE, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_DD_ENABLE) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.DistDataEnable.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.DistDataEnable.defined	= 1;

										if((IEC_GlobalData.DistDataEnable.value<DD_ENABLE_MIN) || (IEC_GlobalData.DistDataEnable.value>DD_ENABLE_MAX)) {
											iec_parser_warn(PARS_ERR_DD_ENABLE_NOT_VALID, "Wert ungueltig:", 0, "DD.Enable =", IEC_GlobalData.DistDataEnable.value, "Min = ", DD_ENABLE_MIN, "Max =", DD_ENABLE_MAX);
										}
										else {
											IEC_Config.DistDataEnable = IEC_GlobalData.DistDataEnable.value;
										}
								}
								/*  Timeout bei Übertragung von Störschrieben */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_DD, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_DD) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_DD_TIMEOUT, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_DD_TIMEOUT) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.DistDataTimeoutMax.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.DistDataTimeoutMax.defined	= 1;

										if((IEC_GlobalData.DistDataTimeoutMax.value<DD_TIMEOUT_MIN) || (IEC_GlobalData.DistDataTimeoutMax.value>DD_TIMEOUT_MAX)) {
											iec_parser_warn(PARS_ERR_DD_TIMEOUT_NOT_VALID, "Wert ungueltig:", 0, "DD.Timeout =", IEC_GlobalData.DistDataTimeoutMax.value, "Min = ", DD_TIMEOUT_MIN, "Max =", DD_TIMEOUT_MAX);
										}
										else {
											IEC_Config.DistDataTimeoutMax = IEC_GlobalData.DistDataTimeoutMax.value;
										}
								}
								/*  Verzögerung bei Übertragung von Störschrieben des gleichen Gerätes */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_DD, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_DD) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_DD_DELAY, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_DD_DELAY) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.DisturbanceDelay.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.DisturbanceDelay.defined		= 1;

										if((IEC_GlobalData.DisturbanceDelay.value<DD_DELAY_MIN) || (IEC_GlobalData.DisturbanceDelay.value>DD_DELAY_MAX)) {
											iec_parser_warn(PARS_ERR_DD_DELAY_NOT_VALID, "Wert ungueltig:", 0, "DD.Delay =", IEC_GlobalData.DisturbanceDelay.value, "Min = ", DD_DELAY_MIN, "Max =", DD_DELAY_MAX);
										}
										else {
											IEC_Config.DisturbanceDelay = IEC_GlobalData.DisturbanceDelay.value;
										}
								}
								/*  Freigabe Übertragung von Kommandos */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_CMD, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_CMD) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_CMD_ENABLE, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_CMD_ENABLE) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.CommandEnable.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.CommandEnable.defined	= 1;

										if((IEC_GlobalData.CommandEnable.value<CMD_ENABLE_MIN) || (IEC_GlobalData.CommandEnable.value>CMD_ENABLE_MAX)) {
											iec_parser_warn(PARS_ERR_CMD_ENABLE_NOT_VALID, "Wert ungueltig:", 0, "CMD.Delay =", IEC_GlobalData.CommandEnable.value, "Min = ", CMD_ENABLE_MIN, "Max =", CMD_ENABLE_MAX);
										}
										else {
											IEC_Config.CommandEnable = IEC_GlobalData.CommandEnable.value;
										}
								}
								/*  Timeout bei der Übertragung von Kommandos */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_CMD, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_CMD) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_CMD_TIMEOUT, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_CMD_TIMEOUT) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.CommandTimeoutMax.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.CommandTimeoutMax.defined	= 1;

										if((IEC_GlobalData.CommandTimeoutMax.value<CMD_TIMEOUT_MIN) || (IEC_GlobalData.CommandTimeoutMax.value>CMD_TIMEOUT_MAX)) {
											iec_parser_warn(PARS_ERR_CMD_TIMEOUT_NOT_VALID, "Wert ungueltig:", 0, "CMD.Delay =", IEC_GlobalData.CommandTimeoutMax.value, "Min = ", CMD_TIMEOUT_MIN, "Max =", CMD_TIMEOUT_MAX);
										}
										else {
											IEC_Config.CommandTimeoutMax = IEC_GlobalData.CommandTimeoutMax.value;
										}
								}
								/*  Timeout Rueckmeldung Zeitsynchronisation */
								else if (strncasecmp(PaDPList.pDataArg1, KW_IEC_TIME, PaDPList.SizeArg1) == 0 && strlen(KW_IEC_TIME) == PaDPList.SizeArg1 &&
										strncasecmp(PaDPList.pDataArg2, KW_IEC_TIME_TOUT, PaDPList.SizeArg2) == 0 && strlen(KW_IEC_TIME_TOUT) == PaDPList.SizeArg2 &&
										PaDPList.pDataArg3 != NULL && PaDPList.SizeArg3 != 0 &&
										IsANumber(PaDPList.pDataArg3) == TRUE) {

										IEC_GlobalData.TimeOutTimeSynch.value		= brsatoi((UDINT) PaDPList.pDataArg3);
										IEC_GlobalData.TimeOutTimeSynch.defined		= 1;

										if((IEC_GlobalData.TimeOutTimeSynch.value<TIME_TIMEOUT_MIN) || (IEC_GlobalData.TimeOutTimeSynch.value>TIME_TIMEOUT_MAX)) {
											iec_parser_warn(PARS_ERR_TIME_TIMEOUT_NOT_VALID, "Wert ungueltig:", 0, "TIME.TIMEOUT =", IEC_GlobalData.TimeOutTimeSynch.value, "Min = ", TIME_TIMEOUT_MIN, "Max =", TIME_TIMEOUT_MAX);
										}
										else {
											IEC_Config.TimeOutTimeSynch = IEC_GlobalData.TimeOutTimeSynch.value;
										}
								}
								/* Unbekannter Eintrag */
								else {
									iec_parser_warn(PARS_ERR_UNKNOWN_GLOBAL_ENTRY,"Eintrag unbekannt:", PaDPList.pParserLine, " / Zeile =", (long) PaDPList.CountLines, 0, 0, 0, 0);
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
void iec_parser_dpinfo(plcbit *pBlockFound, plcbit *pDPFound)
{

		*pDPFound	= 0;
		*pBlockFound= 0;

		/* -------------------------------------------- */
	 	/* Datenpunkt Eintraege fuer IEC-Treiber parsen */
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

					/* Suchen nach Sektion "IEC" */
					do {
						State_find_Section = find_Section (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
															(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
															&PaDPList.pParser, 	 &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
															&PaDPList.pDataArg1, &PaDPList.SizeArg1,
															&PaDPList.pDataArg2, &PaDPList.SizeArg2,
															KW_IEC_SECTION);

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

						*pDPFound		= 1;
						iec_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
										(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
										(Parser_typ *) &PaDPList,
										(IEC_ItemData_typ *) &IEC_DPData,
										IEC_SECTION,DP_PART);

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
						iec_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
										(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
										(Parser_typ *) &PaDPList,
										(IEC_ItemData_typ *) &IEC_DPData,
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

						/* Suchen nach Sektion "IEC" */
						do {
							State_find_Section = find_Section (	(unsigned char*) DOI_dplist.pDatObjMem, PaDPList.pParser,
																(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
																&PaDPList.pParser, 	 &PaDPList.pParserLine, &PaDPList.pParserErr, &PaDPList.CountLines,
																&PaDPList.pDataArg1, &PaDPList.SizeArg1,
																&PaDPList.pDataArg2, &PaDPList.SizeArg2,
																KW_IEC_SECTION);

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

							iec_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
											(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
											(Parser_typ *) &PaDPList,
											(IEC_ItemData_typ *) &IEC_BlockData,
											IEC_SECTION,DP_PART);

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
							iec_parser_data((unsigned char*) DOI_dplist.pDatObjMem,
											(unsigned char*) DOI_dplist.pDatObjMem + DOI_dplist.len,
											(Parser_typ *) &PaDPList,
											(IEC_ItemData_typ *) &IEC_BlockData,
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
plcbit iec_parser_dptype(unsigned char *pName, unsigned short size)
{

		BOOL	TypFound=0;

		/* -------------------------------------------- */
	 	/* Datenpunkt Eintraege fuer IEC-Treiber parsen */
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

					/* Suchen nach Sektion "IEC" */
					State_find_Section = find_Section (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
														(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
														&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
														&PaTypList.pDataArg1, &PaTypList.SizeArg1,
														&PaTypList.pDataArg2, &PaTypList.SizeArg2,
														KW_IEC_SECTION);

					/* Section gefunden ? Daten parsen */
					if (State_find_Section == TRUE) {

						/* Pointer auf Anfang der Sektion stellen */
						if (PaTypList.pDataArg2 != NULL) {
							PaTypList.pParser = PaTypList.pDataArg2;
						}
						else {
							continue;
						}

						iec_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
										(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
										(Parser_typ *) &PaTypList,
										(IEC_ItemData_typ *) &IEC_DPData,
										IEC_SECTION,DP_PART);
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

						iec_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
										(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
										(Parser_typ *) &PaTypList,
										(IEC_ItemData_typ *) &IEC_DPData,
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
plcbit iec_parser_iteminfo()
{

		BOOL	ItemFound=0;

		/* -------------------------------------------- */
	 	/* Datenpunkt Eintraege fuer IEC-Treiber parsen */
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

				/* Suchen nach Sektion "IEC" */
				State_find_Section = find_Section (	(unsigned char*) DOI_typlist.pDatObjMem, PaTypList.pParser,
													(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
													&PaTypList.pParser, &PaTypList.pParserLine, &PaTypList.pParserErr, &PaTypList.CountLines,
													&PaTypList.pDataArg1, &PaTypList.SizeArg1,
													&PaTypList.pDataArg2, &PaTypList.SizeArg2,
													KW_IEC_SECTION);

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

				iec_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
								(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
								(Parser_typ *) &PaTypList,
								(IEC_ItemData_typ *) &IEC_ItemData,
								IEC_SECTION,ITEM_PART);


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

					iec_parser_data((unsigned char*) DOI_typlist.pDatObjMem,
									(unsigned char*) DOI_typlist.pDatObjMem + DOI_typlist.len,
									(Parser_typ *) &PaTypList,
									(IEC_ItemData_typ *) &IEC_ItemData,
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
void iec_parser_data(unsigned char *pSrcStart,unsigned char *pSrcEnd, Parser_typ *pPa, IEC_ItemData_typ *pData, unsigned char SectionInfo, unsigned char PartInfo) {

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
				/* Linie */
				else if (strncasecmp(pPa->pDataArg1, KW_IEC_LINE, pPa->SizeArg1) == 0 && strlen(KW_IEC_LINE) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber  == TRUE) {

						/* Nummer der Linie */
						pData->Line.value			= Arg3Value;
						pData->Line.defined			= 1;
						dataFound					= 1;
				}
				/* Device Adresse */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_IEC_DEV, pPa->SizeArg1) == 0 && strlen(KW_DP_IEC_DEV) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_DP_IEC_DEV_ADR, pPa->SizeArg2) == 0 && strlen(KW_DP_IEC_DEV_ADR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->DeviceAdr.value		= Arg3Value;
						pData->DeviceAdr.defined	= 1;
						dataFound					= 1;
				}
				/*  Zeitsynchronisation */
				else if (strncasecmp(pPa->pDataArg1, KW_DP_IEC_TSYNCH, pPa->SizeArg1) == 0 && strlen(KW_DP_IEC_TSYNCH) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->TSynch.value			= Arg3Value;
						pData->TSynch.defined		= 1;
						dataFound					= 1;
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
			}

			if(((PartInfo==ITEM_PART) || (PartInfo==DP_PART)) && (!dataFound)) {
				/* IEC Type */
				if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_TYPE, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_TYPE) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->IECType.value		= Arg3Value;
						pData->IECType.defined		= 1;
						dataFound					= 1;
				}
				/* Info-Nr. */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_INFO, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_INFO) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_INFO_NR, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_INFO_NR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->IECInfoNr.value		= Arg3Value;
						pData->IECInfoNr.defined	= 1;
						dataFound					= 1;
				}
				/* Analogwert-Nr. */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_ANALOG, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_ANALOG) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_ANALOG_NR, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_ANALOG_NR) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->IECAnalogNr.value	= Arg3Value;
						pData->IECAnalogNr.defined	= 1;
						dataFound					= 1;
				}
				/*  Rohwert Min. */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_RAW, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_RAW) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_RAW_MIN, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_RAW_MIN) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->RawMin.value			= Arg3Value;
						pData->RawMin.defined		= 1;
						dataFound					= 1;
				}
				/*  Rohwert Max. */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_RAW, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_RAW) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_RAW_MAX, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_RAW_MAX) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->RawMax.value			= Arg3Value;
						pData->RawMax.defined		= 1;
						dataFound					= 1;
				}
				/*  Normierter Wert Min. */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_NORM, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_NORM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_NORM_MIN, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_NORM_MIN) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->NormMin.value		= Arg3Value;
						pData->NormMin.defined		= 1;
						dataFound					= 1;
				}
				/*  Normierter Wert Max. */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_NORM, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_NORM) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_NORM_MAX, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_NORM_MAX) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->NormMax.value		= Arg3Value;
						pData->NormMax.defined		= 1;
						dataFound					= 1;
				}
				/*  Meldung mit LED-RST löschen */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_MRUC, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_MRUC) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->MRuc.value			= Arg3Value;
						pData->MRuc.defined			= 1;
						dataFound					= 1;
				}
				/*  Info für LED RST */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_MRUC, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_MRUC) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_MRUC_INFO, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_MRUC_INFO) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->MRucInfo.value		= Arg3Value;
						pData->MRucInfo.defined		= 1;
						dataFound					= 1;
				}
				/*  Type für LED RST */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_MRUC, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_MRUC) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_MRUC_TYPE, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_MRUC_TYPE) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->MRucType.value		= Arg3Value;
						pData->MRucType.defined		= 1;
						dataFound					= 1;
				}
				/*  Modus */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_MODE, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_MODE) == pPa->SizeArg1 &&
						pPa->SizeArg2==0 &&
						Arg3IsANumber == TRUE) {

						pData->Mode.value			= Arg3Value;
						pData->Mode.defined			= 1;
						dataFound					= 1;
				}
				/*  Auswertung Invalid-Bit Analogwerte */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_ERROR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_ERROR) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_INVALID, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_INVALID) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->Invalid.defined		= 1;
						pData->Invalid.value		= Arg3Value;
						dataFound					= 1;
				}
				/*  Auswertung Overflow-Bit Analogwerte */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_ERROR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_ERROR) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_OVERFLOW, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_OVERFLOW) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->Overflow.defined		= 1;
						pData->Overflow.value		= Arg3Value;
						dataFound					= 1;
				}
				/*  Auswertung Kopplungsstoerung */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_ERROR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_ERROR) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_KOPPLUNG, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_KOPPLUNG) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->KopplungsStoerung.defined	= 1;
						pData->KopplungsStoerung.value		= Arg3Value;
						dataFound							= 1;
				}
				/*  Auswertung Fehler Zeitsynchronisation */
				else if (strncasecmp(pPa->pDataArg1, KW_ITEM_IEC_ERROR, pPa->SizeArg1) == 0 && strlen(KW_ITEM_IEC_ERROR) == pPa->SizeArg1 &&
						strncasecmp(pPa->pDataArg2, KW_ITEM_IEC_TIMESYNCH, pPa->SizeArg2) == 0 && strlen(KW_ITEM_IEC_TIMESYNCH) == pPa->SizeArg2 &&
						Arg3IsANumber == TRUE) {

						pData->TimeSynchError.defined	= 1;
						pData->TimeSynchError.value		= Arg3Value;
						dataFound						= 1;
				}
			}

			/* Unbekannter Eintrag */
			if((!dataFound) && (SectionInfo==IEC_SECTION)) {
				iec_parser_warn(PARS_ERR_UNKNOWN_ITEM_ENTRY,"Unbekannter Eintrag im Item-Bereich:", pPa->pParserLine, " / Zeile =", (long) pPa->CountLines, 0, 0, 0, 0);
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
plcbit iec_parser_dpinfo_ok(IEC_ItemData_typ *pData)
{
	unsigned short error;
	plcbit ok;

	ok = 1;
	if(!pData->AnlagenNr.defined) {
		iec_parser_warn(PARS_ERR_ANLNR_NOT_DEFINED,"DP Anlage.Nr fehlt. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}
	if(!pData->DPNr.defined) {
		iec_parser_warn(PARS_ERR_DPNR_NOT_DEFINED,"DP DP.Nr fehlt. DPTyp: ", pData->Typ.name , 0, 0, 0, 0, 0, 0);
		ok = 0;
	}
	if(!pData->Typ.defined) {
		iec_parser_warn(PARS_ERR_TYP_NOT_DEFINED,"DP Typbezeichnung fehlt. DPTyp: ", 0, "DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}
	if(!pData->DeviceAdr.defined) {
		iec_parser_warn(PARS_ERR_DEV_ADR_NOT_DEFINED,"DP Dev.Adr fehlt. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}
	else {
		if((pData->DeviceAdr.value<DEV_ADR_MIN) || (pData->DeviceAdr.value>DEV_ADR_MAX)) {
			iec_parser_warn(PARS_ERR_DEV_ADR_NOT_VALID,"DP Dev.Adr ungueltig. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value,  "Wert:", pData->DeviceAdr.value, 0, 0);
			ok = 0;
		}
	}

	error	= get_line_idx(pData, (IEC_GlobalData_typ *) &IEC_GlobalData, (unsigned short *) &IEC_ParserInfo.LineIdx);

	if(error!=NO_ERROR) {
		if(error==PARS_ERR_LINE_NOT_DEFINED) {
			iec_parser_warn(PARS_ERR_LINE_NOT_DEFINED,"DP Linienangabe fehlt. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value,0,0,0,0);
		}
		else {
			iec_parser_warn(PARS_ERR_LINE_NOT_VALID,"DP Liniennummer nicht vorhanden. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value,0,0,0,0);
		}
		ok = 0;
	}

	if(pData->TSynch.defined) {
		if((pData->TSynch.value<TSYNCH_MIN) || (pData->TSynch.value>TSYNCH_MAX)) {
			iec_parser_warn(PARS_ERR_TSYNCH_NOT_VALID,"DP TSync ungueltig. DPTyp: ", pData->Typ.name ,"DPNr.", pData->DPNr.value, "Wert:", pData->TSynch.value, 0, 0);
			ok = 0;
		}
	}

	return(ok);
}

/*****************************************************************
*	Pruefen, ob alle Angaben eines Items vorhanden sind
******************************************************************/
plcbit iec_parser_iteminfo_ok(IEC_ItemData_typ *pData)
{

	plcbit	ok;
	long	Mode;

	ok = 1;
	/* Datentyp Nummer definiert ? */
	if(!pData->DPTypNr.defined) {
		iec_parser_warn(PARS_ERR_DPTYP_NR_NOT_DEFINED,"Typ.Nr fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}

	/* Item Nummer definiert ? */
	if(!pData->ItemNr.defined) {
		iec_parser_warn(PARS_ERR_ITEM_NR_NOT_DEFINED,"Item.Nr fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, 0, 0, 0, 0);
		ok = 0;
	}


	/* Modus definiert und gültig ? */
	if(pData->Mode.defined) {
		if((pData->Mode.value<MODE_MIN) || (pData->Mode.value>MODE_MAX)) {
			iec_parser_warn(PARS_ERR_MODE_NOT_VALID,"ITEM Mode ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->Mode.value);
			ok = 0;
		}
	}

	/* Item Typ definiert ? */
	if(!pData->ItemTyp.defined) {
		iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_DEFINED,"ITEM Datentyp fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
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
		if(strncasecmp(pData->ItemTyp.name, STREAM_TYP, pData->ItemTyp.size) == 0 && strlen(STREAM_TYP) == pData->ItemTyp.size) {
			pData->ItemTypNr.defined	= 1;
			pData->ItemTypNr.value		= STREAM_IDX;
		}
		if(!pData->ItemTypNr.defined) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_UNKNOWN,"ITEM Datentyp unbekannt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
			ok = 0;
		}
	}


	if((IEC_ItemData.ItemTypNr.value != STREAM_IDX) && (!pData->KopplungsStoerung.defined) && (!pData->TimeSynchError.defined)) {
		/* IEC Type definiert und gültig ? */
		if(!pData->IECType.defined) {
			iec_parser_warn(PARS_ERR_IEC_TYPE_NOT_DEFINED,"ITEM Type fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
		}
		else {
			if((pData->IECType.value<IEC_TYPE_MIN) || (pData->IECType.value>IEC_TYPE_MAX)) {
				iec_parser_warn(PARS_ERR_IEC_TYPE_NOT_VALID,"ITEM Type ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->IECType.value);
				ok = 0;
			}
		}

		/* IEC Info definiert und gültig ? */
		if(!pData->IECInfoNr.defined) {
			iec_parser_warn(PARS_ERR_IEC_INFO_NOT_DEFINED,"ITEM Info.Nr fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
			ok = 0;
		}
		else {
			if((pData->IECInfoNr.value<IEC_INFO_MIN) || (pData->IECInfoNr.value>IEC_INFO_MAX)) {
				iec_parser_warn(PARS_ERR_IEC_INFO_NOT_VALID,"ITEM Info.Nr ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->IECInfoNr.value);
				ok = 0;
			}
		}
	}

	/* IEC Analogwert Nummer definiert und gültig ? */
	if(pData->IECAnalogNr.defined) {
		if((pData->IECAnalogNr.value<IEC_ANALOGNR_MIN) || (pData->IECAnalogNr.value>IEC_ANALOGNR_MAX)) {
			iec_parser_warn(PARS_ERR_IEC_ANALOGNR_NOT_VALID,"ITEM Analog.Nr ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->IECAnalogNr.value);
			ok = 0;
		}
	}

	/* Normierungsangaben vollständig und gültig ? */
	if((pData->IECAnalogNr.defined) && (!pData->Invalid.defined) && (!pData->Overflow.defined)) {
		if(pData->RawMin.defined || pData->RawMax.defined || pData->NormMin.defined || pData->NormMax.defined) {
			/* Raw-Minwert definiert und gültig ? */
			if(!pData->RawMin.defined) {
				iec_parser_warn(PARS_ERR_RAWMIN_NOT_DEFINED,"ITEM Raw.Min fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
				ok = 0;
			}
			else {
				if((pData->RawMin.value<RAW_MIN) || (pData->RawMin.value>RAW_MAX)) {
					iec_parser_warn(PARS_ERR_RAWMIN_NOT_VALID,"ITEM Raw.Min ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->RawMin.value);
					ok = 0;
				}
			}

			/* Raw-Maxwert definiert und gültig ? */
			if(!pData->RawMax.defined) {
				iec_parser_warn(PARS_ERR_RAWMAX_NOT_DEFINED,"ITEM Raw.Max fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
				ok = 0;
			}
			else {
				if((pData->RawMax.value<RAW_MIN) || (pData->RawMax.value>RAW_MAX)) {
					iec_parser_warn(PARS_ERR_RAWMAX_NOT_VALID,"ITEM Raw.Max ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->RawMax.value);
					ok = 0;
				}
			}

			/* Norm-Minwert definiert und gültig ? */
			if(!pData->NormMin.defined) {
				iec_parser_warn(PARS_ERR_NORMMIN_NOT_DEFINED,"ITEM Norm.Min fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
				ok = 0;
			}
			else {
				if((pData->NormMin.value<NORM_MIN) || (pData->NormMin.value>NORM_MAX)) {
					iec_parser_warn(PARS_ERR_NORMMIN_NOT_VALID,"ITEM Norm.Min ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->NormMin.value);
					ok = 0;
				}
			}

			/* Norm-Maxwert definiert und gültig ? */
			if(!pData->NormMax.defined) {
				iec_parser_warn(PARS_ERR_NORMMAX_NOT_DEFINED,"ITEM Norm.Max fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
				ok = 0;
			}
			else {
				if((pData->NormMax.value<NORM_MIN) || (pData->NormMax.value>NORM_MAX)) {
					iec_parser_warn(PARS_ERR_NORMMAX_NOT_VALID,"ITEM Norm.Max ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->NormMax.value);
					ok = 0;
				}
			}

		}
	}

	/* Funktion Meldung rücksetzen definiert und gültig ? */
	if(pData->MRuc.defined) {
		if((pData->MRuc.value<MRUC_MIN) || (pData->MRuc.value>MRUC_MAX)) {
			iec_parser_warn(PARS_ERR_MRUC_NOT_VALID,"ITEM MRuc ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->MRuc.value);
			ok = 0;
		}
		else {
			/* MRuc.Info definiert und gültig ? */
			if(!pData->MRucInfo.defined) {
				iec_parser_warn(PARS_ERR_MRUC_INFO_NOT_DEFINED,"ITEM MRuc.Info fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
				ok = 0;
			}
			else {
				if((pData->MRucInfo.value<MRUC_INFO_MIN) || (pData->MRucInfo.value>MRUC_INFO_MAX)) {
					iec_parser_warn(PARS_ERR_MRUC_INFO_NOT_VALID,"ITEM MRuc.Info ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->MRucInfo.value);
					ok = 0;
				}
			}

			/* MRuc.Type definiert und gültig ? */
			if(!pData->MRucType.defined) {
				iec_parser_warn(PARS_ERR_MRUC_TYPE_NOT_DEFINED,"ITEM MRuc.Type fehlt. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);
				ok = 0;
			}
			else {
				if((pData->MRucType.value<MRUC_TYPE_MIN) || (pData->MRucType.value>MRUC_TYPE_MAX)) {
					iec_parser_warn(PARS_ERR_MRUC_TYPE_NOT_VALID,"ITEM MRuc.Type ungueltig. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, "Wert:", pData->MRucType.value);
					ok = 0;
				}
			}
		}
	}


	/* Pruefen, ob der angegebene ITEM-Datentyp zu den erwarteten Daten passt */
	if(ok) {
		/* Analogwerte muessen vom Typ INT,DINT,UINT,UDINT sein */
		if((pData->IECAnalogNr.defined) && (!pData->Invalid.defined) && (!pData->Overflow.defined)) {
			switch(pData->ItemTypNr.value) {
				case BOOL_IDX:
				case USINT_IDX:
				case SINT_IDX:
				case STREAM_IDX:
/*					iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0); */
					iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend. DPTyp: ", pData->Typ.name , "", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);

					ok = 0;
				break;
			}
		}
		else {
			if(pData->Mode.defined) {
				Mode = pData->Mode.value;
			}
			else {
				Mode = DEFAULT_MODE;
			}
			if(pData->ItemTypNr.value==BOOL_IDX) {
				switch(Mode) {
					case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
					case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
					case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
					case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
/*						iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend. DPTyp: ", pData->Typ.name , "DPNr.", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0); */
						iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend. DPTyp: ", pData->Typ.name , "", pData->DPNr.value, "Item-Nr.", pData->ItemNr.value, 0, 0);

						ok = 0;
					break;
				}
			}
		}
	}

	return(ok);
}

/***************************************************************************
*		Pruefen, ob zu einem Analogwert ein Normierungswert vorhanden ist
****************************************************************************/
plcbit normierung_defined(IEC_ItemData_typ *pItemData)
{
	if(pItemData->IECAnalogNr.defined) {						/* Normierungswert angegeben ? (nur bei Analogwerten) */
		if(	pItemData->RawMin.defined	&& 	pItemData->RawMax.defined &&
			pItemData->NormMin.defined	&&	pItemData->NormMax.defined) {
			return(1);
		}
	}
	return(0);
}

/***************************************************************************
*		Zeiger auf Startadresse Datenmodul stellen, Zeilenzaehler loeschen
****************************************************************************/
void copyItemData(IEC_ItemData_typ *pItemDest, IEC_ItemData_typ *pItemSrc)
{
	if(pItemSrc->AnlagenNr.defined)		memcpy(&pItemDest->AnlagenNr,	&pItemSrc->AnlagenNr,	sizeof(pItemDest->AnlagenNr));
	if(pItemSrc->DPNr.defined)			memcpy(&pItemDest->DPNr,		&pItemSrc->DPNr,		sizeof(pItemDest->DPNr));
	if(pItemSrc->Typ.defined)			memcpy(&pItemDest->Typ,			&pItemSrc->Typ,			sizeof(pItemDest->Typ));
	if(pItemSrc->DeviceAdr.defined)		memcpy(&pItemDest->DeviceAdr,	&pItemSrc->DeviceAdr,	sizeof(pItemDest->DeviceAdr));
	if(pItemSrc->Line.defined)			memcpy(&pItemDest->Line,		&pItemSrc->Line,		sizeof(pItemDest->Line));
	if(pItemSrc->TSynch.defined)		memcpy(&pItemDest->TSynch,		&pItemSrc->TSynch,		sizeof(pItemDest->TSynch));
	if(pItemSrc->DPTypNr.defined)		memcpy(&pItemDest->DPTypNr,		&pItemSrc->DPTypNr,		sizeof(pItemDest->DPTypNr));
	if(pItemSrc->ItemNr.defined)		memcpy(&pItemDest->ItemNr,		&pItemSrc->ItemNr,		sizeof(pItemDest->ItemNr));
	if(pItemSrc->ItemTyp.defined)		memcpy(&pItemDest->ItemTyp,		&pItemSrc->ItemTyp,		sizeof(pItemDest->ItemTyp));
	if(pItemSrc->IECType.defined)		memcpy(&pItemDest->IECType,		&pItemSrc->IECType,		sizeof(pItemDest->IECType));
	if(pItemSrc->IECInfoNr.defined)		memcpy(&pItemDest->IECInfoNr,	&pItemSrc->IECInfoNr,	sizeof(pItemDest->IECInfoNr));
	if(pItemSrc->IECAnalogNr.defined)	memcpy(&pItemDest->IECAnalogNr,	&pItemSrc->IECAnalogNr,	sizeof(pItemDest->IECAnalogNr));
	if(pItemSrc->RawMin.defined)		memcpy(&pItemDest->RawMin,		&pItemSrc->RawMin,		sizeof(pItemDest->RawMin));
	if(pItemSrc->RawMax.defined)		memcpy(&pItemDest->RawMax,		&pItemSrc->RawMax,		sizeof(pItemDest->RawMax));
	if(pItemSrc->NormMin.defined)		memcpy(&pItemDest->NormMin,		&pItemSrc->NormMin,		sizeof(pItemDest->NormMin));
	if(pItemSrc->NormMax.defined)		memcpy(&pItemDest->NormMax,		&pItemSrc->NormMax,		sizeof(pItemDest->NormMax));
	if(pItemSrc->MRuc.defined)			memcpy(&pItemDest->MRuc,		&pItemSrc->MRuc,		sizeof(pItemDest->MRuc));
	if(pItemSrc->MRucInfo.defined)		memcpy(&pItemDest->MRucInfo,	&pItemSrc->MRucInfo,	sizeof(pItemDest->MRucInfo));
	if(pItemSrc->MRucType.defined)		memcpy(&pItemDest->MRucType,	&pItemSrc->MRucType,	sizeof(pItemDest->MRucType));
	if(pItemSrc->Mode.defined)			memcpy(&pItemDest->Mode,		&pItemSrc->Mode,		sizeof(pItemDest->Mode));
	if(pItemSrc->Invalid.defined)		memcpy(&pItemDest->Invalid,		&pItemSrc->Invalid,		sizeof(pItemDest->Invalid));
	if(pItemSrc->Overflow.defined)		memcpy(&pItemDest->Overflow,	&pItemSrc->Overflow,	sizeof(pItemDest->Overflow));
	if(pItemSrc->KopplungsStoerung.defined)		memcpy(&pItemDest->KopplungsStoerung,	&pItemSrc->KopplungsStoerung,	sizeof(pItemDest->KopplungsStoerung));
	if(pItemSrc->TimeSynchError.defined)		memcpy(&pItemDest->TimeSynchError,		&pItemSrc->TimeSynchError,	sizeof(pItemDest->TimeSynchError));
}
/*****************************************************************
*	Linienindex ermitteln
******************************************************************/
unsigned short get_line_idx(IEC_ItemData_typ *pDPInfo, IEC_GlobalData_typ *pGlobalData, unsigned short *LineIdx)
{

	unsigned short	error,i;
	plcbit			found;

	error = NO_ERROR;

	/* Wurde der Datenpunkt einer Linie zugeordnet ? */
	if(!pDPInfo->Line.defined) {
		error = PARS_ERR_LINE_NOT_DEFINED;
	}
	else {
		/* Pruefen, ob die angegebene Linie im allgemeinen Teil deklariert wurde */
		/* Linienindex speichern */
		found = 0;
		for(i=0;i<MAX_LINES;i++) {
			if(pGlobalData->Line[i].LineNr.defined) {
				if(pGlobalData->Line[i].LineNr.value==pDPInfo->Line.value) {
					found		= 1;
					*LineIdx	= i;
				}
			}
		}
		if(!found) {
			/* Fehler: Die dem Datenpunkt zugehörige Liniennummer wurde im globalen Teil nicht deklariert */
			error = PARS_ERR_LINE_NOT_VALID;
		}
	}

	return(error);
}

/***************************************************************************
*		Index eines Gerätes ermitteln
****************************************************************************/

unsigned short get_device_idx(unsigned short LineIdx, long DeviceAdr, unsigned short *DeviceIdx, plcbit *NewDevice)
{
	plcbit found=0;
	unsigned short i;

	/* Suchen, ob Device bereits vorhanden ist */
	for(i=0;i<DEVICES_PRO_LINE;i++) {
		if(stANL[LineIdx].DeviceTAB[i].Adresse	== (USINT) DeviceAdr) {
			found		= 1;
			*DeviceIdx	= i;
			break;
		}
	}
	/* Device noch nicht vorhanden -> ersten freien Platz verwenden */
	if(!found) {
		for(i=0;i<DEVICES_PRO_LINE;i++) {
			if(!stANL[LineIdx].DeviceTAB[i].Adresse) {
				found		= 1;
				*DeviceIdx	= i;
				break;
			}
		}
		*NewDevice = TRUE;
	}
	else {
		*NewDevice = FALSE;
	}
	if(found)	return(NO_ERROR);
	else		return(PARS_ERR_TO_MUCH_DEVICES);
}


/***************************************************************************
*		Zeiger auf Startadresse Datenmodul stellen, Zeilenzaehler loeschen
****************************************************************************/
void iec_parser_dplist_clear()
{

	/* Datenmodul von Anfang an Parsen .. */
	PaDPList.pParser		= (unsigned char*) DOI_dplist.pDatObjMem;
	PaDPList.CountLines		= 0;
}

/***************************************************************************
*		Zeiger auf Startadresse Datenmodul stellen, Zeilenzaehler loeschen
****************************************************************************/
void iec_parser_typlist_clear()
{

	/* Datenmodul von Anfang an Parsen ... */
	PaTypList.pParser		= (unsigned char*) DOI_typlist.pDatObjMem;
	PaTypList.CountLines	= 0;
}
/**********************************************************
*		Fehlermeldung in Log-Buch eintragen
**********************************************************/
void iec_parser_err(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3)
{
	unsigned char ANr[20];
	unsigned long ErrorInfo=0;

	memset(MsgTxt,0,sizeof(MsgTxt));
	if(IEC_ParserInfo.ErrorMsg) {
		IEC_ParserInfo.error = errorNr;
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
		if(IEC_ParserInfo.NrOfLogEntrys>MAX_ERROR_MSG_LOG) {
			ErrStatistics("iec103", ERR, errorNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		else {
			ErrStatistics("iec103", ERRLOG, errorNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		IEC_ParserInfo.NrOfLogEntrys++;
	}
}

/**********************************************************
*		Warnungsmeldung in Log-Buch eintragen
**********************************************************/
void iec_parser_warn(unsigned short warnNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3)
{
	unsigned char ANr[20];
	unsigned long ErrorInfo=0;

	memset(MsgTxt,0,sizeof(MsgTxt));
	if(IEC_ParserInfo.ErrorMsg) {
		IEC_ParserInfo.warning = warnNr;
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
		if(IEC_ParserInfo.NrOfLogEntrys>MAX_ERROR_MSG_LOG) {
			ErrStatistics("iec103", ERR, warnNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		else {
			ErrStatistics("iec103", ERRLOG, warnNr, ErrorInfo, ERR_MSG_PRE_STRG, MsgTxt);
		}
		IEC_ParserInfo.NrOfLogEntrys++;
	}
}

/**********************************************************
*		Speicherdiagnose
**********************************************************/
void iec_mem_diagnose()
{
	IEC_DP_Info_typ *pDPInfo;
	unsigned short	n,i;
	plcbit			found;


	/*
		Anzeige von Normierungswerten
		Uebergabeparameter:	IEC_NormDiagnose.showData			Anzeige der Normierungswerte Start
							IEC_NormDiagnose.NormierungIdx		Index fuer Normierungswert Anzeige

		Rueckgabe:			IEC_NormDiagnose.DataValid			Angezeigte Daten sind gültig
							IEC_NormDiagnose.pNormData			Zeiger auf Normierungsdaten
							pNormDataDiag						Zeiger auf Normierungsdaten (Zeiger innerhalb der Struktur kann im Watch nicht dargestellt werden)
	*/

	if(IEC_NormDiagnose.showData) {
		IEC_NormDiagnose.showData	= 0;
		IEC_NormDiagnose.DataValid	= 0;
		if(!IEC_ParserInfo.allocStatus && IEC_ParserInfo.memAdr) {
			if(IEC_NormDiagnose.NormierungIdx<IEC_ParserInfo.NrOfNormValues) {
				IEC_NormDiagnose.pNormData		 = (IEC_Normierung_typ *) (IEC_ParserInfo.memAdr);
				IEC_NormDiagnose.pNormData		+= IEC_NormDiagnose.NormierungIdx;
				IEC_NormDiagnose.DataValid	 	 = 1;
				pNormDataDiagnose				 = IEC_NormDiagnose.pNormData;
			}
		}
	}

	/*
		Anzeige von Normierungswerten
		Uebergabeparameter:	IEC_ItemDiagnose.showData		Anzeige der Item-Daten Start
							IEC_ItemDiagnose.LineNr				Nummer der Linie, fuer die die Anzeige gewuenscht wird
							IEC_ItemDiagnose.DeviceAdr			Gerätenummer fuer die die Anzeige gewuenscht wird
							IEC_ItemDiagnose.ItemNr				Itemnummer fuer die die Anzeige gewuenscht wird

		Rueckgabe:			IEC_ItemDiagnose.DataValid		Angezeigte Daten sind gültig
							IEC_ItemDiagnose.pItemData			Zeiger auf Daten
	*/

	for(i=0;i<ITEM_DIAGNOSE_SIZE;i++) {
		if(IEC_ItemDiagnose[i].showData) {
			IEC_ItemDiagnose[i].showData	= 0;
			IEC_ItemDiagnose[i].DataValid	= 0;

			found = 0;
			for(n = 0; n < IEC_ParserInfo.NrOfLines; n++) {
				if(stANL[n].LinienNummer == IEC_ItemDiagnose[i].LineNr) {
					found = 1;
					IEC_ItemDiagnose[i].LineIdx = n;
				}
			}

			if(found) {
				found = 0;
				if(stANL[IEC_ItemDiagnose[i].LineIdx].dm.AdrDM_wert) {
					pDPInfo = (IEC_DP_Info_typ *) stANL[IEC_ItemDiagnose[i].LineIdx].dm.AdrDM_wert;
					for(n = 0; n < stANL[IEC_ItemDiagnose[i].LineIdx].dm.MaxWerteDM_wert; n++, pDPInfo++) {
						if(IEC_ItemDiagnose[i].DeviceAdr 	!= pDPInfo->DeviceAdr)	continue;
						if(IEC_ItemDiagnose[i].ItemNr 		!= pDPInfo->ItemNr)		continue;
						found = 1;
						IEC_ItemDiagnose[i].ItemIdx = n;
						break;
						}
					}
			}

			if(found) {
				IEC_ItemDiagnose[i].pItemData		 = (IEC_DP_Info_typ *) stANL[IEC_ItemDiagnose[i].LineIdx].dm.AdrDM_wert;
				IEC_ItemDiagnose[i].pItemData		+= IEC_ItemDiagnose[i].ItemIdx;
				IEC_ItemDiagnose[i].DataValid	 = 1;
				if(i==0) {
					pItemData0Diagnose = IEC_ItemDiagnose[i].pItemData;
				}
				else {
					pItemData1Diagnose = IEC_ItemDiagnose[i].pItemData;
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
			else if ((pData[i]>='0' && pData[i]<='9')||(pData[i]=='+')||(pData[i]=='-'))  {		/* Wai 2.06.4 */
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


