#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Stoerschriebuebertragung
*  Datei:    iec__103_disturbance.c
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
*        |          |      |
******************************************************************************/

#include <bur/plctypes.h>
#include <sys_lib.h>			/* SYS_LIB */
#include "iec__103_date.h"
#include "iec60870_5_103.h"		/* Definition der globalen Variablen */
#include "iec__103_debug.h"

/**********************************************************
*			Routinen zum Empfangen eines Störschriebes
**********************************************************/


/****************************************************************************/
/*		List of recorded disturbances 7.3.1.11			      				*/
/****************************************************************************/
USINT check_m23(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	UINT FaultNumber[2];
	USINT Transmitted[2];
	USINT Clear[2];
	USINT TypeOfValues[2];
	USINT i,k;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);


	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(232,pANL);
		return -1;
	}

	/* pCom->ReadData[7]	Variable Structur Qualifier
		 0  nr nr nr nr nr nr nr
			++ ++ ++ ++ ++ ++ ++----------: number of information elements	*/


	/* pCom->ReadData[10]	Function Type	7.2.5.1 						*/
	/* pCom->ReadData[11]	0 = not used									*/

	/* pCom->ReadData[12]	FAN Fault Number UI16  	7.2.6.6 	Dataset 1	*/
	/* pCom->ReadData[13]										Dataset 1	*/
	/* pCom->ReadData[14]	SOF Status of fault		7.2.6.24 	Dataset 1	*/
	/* pCom->ReadData[15]	Seven octet binary time	7.2.6.29	Dataset 1	*/
	/* pCom->ReadData[16]													*/
	/* pCom->ReadData[17]													*/
	/* pCom->ReadData[18]													*/
	/* pCom->ReadData[19]													*/
	/* pCom->ReadData[20]													*/
	/* pCom->ReadData[21]													*/

	/* pCom->ReadData[22]	FAN Fault Number UI16  	7.2.6.6 	Dataset 2	*/
	/* pCom->ReadData[23]										Dataset 2	*/
	/* pCom->ReadData[24]	SOF Status of fault		7.2.6.24 	Dataset 2	*/
	/* pCom->ReadData[25]	Seven octet binary time	7.2.6.29	Dataset 2	*/
	/* pCom->ReadData[26]													*/
	/* pCom->ReadData[27]													*/
	/* pCom->ReadData[28]													*/
	/* pCom->ReadData[29]													*/
	/* pCom->ReadData[30]													*/
	/* pCom->ReadData[31]													*/

	/* ......																*/
	/* ......																*/

	/* Zwischenspeichern, welche Störschriebe bereits übertragen wurden */

	for(i=0;i<=1;i++) {
		FaultNumber[i]	= pDeviceActiv->FaultInfo[i].Number;
		TypeOfValues[i] = pDeviceActiv->FaultInfo[i].TypeOfValues;
		Transmitted[i]	= pDeviceActiv->FaultInfo[i].Transmitted;
		Clear[i]		= pDeviceActiv->FaultInfo[i].Clear;

		pDeviceActiv->FaultInfo[i].Number		= 0;
		pDeviceActiv->FaultInfo[i].TypeOfValues	= 0;
		pDeviceActiv->FaultInfo[i].Transmitted	= 0;
		pDeviceActiv->FaultInfo[i].Clear		= 0;
		pDeviceActiv->FaultInfo[i].Status		= 0;
	}

	/* Anzahl Stoerschriebe und Funktionstyp */
	pDeviceActiv->FaultFunctionType			= pCom->ReadData[10];
	pDeviceActiv->NrOfFaults				= pCom->ReadData[7] & 0x7f;

	/* Maximal Daten von 2 Stoerschrieben speichern */
	for(i=0;i<=1;i++) {
		if(i>=pDeviceActiv->NrOfFaults) break;
		pDeviceActiv->FaultInfo[i].Number		= *((unsigned short *) &pCom->ReadData[12+i*10]);
		pDeviceActiv->FaultInfo[i].Status		= pCom->ReadData[14+i*10];
		get_7OctetBinaryTime(&(pCom->ReadData[15+i*10]), &pDeviceActiv->FaultInfo[i].Time, &RPSTime);
		pDeviceActiv->FaultInfo[i].IECTime.MilliSeconds1	= pCom->ReadData[15+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.MilliSeconds2	= pCom->ReadData[16+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.Minutes			= pCom->ReadData[17+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.Hours			= pCom->ReadData[18+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.Day				= pCom->ReadData[19+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.Month			= pCom->ReadData[20+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.Year				= pCom->ReadData[21+i*10];
		pDeviceActiv->FaultInfo[i].IECTime.not_used			= 0;
	}


	/* Uebertragungsmerker wieder zuordnen */
	for(i=0;i<=1;i++) {
		if(i>=pDeviceActiv->NrOfFaults) break;
		for(k=0;k<=1;k++) {
			if(pDeviceActiv->FaultInfo[i].Number == FaultNumber[k]) {
				pDeviceActiv->FaultInfo[i].TypeOfValues	= TypeOfValues[k];
				pDeviceActiv->FaultInfo[i].Transmitted	= Transmitted[k];
				pDeviceActiv->FaultInfo[i].Clear		= Clear[k];
			}
		}
	}

	/* Pruefen, ob ein schon geloeschter Stoerschrieb immer noch vorhanden ist */
	/* Dann muss der Stoerschrieb nochmals abgeholt, aber nicht an das Leitsystem uebergeben werden */
	for(i=0;i<=1;i++) {
		if(i>=pDeviceActiv->NrOfFaults) break;
			if(pDeviceActiv->FaultInfo[i].Transmitted==SEND_DELETE_TELEGRAM)
				pDeviceActiv->FaultInfo[i].Transmitted = CLEAR_DATA_AGAIN;
	}

	if(pDeviceActiv->GI_Ok==INIT_GI_START) {
		pDeviceActiv->GI_Ok	= INIT_GI_LAEUFT;
	}

	/* Sendeauftrag Stoerschriebdaten loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;

		/* Liste wurde angefordert -> Funktion loeschen */
		if(pANL->DistData.Function==REQ_FOR_LIST_OF_FAULTS) {
			pANL->DistData.Function	= NO_FUNCTION;
			pANL->DistData.TimeOut	= 0;			/* Timeoutzaehler */
		}
	}

	return 0;
}

/****************************************************************************/
/*		Ready for transmission of disturbance data 7.3.1.12			      				*/
/****************************************************************************/
USINT check_m26(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short Number;
	unsigned char  cW[2];
	unsigned short error;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(262,pANL);
		return -1;
	}

	if(pCom->ReadData[7] != 0x81) {				/* Variable Structure Qualifier 7.3.1.12 */
		Empfangsfehler(263,pANL);
		return -1;
	}

	/* Sendeauftrag loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;
	}

	/* pCom->ReadData[10]	Function Type	7.2.5.1 											*/
	/* pCom->ReadData[11]	0 = not used														*/
	/* pCom->ReadData[12]	0 = not used														*/
	/* pCom->ReadData[13]	TOV Type of disturbance values	7.2.6.27							*/
	/* pCom->ReadData[14]	FAN Fault Number UI16			7.2.6.6								*/
	/* pCom->ReadData[15]																		*/
	/* pCom->ReadData[16]	NOF Number of Grid faults UI16	7.2.6.12							*/
	/* pCom->ReadData[17]																		*/
	/* pCom->ReadData[18]	NOC Number of channels UI8		7.2.6.10 							*/
	/* pCom->ReadData[19]	NOE Number of information elements of a channel UI16 7.2.6.11		*/
	/* pCom->ReadData[20]																		*/
	/* pCom->ReadData[21]	INT Intervall between information elements of a channel UI16 7.2.6.7*/
	/* pCom->ReadData[22]																		*/
	/* pCom->ReadData[23]	Four octet binary time	7.2.6.28									*/
	/* pCom->ReadData[24]																		*/
	/* pCom->ReadData[25]																		*/
	/* pCom->ReadData[26]																		*/


	/* Wurde Stoerschriebuebertragung von diesem Geraet eingeleitet ? */
	if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DistData.DeviceAtWork==pANL->DeviceAtWork)){
		error = 0;
		pDeviceActiv->FaultInfo[pANL->DistData.FaultIdx].TypeOfValues	= pCom->ReadData[13];
		pANL->DistData.TypeOfValues		= pCom->ReadData[13];
		pANL->DistData.WaitForResponse	= 0;
		/* Wird Telegrammtyp erwartet ? */
		if(pANL->DistData.Function==SELECTION_OF_FAULT) {
			cW[0]	= pCom->ReadData[14];
			cW[1]	= pCom->ReadData[15];
			Number	= *((unsigned short *) &cW[0]);

			/* Gleiche Stoerschriebnummer ? */
			if(Number==pANL->DistHead.FaultNumber) {
				cW[0]									= pCom->ReadData[16];
				cW[1]									= pCom->ReadData[17];
				pANL->DistHead.GridFaultNumber			= *((unsigned short *) &cW[0]);
				pANL->DistData.NrOfChannels 			= (UINT) pCom->ReadData[18];
				cW[0]									= pCom->ReadData[19];
				cW[1]									= pCom->ReadData[20];
				pANL->DistHead.NrOfElementsPerChannel	= *((unsigned short *) &cW[0]);
				cW[0]									= pCom->ReadData[21];
				cW[1]									= pCom->ReadData[22];
				pANL->DistHead.IntervallTime			= *((unsigned short *) &cW[0]);

/*				Version: Uhrzeit im RTCTime-Format
				get_4OctetBinaryTime(&(pCom->ReadData[23]),(RTCtime_typ*) &pANL->DistHead.DistStartTime,(RTCtime_typ*) &pANL->DistHead.FaultTime);
*/
				pANL->DistHead.DistStartTime.MilliSeconds1	= pCom->ReadData[23];
				pANL->DistHead.DistStartTime.MilliSeconds2	= pCom->ReadData[24];
				pANL->DistHead.DistStartTime.Minutes		= pCom->ReadData[25];
				pANL->DistHead.DistStartTime.Hours			= pCom->ReadData[26];

				if(save_disturbance_data(DISTURBANCE_START,pANL)==0) {
					pANL->DistData.Function = REQ_DISTURBANCE_TRANSMISSION;
					pANL->DistData.DataSend	= 1;
					pANL->DistData.TimeOut	= 0;			/* Timeoutzaehler */
					pANL->DistData.Trys		= 0;			/* Anzahl Sendeversuche */
				}
				else {
					error = DIST_ERR_MEMORY;
				}
			}
			else {
				error = DIST_ERR_FAULTNUMBER;
			}
		}
		else {
			error = DIST_ERR_TELEGRAM;
		}
		/* Fehlerkennung speichern und Uebertragung des Stoerschriebes abbrechen */
		if(error) {
			save_disturbance_error(pANL,error,pANL->DistData.Function,0,0);
			clear_dist_data_request(pANL);
			abort_disturbance_data(pANL);
		}
	}
	return 0;
}


/****************************************************************************/
/*		Ready for transmission of a channel 7.3.1.13				      	*/
/****************************************************************************/
USINT check_m27(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short Number;
	unsigned char  cW[2];
	unsigned short error;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(272,pANL);
		return -1;
	}

	if(pCom->ReadData[7] != 0x81) {				/* Variable Structure Qualifier 7.3.1.13 */
		Empfangsfehler(273,pANL);
		return -1;
	}

	/* Sendeauftrag loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;
	}

	/* pCom->ReadData[10]	Function Type	7.2.5.1 											*/
	/* pCom->ReadData[11]	0 = not used														*/
	/* pCom->ReadData[12]	0 = not used														*/
	/* pCom->ReadData[13]	TOV Type of disturbance values	7.2.6.27							*/
	/* pCom->ReadData[14]	FAN Fault Number UI16			7.2.6.6								*/
	/* pCom->ReadData[15]																		*/
	/* pCom->ReadData[16]	ACC Actual Channel UI8			7.2.6.1 							*/
	/* pCom->ReadData[17]	RPV Rated primary value R32.23	7.2.6.17							*/
	/* pCom->ReadData[18]																		*/
	/* pCom->ReadData[19]																		*/
	/* pCom->ReadData[20]																		*/
	/* pCom->ReadData[21]	RSV Rated secondary value R32.23	7.2.6.18						*/
	/* pCom->ReadData[22]																		*/
	/* pCom->ReadData[23]																		*/
	/* pCom->ReadData[24]																		*/
	/* pCom->ReadData[25]	RFA Reference factor R32.23		7.2.6.16							*/
	/* pCom->ReadData[26]																		*/
	/* pCom->ReadData[27]																		*/
	/* pCom->ReadData[28]																		*/


	/* Wurde Stoerschriebuebertragung von diesem Geraet eingeleitet ? */
	if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DistData.DeviceAtWork==pANL->DeviceAtWork)){
		error = 0;

		pANL->DistData.WaitForResponse		= 0;
		pANL->DistData.TypeOfValues			= pCom->ReadData[13];
		pANL->DistData.ActualChannelTyp		= pCom->ReadData[16];

		/* Wird ein neuer Kanal uebertragen, oder findet eine Wiederholung statt ? */
		if(pANL->DistData.ActualChannelTyp==pANL->DistData.LastChannelTyp) {
			pANL->DistData.NewChannel = 0;
		}
		else {
			pANL->DistData.NewChannel = 1;
		}

		/* Wird Telegrammtyp erwartet ? */
		if((pANL->DistData.Function==ACK_FOR_TAGS) || (pANL->DistData.Function==ACK_FOR_A_CHANNEL)) {
			cW[0]	= pCom->ReadData[14];
			cW[1]	= pCom->ReadData[15];
			Number	= *((unsigned short *) &cW[0]);

			/* Gleiche Stoerschriebnummer ? */
			if(Number==pANL->DistHead.FaultNumber) {

				if(save_disturbance_data(DISTURBANCE_CHANNEL_START,pANL)==0) {
					pANL->DistData.ChannelDataError	= 0;
					pANL->DistData.TimeOut			= 0;						/* Timeoutzaehler */
					pANL->DistData.Function 		= REQ_FOR_A_CHANNEL;		/* Aufforderung einen Kanal übertragen*/
					pANL->DistData.DataSend			= 1;
				}
				else {
					error = DIST_ERR_MEMORY;
				}
			}
			else {
				error = DIST_ERR_FAULTNUMBER;
			}
		}
		else {
			error = DIST_ERR_TELEGRAM;
		}
		/* Fehlerkennung speichern und Uebertragung des Stoerschriebes abbrechen */
		if(error) {
			save_disturbance_error(pANL,error,pANL->DistData.Function,0,pANL->DistData.ActualChannelTyp);
			clear_dist_data_request(pANL);
			abort_disturbance_data(pANL);
		}
	}
	return 0;
}

