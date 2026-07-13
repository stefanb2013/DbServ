#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Datenuebergabe an das Leitsystem
*  Datei:    iec__103_serv.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 02.05.2
*  Datum:    02-09-2008
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion:
*-------------------------------------------------------------------------------
*  Rev.   | Datum    | Name | Kommentar
*  -------+----------+------+---------------------------------------------------
*  01.00  | 26-11-03 | KOB  | Datei-Erstellung
*  01.01  | 15-12-04 | KOB  | Bei ungültigen Analogwerten (Invalid oder Overflow) wird
*         |          |      | als Ersatzwert 7FFF gesendet.
*  01.02  | 11-04-06 | WAI  | FileIO-Library includiert wegen neuer dbServ-Version mit Archivierung
*  01.06  | 10-01-07 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Auswertung der im privaten Telegramm empfangenen Zählwerte
*         |          |      | (save_analogwert_priv)
*         |          |      | Funktion get_analogwert umbenannt in get_analogwert_cp16
*         |          |      | Funktion get_analogwert_i32_priv neu
*  02.03  | 25-06-07 | WAI  | Mode 13 (DM > EM invers) angepasst
*  02.05.1| 02-09-08 | KOB  | Neue Modi für Doppelmeldungen:
*		  |          |		| DOPPEL_EINZEL_NORMAL_NOINVALID und DOPPEL_EINZEL_INVERS_NOINVALID
*  02.05.2| 02-09-08 | KOB  | Funktionen wie unter 01.06 beschrieben eingefügt.
*         |          |      |
*  02.07.2| 26-07-11 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Type Identification 205: Privates Telegramm zur Übertragung von Zählwerten (Siemens)
*         |          |      | (check_m205, iec__103_sub.c)
*         |          |      | Auswertung der im privaten Telegramm empfangenen Zählwerte
*         |          |      | (save_counter_2, iec__103_serv.c)
*         | 05-08-11 | WAI  | (save_counter_2, norm_analogwert_d2 ohne Begrenzung für Zählwerte
******************************************************************************/
#include <bur/plctypes.h>
#include <sys_lib.h>		/* SYS_LIB */
#include <fileio.h>
#include "iec__103_date.h"
#include "iec60870_5_103.h"		/* Definition der globalen Variablen */
#include "iec__103_debug.h"
#include "iec__103_parse.h"
#include "dbserv.h"
#include "platform.h"
/****************************************************************************/
/*		BR-Server ID besorgen												*/
/****************************************************************************/
void get_idBrServer() {

	if (myidBrServer == 0) {
		statePV_xgetadr = PV_xgetadr ("dbserv:idBrServer", &pv_adresse, &data_len);
		if (statePV_xgetadr == 0) {
			myidBrServer = *(UDINT*)pv_adresse;
		}
	}
}

/****************************************************************************/
/*		Digitalen Messwert abspeichern		  								*/
/* Variante: DbServ															*/
/****************************************************************************/
#ifdef INIT_MODE_DM_DPLIST
USINT save_digitalwert( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT Messwert,
						RTCtime_typ *TimeBuf, USINT set_event)
{
	USINT			WertLS;
	short			n;
	IEC_DP_Info_typ *pDPInfo;
	USINT			found = 0;
	USINT			TypError;
	PAR_TYPE		WertServer;
	UDINT			SizeServer;

	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;				/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 		!= pDPInfo->DeviceAdr)	continue;
		if(FunctionType 				!= pDPInfo->IECType)	continue;
		if(InformationNumber			!= pDPInfo->IECInfoNr)	continue;
		if(pDPInfo->KopplungsStoerung)							continue;
		found = 1;

		#ifdef DEBUG
			dbg_msg(DIGITALMELDUNG,0,0, LinienNummer, pDeviceActiv->Adresse, pDPInfo->IECType, pDPInfo->IECInfoNr,(INT) Messwert, 0);
		#endif

		/* ----------------------------*/
		/* Wert konvertieren           */
		/* ----------------------------*/

		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:		/* Doppelmeldung unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case DOPPEL_INVERS:		/* Doppelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;			break;
					case 1:		WertLS = 2;			break;
					case 2:		WertLS = 1;			break;
					case 3:		WertLS = 3;			break;
					default:	WertLS = Messwert;	break;
				}
				break;
			case DOPPEL_EINZEL_NORMAL:		/* Doppelmeldung als Einzelmeldung weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 1;	break;		/* V2.03 */
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS:		/* Doppelmeldung als Einzelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;		/* V2.03 */
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 1;	break;
					default:	WertLS = 1;	break;
				}
				break;


/* Beginn 02.05.1| 02-09-08 | KOB */

			case DOPPEL_EINZEL_NORMAL_NOINVALID:	/* Doppelmeldung als Einzelmeldung weitergeben: 0>0, 1>0, 2>1, 3>0, default>0 */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;

			case DOPPEL_EINZEL_INVERS_NOINVALID:	/* Doppelmeldung als Einzelmeldung weitergeben : 0>0, 1>1, 2>0, 3>0, default>0  */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;

