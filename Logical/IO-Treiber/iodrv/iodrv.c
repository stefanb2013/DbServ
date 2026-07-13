#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  I/O-Treiber
*  Teil:     Hauptprogramm
*  Datei:    iodrv.c
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
*  01.01 | 26-11-10 | WAI  | Schreibzugriff auf PVs aktiviert
*        |          |      |
******************************************************************************/

/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>			/* RPS-spezifische Typendeklarationen */
#include <string.h>
#include <dvframe.h>
#include <sys_lib.h>
#include <brsystem.h>
#include <fileio.h>
#include "iodrv.h"				/* globale Variablen und Konstanten */
#include "iodrv_parse.h"		/* Parser Funktionen */
#include "dbserv.h"				/* Meldeserver */


_LOCAL_RETAIN	BOOL	go	;
_LOCAL	UINT	LinkStat,SetStat,ScanStat,GetStat;

/***********************************************************************/
/* 					Initialisierungs-Unterprogramm                     */
/***********************************************************************/

void _INIT init_iodrv(void)
{

	int	item,i;
	IODRV_DP_Info_typ		*pIODRV_DP_Info;

	/*--------------------------------*/
	/* Variablen initialisieren       */
	/*--------------------------------*/
	for(i=0;i<ITEM_DIAGNOSE_SIZE;i++) {
		IODRV_ItemDiagnose[i].showData	= 0;
	}

	memset(&SYS_Statistic,0,sizeof(SYS_Statistic));

	myidBrServer			= 0;											/* ID BR-Server */

	rt_info.enable = 1; 													/* Taskklassenzykluszeit ermitteln */
	RTInfo(&rt_info);
	if(rt_info.status==0)	CycleTime	= (rt_info.cycle_time / 1000);		/* Zykluszeit in ms */
	else 					CycleTime	= 10;								/* Zykluszeit in ms */


	Error	= 0;															/* Generelle Variable fuer Initialisierungsfehler */

	initPAB = 1;

	/*****************************************************************************/
	/* Initialisierung nach Konfigurationsdatenmodul dplist und typlist (DbServ) */
	/*****************************************************************************/

	/*--------------------------------------------------*/
	/* Konfiguration aus Konfigurationsmodulen auslesen */
	/*--------------------------------------------------*/

	iodrv_parser();


	/* Kein Schwerwiegender Fehler beim Parsen */
	if(IODRV_ParserInfo.error==0) {
		/* für alle Items die PV suchen */
		for(item=0;item<IODRV_ParserInfo.NrOfItems;item++)	{
			pIODRV_DP_Info = (IODRV_DP_Info_typ *) (IODRV_ParserInfo.memAdr);
			pIODRV_DP_Info += item;

			if(pIODRV_DP_Info->DPNameLen && pIODRV_DP_Info->DPName !=NULL)	{
				/* PV-Namen aus DP- und Item-Namen zusammensetzen */
				strncpy(pIODRV_DP_Info->PVName,pIODRV_DP_Info->DPName,pIODRV_DP_Info->DPNameLen);
				if(pIODRV_DP_Info->ItemNameLen && pIODRV_DP_Info->ItemName !=NULL)	{
					strncat(pIODRV_DP_Info->PVName,pIODRV_DP_Info->ItemName,pIODRV_DP_Info->ItemNameLen);
				}
				/* PV-Adresse bestimmen */
				statePV_xgetadr = PV_xgetadr(pIODRV_DP_Info->PVName, &pIODRV_DP_Info->PVAdrs, &pIODRV_DP_Info->PVDataLen);
				if (statePV_xgetadr != 0) {
					/* Fehler PV */
						iodrv_parser_warn(PARS_ERR_PV_NOT_FOUND,"PV nicht vorhanden", pIODRV_DP_Info->PVName, 0,0, 0, 0, 0, 0);
				}
			}
		}
	}
}
/************************************************************************/
/* schreibt aktuellen Datenpunkt in das PAB 							*/
void WritePVtoPAB(IODRV_DP_Info_typ *pIODRV_DP_Info)
{
	int						do_it;
	PAR_TYPE				WertServer;
	UDINT					SizeServer;

	/* nur Lese- bzw. Schreib/Lese-Items an das PAB übergeben */
	if (myidBrServer == 0) {
		pIODRV_DP_Info->ErrorInfo = 2;				/* Info Server ID unbekannt */
	}
	else {
		pIODRV_DP_Info->AccessCounter += 1;			/* Zaehler inkrementieren: Signal wird an Server uebergeben */
		do_it = 1;

		switch(pIODRV_DP_Info->ItemTypNr) {
			case BOOL_IDX:
				pIODRV_DP_Info->ValueIO = *(BOOL *) pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(BOOL);
				if(pIODRV_DP_Info->Invers)	{
					WertServer.b	= (BOOL)pIODRV_DP_Info->ValueIO ^1;
				}
				else {
					WertServer.b	= (BOOL)pIODRV_DP_Info->ValueIO;
				}
				if(!initPAB && !WertServer.b && pIODRV_DP_Info->Quitt)	{
					do_it = 0;				/* gegangener, quittierpflichtiger DP nicht in PAB eintragen */
				}
				break;
			case USINT_IDX:
				WertServer.ui1	= *(USINT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(USINT);
				break;
			case SINT_IDX:
				WertServer.i1	= *(SINT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(SINT);
				break;
			case UINT_IDX:
				WertServer.ui2	= *(UINT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(UINT);
				break;
			case INT_IDX:
				WertServer.i2	= *(INT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(INT);
				break;
			case UDINT_IDX:
				WertServer.ui4	= *(UDINT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(UDINT);
				break;
			case DINT_IDX:
				WertServer.i4	= *(DINT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(DINT);
				break;
			default:
				WertServer.i2	= *(INT*)	pIODRV_DP_Info->PVAdrs;
				SizeServer  = sizeof(INT);
				break;
		}
		if(do_it)	{
			StatusSetValueItem = SetValueItem ((ACCESS_SERVER*)	myidBrServer,
												pIODRV_DP_Info->AnlagenNr,
												1,									/* Stationsnummer  immer 1 */
												pIODRV_DP_Info->DPTypNr,	  		/* Datenpunkttyp    */
												pIODRV_DP_Info->DPNr,			 	/* Datenpunktnummer */
												pIODRV_DP_Info->ItemNr,				/* Item */
												(USINT *) &WertServer,				/* Adresse Messwert */
												SizeServer,
												TimeBuf,
												0,			/* TimeStampNow=0 wegen Alarmliste, setze Zeitstempel bei Bedarf */
												1);
			if(!StatusSetValueItem)	{
				pIODRV_DP_Info->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pIODRV_DP_Info->ErrorInfo		= 0;
			}
			else {
				pIODRV_DP_Info->ErrorInfo = 3;
			}
		}
	}
}

/***********************************************************************/
/* 						zyklisches Hauptprogramm                       */
/***********************************************************************/

void _CYCLIC cyclic_iodrv(void)
{
	int						item;
	IODRV_DP_Info_typ		*pIODRV_DP_Info;
	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;

	/* Einlesen der Echtzeituhr */
	RTC_gettime(&TimeBuf);

	/* ID Meldeserver besorgen */
	get_idBrServer();


	if(go)	{
		go=0;
		iodrv_parser();

		/* Kein Schwerwiegender Fehler beim Parsen */
		if(IODRV_ParserInfo.error==0) {
			/* für alle Items die PV suchen */
			for(item=0;item<IODRV_ParserInfo.NrOfItems;item++)	{
				pIODRV_DP_Info = (IODRV_DP_Info_typ *) (IODRV_ParserInfo.memAdr);
				pIODRV_DP_Info += item;

				if(pIODRV_DP_Info->DPNameLen && pIODRV_DP_Info->DPName !=NULL)	{
					/* PV-Namen aus DP- und Item-Namen zusammensetzen */
					strncpy(pIODRV_DP_Info->PVName,pIODRV_DP_Info->DPName,pIODRV_DP_Info->DPNameLen);
					if(pIODRV_DP_Info->ItemNameLen && pIODRV_DP_Info->ItemName !=NULL)	{
						strncat(pIODRV_DP_Info->PVName,pIODRV_DP_Info->ItemName,pIODRV_DP_Info->ItemNameLen);
					}
					/* PV-Adresse bestimmen */
					statePV_xgetadr = PV_xgetadr(pIODRV_DP_Info->PVName, &pIODRV_DP_Info->PVAdrs, &pIODRV_DP_Info->PVDataLen);
					if (statePV_xgetadr != 0) {
						/* Fehler PV */
						iodrv_parser_warn(PARS_ERR_PV_NOT_FOUND,"PV nicht vorhanden", pIODRV_DP_Info->PVName, 0,0, 0, 0, 0, 0);
					}
				}
			}
		}
	}

	/* Diagnose */
	iodrv_mem_diagnose();

	/* -------------------------------*/
	/* Uebergabe an Server            */
	/* -------------------------------*/

	for(item=0;item<IODRV_ParserInfo.NrOfItems;item++)	{
		pIODRV_DP_Info = (IODRV_DP_Info_typ *) (IODRV_ParserInfo.memAdr);
		pIODRV_DP_Info += item;

		if(pIODRV_DP_Info->PVAdrs != NULL)	{
			if(pIODRV_DP_Info->Dir == cKW_READ)	{
				WritePVtoPAB(pIODRV_DP_Info);				/* PV ins PAB schreiben */
			}
			else if(pIODRV_DP_Info->Dir == cKW_READWRITE || pIODRV_DP_Info->Dir == cKW_WRITE)	{
				/* Item zum Schreiben anmelden */
				LinkStat = LinkWriteItem((ACCESS_SERVER*)myidBrServer, cINSTANCE_IODRV,
						pIODRV_DP_Info->AnlagenNr, 1, pIODRV_DP_Info->DPTypNr, pIODRV_DP_Info->DPNr, pIODRV_DP_Info->ItemNr,
						(UDINT)pIODRV_DP_Info);
				if (LinkStat != cEC_OK && LinkStat != cEC_ITEM_ALREADY_LINKED)	pIODRV_DP_Info->ErrorInfo = LinkStat;
				/* prüfen, ob Item geschrieben wurde */
				ScanStat = ScanWriteItem((ACCESS_SERVER*)myidBrServer, cINSTANCE_IODRV,
											WriteBuffer, sizeof(WriteBuffer),
											&len, &lenTimeStamp);
				if (ScanStat != cEC_OK)	pIODRV_DP_Info->ErrorInfo = ScanStat;
				if (ScanStat == cEC_OK) {
					/* für mich ? */
					GetStat = cEC_NO_DATA_IN_WRITEBUFFER;
					if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char *)pIODRV_DP_Info) {
						GetStat = GetWriteItem((ACCESS_SERVER*)myidBrServer, cINSTANCE_IODRV,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
						if (GetStat != cEC_OK)	pIODRV_DP_Info->ErrorInfo = GetStat;
						if (GetStat== cEC_OK) {
							if (((WRITE_PIPE_DATA*)WriteBuffer)->length == pIODRV_DP_Info->PVDataLen) {
								pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
/* #if 0 26.11.10 Wai Kommentar entfernt */
								switch(pIODRV_DP_Info->ItemTypNr) {
									case BOOL_IDX:
										*(BOOL *)pIODRV_DP_Info->PVAdrs =  *(BOOL*)pByteValue;
										break;
									case USINT_IDX:
										*(USINT *)pIODRV_DP_Info->PVAdrs =  *(USINT*)pByteValue;
										break;
									case SINT_IDX:
										*(SINT *)pIODRV_DP_Info->PVAdrs =  *(SINT*)pByteValue;
										break;
									case UINT_IDX:
										*(UINT *)pIODRV_DP_Info->PVAdrs =  *(UINT*)pByteValue;
										break;
									case INT_IDX:
										*(INT *)pIODRV_DP_Info->PVAdrs =  *(INT*)pByteValue;
										break;
									case UDINT_IDX:
										*(UDINT *)pIODRV_DP_Info->PVAdrs =  *(UDINT*)pByteValue;
										break;
									case DINT_IDX:
										*(DINT *)pIODRV_DP_Info->PVAdrs =  *(DINT*)pByteValue;
										break;
								}
/* #endif 26.11.10 Wai Kommentar entfernt */

								/* Wert auch nach PAB zurückschreiben */
								SetStat = SetValueItem((ACCESS_SERVER*)myidBrServer,
										pIODRV_DP_Info->AnlagenNr, 1, pIODRV_DP_Info->DPTypNr, pIODRV_DP_Info->DPNr, pIODRV_DP_Info->ItemNr,
										pByteValue, sizeof(INT), TimeBuf, 0, TRUE);
								if (SetStat != cEC_OK)	pIODRV_DP_Info->ErrorInfo = SetStat;
							} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == PVDataLen */
							else {
								SetStat = cEC_INVALID_LENGTH;
								if (SetStat != cEC_OK)	pIODRV_DP_Info->ErrorInfo = SetStat;
							}
						} /* (intStat == cEC_OK) */
					} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
				} /* (intStat == cEC_OK) */
				if (ScanStat != cEC_OK || GetStat != cEC_OK)	{
					/* keine Schreibdaten vorhanden: PV ins PAB schreiben */
					if(pIODRV_DP_Info->Dir == cKW_READWRITE)	{
						WritePVtoPAB(pIODRV_DP_Info);				/* PV ins PAB schreiben */
					}
				}
			} /* else if(pIODRV_DP_Info->Dir == cKW_READWRITE || pIODRV_DP_Info->Dir == cKW_WRITE)	*/
		} /* if(pIODRV_DP_Info->PVAdrs != NULL)	*/
	} /* for all Items */
	if(myidBrServer) initPAB = 0;

}



/*** eof *****************************************************************************/