/****************************************************************************/
/*		Ready for transmission of tags 7.3.1.14						      	*/
/****************************************************************************/
USINT check_m28(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short Number;
	unsigned char  cW[2];
	unsigned short error;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(282,pANL);
		return -1;
	}

	if(pCom->ReadData[7] != 0x81) {				/* Variable Structure Qualifier 7.3.1.14 */
		Empfangsfehler(283,pANL);
		return -1;
	}

	/* Sendeauftrag loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;
	}

	/* pCom->ReadData[10]	Function Type	7.2.5.1 											*/
	/* pCom->ReadData[11]	0 = not used														*/
	/* pCom->ReadData[12]	0 = not used														*/
	/* pCom->ReadData[13]	0 = not used														*/
	/* pCom->ReadData[14]	FAN Fault Number UI16			7.2.6.6								*/
	/* pCom->ReadData[15]																		*/


	/* Wurde Stoerschriebuebertragung von diesem Geraet eingeleitet ? */
	if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DistData.DeviceAtWork==pANL->DeviceAtWork)){
		error = 0;
		pANL->DistData.WaitForResponse	= 0;
		/* Wird Telegrammtyp erwartet ? */
		if(pANL->DistData.Function==REQ_DISTURBANCE_TRANSMISSION) {
			cW[0]	= pCom->ReadData[14];
			cW[1]	= pCom->ReadData[15];
			Number	= *((unsigned short *) &cW[0]);

			/* Gleiche Stoerschriebnummer ? */
			if(Number==pANL->DistHead.FaultNumber) {
				pANL->DistData.TimeOut	= 0;					/* Timeoutzaehler */
				pANL->DistData.Function = REQ_FOR_TAGS;
				pANL->DistData.DataSend	= 1;
			}
			else {
				error = DIST_ERR_FAULTNUMBER;
			}
		}
		else {
			error = DIST_ERR_TELEGRAM;
		}

		/* Fehlerkennung speichern und Uebertragung des Stoerschriebes abbrechen */
		if(error) {
			save_disturbance_error(pANL,error,pANL->DistData.Function,0,0);
			clear_dist_data_request(pANL);
			abort_disturbance_data(pANL);
		}
	}
	return 0;
}