/* Ende   02.05.1| 02-09-08 | KOB */

			case EINZEL_NORMAL:				/* Wert unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case EINZEL_INVERS:				/* Einzelmeldung invertiert weitergeben */
				if(Messwert==1) WertLS = 0;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 2;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 1;
				else			WertLS = 2;
				break;
			default:
				WertLS = Messwert;
				break;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError = 0;
		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(pDPInfo->ItemTypNr==BOOL_IDX) TypError = 1;
			default:
				break;
		}
		if(pDPInfo->ItemTypNr==STREAM_IDX)	TypError = 1;

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250) {				/* Zaehler fuer die Anzahl der Zugriffe auf diesen Datenpunkt */
			pDPInfo->AccessCounter = 10;				/* Zaehler soll Null nicht wieder erreichen */
		}
		pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wurde parametriert */

		pDPInfo->ValueDevice	= (long) Messwert;		/* Empfangener Wert des Geraetes */
		pDPInfo->ValueServer	= (long) WertLS;		/* An den Server uebergebener Wert */


		/* -------------------------------*/
		/* Uebergabe an Server            */
		/* -------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;							/* Info ueber fehlerhaften Datentyp */
		}
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;							/* Info Server ID unbekannt */
		}
		else {
			pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			switch(pDPInfo->ItemTypNr) {
				case BOOL_IDX:		WertServer.b	= (BOOL)	WertLS;	SizeServer  = sizeof(BOOL);	break;
				case USINT_IDX:		WertServer.ui1	= (USINT)	WertLS;	SizeServer  = sizeof(USINT);break;
				case SINT_IDX:		WertServer.i1	= (SINT)	WertLS;	SizeServer  = sizeof(SINT);	break;
				case UINT_IDX:		WertServer.ui2	= (UINT)	WertLS;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	WertLS;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	WertLS;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ( (void *)	myidBrServer,
															pDeviceActiv->AnlagenNr,
															pDeviceActiv->AdresseLeitsystem,	/* Stationsnummer   */
															pDPInfo->DPTypNr,	  				/* Datenpunkttyp    */
															pDPInfo->DPNr,			 			/* Datenpunktnummer */
															pDPInfo->ItemNr,					/* Item */
															(USINT *) &WertServer,				/* Adresse Messwert */
															SizeServer,
															*TimeBuf,
															0,
															1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}


	/* ------------------------------------------------------------------- */
	/* Pruefen, ob mit diesem Signal Meldungen zuruckgesetzt werden sollen */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;	/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 	!=	pDPInfo->DeviceAdr)	continue;
		if(pDPInfo->MRuc			!=	1)					continue;
		if(pDPInfo->MRucType		!=	FunctionType)		continue;
		if(pDPInfo->MRucInfo		!=	InformationNumber)	continue;

		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung normal */
				WertLS = 1;
				break;
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert */
				WertLS = 2;
				break;
			case DOPPEL_EINZEL_NORMAL:		/* Doppelmeldung als Einzelmeldung weitergeben */
				WertLS = 0;
				break;
			case DOPPEL_EINZEL_INVERS:		/* Doppelmeldung als Einzelmeldung invertiert weitergeben */
				WertLS = 1;
				break;

/* Beginn 02.05.1| 02-09-08 | KOB */

			case DOPPEL_EINZEL_NORMAL_NOINVALID:	/* Doppelmeldung als Einzelmeldung weitergeben */
				WertLS = 0;
				break;
			case DOPPEL_EINZEL_INVERS_NOINVALID:	/* Doppelmeldung als Einzelmeldung invertiert weitergeben */
				WertLS = 1;
				break;

/* Ende   02.05.1| 02-09-08 | KOB */

			case EINZEL_NORMAL:				/* Einzelmeldung normal */
				WertLS = 0;
				break;
			case EINZEL_INVERS:				/* Einzelmeldung invertiert */
				WertLS = 1;
				break;
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
				WertLS = 1;
				break;
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				WertLS = 2;
				break;
			default:
				WertLS = 0;
				break;
		}

		#ifdef DEBUG
			dbg_msg(DIGITALMELDUNG,0,0, LinienNummer, pDeviceActiv->Adresse, pDPInfo->IECType, pDPInfo->IECInfoNr,(INT) WertLS, 0);
		#endif

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError = 0;
		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(pDPInfo->ItemTypNr==BOOL_IDX) TypError = 1;
			default:
				break;
		}
		if(pDPInfo->ItemTypNr==STREAM_IDX)	TypError = 1;

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250) {					/* Zaehler fuer die Anzahl der Zugriffe auf diesen Datenpunkt */
			pDPInfo->AccessCounter = 10;					/* Zaehler soll Null nicht wieder erreichen */
		}
		pDPInfo->AccessCounter += 1;						/* Zaehler inkrementieren: Signal wurde parametriert */
		pDPInfo->ValueServer	= (long) WertLS;


		/* -------------------------------*/
		/* Uebergabe an Server            */
		/* -------------------------------*/

		if(TypError) {
			pDPInfo->ErrorInfo = 1;							/* Info ueber fehlerhaften Datentyp */
		}
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;							/* Info Server ID unbekannt */
		}
		else {
			pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			switch(pDPInfo->ItemTypNr) {
				case BOOL_IDX:		WertServer.b	= (BOOL)	WertLS;	SizeServer  = sizeof(BOOL);	break;
				case USINT_IDX:		WertServer.ui1	= (USINT)	WertLS;	SizeServer  = sizeof(USINT);break;
				case SINT_IDX:		WertServer.i1	= (SINT)	WertLS;	SizeServer  = sizeof(SINT);	break;
				case UINT_IDX:		WertServer.ui2	= (UINT)	WertLS;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	WertLS;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	WertLS;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *)	myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem, 				/* Stationsnummer   */
														pDPInfo->DPTypNr,	  			/* Datenpunkttyp    */
														pDPInfo->DPNr,			 		/* Datenpunktnummer */
														pDPInfo->ItemNr,				/* Item */
														(USINT *) &WertServer,			/* Adresse Messwert */
														SizeServer,
														*TimeBuf,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}
	if(found)	return(0);
	else		return(-1);
}
/****************************************************************************/
/*		Kopplungsstoerung abspeichern		  								*/
/* Variante: DbServ															*/
/****************************************************************************/
USINT save_kopplungsstoerung( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv, USINT Messwert, RTCtime_typ *TimeBuf, USINT set_event)
{
	USINT			WertLS;
	short			n;
	IEC_DP_Info_typ *pDPInfo;
	USINT			found = 0;
	USINT			TypError;
	PAR_TYPE		WertServer;
	UDINT			SizeServer;

	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;				/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 		!= pDPInfo->DeviceAdr)	continue;
		if(!pDPInfo->KopplungsStoerung)							continue;
		found = 1;

		#ifdef DEBUG
			dbg_msg(KOPPLUNGSSTOERUNG,0,0, LinienNummer, pDeviceActiv->Adresse, pDPInfo->IECType, pDPInfo->IECInfoNr,(INT) Messwert, 0);
		#endif

		/* ----------------------------*/
		/* Wert konvertieren           */
		/* ----------------------------*/

		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;			break;
					case 1:		WertLS = 2;			break;
					case 2:		WertLS = 1;			break;
					case 3:		WertLS = 3;			break;
					default:	WertLS = Messwert;	break;
				}
				break;
			case DOPPEL_EINZEL_NORMAL:		/* Doppelmeldung als Einzelmeldung weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 1;	break;		/* V2.03 */
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS:		/* Doppelmeldung als Einzelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 1;	break;		/* V2.03 */
					default:	WertLS = 1;	break;
				}
				break;


/* Beginn 02.05.1| 02-09-08 | KOB */

			case DOPPEL_EINZEL_NORMAL_NOINVALID:		/* Doppelmeldung als Einzelmeldung weitergeben: 0>0, 1>0, 2>1, 3>0, default>0  */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS_NOINVALID:		/* Doppelmeldung als Einzelmeldung weitergeben : 0>0, 1>1, 2>0, 3>0, default>0   */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;

/* Ende 02.05.1| 02-09-08 | KOB */


			case EINZEL_NORMAL:				/* Wert unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case EINZEL_INVERS:				/* Einzelmeldung invertiert weitergeben */
				if(Messwert==1) WertLS = 0;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 2;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 1;
				else			WertLS = 2;
				break;
			default:
				WertLS = Messwert;
				break;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError = 0;
		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(pDPInfo->ItemTypNr==BOOL_IDX) TypError = 1;
			default:
				break;
		}
		if(pDPInfo->ItemTypNr==STREAM_IDX)	TypError = 1;

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250) {				/* Zaehler fuer die Anzahl der Zugriffe auf diesen Datenpunkt */
			pDPInfo->AccessCounter = 10;				/* Zaehler soll Null nicht wieder erreichen */
		}
		pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wurde parametriert */

		pDPInfo->ValueDevice	= (long) Messwert;		/* Empfangener Wert des Geraetes */
		pDPInfo->ValueServer	= (long) WertLS;		/* An den Server uebergebener Wert */


		/* -------------------------------*/
		/* Uebergabe an Server            */
		/* -------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;							/* Info ueber fehlerhaften Datentyp */
		}
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;							/* Info Server ID unbekannt */
		}
		else {
			pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			switch(pDPInfo->ItemTypNr) {
				case BOOL_IDX:		WertServer.b	= (BOOL)	WertLS;	SizeServer  = sizeof(BOOL);	break;
				case USINT_IDX:		WertServer.ui1	= (USINT)	WertLS;	SizeServer  = sizeof(USINT);break;
				case SINT_IDX:		WertServer.i1	= (SINT)	WertLS;	SizeServer  = sizeof(SINT);	break;
				case UINT_IDX:		WertServer.ui2	= (UINT)	WertLS;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	WertLS;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	WertLS;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *) myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem,	/* Stationsnummer   */
														pDPInfo->DPTypNr,	  				/* Datenpunkttyp    */
														pDPInfo->DPNr,			 			/* Datenpunktnummer */
														pDPInfo->ItemNr,					/* Item */
														(USINT *) &WertServer,				/* Adresse Messwert */
														SizeServer,
														*TimeBuf,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}
	if(found)	return(0);
	else		return(-1);
}