/****************************************************************************/
/*		Transmission of tags 7.3.1.15				      					*/
/****************************************************************************/
USINT check_m29(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short Number;
	unsigned char  cW[2];
	unsigned short error;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(292,pANL);
		return -1;
	}

	if(pCom->ReadData[7] != 0x81) {				/* Variable Structure Qualifier 7.3.1.15 */
		Empfangsfehler(293,pANL);
		return -1;
	}

	/* Sendeauftrag loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;
	}

	/* pCom->ReadData[10]	Function Type	7.2.5.1 											*/
	/* pCom->ReadData[11]	0 = not used														*/
	/* pCom->ReadData[12]	FAN Fault Number UI16			7.2.6.6								*/
	/* pCom->ReadData[13]																		*/
	/* pCom->ReadData[14]	NOT Number of tags UI8			7.2.6.13 							*/
	/* pCom->ReadData[15]	TAP Tag position UI16			7.2.6.25							*/
	/* pCom->ReadData[16]																		*/
	/* pCom->ReadData[17]	FUNCTION TYPE					7.2.5.1		tag 1					*/
	/* pCom->ReadData[18]	INFORMATION NUMBER				7.2.5.2		tag 1					*/
	/* pCom->ReadData[19]	DPI								7.2.6.5		tag 1					*/
	/*.........*/
	/*.........*/


	/* Wurde Stoerschriebuebertragung von diesem Geraet eingeleitet ? */
	if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DistData.DeviceAtWork==pANL->DeviceAtWork)){
		error = 0;
		pANL->DistData.WaitForResponse	= 0;
		/* Wird Telegrammtyp erwartet ? */
		if(pANL->DistData.Function==REQ_FOR_TAGS) {
			cW[0]	= pCom->ReadData[12];
			cW[1]	= pCom->ReadData[13];
			Number	= *((unsigned short *) &cW[0]);

			/* Gleiche Stoerschriebnummer ? */
			if(Number==pANL->DistHead.FaultNumber) {
				pANL->DistData.NrOfReceivedTags	= pANL->DistData.NrOfReceivedTags + (UINT) pCom->ReadData[14];		/* Anzahl übertragener Tags */
				pANL->DistData.TimeOut			= 0;					/* Timeoutzaehler */
				if(save_disturbance_data(DISTURBANCE_TAGS,pANL)!=0) {
					error = DIST_ERR_MEMORY;
				}
			}
			else {
				error = DIST_ERR_FAULTNUMBER;
			}
		}
		else {
			error = DIST_ERR_TELEGRAM;
		}

		/* Fehlerkennung speichern und Uebertragung des Stoerschriebes abbrechen */
		if(error) {
			save_disturbance_error(pANL,error,pANL->DistData.Function,0,0);
			clear_dist_data_request(pANL);
			abort_disturbance_data(pANL);
		}
	}
	return 0;
}

/****************************************************************************/
/*		Transmission of disturbance values 7.3.1.16				      		*/
/****************************************************************************/
USINT check_m30(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short Number;
	unsigned char  cW[2];
	unsigned short error;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(302,pANL);
		return -1;
	}

	if(pCom->ReadData[7] != 0x81) {				/* Variable Structure Qualifier 7.3.1.16 */
		Empfangsfehler(303,pANL);
		return -1;
	}

	/* Sendeauftrag loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;
	}

	/* pCom->ReadData[10]	Function Type	7.2.5.1 											*/
	/* pCom->ReadData[11]	0 = not used														*/
	/* pCom->ReadData[12]	0 = not used														*/
	/* pCom->ReadData[13]	TOV Type of disturbance values	7.2.6.27							*/
	/* pCom->ReadData[14]	FAN Fault Number UI16			7.2.6.6								*/
	/* pCom->ReadData[15]																		*/
	/* pCom->ReadData[16]	ACC Actual Channel UI8			7.2.6.1 							*/
	/* pCom->ReadData[17]	NDV Number of relevant disturbance values per ASDU UI8	7.2.6.14 	*/
	/* pCom->ReadData[18]	NFE Number of the ASDU first information element UI16	7.2.6.9 	*/
	/* pCom->ReadData[19]																		*/
	/* pCom->ReadData[20]	SDV 1 Single disturbance value F16	7.2.6.22						*/
	/* pCom->ReadData[21]																		*/
	/*.........*/
	/*.........*/


	/* Wurde Stoerschriebuebertragung von diesem Geraet eingeleitet ? */
	if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DistData.DeviceAtWork==pANL->DeviceAtWork)){

		error = 0;
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.TimeOut			= 0;						/* Timeoutzaehler */

		/* Wird Telegrammtyp erwartet ? */
		if(pANL->DistData.Function==REQ_FOR_A_CHANNEL) {
			cW[0]	= pCom->ReadData[14];
			cW[1]	= pCom->ReadData[15];
			Number	= *((unsigned short *) &cW[0]);

			/* Gleiche Stoerschriebnummer ? */
			if(Number==pANL->DistHead.FaultNumber) {
				if(pANL->DistData.ChannelDataError==0) {
					cW[0]	= pCom->ReadData[18];
					cW[1]	= pCom->ReadData[19];
					Number	= *((unsigned short *) &cW[0]);
					/* Elementnummer fortlaufend ? */
					if(pANL->DistData.EstimateChannelPos!=Number) {
						pANL->DistData.ChannelDataError = DIST_ERR_VAL_STARTPOS;
					}
					/* Kanalnummer gleich ? */
					if(pANL->DistData.ActualChannelTyp!=pCom->ReadData[16]) {
						pANL->DistData.ChannelDataError = DIST_ERR_CHANNEL_TYPE;
					}

					/* Daten korrekt -> speichern */
					if(pANL->DistData.ChannelDataError==0) {
						if(save_disturbance_data(DISTURBANCE_VALUES,pANL)!=0) {
							error = DIST_ERR_MEMORY;
						}
					}
				}
			}
			else {
				error = DIST_ERR_FAULTNUMBER;
			}
		}
		else {
			if(pANL->DistData.Function!=ABORT_CHANNEL_TRANSMISSION) {
				error = DIST_ERR_TELEGRAM;
			}
		}

		/* Fehlerkennung speichern und Uebertragung des Stoerschriebes abbrechen */
		if(error) {
			save_disturbance_error(pANL,error,pANL->DistData.Function,0,0);
			clear_dist_data_request(pANL);
			abort_disturbance_data(pANL);
		}
	}
	return 0;
}

/****************************************************************************/
/*		End of transmission 7.3.1.17								      	*/
/****************************************************************************/
USINT check_m31(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short Number;
	unsigned char  cW[2];
	unsigned short error,abort_on_error;


	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(302,pANL);
		return -1;
	}

	if(pCom->ReadData[7] != 0x81) {				/* Variable Structure Qualifier 7.3.1.17 */
		Empfangsfehler(303,pANL);
		return -1;
	}

	/* Sendeauftrag loeschen */
	if(pANL->DistData.DeviceAtWork==pANL->DeviceAtWork){
		pANL->DistData.WaitForResponse	= 0;
		pANL->DistData.DataSend			= 0;
	}

	/* pCom->ReadData[10]	Function Type	7.2.5.1 											*/
	/* pCom->ReadData[11]	0 = not used														*/
	/* pCom->ReadData[12]	TOO Type of order UI8	7.2.6.26									*/
	/* pCom->ReadData[13]	TOV Type of disturbance values	7.2.6.27							*/
	/* pCom->ReadData[14]	FAN Fault Number UI16			7.2.6.6								*/
	/* pCom->ReadData[15]																		*/
	/* pCom->ReadData[16]	ACC Actual Channel UI8			7.2.6.1 							*/


	error			= 0;
	abort_on_error	= 1;

	/* Wurde Stoerschriebuebertragung von diesem Geraet eingeleitet ? */
	if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DistData.DeviceAtWork==pANL->DeviceAtWork)){
		pANL->DistData.WaitForResponse	= 0;
		cW[0]	= pCom->ReadData[14];
		cW[1]	= pCom->ReadData[15];
		Number	= *((unsigned short *) &cW[0]);

		/* Gleiche Stoerschriebnummer ? */
		if(Number==pANL->DistHead.FaultNumber) {
			switch(pCom->ReadData[12]) {

				case 32:										/* end of disturbance data transmission without abortion */
					if(pANL->DistData.Function==ACK_FOR_A_CHANNEL) {
						if(pANL->DistData.NrOfReceivedChannels==pANL->DistData.NrOfChannels) {
							save_disturbance_data(DISTURBANCE_END,pANL);
							pANL->DistData.Function	= NO_FUNCTION;
							pANL->DistData.Delay	= IEC_Config.DisturbanceDelay;
						}
						else {
							error = DIST_ERR_NR_OF_CHANNELS;
						}
					}
					break;
				case 33:										/* end of disturbance data transmission with abortion by control system */
				case 34:										/* end of disturbance data transmission with abortion by protection equipment */
					error = DIST_ERR_ABORT;
					break;

				case 35:										/* end of channel transmission without abortion */
					if(pANL->DistData.Function==REQ_FOR_A_CHANNEL) {
						if((pANL->DistData.EstimateChannelPos!=pANL->DistHead.NrOfElementsPerChannel) && (pANL->DistData.ChannelDataError==0)) {
							pANL->DistData.ChannelDataError = DIST_ERR_VAL_ENDPOS;
						}
						if(pANL->DistData.ChannelDataError==0) {
							if(pANL->DistData.NewChannel) {
								if(pANL->DistData.StoreChannel) {
									pANL->DistHead.NrOfStoredChannels++;
								}
								pANL->DistData.NrOfReceivedChannels++;
							}
							pANL->DistData.LastChannelTyp	= pANL->DistData.ActualChannelTyp;
							pANL->DistData.DataSend			= 1;
							pANL->DistData.Function 		= ACK_FOR_A_CHANNEL;
							pANL->DistData.TypeOfOrder		= CHANNEL_TRANS_SUCCESSFULLY;		/* channel transmitted successfully */
							pANL->DistData.TimeOut			= 0;		/* Timeoutzaehler */
							pANL->DistData.Trys				= 0;
						}
						else {
							pANL->DistData.Trys++;

							if(pANL->DistData.Trys<=3) {
								pANL->DistData.DataSend		= 1;
								pANL->DistData.Function 	= ACK_FOR_A_CHANNEL;
								pANL->DistData.TypeOfOrder	= CHANNEL_TRANS_NOT_SUCCESSFULLY;		/* channel transmitted not successfully */
								pANL->DistData.TimeOut		= 0;		/* Timeoutzaehler */
							}
							else {
								error = pANL->DistData.ChannelDataError;
							}
						}
					}
					else {
						error = DIST_ERR_TELEGRAM;
					}
					break;
				case 36:										/* end of channel transmission with abortion by control system */
				case 37:										/* end of channel transmission with abortion by protection equipment */
					if((pANL->DistData.Function==REQ_FOR_A_CHANNEL) || (pANL->DistData.Function==ABORT_CHANNEL_TRANSMISSION)) {
						error = DIST_ERR_CHANNEL_ABORT;
					}
					else {
						error = DIST_ERR_TELEGRAM;
					}
					break;

				case 38:															/* end of tag transmission without abortion */
					if(pANL->DistData.Function==REQ_FOR_TAGS) {
						pANL->DistData.DataSend		= 1;
						pANL->DistData.Function		= ACK_FOR_TAGS;
						pANL->DistData.TypeOfOrder	= TAGS_TRANS_SUCCESSFULLY;		/* tags transmitted successfully */
						pANL->DistData.TimeOut		= 0;							/* Timeoutzaehler */
						pANL->DistData.Trys			= 0;
					}
					else {
						error = DIST_ERR_TELEGRAM;
					}
					break;
				case 39:										/* end of tag transmission with abortion by control system */
				case 40:										/* end of tag transmission with abortion by protection equipment */
					if(pANL->DistData.Function==REQ_FOR_TAGS) {
						error = DIST_ERR_TAG_ABORT;
					}
					else {
						error = DIST_ERR_TELEGRAM;
					}
					break;
			}
		}
		else {
			error = DIST_ERR_FAULTNUMBER;
		}

		/* Fehlerkennung speichern und Uebertragung des Stoerschriebes abbrechen */
		if(error) {
			save_disturbance_error(pANL,error,pANL->DistData.Function,(UINT) pCom->ReadData[12] ,(UINT) pANL->DistData.ActualChannelTyp);
			if(abort_on_error) {
				clear_dist_data_request(pANL);			/* Buffer freigeben */
				abort_disturbance_data(pANL);			/* Uebertragung abbrechen */
			}
		}
	}
	return 0;
}
/****************************************************************************/
/*		Stoerschriebdaten abspeichern		  								*/
/****************************************************************************/
signed short save_disturbance_data(UINT code,AnlagenType *pANL)
{
	StoerDM_typ			*pStoerDM;
	WertType			*pWerte;
	UINT				n;
	ComType 			*pCom;
	DeviceActivType 	*pDeviceActiv;
	DisturbanceDataType *pDistData;
	USINT				Idx;
	UDINT				MemSize;
	signed short		error=0;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);
	pDistData		= &(pANL->DistData);

	/* Basisadresse des Datenbuffers */
	Adresse		= DataBuffer.Info[pDistData->BufferIdx].DataAdr;
	MemSize 	= DataBuffer.Info[pDistData->BufferIdx].Size;

	switch(code) {
		case DISTURBANCE_START:
			break;

		case DISTURBANCE_TAGS:
			/* pCom->ReadData[14]	NOT Number of tags UI8			7.2.6.13 							*/
			/* pCom->ReadData[15]	TAP Tag position UI16			7.2.6.25							*/
			/* pCom->ReadData[16]																		*/
			/* pCom->ReadData[17]	FUNCTION TYPE					7.2.5.1		tag 1					*/
			/* pCom->ReadData[18]	INFORMATION NUMBER				7.2.5.2		tag 1					*/
			/* pCom->ReadData[19]	DPI								7.2.6.5		tag 1					*/


			/* Anzahl Tags im Telegramm */
			NrOfTags	= pCom->ReadData[14];

			CopySize	= sizeof(DisturbanceTagInfo);

			/* Position innerhalb des Stoerschriebes */
			memcpy(&DisturbanceTagInfo.TagPosition,&pCom->ReadData[15],sizeof(DisturbanceTagInfo.TagPosition));

			DisturbanceTagInfo.Res1 = 0;

			/* Suchen, ob Digitalsignal konfiguriert wurde */
			if(NrOfTags>0) {
				for(aktTag=0;aktTag<NrOfTags;aktTag++) {
					FunctionType 		= pCom->ReadData[17+aktTag*3];
					InformationNumber	= pCom->ReadData[18+aktTag*3];
					/* Wert des Binärsignales */
					Messwert			= pCom->ReadData[19+aktTag*3];


/* Beginn Variante 1 */

					/* Offset im Speicher fuer die zu speichernden Tags */
					Offset	= sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
					/* Ausreichend Speicher vorhanden ? */
					if(Offset+CopySize>MemSize) error = -1;

					if(error==0) {
						DisturbanceTagInfo.DatenTyp 	= FunctionType;
						DisturbanceTagInfo.DatenpunktNr = InformationNumber;
						DisturbanceTagInfo.Item 		= 0;
						DisturbanceTagInfo.Value		= Messwert;
						memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);
						pANL->DistHead.NrOfTags++;
					}

/* Ende Variante 1 */










/* Beginn Variante 2 */
if(0==1) {
					pWerte = (WertType*) pANL->dm.AdrDM_wert;

					found = 0;
					for(n = 0; n < pANL->dm.MaxWerteDM_wert; n++, pWerte++) {
						if(pDeviceActiv->Adresse != pWerte->Device) continue;
						if(FunctionType != pWerte->Type) continue;
						if(InformationNumber != pWerte->Info) continue;
						found = 1;
						break;
					}

					/* Pruefen, ob genuegend Speicher zur Verfuegung steht */
					Offset	= sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
					/* Maximal koennen 2 neue Tags dazukommen */
					if(Offset+2*CopySize>MemSize) error = -1;

					if((found) && (error==0)) {

						if(pWerte->WertCode==0) {
							WertLS0 = 0;			/* Wert invertieren ? 0=nein 1=ja */
							WertLS1 = 1;
						}
						else {
							WertLS0 = 1;			/* Wert invertieren ? 0=nein 1=ja */
							WertLS1 = 0;
						}

						/* Übergabe ans Leitsystem */


						if(pWerte->Info1==1) {			/* Abbildung auf 2 Datenpunkte */
							switch(Messwert) {
								case 1:		/* Zustand aus */
									DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
									DisturbanceTagInfo.DatenpunktNr = pWerte->DatenpunktNr;
									DisturbanceTagInfo.Item 		= 0;
									DisturbanceTagInfo.Value		= WertLS1;

									Offset	= sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
									memcpy((UDINT *) (Adresse + Offset), &(DisturbanceTagInfo), CopySize);

									pANL->DistHead.NrOfTags++;

									DisturbanceTagInfo.DatenTyp 	= (UINT) pWerte->Datentyp;
									DisturbanceTagInfo.DatenpunktNr = (UINT) pWerte->Info2;
									DisturbanceTagInfo.Item 		= 0;
									DisturbanceTagInfo.Value		= WertLS0;

									Offset = sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
									memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);

									pANL->DistHead.NrOfTags++;
									break;
								case 2:		/* Zustand ein */
									DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
									DisturbanceTagInfo.DatenpunktNr = pWerte->DatenpunktNr;
									DisturbanceTagInfo.Item 		= 0;
									DisturbanceTagInfo.Value		= WertLS0;

									Offset = sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
									memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);

									pANL->DistHead.NrOfTags++;

									DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
									DisturbanceTagInfo.DatenpunktNr = pWerte->Info2;
									DisturbanceTagInfo.Item 		= 0;
									DisturbanceTagInfo.Value		= WertLS1;

									Offset = sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
									memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);

									pANL->DistHead.NrOfTags++;

									break;
								default:	/* undefiniert */
									DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
									DisturbanceTagInfo.DatenpunktNr = pWerte->DatenpunktNr;
									DisturbanceTagInfo.Item 		= 0;
									DisturbanceTagInfo.Value		= WertLS0;

									Offset = sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
									memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);

									pANL->DistHead.NrOfTags++;

									DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
									DisturbanceTagInfo.DatenpunktNr = pWerte->Info2;
									DisturbanceTagInfo.Item 		= 0;
									DisturbanceTagInfo.Value		= WertLS0;

									Offset = sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
									memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);

									pANL->DistHead.NrOfTags++;

									break;
							}
						}
						else {									/* Abbildung auf 1 Datenpunkt */
							if(Messwert==2) {	/* Zustand ein */
								DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
								DisturbanceTagInfo.DatenpunktNr = pWerte->DatenpunktNr;
								DisturbanceTagInfo.Item 		= 0;
								DisturbanceTagInfo.Value		= WertLS1;
							}
							else {
								DisturbanceTagInfo.DatenTyp 	= pWerte->Datentyp;
								DisturbanceTagInfo.DatenpunktNr = pWerte->DatenpunktNr;
								DisturbanceTagInfo.Item 		= 0;
								DisturbanceTagInfo.Value		= WertLS0;

							}
							Offset = sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * CopySize;
							memcpy((UDINT *) (Adresse + Offset),&(DisturbanceTagInfo),CopySize);

							pANL->DistHead.NrOfTags++;


						}
					} /* if(found)*/
}
/* Ende Variante 2 */

					/* Bei Fehler Abbruch */
					if(error) break;

				} /*for(aktTag=0;aktTag<NrOfTags;aktTag++)*/
			} /* if(NrOfTags>0) */
			break;

		case DISTURBANCE_CHANNEL_START:

			/* Erwartete Startposition des naechsten Telegrammes mit Analogwerten */
			pDistData->EstimateChannelPos	= 0;