/****************************************************************************/
/*		Fehler bei Zeitsynchronisation abspeichern		  					*/
/* Variante: DbServ															*/
/****************************************************************************/
USINT save_TimeSynchError( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv, USINT Messwert, RTCtime_typ *TimeBuf, USINT set_event)
{
	USINT			WertLS;
	short			n;
	IEC_DP_Info_typ *pDPInfo;
	USINT			found = 0;
	USINT			TypError;
	PAR_TYPE		WertServer;
	UDINT			SizeServer;

	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;				/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 		!= pDPInfo->DeviceAdr)	continue;
		if(!pDPInfo->TimeSynchError)							continue;
		found = 1;

		#ifdef DEBUG
			dbg_msg(TIMESYNCHERROR,0,0, LinienNummer, pDeviceActiv->Adresse, pDPInfo->IECType, pDPInfo->IECInfoNr,(INT) Messwert, 0);
		#endif

		/* ----------------------------*/
		/* Wert konvertieren           */
		/* ----------------------------*/

		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;			break;
					case 1:		WertLS = 2;			break;
					case 2:		WertLS = 1;			break;
					case 3:		WertLS = 3;			break;
					default:	WertLS = Messwert;	break;
				}
				break;
			case DOPPEL_EINZEL_NORMAL:		/* Doppelmeldung als Einzelmeldung weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 1;	break;		/* V2.03 */
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS:		/* Doppelmeldung als Einzelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 1;	break;		/* V2.03 */
					default:	WertLS = 1;	break;
				}
				break;

			case DOPPEL_EINZEL_NORMAL_NOINVALID:		/* Doppelmeldung als Einzelmeldung weitergeben: 0>0, 1>0, 2>1, 3>0, default>0  */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS_NOINVALID:		/* Doppelmeldung als Einzelmeldung weitergeben : 0>0, 1>1, 2>0, 3>0, default>0   */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;


			case EINZEL_NORMAL:				/* Wert unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case EINZEL_INVERS:				/* Einzelmeldung invertiert weitergeben */
				if(Messwert==1) WertLS = 0;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 2;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 1;
				else			WertLS = 2;
				break;
			default:
				WertLS = Messwert;
				break;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError = 0;
		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(pDPInfo->ItemTypNr==BOOL_IDX) TypError = 1;
			default:
				break;
		}
		if(pDPInfo->ItemTypNr==STREAM_IDX)	TypError = 1;

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250) {				/* Zaehler fuer die Anzahl der Zugriffe auf diesen Datenpunkt */
			pDPInfo->AccessCounter = 10;				/* Zaehler soll Null nicht wieder erreichen */
		}
		pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wurde parametriert */

		pDPInfo->ValueDevice	= (long) Messwert;		/* Empfangener Wert des Geraetes */
		pDPInfo->ValueServer	= (long) WertLS;		/* An den Server uebergebener Wert */


		/* -------------------------------*/
		/* Uebergabe an Server            */
		/* -------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;							/* Info ueber fehlerhaften Datentyp */
		}
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;							/* Info Server ID unbekannt */
		}
		else {
			pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			switch(pDPInfo->ItemTypNr) {
				case BOOL_IDX:		WertServer.b	= (BOOL)	WertLS;	SizeServer  = sizeof(BOOL);	break;
				case USINT_IDX:		WertServer.ui1	= (USINT)	WertLS;	SizeServer  = sizeof(USINT);break;
				case SINT_IDX:		WertServer.i1	= (SINT)	WertLS;	SizeServer  = sizeof(SINT);	break;
				case UINT_IDX:		WertServer.ui2	= (UINT)	WertLS;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	WertLS;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	WertLS;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *) myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem,	/* Stationsnummer   */
														pDPInfo->DPTypNr,	  				/* Datenpunkttyp    */
														pDPInfo->DPNr,			 			/* Datenpunktnummer */
														pDPInfo->ItemNr,					/* Item */
														(USINT *) &WertServer,				/* Adresse Messwert */
														SizeServer,
														*TimeBuf,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}
	if(found)	return(0);
	else		return(-1);
}
#endif

/****************************************************************************/
/*		Digitalen Messwert abspeichern		  								*/
/* Variante: Ursprung														*/
/****************************************************************************/
#ifdef	INIT_MODE_DM_WERT
USINT save_digitalwert( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT Messwert,
						RTCtime_typ *TimeBuf, USINT set_event)
{
	USINT found;
	USINT WertLS0,WertLS1;
	WertType* PtrWerte;
	MrukType* PtrMruk;
	short n,m;

	PtrWerte = (WertType*) pDMInfo->AdrDM_wert;	/* Strukturadresse für Werteintrag belegen */

	found = 0;
	for(n = 0; n < pDMInfo->MaxWerteDM_wert; n++, PtrWerte++) {
		if(pDeviceActiv->Adresse	!= PtrWerte->Device)	continue;
		if(FunctionType				!= PtrWerte->Type)		continue;
		if(InformationNumber		!= PtrWerte->Info)		continue;
		found = 1;
		break;
	}

	if(!found) return -1;

		#ifdef DEBUG
			dbg_msg(DIGITALMELDUNG,0,0, LinienNummer, pDeviceActiv->Adresse, FunctionType, InformationNumber,(INT) Messwert, 0);
		#endif

/*	Filter für Meldung 160/19 (LED-Rücksetzen)
	alle Meldungen aus Tabelle MRUK werden damit zurückgesetzt	*/

	if(PtrWerte->Type == 160 && PtrWerte->Info == 19) {
		PtrMruk = (MrukType*) pDMInfo->AdrDM_mruk;
		for(n = 0; n < pDMInfo->MaxWerteDM_mruk; n++, PtrMruk++) {
			PtrWerte = (WertType*) pDMInfo->AdrDM_wert;
			found = 0;
			for(m = 0; m < pDMInfo->MaxWerteDM_wert; m++, PtrWerte++) {
				if(pDeviceActiv->Adresse	!= PtrWerte->Device)	continue;
				if(PtrMruk->Type			!= PtrWerte->Type)		continue;
				if(PtrMruk->Info			!= PtrWerte->Info)		continue;
				found = 1;
				break;
			}
			if(found) {

				#ifdef DEBUG
					dbg_msg(DIGITALMELDUNG,0,0, LinienNummer, pDeviceActiv->Adresse, PtrWerte->Type, PtrWerte->Info, 0, 0);
				#endif

				/* Übergabe ans Leitsystem */

				if(PtrWerte->WertCode==0)	WertLS0 = 0;			/* Wert invertieren ? 0=nein 1=ja */
				else						WertLS0 = 1;

				if (myidBrServer != 0) {
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS0,				/* Adresse Messwert */
																						sizeof(WertLS0),
																						*TimeBuf,
																						0,
																						1);
				}
			}
		}
	}

	/*  normale Meldungsverarbeitung */

	else {

		if(PtrWerte->WertCode==0) {
			WertLS0 = 0;			/* Wert invertieren ? 0=nein 1=ja */
			WertLS1 = 1;
		}
		else {
			WertLS0 = 1;			/* Wert invertieren ? 0=nein 1=ja */
			WertLS1 = 0;
		}

		/* Übergabe ans Leitsystem */
		if (myidBrServer != 0) {
			if(PtrWerte->Info1==1) {			/* Abbildung auf 2 Datenpunkte */
				switch(Messwert) {
					case 1:		/* Zustand aus */
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS1,				/* Adresse Messwert */
																						sizeof(WertLS1),
																						*TimeBuf,
																						0,
																						1);



					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->Info2, 				/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS0,				/* Adresse Messwert */
																						sizeof(WertLS0),
																						*TimeBuf,
																						0,
																						1);
						break;
					case 2:		/* Zustand ein */
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS0,				/* Adresse Messwert */
																						sizeof(WertLS0),
																						*TimeBuf,
																						0,
																						1);
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->Info2, 				/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS1,				/* Adresse Messwert */
																						sizeof(WertLS1),
																						*TimeBuf,
																						0,
																						1);
						break;
					default:	/* undefiniert */
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS0,				/* Adresse Messwert */
																						sizeof(WertLS0),
																						*TimeBuf,
																						0,
																						1);
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->Info2, 				/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS0,				/* Adresse Messwert */
																						sizeof(WertLS0),
																						*TimeBuf,
																						0,
																						1);
						break;
				}
			}
			else {									/* Abbildung auf 1 Datenpunkt */
				if(Messwert==2) {	/* Zustand ein */
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS1,				/* Adresse Messwert */
																						sizeof(WertLS1),
																						*TimeBuf,
																						0,
																						1);
				}
				else {
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &WertLS0,				/* Adresse Messwert */
																						sizeof(WertLS0),
																						*TimeBuf,
																						0,
																						1);

				}
			}  /* if(PtrWerte->Info1==1) */
		} /*	if (myidBrServer != 0)  */
	}
	return 0;
}