/* Beginn Variante 1 */

			DisturbanceChannelInfo.DatenTyp		= pANL->DistData.FunctionType;
			DisturbanceChannelInfo.DatenpunktNr	= pANL->DistData.ActualChannelTyp;
			DisturbanceChannelInfo.Item			= 0;

			memcpy(&DisturbanceChannelFaktor.RPV,&pCom->ReadData[17],sizeof(DisturbanceChannelFaktor.RPV));
			memcpy(&DisturbanceChannelFaktor.RSV,&pCom->ReadData[21],sizeof(DisturbanceChannelFaktor.RSV));
			memcpy(&DisturbanceChannelFaktor.RFA,&pCom->ReadData[25],sizeof(DisturbanceChannelFaktor.RFA));

			pANL->DistData.StoreChannel			= 1;


			ChannelDataLen	= 2 * (UDINT) pANL->DistHead.NrOfElementsPerChannel + sizeof(DisturbanceChannelInfo) + sizeof(DisturbanceChannelFaktor);
			Offset			= sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * sizeof(DisturbanceTagInfo);
			Offset			= Offset + (UDINT) pANL->DistHead.NrOfStoredChannels * ChannelDataLen;

			/* Pruefen, ob genuegend Speicher zur Verfuegung steht */
			if(Offset+sizeof(DisturbanceChannelInfo)+sizeof(DisturbanceChannelFaktor)>MemSize) {
				error = -1;
			}
			else {
				memcpy((UDINT *) (Adresse + Offset),&(DisturbanceChannelInfo),sizeof(DisturbanceChannelInfo));
				Offset += sizeof(DisturbanceChannelInfo);
				memcpy((UDINT *) (Adresse + Offset),&(DisturbanceChannelFaktor),sizeof(DisturbanceChannelFaktor));
			}

/* Ende Variante 1 */

/* Beginn Variante 2 */


if(0==1) {
			/* Suchen, ob Analogkanal konfiguriert wurde */
			pStoerDM = (StoerDM_typ*) pANL->dm.AdrDM_stoer;
			found = 0;
			for(n = 0; n < pANL->dm.MaxWerteDM_stoer; n++, pStoerDM++) {
				if(pDeviceActiv->Adresse != pStoerDM->Device) continue;
				if(pANL->DistData.ActualChannelTyp != pStoerDM->Type) continue;
				found = 1;
				break;
			}

			if(found) {

				DisturbanceChannelInfo.DatenpunktNr	= pStoerDM->DatenpunktNr;
				DisturbanceChannelInfo.DatenTyp		= pStoerDM->Datentyp;
				DisturbanceChannelInfo.Item			= 0;

				memcpy(&DisturbanceChannelFaktor.RPV,&pCom->ReadData[17],sizeof(DisturbanceChannelFaktor.RPV));
				memcpy(&DisturbanceChannelFaktor.RSV,&pCom->ReadData[21],sizeof(DisturbanceChannelFaktor.RSV));
				memcpy(&DisturbanceChannelFaktor.RFA,&pCom->ReadData[25],sizeof(DisturbanceChannelFaktor.RFA));

				pANL->DistData.StoreChannel			= 1;


				ChannelDataLen	= 2 * (UDINT) pANL->DistHead.NrOfElementsPerChannel + sizeof(DisturbanceChannelInfo) + sizeof(DisturbanceChannelFaktor);
				Offset			= sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * sizeof(DisturbanceTagInfo);
				Offset			= Offset + (UDINT) pANL->DistHead.NrOfStoredChannels * ChannelDataLen;

				/* Pruefen, ob genuegend Speicher zur Verfuegung steht */
				if(Offset+sizeof(DisturbanceChannelInfo)+sizeof(DisturbanceChannelFaktor)>MemSize) {
					error = -1;
				}
				else {
					memcpy((UDINT *) (Adresse + Offset),&(DisturbanceChannelInfo),sizeof(DisturbanceChannelInfo));
					Offset += sizeof(DisturbanceChannelInfo);
					memcpy((UDINT *) (Adresse + Offset),&(DisturbanceChannelFaktor),sizeof(DisturbanceChannelFaktor));
				}
			}
			else {
				pANL->DistData.StoreChannel = 0;
			}
}
/* Ende Variante 2 */

			break;

		case DISTURBANCE_VALUES:
			/* pCom->ReadData[17]	NDV Number of relevant disturbance values per ASDU UI8	7.2.6.14 	*/
			/* pCom->ReadData[18]	NFE Number of the ASDU first information element UI16	7.2.6.9 	*/
			/* pCom->ReadData[19]																		*/
			/* pCom->ReadData[20]	SDV 1 Single disturbance value F16	7.2.6.22						*/
			/* pCom->ReadData[21]																		*/

			memcpy(&StartNr,&pCom->ReadData[18],sizeof(StartNr));
			Anzahl			= (UDINT) pCom->ReadData[17];

			if(pANL->DistData.StoreChannel) {
				ChannelDataLen	= 2 * (UDINT) pANL->DistHead.NrOfElementsPerChannel + sizeof(DisturbanceChannelInfo) + sizeof(DisturbanceChannelFaktor);
				Offset			= sizeof(pANL->DistHead) + pANL->DistHead.NrOfTags * sizeof(DisturbanceTagInfo);
				Offset			= Offset + (UDINT) pANL->DistHead.NrOfStoredChannels * ChannelDataLen;
				Offset			= Offset + sizeof(DisturbanceChannelInfo) + sizeof(DisturbanceChannelFaktor);
				Offset			= Offset + 2 * (UDINT) StartNr;
				CopySize		= 2 * Anzahl;

				/* Pruefen, ob genuegend Speicher zur Verfuegung steht */
				if(Offset+CopySize>MemSize) {
					error = -1;
				}
				else {
					memcpy((UDINT *) (Adresse + Offset),&(pCom->ReadData[20]),CopySize);
				}
			}
			pDistData->EstimateChannelPos = StartNr + Anzahl;

			break;

		case DISTURBANCE_END:

			/* Stoerschrieb kann gleich geloescht werden */
			if(pDeviceActiv->FaultInfo[pANL->DistData.FaultIdx].Transmitted==CLEAR_DATA_AGAIN) {
				pDeviceActiv->FaultInfo[pANL->DistData.FaultIdx].Clear			= 1;
				pDeviceActiv->FaultInfo[pANL->DistData.FaultIdx].Transmitted	= DELETE_DATA;
				Idx 						= pANL->DistData.BufferIdx;
				DataBuffer.Info[Idx].Code 	= NODATA;
			}
			/* Stoerschrieb wird an Leitsystem uebergeben */
			else {

				/* Header in Buffer eintragen */

				memcpy((UDINT *) Adresse,&(pANL->DistHead),sizeof(pANL->DistHead));

				pDeviceActiv->FaultInfo[pANL->DistData.FaultIdx].Transmitted= DATA_READY;

				Idx = pANL->DistData.BufferIdx;

				DataBuffer.Info[Idx].Code 			= DATA_READY;
				DataBuffer.Info[Idx].DevicePointer	= pDeviceActiv;
				DataBuffer.Info[Idx].FaultNumber	= pANL->DistHead.FaultNumber;
				DataBuffer.Info[Idx].DataLen		= pANL->DistHead.FaultNumber;

				ChannelDataLen	= 2 * (UDINT) pANL->DistHead.NrOfElementsPerChannel + sizeof(DisturbanceChannelInfo) + sizeof(DisturbanceChannelFaktor);

				DataBuffer.Info[Idx].DataLen		= sizeof(pANL->DistHead);
				DataBuffer.Info[Idx].DataLen	   += (UDINT) pANL->DistHead.NrOfTags * sizeof(DisturbanceTagInfo);
				DataBuffer.Info[Idx].DataLen	   += (UDINT) pANL->DistHead.NrOfStoredChannels * ChannelDataLen;
			}
			break;
	}
	return(error);
}

/****************************************************************************/
/*		       Disturbance data Response 			      					*/
/****************************************************************************/
void disturbance_data_response(AnlagenType *pANL,USINT code,USINT info)
{
	DeviceActivType *pDeviceActiv;
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	switch(code) {
		case ACK:
		case NAK:
			switch(pANL->DistData.Function) {
				case REQ_FOR_LIST_OF_FAULTS:				/* Störschriebliste anfordern */
				case SELECTION_OF_FAULT:					/* Störschrieb anfordern */
				case REQ_DISTURBANCE_TRANSMISSION:			/* Aufforderung Störschrieb übertragen*/
				case REQ_FOR_TAGS:							/* Aufforderung Tags übertragen*/
				case ACK_FOR_TAGS:							/* Bestätigung für Übertragene Tags */
				case ABORT_CHANNEL_TRANSMISSION:			/* Abbruch der Uebertragung eines Kanales */
				case REQ_FOR_A_CHANNEL:						/* Aufforderung zur Uebertragung eines Analogwertekanals */
				case ACK_FOR_A_CHANNEL:						/* Bestätigung für Übertragenen Kanal */
					pANL->DistData.WaitForResponse	= 0;
					pANL->DistData.DataSend			= 0;
					break;
				case ABORT_DISTURBANCE_TRANSMISSION:
				case ACK_FOR_DISTURBANCE_DATA:				/* Bestätigung für Übertragenen Störschrieb */
					pANL->DistData.WaitForResponse	= 0;
					pANL->DistData.DataSend			= 0;
					pANL->DistData.Function			= NO_FUNCTION;
					break;
			}
			break;
	}
}