/****************************************************************************/
/*		Kopplungsstoerung abspeichern		  								*/
/* Variante: Ursprung														*/
/****************************************************************************/
USINT save_kopplungsstoerung( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv, USINT Messwert, RTCtime_typ *TimeBuf, USINT set_event)
{
	return(save_digitalwert( pDMInfo,LinienNummer,pDeviceActiv,TYPE_CONNECTION_ERROR,INFO_CONNECTION_ERROR, Messwert, TimeBuf, set_event));
}

#endif

/****************************************************************************/
/*		Analogen Messwert aus Datentelegramm decodieren: Format CP16		*/
/****************************************************************************/

INT get_analogwert_cp16(USINT *MesswertBuffer, USINT *invalid, USINT *overflow)
{
	INT ret;
	USINT sign;
	USINT BW[2];

	/*	Messwert nach 7.2.6.8
		1. Intel nach Motorola wandeln
		2. Format Sxxx xxxx xxxx xRIO
		   S = Signed
		   x = Binaerwert
		   R = Reserve
		   I = Invalid
		   O = Overflow

		Empfangener Wert entspricht einem Faktor.
		z.B.	Nennwert = 400 A
				Variable von Geraet = 182
				Strom = ((400 * 2.4) / 4096) * 182 = 43,6 A
	*/



	/* m68k */
	/* 1. Bytes tauschen */


#ifdef MOTOROLA
	BW[1] = *MesswertBuffer;
	BW[0] = *(MesswertBuffer+1);
#endif

	/* i386 */
	/* Bytes nicht tauschen */

#ifdef	INTEL
	BW[0] = *MesswertBuffer;
	BW[1] = *(MesswertBuffer+1);
#endif

	ret  = *( (INT *) &BW);

	/* 2. Informationsbits decodieren */

	if(ret&0x8000)	sign = 1;
	else			sign = 0;

	if(ret&0x01)	*overflow = 1;
	else			*overflow = 0;

	if(ret&0x02)	*invalid = 1;
	else			*invalid = 0;


	/* Wert zurechtruecken */

	ret >>= 3;

	/* Vorzeichenrichtig ergänzen */

	if(sign) ret |= 0xE000;
	return(ret);
}
/************************************************************************************************/
/*		Analogen Messwert aus Datentelegramm decodieren: Format I32, privates Telegramm			*/
/************************************************************************************************/

DINT get_analogwert_i32_priv(USINT *MesswertBuffer, USINT *invalid, USINT *overflow)
{
	DINT ret;
	USINT sign;
	USINT BW[4];
	USINT info;
	/*
		1. Intel nach Motorola wandeln
		2. Format Sxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
		   S = Signed
	*/



	/* m68k */
	/* 1. Bytes tauschen */


#ifdef MOTOROLA
	BW[3] = *MesswertBuffer;
	BW[2] = *(MesswertBuffer+1);
	BW[1] = *(MesswertBuffer+2);
	BW[0] = *(MesswertBuffer+3);
#endif

	/* i386 */
	/* Bytes nicht tauschen */

#ifdef	INTEL
	BW[0] = *MesswertBuffer;
	BW[1] = *(MesswertBuffer+1);
	BW[2] = *(MesswertBuffer+2);
	BW[3] = *(MesswertBuffer+3);
#endif

	ret  = *( (DINT *) &BW);

	if(ret&0x80000000)	sign = 1;
	else				sign = 0;

	ret  = *( (DINT *) &BW) & 0x7FFFFFFF;

	if(sign) ret = -ret;

	/* 2. Informationsbits decodieren */

	info = *(MesswertBuffer+4);

	if(info&0x80)		*invalid	= 1;
	else				*invalid	= 0;

	*overflow	= 0;

	return(ret);
}
/************************************************************************************************/
/*		Analogen Messwert aus Datentelegramm decodieren: Format I28, privates Telegramm			*/
/************************************************************************************************/

DINT get_analogwert_i28_priv(USINT *MesswertBuffer, USINT *invalid, USINT *overflow)
{
	DINT ret;
	USINT sign;
	USINT BW[4];
	/*
		1. Intel nach Motorola wandeln
		2. Format IEISxxxx xxxxxxxx xxxxxxxx xxxxxxxx
			I = irrelevant
			E = internal error
			I = irrelevant
			S = Signed
	*/

	/* m68k */
	/* 1. Bytes tauschen */


#ifdef MOTOROLA
	BW[3] = *MesswertBuffer;
	BW[2] = *(MesswertBuffer+1);
	BW[1] = *(MesswertBuffer+2);
	BW[0] = *(MesswertBuffer+3);
#endif

	/* i386 */
	/* Bytes nicht tauschen */

#ifdef	INTEL
	BW[0] = *MesswertBuffer;
	BW[1] = *(MesswertBuffer+1);
	BW[2] = *(MesswertBuffer+2);
	BW[3] = *(MesswertBuffer+3);
#endif

	ret  = *( (DINT *) &BW);

	if(ret&0x10000000)	sign = 1;
	else				sign = 0;

	if(ret&0x40000000)	*invalid = 1;
	else				*invalid = 0;

	ret  = *( (DINT *) &BW) & 0xFFFFFFF;

	if(sign) ret = -ret;


	*overflow	= 0;

	return(ret);
}
/****************************************************************************/
/*		Analogen Messwert normieren  (Integer) 								*/
/****************************************************************************/
INT norm_analogwert(INT x, INT x1, INT y1, INT x2, INT y2)
{
	float m,t;

	if(x<x1) return(y1);
	if(x>x2) return(y2);
	if(x1==x2) return(y2);
	if(y1==y2) return(y1);


	m = (float) (y2-y1) / (float) (x2-x1);
	t = (float) y2 - m * (float) x2;

	return( (INT) ((float) x * m + t ) );
}
/****************************************************************************/
/*		Analogen Messwert normieren  (Double Integer)						*/
/****************************************************************************/
DINT norm_analogwert_d(DINT x, DINT x1, DINT y1, DINT x2, DINT y2)
{
	float m,t;

	if(x<x1) return(y1);
	if(x>x2) return(y2);
	if(x1==x2) return(y2);
	if(y1==y2) return(y1);

	m = ((float) y2 - (float)y1) / ((float) x2 - (float) x1);
	t = (float) y2 - m * (float) x2;

	return( (DINT) ((float) x * m + t ) );
}
DINT norm_analogwert_d2(DINT x, DINT x1, DINT y1, DINT x2, DINT y2)
{
	float m,t;

/* 05.08.11 WAI Sonderfall ohne Begrenzung, für Zählwerte */
/*	if(x<x1) return(y1);
	if(x>x2) return(y2);*/
	if(x1==x2) return(y2);
	if(y1==y2) return(y1);

	m = ((float) y2 - (float)y1) / ((float) x2 - (float) x1);
	t = (float) y2 - m * (float) x2;

	return( (DINT) ((float) x * m + t ) );
}

/****************************************************************************/
/*		Analogen Messwert abspeichern		  								*/
/* Variante: DbServ															*/
/****************************************************************************/
#ifdef INIT_MODE_DM_DPLIST
USINT save_analogwert(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT AnzahlMesswerte, USINT *AdresseMesswerte,
						USINT set_event)
{

	short 				n;
	short				AnalogIdx;
	IEC_DP_Info_typ 	*pDPInfo;
	IEC_Normierung_typ	*PtrNorm;
	USINT				found=0;
	USINT				TypError,TypWarn;
	PAR_TYPE			WertServer;
	UDINT				SizeServer;


	memset(&Overflow,0,sizeof(Overflow));
	memset(&Invalid,0,sizeof(Invalid));

	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;	/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 	!= pDPInfo->DeviceAdr)	continue;
		if(FunctionType 			!= pDPInfo->IECType)	continue;
		if(InformationNumber		!= pDPInfo->IECInfoNr)	continue;
		if((pDPInfo->Invalid) 		|| (pDPInfo->Overflow))	continue;
		/* Gewünschte Analognummer groesser als die Anzahl empfangener Werte ? */
		if(pDPInfo->IECAnalogNr>AnzahlMesswerte)			continue;
		if((pDPInfo->IECAnalogNr<1) || (pDPInfo->IECAnalogNr>MAX_ANALOGWERTE))  continue;

		found					= 1;
		AnalogIdx				= pDPInfo->IECAnalogNr - 1;

		pDPInfo->ValueDevice	= (DINT) get_analogwert_cp16(AdresseMesswerte + AnalogIdx * 2, &Invalid[AnalogIdx], &Overflow[AnalogIdx]);

		#ifdef DEBUG
			dbg_msg(ANALOGMELDUNG,(UDINT) pDPInfo->IECAnalogNr,0, LinienNummer, pDeviceActiv->Adresse, FunctionType, InformationNumber, (INT) pDPInfo->ValueDevice, 0);
		#endif

		/* ----------------------------*/
		/* Wert evtl. normieren        */
		/* ----------------------------*/

		if(pDPInfo->Normierung==1) {
			if((pDMInfo->MaxWerteDM_norm>0) && (pDMInfo->MaxWerteDM_norm>pDPInfo->NormierungIdx)) {
				PtrNorm = (IEC_Normierung_typ *) pDMInfo->AdrDM_norm;
				PtrNorm += pDPInfo->NormierungIdx;
				pDPInfo->ValueServer = norm_analogwert_d(pDPInfo->ValueDevice,PtrNorm->x1,PtrNorm->y1,PtrNorm->x2,PtrNorm->y2);
			}
			else {
				pDPInfo->ValueServer = pDPInfo->ValueDevice;
			}
		}
		else {
			pDPInfo->ValueServer = pDPInfo->ValueDevice;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError	= 0;
		TypWarn		= 0;
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		switch(pDPInfo->ItemTypNr) {
			case BOOL_IDX:
			case USINT_IDX:
			case SINT_IDX:
			case STREAM_IDX:
				TypError = 1;
			break;
		}

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250)	pDPInfo->AccessCounter = 10;

		pDPInfo->AccessCounter += 1;										/* Zahler fuer Diagnosezwecke */


		/* --------------------------------*/
		/* Analogwert an Server uebergeben */
		/* --------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;								/* Info ueber fehlerhaften Datentyp */
		}
/*
		else if(Invalid[AnalogIdx]) {
			pDPInfo->ErrorInfo = 5;
		}
		else if(Overflow[AnalogIdx]) {
			pDPInfo->ErrorInfo = 6;
		}
*/
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;
		}
		else {
			pDPInfo->AccessCounter += 1;						/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			if(Invalid[AnalogIdx] || Overflow[AnalogIdx]) {
				pDPInfo->ValueServer = 0x7FFF;
			}


			switch(pDPInfo->ItemTypNr) {
				case UINT_IDX:		WertServer.ui2	= (UINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		pDPInfo->ValueServer;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		pDPInfo->ValueServer;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *) myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
														pDPInfo->DPTypNr,	  			/* Datenpunkttyp    */
														pDPInfo->DPNr,			 		/* Datenpunktnummer */
														pDPInfo->ItemNr,				/* Item */
														(USINT *) &WertServer,			/* Adresse Messwert */
														SizeServer,
														RPSTime,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}

	/* --------------------------------------------------*/
	/* Signal Invalid bzw. Overflow an Server uebergeben */
	/* --------------------------------------------------*/

	save_analogwert_error(pDMInfo, LinienNummer, pDeviceActiv, FunctionType, InformationNumber, set_event);

	if(found)	return(0);
	else		return(-1);
}

/****************************************************************************/
/*		Analogen Messwert abspeichern		  								*/
/*	 	Analoger Messwert aus privatem Telegramm							*/
/****************************************************************************/
USINT save_analogwert_priv(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
							USINT FunctionType,USINT InformationNumber, USINT AnzahlMesswerte, USINT *AdresseMesswerte,
							USINT set_event)
{

	short 				n;
	short				AnalogIdx,AdrOffset;
	IEC_DP_Info_typ 	*pDPInfo;
	IEC_Normierung_typ	*PtrNorm;
	USINT				found=0;
	USINT				TypError,TypWarn;
	PAR_TYPE			WertServer;
	UDINT				SizeServer;


	memset(&Overflow,0,sizeof(Overflow));
	memset(&Invalid,0,sizeof(Invalid));

	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;	/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 	!= pDPInfo->DeviceAdr)	continue;
		if(FunctionType 			!= pDPInfo->IECType)	continue;
		if(InformationNumber		!= pDPInfo->IECInfoNr)	continue;
		if((pDPInfo->Invalid) 		|| (pDPInfo->Overflow))	continue;
		/* Gewünschte Analognummer groesser als die Anzahl empfangener Werte ? */
		if(pDPInfo->IECAnalogNr>AnzahlMesswerte)			continue;
		if((pDPInfo->IECAnalogNr<1) || (pDPInfo->IECAnalogNr>MAX_ANALOGWERTE))  continue;

		found			= 1;
		AnalogIdx		= pDPInfo->IECAnalogNr - 1;
		AdrOffset		= AnalogIdx * 5;

		pDPInfo->ValueDevice	= (DINT) get_analogwert_i32_priv(AdresseMesswerte + AdrOffset, &Invalid[AnalogIdx], &Overflow[AnalogIdx]);

		#ifdef DEBUG
			dbg_msg(ANALOGMELDUNG,(UDINT) pDPInfo->IECAnalogNr,0, LinienNummer, pDeviceActiv->Adresse, FunctionType, InformationNumber, (INT) pDPInfo->ValueDevice, 0);
		#endif

		/* ----------------------------*/
		/* Wert evtl. normieren        */
		/* ----------------------------*/

		if(pDPInfo->Normierung==1) {
			if((pDMInfo->MaxWerteDM_norm>0) && (pDMInfo->MaxWerteDM_norm>pDPInfo->NormierungIdx)) {
				PtrNorm = (IEC_Normierung_typ *) pDMInfo->AdrDM_norm;
				PtrNorm += pDPInfo->NormierungIdx;
				pDPInfo->ValueServer = norm_analogwert_d(pDPInfo->ValueDevice,PtrNorm->x1,PtrNorm->y1,PtrNorm->x2,PtrNorm->y2);
			}
			else {
				pDPInfo->ValueServer = pDPInfo->ValueDevice;
			}
		}
		else {
			pDPInfo->ValueServer = pDPInfo->ValueDevice;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError	= 0;
		TypWarn		= 0;
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		switch(pDPInfo->ItemTypNr) {
			case BOOL_IDX:
			case USINT_IDX:
			case SINT_IDX:
			case STREAM_IDX:
				TypError = 1;
			break;
		}

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250)	pDPInfo->AccessCounter = 10;

		pDPInfo->AccessCounter += 1;										/* Zahler fuer Diagnosezwecke */


		/* --------------------------------*/
		/* Analogwert an Server uebergeben */
		/* --------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;								/* Info ueber fehlerhaften Datentyp */
		}
/*
		else if(Invalid[AnalogIdx]) {
			pDPInfo->ErrorInfo = 5;
		}
		else if(Overflow[AnalogIdx]) {
			pDPInfo->ErrorInfo = 6;
		}
*/
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;
		}
		else {
			pDPInfo->AccessCounter += 1;						/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			if(Invalid[AnalogIdx] || Overflow[AnalogIdx]) {
				pDPInfo->ValueServer = 0x7FFF;
			}


			switch(pDPInfo->ItemTypNr) {
				case UINT_IDX:		WertServer.ui2	= (UINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		pDPInfo->ValueServer;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		pDPInfo->ValueServer;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *) myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
														pDPInfo->DPTypNr,	  			/* Datenpunkttyp    */
														pDPInfo->DPNr,			 		/* Datenpunktnummer */
														pDPInfo->ItemNr,				/* Item */
														(USINT *) &WertServer,			/* Adresse Messwert */
														SizeServer,
														RPSTime,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}

	/* --------------------------------------------------*/
	/* Signal Invalid bzw. Overflow an Server uebergeben */
	/* --------------------------------------------------*/

	save_analogwert_error(pDMInfo, LinienNummer, pDeviceActiv, FunctionType, InformationNumber, set_event);

	if(found)	return(0);
	else		return(-1);
}