/****************************************************************************/
/*		       Disturbance data Request 			      					*/
/****************************************************************************/
void disturbance_data_request(AnlagenType *pANL)
{
		unsigned char *c;

		ComType *pCom;
		DeviceActivType *pDeviceActiv;
		DisturbanceDataType *pDistData;
		DisturbanceHeadType *pDistHead;

		pCom			= &(pANL->com);
		pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);
		pDistData		= &(pANL->DistData);
		pDistHead		= &(pANL->DistHead);

		clear_buffer(pCom);

		switch(pDistData->Function) {

			case ABORT_DISTURBANCE_TRANSMISSION:
				pDistData->FunctionType 			= pDeviceActiv->FaultFunctionType;
				pDistData->TypeOfOrder				= 3;	/* TOO	Type of order: 3=abortion of disturbance data */
				pDistData->TypeOfValues				= 0;
				pDistData->ActualChannelTyp			= 0;
				break;

			case REQ_FOR_LIST_OF_FAULTS:		/* Aktuelle Stoerchriebliste anfordern */
				pDistData->FunctionType 			= pDeviceActiv->FaultFunctionType;
				pDistData->TypeOfOrder				= 24;	/* TOO	Type of order: 24=request for list of recorded disturbances iec 870-5-103 7.2.6.26  */
				pDistData->TypeOfValues				= 0;
				pDistData->ActualChannelTyp			= 0;
				pDistHead->FaultNumber				= 0;
				break;

			case SELECTION_OF_FAULT:			/* Störschrieb anfordern */
				pDistData->FunctionType 			= pDeviceActiv->FaultFunctionType;
				pDistData->TypeOfOrder				= 1;	/* TOO	Type of order: 1=Selection of fault, iec 870-5-103 7.2.6.26  */
				pDistData->TypeOfValues				= 0;
				pDistData->ActualChannelTyp			= 0;
				pDistData->NrOfReceivedTags			= 0;
				pDistData->NrOfReceivedChannels		= 0;
				pDistData->NrOfChannels				= 0;

				pDistHead->FaultNumber				= pDeviceActiv->FaultInfo[pDistData->FaultIdx].Number;
				pDistHead->Status					= pDeviceActiv->FaultInfo[pDistData->FaultIdx].Status;
				memcpy(&pDistHead->FaultTime,&pDeviceActiv->FaultInfo[pDistData->FaultIdx].IECTime,sizeof(pDistHead->FaultTime));
				pDistHead->NrOfTags					= 0;
				pDistHead->NrOfStoredChannels		= 0;
				pDistHead->NrOfElementsPerChannel	= 0;

				memcpy(&(pDistData->LastError),&(pDistData->ActError),sizeof(pDistData->LastError));

				pDistData->ActError.ErrorNr			= 0;
				pDistData->ActError.Function		= 0;
				pDistData->ActError.TypeOfOrder		= 0;
				pDistData->ActError.ChannelNr 		= 0;
				break;

			case REQ_DISTURBANCE_TRANSMISSION:		/* Aufforderung Störschrieb übertragen*/
				pDistHead->NrOfStoredChannels		= 0;
				pDistData->TypeOfOrder				= 2;	/* TOO	Type of order: 2=request for disturbance data, iec 870-5-103 7.2.6.26  */
				break;

			case REQ_FOR_TAGS:								/* Aufforderung Tags übertragen*/
				pDistData->TypeOfOrder				= 16;	/* TOO	Type of order: 16=request for tags, iec 870-5-103 7.2.6.26  */
				pDistData->NrOfReceivedTags			= 0;	/* Anzahl übertragener Tags */
				break;

			case ABORT_CHANNEL_TRANSMISSION:		/* Abbruch der Uebertragung eines Kanales */
				pDistData->TypeOfOrder				= 9;	/* TOO	Type of order: 3=abortion of disturbance data */
				break;

			case REQ_FOR_A_CHANNEL:							/* Aufforderung zur Uebertragung eines Analogwertekanals */
				pDistData->TypeOfOrder				= 8;	/* TOO	Type of order: 8=request for a channel, iec 870-5-103 7.2.6.26  */
				break;

			case ACK_FOR_TAGS:								/* Bestätigung für Übertragene Tags */
			case ACK_FOR_A_CHANNEL:							/* Bestätigung für Übertragenen Kanal */
			case ACK_FOR_DISTURBANCE_DATA:					/* Bestätigung für Übertragenen Störschrieb */
				break;
		}

		switch(pDistData->Function) {
			case ABORT_DISTURBANCE_TRANSMISSION:
			case REQ_FOR_LIST_OF_FAULTS:
			case SELECTION_OF_FAULT:					/* Störschrieb anfordern */
			case REQ_DISTURBANCE_TRANSMISSION:			/* Aufforderung Störschrieb übertragen*/
			case REQ_FOR_TAGS:							/* Aufforderung Tags übertragen*/
			case REQ_FOR_A_CHANNEL:						/* Aufforderung zur Uebertragung eines Analogwertekanals */
			case ABORT_CHANNEL_TRANSMISSION:			/* Abbruch der Uebertragung eines Kanales */

				/*ASDU 24: Order for disturbance data transmission*/

				pCom->WriteData[0] = 0x68;				/* Startbyte Langsatz */
				pCom->WriteData[1] = 13;				/* Länge              */
				pCom->WriteData[2] = 13;
				pCom->WriteData[3] = 0x68;

				if(!pCom->sendewiederholung)
					pDeviceActiv->FCB_Toggle ^= 1;

				pCom->WriteData[4] = 0x53 + (pDeviceActiv->FCB_Toggle << 5);

				/*             0%01x10011;	  	see iec 870-5-2 Page 35
				                     ----		Function		= 3 	user data
				                    -....		FCV				= 1
				                   -.....		FCB						Wert laut vorheriger Berechnung
				                  -......		PRM				= 1		Message from master
				*/


				pCom->WriteData[5]	= pDeviceActiv->Adresse;

				pCom->WriteData[6]	= 24;							/* Type Identification   					iec 870-5-103 page 31 	*/
				pCom->WriteData[7]	= 0x81;							/* Variable Structur Identifier					*/
				pCom->WriteData[8]	= 31;							/* Cause of Transmission 					iec 870-5-103 page 33 Table 6 */
				pCom->WriteData[9]	= pDeviceActiv->Adresse;
				pCom->WriteData[10] = pDistData->FunctionType;		/* Funktion Type         					iec 870-5-103 page 35 	*/
				pCom->WriteData[11] = 0;							/* not used	             					iec 870-5-103 page 37 	*/
				pCom->WriteData[12] = pDistData->TypeOfOrder;		/* TOO	Type of order (selection of fault)	iec 870-5-103 7.2.6.26  	*/
				pCom->WriteData[13]	= pDistData->TypeOfValues; 		/* TOV Type of disturbance values			iec 870-5-103 7.2.6.27	*/

				c = (unsigned char *) &pDistHead->FaultNumber;

				pCom->WriteData[14]	= *c;							/* FAN Fault Number UI16					iec 870-5-103 7.2.6.6	*/
				pCom->WriteData[15]	= *(c+1);
				pCom->WriteData[16]	= pDistData->ActualChannelTyp;	/* ACC Actual Channel UI8					iec 870-5-103 7.2.6.1 */

				pCom->WriteData[17] = MakeCRC(pCom->WriteData, 4, 13);
				pCom->WriteData[18] = 0x16;		/* Endezeichen */
				pCom->WriteData[19] = 0;
				pCom->WriteDataLen	= 19;
				break;


			case ACK_FOR_TAGS:							/* Bestätigung für Übertragene Tags */
			case ACK_FOR_A_CHANNEL:						/* Bestätigung für Übertragenen Kanal */
			case ACK_FOR_DISTURBANCE_DATA:				/* Bestätigung für Übertragenen Störschrieb */

				/*ASDU 25: Acknowledgement for disturbance data transmission*/

				pCom->WriteData[0] = 0x68;	/* Startbyte Langsatz */
				pCom->WriteData[1] = 13;		/* Länge              */
				pCom->WriteData[2] = 13;
				pCom->WriteData[3] = 0x68;

				if(!pCom->sendewiederholung)
					pDeviceActiv->FCB_Toggle ^= 1;

				pCom->WriteData[4] = 0x53 + (pDeviceActiv->FCB_Toggle << 5);

				/*             0%01x10011;	  	see iec 870-5-2 Page 35
				                     ----		Function		= 3 	user data
				                    -....		FCV				= 1
				                   -.....		FCB						Wert laut vorheriger Berechnung
				                  -......		PRM				= 1		Message from master
				*/

				pCom->WriteData[5]	= pDeviceActiv->Adresse;
				pCom->WriteData[6]	= 25;							/* Type Identification   				iec 870-5-103 page 31 	*/
				pCom->WriteData[7]	= 0x81;							/* Variable Structur Identifier					*/
				pCom->WriteData[8]	= 31;							/* Cause of Transmission 				iec 870-5-103 page 33 Table 6 */
				pCom->WriteData[9]	= pDeviceActiv->Adresse;
				pCom->WriteData[10] = pDistData->FunctionType;		/* Funktion Type         				iec 870-5-103 page 35 	*/
				pCom->WriteData[11] = 0;							/* not used	             				iec 870-5-103 page 37 	*/

				pCom->WriteData[12] = pDistData->TypeOfOrder;		/* TOO	Type of order 					iec 870-5-103 7.2.6.26  */
				pCom->WriteData[13]	= pDistData->TypeOfValues; 		/* TOV Type of disturbance values		iec 870-5-103 7.2.6.27	*/

				c = (unsigned char *) &pDistHead->FaultNumber;
				pCom->WriteData[14]	= *c;							/* FAN Fault Number UI16				iec 870-5-103 7.2.6.6	*/
				pCom->WriteData[15]	= *(c+1);

				pCom->WriteData[16]	= pDistData->ActualChannelTyp;	/* ACC Actual Channel UI8				iec 870-5-103 7.2.6.1 */

				pCom->WriteData[17] = MakeCRC(pCom->WriteData, 4, 13);
				pCom->WriteData[18] = 0x16;		/* Endezeichen */
				pCom->WriteData[19] = 0;
				pCom->WriteDataLen	= 19;
				break;
		}

		send_buffer(pCom,pANL->LinienNummer,pDeviceActiv->Adresse);
		pDistData->WaitForResponse	= 1;
		pDistData->DataSend			= 0;
		pCom->timeout				= 0;
		pCom->write_read_activ		= READ;

}
/****************************************************************************/
/*		Timeout beim Empfang von Stoerschriebtelegrammen					*/
/****************************************************************************/
void disturbance_data_timeout(AnlagenType *pANL)
{
	DeviceActivType 	*pDevice;
	DisturbanceDataType *pDistData;
	DisturbanceHeadType *pDistHead;


	pDistData	= &(pANL->DistData);
	pDistHead	= &(pANL->DistHead);
	pDevice		= &(pANL->DeviceActiv[0]);		/* Um Compiler Warnung zu verhindern */

	/* Verzoegerung fuer den Empfang von Stoerschrieben gleicher Stationen */
	if(pANL->DistData.Delay>CycleTime)
		pANL->DistData.Delay -= CycleTime;
	else
		pANL->DistData.Delay = 0;

	/* Timeoutzaehler */
	if(pANL->DistData.Function!=0) {
		pANL->DistData.TimeOut += CycleTime;
	}
	else {
		pANL->DistData.TimeOut = 0;
	}

	/* Timeoutzeit ueberschritten ? */
	if(pDistData->TimeOut>IEC_Config.DistDataTimeoutMax) {

		/* Gueltige Device Adresse ? */
		if(pDistData->DeviceAtWork==0) {
			clear_dist_data_request(pANL);
		}
		else {
			pDevice	= &(pANL->DeviceActiv[(pDistData->DeviceAtWork)-1]);
			switch(pDistData->Function) {
				case ABORT_DISTURBANCE_TRANSMISSION:
				case REQ_FOR_LIST_OF_FAULTS:
					/* Maximal 3 Versuche, dann wird die Anforderung geloescht und die Liste neu angefordert */
					pDistData->Trys++;
					if(pDistData->Trys>3) {
						save_disturbance_error(pANL,DIST_ERR_TIMEOUT, pDistData->Function, 0 ,(UINT) pDistData->ActualChannelTyp);
						pDistData->Function 		= NO_FUNCTION;
						pDistData->TimeOut			= 0;
						pDistData->Trys				= 0;
						pDistData->DataSend			= 0;
						pDistData->WaitForResponse	= 0;
					}
					else {
						pDistData->DataSend	= 1;
						pDistData->TimeOut	= 0;
					}
					break;


				case SELECTION_OF_FAULT:					/* Störschrieb anfordern */
				case REQ_DISTURBANCE_TRANSMISSION:			/* Aufforderung Störschrieb übertragen*/
				case REQ_FOR_TAGS:							/* Aufforderung Tags übertragen*/
				case REQ_FOR_A_CHANNEL:						/* Aufforderung zur Uebertragung eines Analogwertekanals */
				case ABORT_CHANNEL_TRANSMISSION:			/* Abbruch der Uebertragung eines Kanales */
				case ACK_FOR_TAGS:							/* Bestätigung für Übertragene Tags */
				case ACK_FOR_A_CHANNEL:						/* Bestätigung für Übertragenen Kanal */
				case ACK_FOR_DISTURBANCE_DATA:				/* Bestätigung für Übertragenen Störschrieb */

					/* Kein Stoerschrieb mehr vorhanden oder Nummer gleich 0 */
					if((pDevice->NrOfFaults==0) || (pDistHead->FaultNumber==0)) {
						clear_dist_data_request(pANL);
					}
					else {
						/* Maximal 3 Versuche, dann wird die Anforderung geloescht und die Liste neu angefordert */
						save_disturbance_error(pANL,DIST_ERR_TIMEOUT,pDistData->Function, 0 ,(UINT) pDistData->ActualChannelTyp);
						pDistData->Trys++;
						if(pDistData->Trys>3) {
							clear_dist_data_request(pANL);
							pDistData->Function = REQ_FOR_LIST_OF_FAULTS;
							pDistData->DataSend	= 1;
							pDistData->TimeOut	= 0;
							pDistData->Trys		= 0;
						}
						else {
							pDistData->DataSend	= 1;
							pDistData->TimeOut	= 0;
						}
					}
					break;
			}
		}
	}
}
/****************************************************************************/
/*		Fehlerinformation speichern											*/
/****************************************************************************/
void save_disturbance_error(AnlagenType *pANL,UINT Error, UINT Function, UINT TypeOfOrder, UINT Channel)
{
	if(pANL->DistData.ActError.ErrorNr==0) {
		pANL->DistData.ActError.ErrorNr		= Error;
		pANL->DistData.ActError.Function	= Function;
		pANL->DistData.ActError.TypeOfOrder	= TypeOfOrder;
		pANL->DistData.ActError.ChannelNr 	= Channel;
	}
}
/****************************************************************************/
/*		Stoerschriebuebertragung abbrechen									*/
/****************************************************************************/
void abort_disturbance_data(AnlagenType *pANL)
{
	pANL->DistData.Function = ABORT_DISTURBANCE_TRANSMISSION;
	pANL->DistData.TimeOut	= 0;
	pANL->DistData.Trys		= 0;
	pANL->DistData.DataSend	= 1;
}