/********************************************************************/
/*		Zählerwerte abspeichern		  								*/
/*	 	Zählerwerte aus privatem Telegramm							*/
/********************************************************************/
USINT save_counter_2(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
							USINT FunctionType,USINT InformationNumber, USINT AnzahlMesswerte, USINT *AdresseMesswerte,
							USINT set_event)
{

	short 				n;
	short				AnalogIdx,AdrOffset;
	IEC_DP_Info_typ 	*pDPInfo;
	IEC_Normierung_typ	*PtrNorm;
	USINT				found=0;
	USINT				TypError,TypWarn;
	PAR_TYPE			WertServer;
	UDINT				SizeServer;


	memset(&Overflow,0,sizeof(Overflow));
	memset(&Invalid,0,sizeof(Invalid));

	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;	/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 	!= pDPInfo->DeviceAdr)	continue;
		if(FunctionType 			!= pDPInfo->IECType)	continue;
		if(InformationNumber		!= pDPInfo->IECInfoNr)	continue;
		if((pDPInfo->Invalid) 		|| (pDPInfo->Overflow))	continue;

		found			= 1;

		/* Es existiert lediglich ein Messwert im Telegramm, deshalb wird die Analognummer nicht ausgewertet */
		AnalogIdx		= 0;
		AdrOffset		= AnalogIdx * 5;

		pDPInfo->ValueDevice	= (DINT) get_analogwert_i28_priv(AdresseMesswerte + AdrOffset, &Invalid[AnalogIdx], &Overflow[AnalogIdx]);

		#ifdef DEBUG
			dbg_msg(ANALOGMELDUNG,(UDINT) pDPInfo->IECAnalogNr,0, LinienNummer, pDeviceActiv->Adresse, FunctionType, InformationNumber, (INT) pDPInfo->ValueDevice, 0);
		#endif

		/* ----------------------------*/
		/* Wert evtl. normieren        */
		/* ----------------------------*/

		if(pDPInfo->Normierung==1) {
			if((pDMInfo->MaxWerteDM_norm>0) && (pDMInfo->MaxWerteDM_norm>pDPInfo->NormierungIdx)) {
				PtrNorm = (IEC_Normierung_typ *) pDMInfo->AdrDM_norm;
				PtrNorm += pDPInfo->NormierungIdx;
				pDPInfo->ValueServer = norm_analogwert_d2(pDPInfo->ValueDevice,PtrNorm->x1,PtrNorm->y1,PtrNorm->x2,PtrNorm->y2);
			}
			else {
				pDPInfo->ValueServer = pDPInfo->ValueDevice;
			}
		}
		else {
			pDPInfo->ValueServer = pDPInfo->ValueDevice;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError	= 0;
		TypWarn		= 0;
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		switch(pDPInfo->ItemTypNr) {
			case BOOL_IDX:
			case USINT_IDX:
			case SINT_IDX:
			case STREAM_IDX:
				TypError = 1;
			break;
		}

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250)	pDPInfo->AccessCounter = 10;

		pDPInfo->AccessCounter += 1;										/* Zahler fuer Diagnosezwecke */


		/* --------------------------------*/
		/* Analogwert an Server uebergeben */
		/* --------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;								/* Info ueber fehlerhaften Datentyp */
		}
/*
		else if(Invalid[AnalogIdx]) {
			pDPInfo->ErrorInfo = 5;
		}
		else if(Overflow[AnalogIdx]) {
			pDPInfo->ErrorInfo = 6;
		}
*/
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;
		}
		else {
			pDPInfo->AccessCounter += 1;						/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			if(Invalid[AnalogIdx] || Overflow[AnalogIdx]) {
				pDPInfo->ValueServer = 0x7FFF;
			}


			switch(pDPInfo->ItemTypNr) {
				case UINT_IDX:		WertServer.ui2	= (UINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		pDPInfo->ValueServer;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	pDPInfo->ValueServer;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		pDPInfo->ValueServer;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *) myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
														pDPInfo->DPTypNr,	  			/* Datenpunkttyp    */
														pDPInfo->DPNr,			 		/* Datenpunktnummer */
														pDPInfo->ItemNr,				/* Item */
														(USINT *) &WertServer,			/* Adresse Messwert */
														SizeServer,
														RPSTime,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}

	/* --------------------------------------------------*/
	/* Signal Invalid bzw. Overflow an Server uebergeben */
	/* --------------------------------------------------*/

	save_analogwert_error(pDMInfo, LinienNummer, pDeviceActiv, FunctionType, InformationNumber, set_event);

	if(found)	return(0);
	else		return(-1);
}

/****************************************************************************/
/*		Fehlerbits eines analogen Messwert abspeichern		  				*/
/*                                                                          */
/****************************************************************************/
void save_analogwert_error(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT set_event)
{

	USINT			WertLS;
	USINT			Messwert;
	USINT			AnalogError;
	short			n;
	short			AnalogIdx;
	IEC_DP_Info_typ *pDPInfo;
	USINT			TypError;
	PAR_TYPE		WertServer;
	UDINT			SizeServer;


	/* ------------------------------------------------------------------- */
	/* Pruefen, ob das Signal parametriert wurde						   */
	/* ------------------------------------------------------------------- */

	pDPInfo = (IEC_DP_Info_typ *) pDMInfo->AdrDM_wert;				/* Strukturadresse für Werteintrag belegen */
	pDPInfo += pDeviceActiv->DPInfoFirstEntry;

	for(n = pDeviceActiv->DPInfoFirstEntry; n <= pDeviceActiv->DPInfoLastEntry; n++, pDPInfo++) {
		if(pDeviceActiv->Adresse 	!= pDPInfo->DeviceAdr)	continue;
		if(FunctionType 			!= pDPInfo->IECType)	continue;
		if(InformationNumber		!= pDPInfo->IECInfoNr)	continue;
		if(!pDPInfo->Invalid 		&& !pDPInfo->Overflow)	continue;
		if((pDPInfo->IECAnalogNr<1) || (pDPInfo->IECAnalogNr>MAX_ANALOGWERTE))  continue;


		AnalogIdx	= pDPInfo->IECAnalogNr - 1;
		AnalogError = 0;
		if(pDPInfo->Invalid		&& Invalid[AnalogIdx])	AnalogError = 1;
		if(pDPInfo->Overflow	&& Overflow[AnalogIdx]) AnalogError = 1;


		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:		/* Doppelmeldung  */
			case DOPPEL_INVERS:
			case DOPPEL_EINZEL_NORMAL:
			case DOPPEL_EINZEL_INVERS:

/* Beginn 02.05.1| 02-09-08 | KOB */

			case DOPPEL_EINZEL_NORMAL_NOINVALID:
			case DOPPEL_EINZEL_INVERS_NOINVALID:

/* Ende   02.05.1| 02-09-08 | KOB */

				if(AnalogError)	Messwert = 2;
				else			Messwert = 1;
				break;
			default:	/* Einzelmeldung */
				if(AnalogError)	Messwert = 1;
				else			Messwert = 0;
				break;
		}


		#ifdef DEBUG
			dbg_msg(DIGITALMELDUNG,0,0, LinienNummer, pDeviceActiv->Adresse, pDPInfo->IECType, pDPInfo->IECInfoNr,(INT) Messwert, 0);
		#endif

		/* ----------------------------*/
		/* Wert konvertieren           */
		/* ----------------------------*/

		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;			break;
					case 1:		WertLS = 2;			break;
					case 2:		WertLS = 1;			break;
					case 3:		WertLS = 3;			break;
					default:	WertLS = Messwert;	break;
				}
				break;
			case DOPPEL_EINZEL_NORMAL:		/* Doppelmeldung als Einzelmeldung weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 1;	break;		/* V2.03 */
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS:		/* Doppelmeldung als Einzelmeldung invertiert weitergeben */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;		/* V2.03 */
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 1;	break;
					default:	WertLS = 1;	break;
				}
				break;