/****************************************************************************/
/*		Buffer fuer Stoerschrieb freigeben, bei Abbruch der Uebertragung	*/
/****************************************************************************/
void clear_dist_data_request(AnlagenType *pANL)
{
	pANL->DistData.Function			= NO_FUNCTION;
	pANL->DistData.WaitForResponse	= 0;
	pANL->DistData.DataSend			= 0;

	/* Buffer freigeben */
	DataBuffer.Info[pANL->DistData.BufferIdx].Code = NODATA;
	pANL->DistData.Delay = IEC_Config.DisturbanceDelay;
}


/********************************************************************************/
/*		Pruefen, ob eine Stoerschrieb geloescht werden kann						*/
/********************************************************************************/
void check_device_dist_data(AnlagenType *pANL)
{
	unsigned short i,BufIdx;
	unsigned short Found;
	DeviceActivType *pDeviceActiv;
	DisturbanceDataType *pDistData;
	DisturbanceHeadType *pDistHead;

	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);
	pDistData		= &(pANL->DistData);
	pDistHead		= &(pANL->DistHead);


	if((pDeviceActiv->GI_Ok!=READY) || (pDeviceActiv->Enabled==0)) return;

	/* Pruefen, ob Stoerschriebe geloescht werden sollen */
	if(pDistData->Function==NO_FUNCTION) {
		for(i=0;i<=1;i++) {
			if(pDeviceActiv->FaultInfo[i].Clear) {
				pDistData->DeviceAtWork					= pANL->DeviceAtWork;
				pDistData->FunctionType					= pDeviceActiv->FaultFunctionType;
				pDistHead->FaultNumber					= pDeviceActiv->FaultInfo[i].Number;
				pDistData->TypeOfOrder					= DIST_DATA_TRANS_SUCCESSFULLY;
				pDistData->TypeOfValues					= pDeviceActiv->FaultInfo[i].TypeOfValues;
				pDistData->ActualChannelTyp				= 0;
				pDistData->LastChannelTyp				= 0;
				pDistData->Function						= ACK_FOR_DISTURBANCE_DATA;
				pDistData->DataSend						= 1;
				pDeviceActiv->FaultInfo[i].Clear		= 0;
				pDeviceActiv->FaultInfo[i].Transmitted	= SEND_DELETE_TELEGRAM;
				pDistData->Trys							= 0;
			}
			if(pDistData->Function!=NO_FUNCTION) break;
		}
	}

	if(pDistData->Function==NO_FUNCTION) {
	/* Uebertragung von Stoerschrieben einleiten:
		- Stoerschriebe sind im Geraet vorhanden
		- Stoerschriebe des Geraetes sind noch nicht uebertragen worden
		- Ein Buffer fuer den Stoerschrieb ist frei*/

		Found = 0;
		if(pDeviceActiv->NrOfFaults>0) {	/* Stoerschriebe sind im Geraet vorhanden */
			for(BufIdx=0;BufIdx<DataBuffer.NrOfBuffers;BufIdx++) {
				if((DataBuffer.Info[BufIdx].status==0) && (DataBuffer.Info[BufIdx].Code==NODATA)) Found = 1;
				if(Found) break;
			}
			if(Found) {
				if((pANL->DeviceAtWork!=pDistData->DeviceAtWork) || (pDistData->Delay==0)) {
					if((pDeviceActiv->FaultInfo[0].Transmitted==0) || (pDeviceActiv->FaultInfo[0].Transmitted==CLEAR_DATA_AGAIN)){
						DataBuffer.Info[BufIdx].Code	= WRITE_BUFFER;
						pDistData->DeviceAtWork			= pANL->DeviceAtWork;
						pDistData->BufferIdx			= BufIdx;
						pDistData->FaultIdx				= 0;
						pDistData->Function				= SELECTION_OF_FAULT;
						pDistData->DataSend				= 1;
						pDistData->Trys					= 0;
						pDistData->TimeOut				= 0;
					}
				}
			} /* if(Found) */
		} /* if(pDeviceActiv->NrOfFaults>0) */
	} /* if(pDistData->Function==0) */
}

/********************************************************************************/
/*		Simulation eines Stoerschriebes											*/
/********************************************************************************/
/* Uebergabe eines Stoerschriebes, der im Datenmodul dd_1 enthalten ist an dem Meldeserver */

void disturbance_simulation()
{
	BOOL Found;
	UINT BufIdx;

	if(DisturbanceSimulation) {
		DisturbanceSimulation = 0;
		Found = 0;
		for(BufIdx=0;BufIdx<DataBuffer.NrOfBuffers;BufIdx++) {
			if((DataBuffer.Info[BufIdx].status==0) && (DataBuffer.Info[BufIdx].Code==NODATA)) Found = 1;
			if(Found) break;
		}
		DOISimu.enable	= 1;
		DOISimu.pName	= (UDINT) "dd_1";
		DatObjInfo(&DOISimu);

		if(DOISimu.status==0) {
			DataBuffer.Info[BufIdx].Code 			= DATA_READY;
			DataBuffer.Info[BufIdx].DevicePointer	= &(stANL[0].DeviceActiv[0]);
			DataBuffer.Info[BufIdx].FaultNumber		= 0;
			DataBuffer.Info[BufIdx].DataLen			= DOISimu.len;
			DataBuffer.Info[BufIdx].DataAdr			= DOISimu.pDatObjMem;
		}
	}
}

/*** eof *****************************************************************************/