/* Beginn 02.05.1| 02-09-08 | KOB */

			case DOPPEL_EINZEL_NORMAL_NOINVALID:		/* Doppelmeldung als Einzelmeldung weitergeben: 0>0, 1>0, 2>1, 3>0, default>0   */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 0;	break;
					case 2:		WertLS = 1;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;
			case DOPPEL_EINZEL_INVERS_NOINVALID:		/* Doppelmeldung als Einzelmeldung weitergeben : 0>0, 1>1, 2>0, 3>0, default>0   */
				switch(Messwert) {
					case 0:		WertLS = 0;	break;
					case 1:		WertLS = 1;	break;
					case 2:		WertLS = 0;	break;
					case 3:		WertLS = 0;	break;
					default:	WertLS = 0;	break;
				}
				break;

/* Ende   02.05.1| 02-09-08 | KOB */

			case EINZEL_NORMAL:				/* Wert unveraendert weitergeben */
				WertLS = Messwert;
				break;
			case EINZEL_INVERS:				/* Einzelmeldung invertiert weitergeben */
				if(Messwert==1) WertLS = 0;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 2;
				else			WertLS = 1;
				break;
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(Messwert==1) WertLS = 1;
				else			WertLS = 2;
				break;
			default:
				WertLS = Messwert;
				break;
		}

		/* ---------------------------------------------*/
		/* Pruefen, ob angegebener Datentyp korrekt ist */
		/* ---------------------------------------------*/

		TypError = 0;
		switch(pDPInfo->Mode) {
			case DOPPEL_NORMAL:				/* Doppelmeldung unveraendert weitergeben */
			case DOPPEL_INVERS:				/* Doppelmeldung invertiert weitergeben */
			case EINZEL_DOPPEL_NORMAL:		/* Einzelmeldung als Doppelmeldung weitergeben */
			case EINZEL_DOPPEL_INVERS:		/* Einzelmeldung invertiert als Doppelmeldung weitergeben */
				if(pDPInfo->ItemTypNr==BOOL_IDX) TypError = 1;
			default:
				break;
		}
		if(pDPInfo->ItemTypNr==STREAM_IDX)	TypError = 1;

		/* ----------------------------------------------------------------------------------------------*/
		/* Bei erstmaligem Zugriff auf diesen Datenpunkt Fehlermeldung, falls Datentyp nicht korrekt ist */
		/* ----------------------------------------------------------------------------------------------*/

		if((pDPInfo->AccessCounter == 0) && (TypError)) {
			iec_parser_warn(PARS_ERR_ITEM_TYP_NOT_VALID,"ITEM Datentyp unpassend.", 0 , "DPNr.", (long) pDPInfo->DPNr, "Item-Nr.", (long) pDPInfo->ItemNr, 0, 0);
		}

		/* ------------------------------------------*/
		/* Eintraege fuer Diagnosezwecke             */
		/* ------------------------------------------*/

		if(pDPInfo->AccessCounter>250) {				/* Zaehler fuer die Anzahl der Zugriffe auf diesen Datenpunkt */
			pDPInfo->AccessCounter = 10;				/* Zaehler soll Null nicht wieder erreichen */
		}
		pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wurde parametriert */

		pDPInfo->ValueDevice	= (long) Messwert;		/* Empfangener Wert des Geraetes */
		pDPInfo->ValueServer	= (long) WertLS;		/* An den Server uebergebener Wert */


		/* -------------------------------*/
		/* Uebergabe an Server            */
		/* -------------------------------*/


		if(TypError) {
			pDPInfo->ErrorInfo = 1;							/* Info ueber fehlerhaften Datentyp */
		}
		else if (myidBrServer == 0) {
			pDPInfo->ErrorInfo = 2;							/* Info Server ID unbekannt */
		}
		else {
			pDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wird an Server uebergeben */

			switch(pDPInfo->ItemTypNr) {
				case BOOL_IDX:		WertServer.b	= (BOOL)	WertLS;	SizeServer  = sizeof(BOOL);	break;
				case USINT_IDX:		WertServer.ui1	= (USINT)	WertLS;	SizeServer  = sizeof(USINT);break;
				case SINT_IDX:		WertServer.i1	= (SINT)	WertLS;	SizeServer  = sizeof(SINT);	break;
				case UINT_IDX:		WertServer.ui2	= (UINT)	WertLS;	SizeServer  = sizeof(UINT);	break;
				case INT_IDX:		WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
				case UDINT_IDX:		WertServer.ui4	= (UDINT)	WertLS;	SizeServer  = sizeof(UDINT);break;
				case DINT_IDX:		WertServer.i4	= (DINT)	WertLS;	SizeServer  = sizeof(DINT);	break;
				default:			WertServer.i2	= (INT)		WertLS;	SizeServer  = sizeof(INT);	break;
			}

			StatusSetValueItem = SetValueItem ((void *) myidBrServer,
														pDeviceActiv->AnlagenNr,
														pDeviceActiv->AdresseLeitsystem,	/* Stationsnummer   */
														pDPInfo->DPTypNr,	  				/* Datenpunkttyp    */
														pDPInfo->DPNr,			 			/* Datenpunktnummer */
														pDPInfo->ItemNr,					/* Item */
														(USINT *) &WertServer,				/* Adresse Messwert */
														SizeServer,
														RPSTime,
														0,
														1);
			if(!StatusSetValueItem)	{
				pDPInfo->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
				pDPInfo->ErrorInfo		= 0;
			}
			else {
				pDPInfo->ErrorInfo = 3;
			}
		}
	}

}
#endif
/****************************************************************************/
/*		Analogen Messwert abspeichern		  								*/
/* Variante: Ursprung														*/
/****************************************************************************/
#ifdef	INIT_MODE_DM_WERT
USINT save_analogwert(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT AnzahlMesswerte, USINT *AdresseMesswerte,
						USINT set_event)
{
	USINT found;
	INT   Messwert;
	USINT invalid,overflow;
	WertType* PtrWerte;
	NormierungType* PtrNorm;
	UINT n;

	PtrWerte = (WertType*) pDMInfo->AdrDM_wert;			/* Strukturadresse für Werteintrag belegen */
	PtrNorm  = (NormierungType*) pDMInfo->AdrDM_norm;	/* Strukturadresse DM norm */

	found = 0;

	for(n = 0; n < pDMInfo->MaxWerteDM_wert; n++, PtrWerte++) {
		if(pDeviceActiv->Adresse	!= PtrWerte->Device) continue;
		if(FunctionType				!= PtrWerte->Type) continue;
		if(InformationNumber		!= PtrWerte->Info) continue;
		/* Gewünschte Analognummer groesser als die Anzahl empfangener Werte ? */
		if(PtrWerte->WertCode>AnzahlMesswerte) continue;

		found = 1;

		Messwert = get_analogwert_cp16(AdresseMesswerte + (PtrWerte->WertCode - 1) * 2, &invalid, &overflow);

		/* Normierung */

		if(PtrWerte->Info1==1) {
			if((pDMInfo->MaxWerteDM_norm>0) && (pDMInfo->MaxWerteDM_norm>PtrWerte->Info2)) {
				PtrNorm = (NormierungType *) pDMInfo->AdrDM_norm;
				PtrNorm += PtrWerte->Info2;
				Messwert = norm_analogwert(Messwert,PtrNorm->x1,PtrNorm->y1,PtrNorm->x2,PtrNorm->y2);
			}
		}

		#ifdef DEBUG
			dbg_msg(ANALOGMELDUNG,(UDINT) PtrWerte->WertCode ,0, LinienNummer,pDeviceActiv->Adresse, FunctionType, InformationNumber, Messwert, 0);
		#endif


		/* Übergabe ans Leitsystem */

		if((!invalid && !overflow) && (myidBrServer != 0)) {
					StatusSetValueItem = ((ACCESS_SERVER*)myidBrServer)->SetValueItem (	myidBrServer,
																						pDeviceActiv->AnlagenNr,
																						pDeviceActiv->AdresseLeitsystem,/* Stationsnummer   */
																						PtrWerte->Datentyp,  			/* Datenpunkttyp    */
																						PtrWerte->DatenpunktNr, 		/* Datenpunktnummer */
																						0,								/* Item */
																						(USINT *) &Messwert,				/* Adresse Messwert */
																						sizeof(Messwert),
																						RPSTime,
																						0,
																						1);
		}
	}

	if(!found) return(-1);

	return 0;
}
#endif
/****************************************************************************/
/*		Stoerschrieb uebergeben, bzw. Stoerschriebbuffer freigeben			*/
/* Variante: DbServ															*/
/****************************************************************************/
#ifdef INIT_MODE_DM_DPLIST
void check_dist_data_transmission()
{
	DeviceActivType *pDeviceActiv;
	unsigned short BufIdx,i;


	pDeviceActiv	= &(stANL[0].DeviceActiv[0]);		/* um Compiler Meldung zu verhindern */

	if (myidBrServer != 0) {
		for(BufIdx=0;BufIdx<DataBuffer.NrOfBuffers;BufIdx++) {

			/* Daten in Buffer warten auf die Uebergabe an den Meldeserver */

			if(DataBuffer.Info[BufIdx].Code == DATA_READY) {

				pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;

				if(pDeviceActiv->pFaultDPInfo>0) {

					pDeviceActiv->pFaultDPInfo->AccessCounter += 1;					/* Zaehler inkrementieren: Signal wurde parametriert */

					SetEventStreamStatus = SetEventItemStream ((void *) myidBrServer,
																		pDeviceActiv->AnlagenNr,
																		pDeviceActiv->AdresseLeitsystem,
																		pDeviceActiv->pFaultDPInfo->DPTypNr,
																		pDeviceActiv->pFaultDPInfo->DPNr,
																		pDeviceActiv->pFaultDPInfo->ItemNr,
																		(USINT*) DataBuffer.Info[BufIdx].DataAdr,
																		DataBuffer.Info[BufIdx].DataLen,
																		(USINT*) &DataBuffer.Info[BufIdx].SetEventStream,
																		RPSTime,
																		0,						/*USINT setTimeStampNow, */
																		1);						/* USINT enableEvent */
					if(SetEventStreamStatus==0)	{
						DataBuffer.Info[BufIdx].Code = WAIT_FOR_QUIT;
						pDeviceActiv->pFaultDPInfo->AccessCounter += 1;				/* Zaehler inkrementieren: Uebergabe an Server korrekt erfolgt */
						pDeviceActiv->pFaultDPInfo->ErrorInfo	= 0;
					}
					else {
						pDeviceActiv->pFaultDPInfo->ErrorInfo = 3;
					}
				}
				/* Stoerschrieb wurde fuer dieses Geraet nicht deklariert */
				else {
					pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;
					/* Suchen,ob Stoerschriebnummer noch vorhanden ist, wenn ja dann diesen Stoerschrieb loeschen */
					for(i=0;i<=1;i++) {
						if(i>=pDeviceActiv->NrOfFaults) break;
						if(pDeviceActiv->FaultInfo[i].Number==DataBuffer.Info[BufIdx].FaultNumber) {
							pDeviceActiv->FaultInfo[i].Clear		= 1;
							pDeviceActiv->FaultInfo[i].Transmitted	= DELETE_DATA;
							break;
						}
					}
					/* Buffer freigeben */
					DataBuffer.Info[BufIdx].Code = NODATA;
				}
			}
			else {

				/* Daten in Buffer wurden an den Meldeserver übergeben. Es wird eine Quittierung erwartet */
				if(DataBuffer.Info[BufIdx].Code == WAIT_FOR_QUIT) {
					pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;

					if(DataBuffer.Info[BufIdx].SetEventStream == 0) {
						pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;
						/* Suchen,ob Stoerschriebnummer noch vorhanden ist, wenn ja dann diesen Stoerschrieb loeschen */
						for(i=0;i<=1;i++) {
							if(i>=pDeviceActiv->NrOfFaults) break;
							if(pDeviceActiv->FaultInfo[i].Number==DataBuffer.Info[BufIdx].FaultNumber) {
								pDeviceActiv->FaultInfo[i].Clear		= 1;
								pDeviceActiv->FaultInfo[i].Transmitted	= DELETE_DATA;
								break;
							}
						}
						/* Buffer freigeben */
						DataBuffer.Info[BufIdx].Code = NODATA;
					}
				}
			} /* DataBuffer.Info[BufIdx].Code == DATA_READY) */
		} /* for() */
	} /* if (myidBrServer != NULL) */
}
#endif

/****************************************************************************/
/*		Stoerschrieb uebergeben, bzw. Stoerschriebbuffer freigeben			*/
/* Variante: Ursprung														*/
/****************************************************************************/
#ifdef	INIT_MODE_DM_WERT
void check_dist_data_transmission()
{
	DeviceActivType *pDeviceActiv;
	unsigned short BufIdx,i;


	pDeviceActiv	= &(stANL[0].DeviceActiv[0]);		/* um Compiler Meldung zu verhindern */

	if (myidBrServer != 0) {
		for(BufIdx=0;BufIdx<DataBuffer.NrOfBuffers;BufIdx++) {

			/* Daten in Buffer warten auf die Uebergabe an den Meldeserver */

			if(DataBuffer.Info[BufIdx].Code == DATA_READY) {
				pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;

				SetEventStreamStatus =  ((ACCESS_SERVER*)myidBrServer)->SetEventItemStream (myidBrServer,
																							pDeviceActiv->AnlagenNr,
																							pDeviceActiv->AdresseLeitsystem,
																							DistDataType,
																							DistDataPoint,
																							0,
																							(USINT*) DataBuffer.Info[BufIdx].DataAdr,
																							DataBuffer.Info[BufIdx].DataLen,
																							(USINT*) &DataBuffer.Info[BufIdx].SetEventStream,
																							RPSTime,
																							0,						/*USINT setTimeStampNow, */
																							1);						/* USINT enableEvent */
				if(SetEventStreamStatus==0) {
					DataBuffer.Info[BufIdx].Code = WAIT_FOR_QUIT;
				}
			}
			else {

				/* Daten in Buffer wurden ans den Meldeserver übergeben. Es wird eine Quittierung erwartet */
				if(DataBuffer.Info[BufIdx].Code == WAIT_FOR_QUIT) {
					pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;

					if(DataBuffer.Info[BufIdx].SetEventStream == 0) {
						pDeviceActiv = DataBuffer.Info[BufIdx].DevicePointer;
						/* Suchen,ob Stoerschriebnummer noch vorhanden ist, wenn ja dann diesen Stoerschrieb loeschen */
						for(i=0;i<=1;i++) {
							if(i>=pDeviceActiv->NrOfFaults) break;
							if(pDeviceActiv->FaultInfo[i].Number==DataBuffer.Info[BufIdx].FaultNumber) {
								pDeviceActiv->FaultInfo[i].Clear		= 1;
								pDeviceActiv->FaultInfo[i].Transmitted	= DELETE_DATA;
								break;
							}
						}
						/* Buffer freigeben */
						DataBuffer.Info[BufIdx].Code = NODATA;
					}
				}
			} /* DataBuffer.Info[BufIdx].Code == DATA_READY) */
		} /* for() */
	} /* if (myidBrServer != NULL) */
}
#endif

/*** eof *****************************************************************************/


