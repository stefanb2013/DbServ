#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Protkollroutinen
*  Datei:    iec__103_sub.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 03.00.5
*  Datum:    07-02-19
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion:
*-------------------------------------------------------------------------------
*  Rev.   | Datum    | Name | Kommentar
*  -------+----------+------+---------------------------------------------------
*  01.00  | 26-11-03 | KOB  | Datei-Erstellung
*         |          |      |
*  01.06  | 10-01-07 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Type Identification 3 : Measurands 1
*         |          |      | (check_m3)
*         |          |      | Type Identification 79: Privates Telegramm zur Übertragung von Zählwerten
*         |          |      | (check_m79)
*  02.05.1| 02-09-08 | KOB  | KE_Init: Bei der Auswertungt des Rückmeldetelegrammes
*		  |			 |		| Identification Message (check_m5) wird die Überprüfung des
*		  |			 |		| Function Type weggelassen, d.h. es wird für alle Geräte aktzeptiert.
*		  |			 |		| Bisher wurde das Telegramm nur für Function Type 160,192,240 zugelassen.
*  02.05.2| 02-09-08 | KOB  | Funktionen wie unter 01.06 beschrieben eingefügt.
*         |          |      |
*  02.07.2| 26-07-11 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Type Identification 205: Privates Telegramm zur Übertragung von Zählwerten (Siemens)
*         |          |      | (check_m205, iec__103_sub.c)
*         |          |      | Auswertung der im privaten Telegramm empfangenen Zählwerte
*         |          |      | (save_counter_2, iec__103_serv.c)
*  02.07.3| 04-12-13 | KOB  | check_m5 angepasst: Zustand von ACD Bit ignoriert, Kompatibilitätsmodus 3 akzeptieren
*         |          |      | check_m6 nicht verwendete Bits der zurueckgemeldeten Zeit ausmaskieren
*  03.00.2| 28-01-14 | KOB  | check_m5 angepasst: Info Nr 2 akzeptieren
*  03.00.3| 04-04-14 | KOB  | check_m6() Uhrzeitdifferenz wird in Struktur eingetragen
*         |          |      | Ist die Uhrzeitdifferenz zu groß, wird erst beim 2.Mal Felhler generiert
*  03.00.4| 15-05-14 | WAI  | check_m6() angepasst (wday=0) wegen Inkompatibilität von DTStructure_TO_DT
*  03.00.5| 07-02-19 | KOB  | Data_write() Senden von Kommandos, Eingangsparameter Prüfung
******************************************************************************/

#include <bur/plctypes.h>
#include <sys_lib.h>		/* SYS_LIB */
#include <astime.h>
#include "iec__103_date.h"
#include "iec60870_5_103.h"		/* Definition der globalen Variablen */
#include "iec__103_debug.h"

extern DebugConfig_typ	DebugConfig;

/**********************************************************
*				Daten schreiben
**********************************************************/
short int send_buffer(ComType *pCom, UINT LinienNummer, USINT Adresse)
{
	#ifdef DEBUG
		dbg_msg(SENDBUFFER,
				(UDINT) pCom->WriteDataLen,
				0,
				LinienNummer,
				Adresse,
				0,
				0,
				0,
				(USINT *) &(pCom->WriteData));
	#endif

	FrameWriteStruct.ident 	= pCom->Ident;
	FrameWriteStruct.buffer = (UDINT) &(pCom->WriteData);
	FrameWriteStruct.buflng = (UINT) pCom->WriteDataLen;
	FrameWriteStruct.enable = 1;

	FRM_write(&FrameWriteStruct); 					/* write data to interface */

	if((FrameWriteStruct.status==0) && (pCom->WriteError!=0)) {
		pCom->LastWriteError	= pCom->WriteError;
	}

	pCom->WriteError = FrameWriteStruct.status; 	/* get status */

	if (pCom->WriteError == 0) { 	 				/* check status */
		pCom->WriteErrorCount	= 0;
		pCom->WriteError		= 0;
		return 0;
	}
	else {
		pCom->WriteErrorCount++;
		if(pCom->FirstWriteError==0) {
			pCom->FirstWriteError = pCom->WriteError;
		}
		return -1;
	}
}

/****************************************************************************/
/*		Zeitmerker der Devices decrementieren								*/
/****************************************************************************/
void Zeitmessung(AnlagenType *pANL)
{
	short n;

	for(n = 0; n <DEVICES_PRO_LINE; n++) {
		if(!pANL->DeviceTAB[n].Aktiv) continue;
		pANL->DeviceActiv[n].ZeitLetzteVerbindung += CycleTime;
		if(!pANL->DeviceActiv[n].ZeitsynchAktiv) {
			pANL->DeviceActiv[n].ZeitLetzteZeitsync   += CycleTime;
		}

		if(pANL->DeviceActiv[n].TimoutNext>CycleTime)
			pANL->DeviceActiv[n].TimoutNext -= CycleTime;
		else
			pANL->DeviceActiv[n].TimoutNext = 0;

		if(pANL->DeviceActiv[n].Class2Verz>CycleTime)
			pANL->DeviceActiv[n].Class2Verz -= CycleTime;
		else
			pANL->DeviceActiv[n].Class2Verz = 0;

		if(pANL->DeviceActiv[n].KE_InitOk == INIT_KE_LAEUFT)
			pANL->DeviceActiv[n].Timeout_KE += CycleTime;
		else
			pANL->DeviceActiv[n].Timeout_KE = 0;

		if((pANL->DeviceActiv[n].GI_Ok == INIT_GI_LAEUFT) || (pANL->DeviceActiv[n].GI_Ok == INIT_GI_START))
			pANL->DeviceActiv[n].Timeout_GI += CycleTime;
		else
			pANL->DeviceActiv[n].Timeout_GI = 0;


	}
}

/****************************************************************************/
/*		Ueberwachung, ob Zeitsynchronisation beendet wird					*/
/****************************************************************************/

void timesynch_timeout(AnlagenType *pANL)
{
	short n;
	DeviceActivType *pDeviceActiv;


	for(n = 0; n <DEVICES_PRO_LINE; n++) {
		if(!pANL->DeviceTAB[n].Aktiv) continue;
//		if(pANL->DeviceAtWork!=n) continue;

		pDeviceActiv = &(pANL->DeviceActiv[n]);

		if(pDeviceActiv->ZeitsynchAktiv)
			pDeviceActiv->TimeoutTimeSynch += CycleTime;
		else
			pDeviceActiv->TimeoutTimeSynch = 0;

		if(pDeviceActiv->TimeoutTimeSynch > IEC_Config.TimeOutTimeSynch) {
			pDeviceActiv->ZeitsynchAktiv	= 0;
			pDeviceActiv->ZeitLetzteZeitsync= 0;

			if(pDeviceActiv->StateTimeSynchError != STATE_ERROR_ON) {
				save_TimeSynchError( &(pANL->dm),				/* Adresse Datenmodul-Infostruktur */
										pANL->LinienNummer,			/* Nummer der Linie */
										pDeviceActiv,				/* Zeiger auf aktives Geraet */
						 				STATE_ERROR_ON,				/* Messwert		(Doppelmeldung 1=aus 2=ein)	 */
		 								&RPSTime,					/* Zeitstempel 		 */
						 				TRUE);						/* Event für Leitsystem */

				pDeviceActiv->StateTimeSynchError = STATE_ERROR_ON;
			}
		}
	}
}

/****************************************************************************/
/*		Devices nach 1 min reaktivieren      								*/
/****************************************************************************/
void Device_Reaktivieren(AnlagenType *pANL)
{
	short n;

	for(n = 0; n < DEVICES_PRO_LINE; n++) {
		if(!pANL->DeviceTAB[n].Aktiv) continue;
		if(pANL->DeviceActiv[n].ZeitLetzteVerbindung > IEC_Config.WaitIntv) {
			if(!pANL->DeviceActiv[n].Enabled) {
				pANL->DeviceActiv[n].Enabled				= 1;
				pANL->DeviceActiv[n].Adresse				= pANL->DeviceTAB[n].Adresse;
				pANL->DeviceActiv[n].Verbindungsversuche	= 0;
				pANL->DeviceActiv[n].ZeitsynchAktiv			= 0;
				pANL->DeviceActiv[n].FCB_Toggle				= 0;
				pANL->DeviceActiv[n].KE_InitOk				= 0;
				pANL->DeviceActiv[n].ZeitLetzteZeitsync		= 0;
				pANL->DeviceActiv[n].Class1Request			= 0;
				pANL->DeviceActiv[n].TimoutNext				= 0;
				pANL->DeviceActiv[n].GI_Ok					= 0;
				pANL->DeviceActiv[n].LetzterACD				= 0;
				pANL->DeviceActiv[n].Class2Verz				= 0;
				pANL->DeviceActiv[n].Class1Verz				= 0;
				pANL->DeviceActiv[n].set_event				= 0;
				pANL->DeviceActiv[n].Timeout_KE 			= 0;
				pANL->DeviceActiv[n].Timeout_GI 			= 0;
			}

			pANL->DeviceActiv[n].ZeitLetzteVerbindung	= 0;
			if(!pANL->DeviceAtWork) {
				pANL->DeviceAtWork = n + 1;
			}
		}

		if(pANL->DeviceActiv[n].Enabled) {
			if(pANL->DeviceActiv[n].Timeout_KE > IEC_Config.WaitIntv) {
				pANL->DeviceActiv[n].KE_InitOk	= 0;
				pANL->DeviceActiv[n].GI_Ok		= 0;
				pANL->DeviceActiv[n].Timeout_KE = 0;
				pANL->DeviceActiv[n].Timeout_GI = 0;
			}

			if(pANL->DeviceActiv[n].Timeout_GI > IEC_Config.WaitIntv) {
				pANL->DeviceActiv[n].GI_Ok		= 0;
				pANL->DeviceActiv[n].Timeout_GI = 0;
			}
		}
	}
}

/****************************************************************************/
/*		Empfangsbuffer löschen			     								*/
/****************************************************************************/
void clear_buffer(ComType *pCom)
{
	pCom->RBufferAdr	= 0;
	memset(pCom->ReadData, 0, sizeof(pCom->ReadData));
}

/****************************************************************************/
/*		Daten zum Device senden			     								*/
/****************************************************************************/
USINT Data_write(AnlagenType *pANL)
{
	DeviceActivType *pDeviceActiv;
	BOOL	tel_send=0;
	UINT	CommandError;
	
	pDeviceActiv = &(pANL->DeviceActiv[0]);		/* nur um Compiler-Warnung zu verhindern */

	if(!pANL->DeviceAtWork) return 1;			/* nix aktiv im Moment */


	pDeviceActiv = &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	/* Telegrammverzoegerung zu einer Station, fuer Debugzwecke */
#ifdef DEBUG
	if(DebugConfig.DebugDelayCounter>CycleTime)
		DebugConfig.DebugDelayCounter -= CycleTime;
	else
		DebugConfig.DebugDelayCounter = 0;
	if( ((pDeviceActiv->Adresse==DebugConfig.DebugDelayAdresse) || (DebugConfig.DebugDelayAdresse==0)) && (DebugConfig.DebugDelayCounter>0)) return(1);
	DebugConfig.DebugDelayCounter = DebugConfig.DebugDelayTime;
#endif


	check_device_dist_data(pANL);				/* Pruefen,ob Stoerschriebdaten geladen oder geloescht werden koennen */


	/* Wiederholung des letzten Telegrammes */

	if(pANL->com.sendewiederholung) {
		clear_buffer(&(pANL->com));

		send_buffer(&(pANL->com),pANL->LinienNummer,pDeviceActiv->Adresse);

		pANL->com.timeout			= 0;
		pANL->com.sendewiederholung = 0;
		pANL->com.write_read_activ	= READ;
		tel_send = 1;
	}

	/* Normaler Telegrammverkehr */

	/* Reset Kommunikation */
	if(!tel_send) {
		if(pDeviceActiv->KE_InitOk == FALSE){
#ifdef DEBUG
			dbg_msg(MESSAGE,0,0,pANL->LinienNummer, pDeviceActiv->Adresse , 0, 0, 0, 0);
#endif
			Reset_KE(pANL->LinienNummer, pDeviceActiv, &(pANL->com));
			tel_send = 1;
		}
	}


	/* Start Generalabrage */
	if((!tel_send) && (pDeviceActiv->KE_InitOk == READY)) {
		if((pDeviceActiv->GI_Ok == FALSE) ) {
#ifdef DEBUG
			dbg_msg(MESSAGE,3,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
#endif
			Init_GI(pANL->LinienNummer, pDeviceActiv, &(pANL->com));
			tel_send = 1;
		}
	}

	/* Zeit Synchronisation */
	if((!tel_send) && (pDeviceActiv->KE_InitOk == READY)) {
		if((pDeviceActiv->ZeitLetzteZeitsync > IEC_Config.TimeTS) && (pANL->DeviceTAB[pANL->DeviceAtWork-1].TimeSynch)) {
			#ifdef DEBUG
				dbg_msg(MESSAGE,2,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
			#endif
			if(!pDeviceActiv->ZeitsynchAktiv) {
				Zeit_Sync(pANL->LinienNummer, pDeviceActiv, &(pANL->com));
				tel_send = 1;
			}
		}
	}

	/* Kommando senden */
	if(!tel_send) {

		if(pANL->Command.DataSend) {
			CommandError = 0;

			// Kommando Senden freigegeben ?
			if(IEC_Config.CommandEnable == 0) {
				CommandError = COMMAND_ERR_DISABLED;
			}
				// Kommando Senden bereits aktiv ?
			else if(pANL->Command.Function != NO_FUNCTION) {
				CommandError = COMMAND_ERR_FUNCTION_ACTIVE;
			}
				// Geräteadresse korrekt ?
			else if(DeviceIsActive(pANL, pANL->Command.DeviceAdress) == FALSE) {
				CommandError	= COMMAND_ERR_DEVICE_ADR;
			}
			
			if(CommandError != 0) {
				pANL->Command.DataSend	= FALSE;
				pANL->Command.Error		= CommandError;
				pANL->Command.Ready 	= TRUE;
			}
			else {
				// Zugriff über Geräte Adresse
				if(pANL->Command.DeviceAdress == pDeviceActiv->Adresse) {
					// Besteht Verbindung zu Gerät  ?
					if(pDeviceActiv->KE_InitOk == READY) {
						pANL->Command.DeviceAtWork = pANL->DeviceAtWork;
						#ifdef DEBUG
							dbg_msg(MESSAGE,11,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
						#endif
						command_request(pANL);
						tel_send = 1;
					}
					else {
						pANL->Command.DataSend	= FALSE;
						pANL->Command.Error 	= COMMAND_ERR_DEVICE_NOT_READY;
						pANL->Command.Ready 	= TRUE;
					}
				}	// if(pANL->Command.DeviceAdress == pDeviceActiv->Adresse) {
			}	// if(CommandError != 0) {
		}	// if(pANL->Command.DataSend) {
	}	// if(!tel_send) 
	

			//	if((!tel_send) && (pDeviceActiv->KE_InitOk == READY)) {
			//		if((pANL->Command.DataSend) && (pANL->DeviceAtWork==pANL->Command.DeviceAtWork) && IEC_Config.CommandEnable) {
			//			#ifdef DEBUG
			//				dbg_msg(MESSAGE,11,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
			//			#endif
			//			command_request(pANL);
			//			tel_send = 1;
			//		}
			//	}

			/* Anforderung Störschriebdaten */
			if((!tel_send) && (pDeviceActiv->KE_InitOk == READY)) {
				if((pANL->DistData.DataSend) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork)) {
					#ifdef DEBUG
						dbg_msg(MESSAGE,9,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
					#endif
					disturbance_data_request(pANL);
					tel_send = 1;
				}
			}


			/* Class 1 Anforderung */
			if(!tel_send) {
				if((pDeviceActiv->LetzterACD == TRUE) || (pDeviceActiv->Class1Verz>5)) {
					#ifdef DEBUG
						dbg_msg(MESSAGE,5,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
					#endif
					Class_1_Request(pANL->LinienNummer, pDeviceActiv, &(pANL->com));
					tel_send = 1;
					pDeviceActiv->Class1Verz = 0;
				}
			}

			/* Class 2 Anforderung */
			if(!tel_send) {
				if(pDeviceActiv->Class2Verz == 0) {
					#ifdef DEBUG
						dbg_msg(MESSAGE,6,0,pANL->LinienNummer,pDeviceActiv->Adresse , 0, 0, 0, 0);
					#endif
					Class_2_Request(pANL->LinienNummer, pDeviceActiv, &(pANL->com));
					if((pDeviceActiv->GI_Ok!=READY) || ((pANL->DistData.Function!=0) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork)) || (pDeviceActiv->ZeitsynchAktiv)) {
						pDeviceActiv->Class2Verz = IEC_Config.TimeClass2PollFast;
					}
					else {
						pDeviceActiv->Class2Verz = IEC_Config.TimeClass2Poll;
					}
					/*			pDeviceActiv->Class1Verz++;*/
					tel_send = 1;
				}
			}

			/* Keine Aktion: Nächstes Gerät auswählen */
			if(!tel_send) {
				GetNewDevice(pANL);
				return 2;
			}

			return 0;
		}

/****************************************************************************/
/*		                      			      								*/
/****************************************************************************/
void Status_Of_Link(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom)
{
	clear_buffer(pCom);

	pCom->WriteData[0] = 0x10;	/* Startbyte Kurzsatz */
	pCom->WriteData[1] = 0x49;

/*             0%01000000;	  	see iec 870-5-2 Page 35
	                     ----		Function		= 0		Reset of remot link
    	               --....		FCB & FCV		= 0
        	          -......		PRM				= 1		Message from master
*/
	pCom->WriteData[2] = pDeviceActiv->Adresse;
	pCom->WriteData[3] = MakeCRC(pCom->WriteData, 1, 2);
	pCom->WriteData[4] = 0x16;	/* Startbyte Langsatz */
	pCom->WriteData[5] = 0;
	pCom->WriteDataLen = 5;

	send_buffer(pCom,LinienNummer,pDeviceActiv->Adresse);
	pCom->timeout			= 0;
	pCom->write_read_activ	= READ;

	if(IEC_Info_Enable) dbg_buf_iec_short(WRITE, (UDINT) &pCom->WriteData, pCom->WriteDataLen);
}


/****************************************************************************/
/*		                      			      								*/
/****************************************************************************/
void Reset_KE(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom)
{
	clear_buffer(pCom);

	pCom->WriteData[0] = 0x10;	/* Startbyte Kurzsatz */
	pCom->WriteData[1] = 0x40;

/*             0%01000000;	  	see iec 870-5-2 Page 35
	                     ----		Function		= 0		Reset of remot link
    	               --....		FCB & FCV		= 0
        	          -......		PRM				= 1		Message from master
*/
	pCom->WriteData[2] = pDeviceActiv->Adresse;
	pCom->WriteData[3] = MakeCRC(pCom->WriteData, 1, 2);
	pCom->WriteData[4] = 0x16;	/* Startbyte Langsatz */
	pCom->WriteData[5] = 0;
	pCom->WriteDataLen = 5;

	send_buffer(pCom,LinienNummer,pDeviceActiv->Adresse);
	pCom->timeout			= 0;
	pCom->write_read_activ	= READ;

	if(IEC_Info_Enable) dbg_buf_iec_short(WRITE, (UDINT) &pCom->WriteData, pCom->WriteDataLen);

}


/****************************************************************************/
/*		   Zeitsynchronisation			      								*/
/****************************************************************************/
void Zeit_Sync(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom)
{

	unsigned short milliseconds;

	clear_buffer(pCom);

	pCom->WriteData[0] = 0x68;	/* Startbyte Langsatz */
	pCom->WriteData[1] = 15;		/* Länge              */
	pCom->WriteData[2] = 15;
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
	pCom->WriteData[6]	= 6;		/* Type Identification   iec 870-5-103 page 31 	*/
	pCom->WriteData[7]	= 0x81;		/* Variable Structur Identifier					*/
	pCom->WriteData[8]	= 8;		/* Cause of Transmission iec 870-5-103 page 33 Table 6 */
	pCom->WriteData[9]	= pDeviceActiv->Adresse;
	pCom->WriteData[10] = 0xff;		/* Funktion Type         iec 870-5-103 page 35 	*/
	pCom->WriteData[11] = 0;		/* Inform. Nr.           iec 870-5-103 page 37 	*/


	milliseconds 		= RPSTime.second * 1000 + RPSTime.millisec;

	pCom->WriteData[12] = (USINT) (milliseconds & 0x00FF);				/*  870-5-4 page 33 */
	pCom->WriteData[13] = (USINT) (milliseconds / 0x0100);

	pCom->WriteData[14] = RPSTime.minute;
	pCom->WriteData[15] = RPSTime.hour;
	pCom->WriteData[16] = RPSTime.day;
	pCom->WriteData[17] = RPSTime.month;
	pCom->WriteData[18] = RPSTime.year - 2000;
	pCom->WriteData[19] = MakeCRC(pCom->WriteData, 4, 15);
	pCom->WriteData[20] = 0x16;	/* Endezeichen */
	pCom->WriteData[21] = 0;
	pCom->WriteDataLen	= 21;

	pDeviceActiv->ZeitsynchAktiv = 1;

	/* Gesendete Zeit speichern */
	pDeviceActiv->TSynch.year 		= RPSTime.year;
	pDeviceActiv->TSynch.month		= RPSTime.month;
	pDeviceActiv->TSynch.day		= RPSTime.day;
	pDeviceActiv->TSynch.wday		= RPSTime.reserve;
	pDeviceActiv->TSynch.hour		= RPSTime.hour;
	pDeviceActiv->TSynch.minute		= RPSTime.minute;
	pDeviceActiv->TSynch.second		= RPSTime.second;
	pDeviceActiv->TSynch.millisec	= RPSTime.millisec;
	pDeviceActiv->TSynch.microsec	= 0;

	send_buffer(pCom,LinienNummer,pDeviceActiv->Adresse);
	pCom->timeout			= 0;
	pCom->write_read_activ	= READ;

	if(IEC_Info_Enable) dbg_buf_iec_long(WRITE, (UDINT) &pCom->WriteData, pCom->WriteDataLen);
}

/****************************************************************************/
/*		     Generalabfrage GI starten      								*/
/****************************************************************************/
void Init_GI(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom)
{
	clear_buffer(pCom);

	pCom->WriteData[0] = 0x68;	/* Startbyte Langsatz */
	pCom->WriteData[1] = 9;		/* Länge              */
	pCom->WriteData[2] = 9;
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
	pCom->WriteData[6]	= 7;		/* Type Identification   iec 870-5-103 page 31 	*/
	pCom->WriteData[7]	= 0x81;		/* Variable Structur Identifier					*/
	pCom->WriteData[8]	= 9;		/* Cause of Transmission iec 870-5-103 page 33 Table 6 */
	pCom->WriteData[9]	= pDeviceActiv->Adresse;
	pCom->WriteData[10] = 0xff;		/* Funktion Type         iec 870-5-103 page 35 	*/
	pCom->WriteData[11] = 0;		/* Inform. Nr.           iec 870-5-103 page 37 	*/
	pCom->WriteData[12] = 0x39;		/* Scan Number			 iec 870-5-103 page 51 */
	pCom->WriteData[13] = MakeCRC(pCom->WriteData, 4, 9);
	pCom->WriteData[14] = 0x16;		/* Endezeichen */
	pCom->WriteData[15] = 0;
	pCom->WriteDataLen	= 15;

	pDeviceActiv->GI_Ok = INIT_GI_START;

	send_buffer(pCom,LinienNummer,pDeviceActiv->Adresse);
	pCom->timeout			= 0;
	pCom->write_read_activ	= READ;

	if(IEC_Info_Enable) dbg_buf_iec_long(WRITE, (UDINT) &pCom->WriteData, pCom->WriteDataLen);
}

/****************************************************************************/
/*		       Class 1 Request 			      								*/
/****************************************************************************/
void Class_1_Request(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom)
{
	clear_buffer(pCom);
	pCom->WriteData[0] = 0x10;	/* Startbyte Kurzsatz */

	if(!pCom->sendewiederholung)
		pDeviceActiv->FCB_Toggle ^= 1;

	pCom->WriteData[1] = 0x5A + (pDeviceActiv->FCB_Toggle << 5);

/*             0%01x11010;	  	see iec 870-5-2 Page 35
                     ----		Function		= 10	Request user data class 1
                    -....		FCV				= 1
                   -.....		FCB						Wert laut vorheriger Berechnung
                  -......		PRM				= 1		Message from master
*/
	pCom->WriteData[2] = pDeviceActiv->Adresse;
	pCom->WriteData[3] = MakeCRC(pCom->WriteData, 1, 2);
	pCom->WriteData[4] = 0x16;	/* Startbyte Langsatz */
	pCom->WriteData[5] = 0;
	pCom->WriteDataLen	= 5;

	send_buffer(pCom,LinienNummer,pDeviceActiv->Adresse);
	pCom->timeout			= 0;
	pCom->write_read_activ	= READ;

	if(IEC_Info_Enable) dbg_buf_iec_short(WRITE, (UDINT) &pCom->WriteData, pCom->WriteDataLen);
}

/****************************************************************************/
/*		       Class 2 Request 			      								*/
/****************************************************************************/

void Class_2_Request(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom)
{
	clear_buffer(pCom);
	pCom->WriteData[0] = 0x10;	/* Startbyte Kurzsatz */

	if(!pCom->sendewiederholung)
		pDeviceActiv->FCB_Toggle ^= 1;

	pCom->WriteData[1] = 0x5B + (pDeviceActiv->FCB_Toggle << 5);

/*             0%01x11011;	  	see iec 870-5-2 Page 35
                     ----		Function		= 11	Request user data class 2
                    -....		FCV				= 1
                   -.....		FCB						Wert laut vorheriger Berechnung
                  -......		PRM				= 1		Message from master
*/

	pCom->WriteData[2]	= pDeviceActiv->Adresse;
	pCom->WriteData[3]	= MakeCRC(pCom->WriteData, 1, 2);
	pCom->WriteData[4]	= 0x16;	/* Startbyte Langsatz */
	pCom->WriteData[5]	= 0;
	pCom->WriteDataLen	= 5;

	send_buffer(pCom,LinienNummer,pDeviceActiv->Adresse);
	pCom->timeout			= 0;
	pCom->write_read_activ	= READ;

	if(IEC_Info_Enable) dbg_buf_iec_short(WRITE, (UDINT) &pCom->WriteData, pCom->WriteDataLen);
}

/****************************************************************************/
/*		Daten vom Device lesen			      								*/
/****************************************************************************/

void Data_read(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	pCom->timeout += CycleTime;

	if(pCom->timeout > pCom->receive_timeout) {	/* Timeout eingetreten */
		pDeviceActiv->Timeouts++;
		Empfangsfehler(0xFFFF,pANL);
	}
	else if(pCom->RBufferAdr > 0) {

		switch(pCom->ReadData[0]) {

			case 0xE5:			/* ACK */
				pCom->write_read_activ	= WRITE;
				pCom->sendewiederholung = 0;
				pDeviceActiv->Verbindungsversuche	= 0;
				pDeviceActiv->ZeitLetzteVerbindung	= 0;

				/* Meldung an Leitsystem, Verbindung wieder in Ordnung */
				if(pDeviceActiv->VerbindungsStoerung == 1) {
					save_kopplungsstoerung( &(pANL->dm),				/* Adresse Datenmodul-Infostruktur */
											pANL->LinienNummer,			/* Nummer der Linie */
											pDeviceActiv,				/* Zeiger auf aktives Geraet */
							 				1,							/* Messwert		(Doppelmeldung 1=aus 2=ein)	 */
			 								&RPSTime,					/* Zeitstempel 		 */
							 				TRUE);						/* Event für Leitsystem */
					pDeviceActiv->VerbindungsStoerung = 0;
				}

				/* Sendeauftrag Stoerschriebdaten loeschen */
				if((pANL->DistData.WaitForResponse) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork)) {
					disturbance_data_response(pANL,ACK,0);
				}
				/* Wai: wegen ABB anderst antworten */
				if(pDeviceActiv->KE_InitOk==FALSE)
					pDeviceActiv->KE_InitOk = INIT_KE_LAEUFT;

				if(pDeviceActiv->GI_Ok == INIT_GI_START)
					pDeviceActiv->GI_Ok = INIT_GI_LAEUFT;


				if(IEC_Info_Enable) dbg_buf_iec_short_ack(READ, (UDINT) &pCom->ReadData, 1);

				break;

			case 0x10:			/* Frame Auswertung Kurzsatz */
								/* see IEC 870-5-2 page 35	 */
				if(pCom->RBufferAdr >= 5) {
					if(pCom->ReadData[4] != 0x16)
						Empfangsfehler(1,pANL);
					else if(MakeCRC(pCom->ReadData, 1, 2) != pCom->ReadData[3])
						Empfangsfehler(2,pANL);
					else if(pCom->ReadData[2] != pDeviceActiv->Adresse)
						Empfangsfehler(3,pANL);
					else if(pCom->ReadData[1] & 0x40)		/* Message nicht von einem Slave */
						Empfangsfehler(4,pANL);
					else {

						if(IEC_Info_Enable) dbg_buf_iec_short(READ, (UDINT) &pCom->ReadData, pCom->RBufferAdr);

						/* Control field IEC 870-5-2 page 35 */
						/* access demand for class 1 data transmission  */
						if(pCom->ReadData[1] & 0x20)	ControlFieldPrimary.ACD = 1;
						else							ControlFieldPrimary.ACD = 0;

						/* data flow control */
						if(pCom->ReadData[1] & 0x10)	ControlFieldPrimary.DFC = 1;
						else							ControlFieldPrimary.DFC = 0;

						/* function code of control field 5.1.2 */
						ControlFieldPrimary.Function	= pCom->ReadData[1] & 0x0F;


						pDeviceActiv->LetzterACD		= ControlFieldPrimary.ACD;
						pCom->class1_res_adc1			= ControlFieldPrimary.ACD;


						if(ControlFieldPrimary.Function==0)	{											/* ACK */
							if(pDeviceActiv->KE_InitOk == FALSE) {
								pDeviceActiv->KE_InitOk = INIT_KE_LAEUFT;
							}
							if(pDeviceActiv->GI_Ok == INIT_GI_START) {
								pDeviceActiv->GI_Ok = INIT_GI_LAEUFT;
							}
						}


						if((ControlFieldPrimary.Function==9) || (ControlFieldPrimary.Function==1)) {			/* NACK */
							/* Falls Start der GI Abfrage nicht akzeptiert wird, dann Reset der Kommunikation */
							if(pDeviceActiv->GI_Ok == INIT_GI_START) {
								pDeviceActiv->GI_Ok		= FALSE;
								pDeviceActiv->KE_InitOk = FALSE;
							}
						}

						/* Sendeauftrag Stoerschriebdaten loeschen */
						if((pANL->DistData.WaitForResponse) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork)) {
							if(ControlFieldPrimary.Function==0)	{											/* ACK */
								disturbance_data_response(pANL,ACK,0);
							}
							if((ControlFieldPrimary.Function==9) || (ControlFieldPrimary.Function==1)) {	/* NACK */
								disturbance_data_response(pANL,NAK,0);
							}
						}

						pDeviceActiv->Verbindungsversuche	= 0;
						pDeviceActiv->ZeitLetzteVerbindung	= 0;
						pCom->write_read_activ				= WRITE;
						pCom->sendewiederholung				= 0;


						/* Meldung an Leitsystem, Verbindung wieder in Ordnung */
						if(pDeviceActiv->VerbindungsStoerung == 1) {
							save_kopplungsstoerung( &(pANL->dm),				/* Adresse Datenmodul-Infostruktur */
													pANL->LinienNummer,			/* Nummer der Linie */
													pDeviceActiv,				/* Zeiger auf aktives Geraet */
										 			1,							/* Messwert (Doppelmeldung 1=aus 2=ein) */
					 								&RPSTime,					/* Zeitstempel 		 */
									 				TRUE);						/* Event für Leitsystem */
							pDeviceActiv->VerbindungsStoerung = 0;
						}

						/* Weiter zu naechstem Device */
						if((ControlFieldPrimary.ACD) || ((ControlFieldPrimary.Function==9) || (ControlFieldPrimary.Function==1)) )
							GetNewDevice(pANL);
					}
				}
				break;

			case 0x68:			/* Frame Auswertung Langsatz */
									/* see IEC 870-5-2 page 19	 */
				if(pCom->RBufferAdr < 4)
					break;						/* 68 L L 68 --> Erste 4 Byte eines Langsatzes (L = Länge) */
				if(pCom->ReadData[3] != 0x68) {
					Empfangsfehler(5,pANL);
					break;
					}
				if(pCom->RBufferAdr < (UINT)(pCom->ReadData[1] + 6))	/* Lange + 4 Byte Header + CRC + Schlußbyte */
					break;
				if(pCom->ReadData[5] != pDeviceActiv->Adresse) {
					Empfangsfehler(6,pANL);
					break;
				}
				if(pCom->ReadData[4] & 0x40) {		/* Message nicht von einem Slave */
					Empfangsfehler(4,pANL);
					break;
				}
				if((pCom->ReadData[4] & 0x0f) != 8) {	/* Function = 8 (user data) */
					Empfangsfehler(7,pANL);
					break;
					}
				if(MakeCRC(pCom->ReadData, 4, pCom->ReadData[1]) != pCom->ReadData[4 + pCom->ReadData[1]]) {
					Empfangsfehler(8,pANL);			/* CRC-Fehler */
					break;
					}
				if(pCom->ReadData[5 + pCom->ReadData[1]] != 0x16) {
					Empfangsfehler(9,pANL);			/* Endebyte falsch */
					break;
					}
				else {

					if(IEC_Info_Enable) dbg_buf_iec_long(READ, (UDINT) &pCom->ReadData, pCom->RBufferAdr);

					/* Control field IEC 870-5-2 page 35 */
					/* access demand for class 1 data transmission  */
					if(pCom->ReadData[4] & 0x20)	ControlFieldPrimary.ACD = 1;
					else							ControlFieldPrimary.ACD = 0;

					/* data flow control */
					if(pCom->ReadData[4] & 0x10)	ControlFieldPrimary.DFC = 1;
					else							ControlFieldPrimary.DFC = 0;
					/* function code of control field 5.1.2 */
					ControlFieldPrimary.Function	= pCom->ReadData[4] & 0x0F;
					pCom->class1_res_adc1			= ControlFieldPrimary.ACD;

					/* Cause of transmission IEC 60870-5-103 page 33*/
					/* - Generalabfrage: Timeout fuer Generalabfrage loeschen */

					if(pCom->ReadData[8] == 9) {
						pDeviceActiv->Timeout_GI = 0;
					}

					/* Type identification */

					switch(pCom->ReadData[6]) {
						case 1:							/* Time-tagged message 7.3.1.1 					*/
							if(check_m1(pANL) == 0)		/* Auswertung Type Identifikation 1 			*/
									tel_ok(pANL);
								break;
							case 2:							/* Time-tagged message with rel. time 7.3.1.2	*/
								if(check_m2(pANL) == 0)		/* Auswertung Type Identifikation 2 			*/
									tel_ok(pANL);
								break;
							case 3:							/* Measurands 1 7.3.1.3                         */
								if(check_m3(pANL) == 0)		/* Auswertung Type Identifikation 3 			*/
									tel_ok(pANL);
								break;
		/*	gibts nicht		case 4:							   Time-tagged message with rel. time 7.3.1.4	*/
		/*							if(check_m4(pANL) == 0)		   Auswertung Type Identifikation 4 			*/
		/*							tel_ok(pANL);
								break;																	*/
							case 5:							/* Identification Message 7.3.1.5	            */
								if(check_m5(pANL) == 0)		/* Auswertung Type Identifikation 5 			*/
									tel_ok(pANL);
								break;
							case 6:							/* Time Synchronistion    7.3.1.6	            */
								if(check_m6(pANL) == 0)		/* Auswertung Type Identifikation 6 			*/
									tel_ok(pANL);
								break;
							case 8:							/* Termination of general interrogation 7.3.1.7 */
								if(check_m8(pANL) == 0)		/* Auswertung Type Identifikation 8 			*/
									tel_ok(pANL);
								break;
							case 9:							/* Measurands 2 7.3.1.8							*/
								if(check_m9(pANL) == 0)		/* Auswertung Type Identifikation 9 			*/
									tel_ok(pANL);
								break;
							case 23:
								if(check_m23(pANL)==0)		/* List of recorded dirturbances  7.3.1.11		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 23			*/
								break;
							case 26:
								if(check_m26(pANL)==0)		/* Ready for transmission of disturbance date  7.3.1.12		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 26			*/
								break;
							case 27:
								if(check_m27(pANL)==0)		/* Ready for transmission of a channel 7.3.1.13		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 27			*/
								break;
							case 28:
								if(check_m28(pANL)==0)		/* Ready for transmission of a tag 7.3.1.14		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 28			*/
								break;
							case 29:
								if(check_m29(pANL)==0)		/* Transmission of tags  7.3.1.15		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 29			*/
								break;
							case 30:
								if(check_m30(pANL)==0)		/* Transmission of disturbance values  7.3.1.16		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 30			*/
								break;
							case 31:
								if(check_m31(pANL)==0)		/* End of transmission 7.3.1.17		*/
									tel_ok(pANL);			/* Auswertung Type Identifikation 31			*/
								break;
							case 79:
								if(check_m79(pANL)==0)		/* Privates Telegramm zur Übertragung von Zählwerten */
									tel_ok(pANL);			/* */
								break;
							case 205:
								if(check_m205(pANL)==0)		/* Privates Telegramm zur Übertragung von Zählwerten (Siemens) */
									tel_ok(pANL);			/* */
								break;
							default:
								tel_ok(pANL);				/* Alle anderen Telegramme werden nicht benötigt */
								break;
					}
				}
				break;
			default:			/* Fehler */
				break;
		}
	}
}

/****************************************************************************/
/*		Time-tagged message 7.3.1.1		      								*/
/****************************************************************************/
USINT check_m1(AnlagenType *pANL)
{

	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	SQ  .  .  .  nr nr nr nr
				 ++ ++ ++ ++----------: number of information elements
				 						hier 1
	++--------------------------------: SQ-Bit	1 = feste Länge
												0 = variable Länge	*/
	if(pCom->ReadData[7] != 0x81) {
		Empfangsfehler(11,pANL);
		return -1;
	}

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(12,pANL);
		return -1;
	}

	if((pCom->ReadData[8]!=21)  && (pCom->ReadData[8]!=20)){	/* Cause of Transmission: 21 = negativ acknowledgement of command */
																/* Cause of Transmission: 20 = positiv acknowledgement of command */

		get_4OctetBinaryTime(&(pCom->ReadData[13]),(RTCtime_typ*) &TimeBuf, (RTCtime_typ*) &RPSTime);

		/*	Digitalwert abspeichern	*/

		save_digitalwert(	&(pANL->dm),						/* Adresse Datenmodul-Infostruktur */
							pANL->LinienNummer,					/* Liniennummer */
							pDeviceActiv,						/* Zeiger auf aktives Geraet */
							pCom->ReadData[10],					/* FunctionType		 */
							pCom->ReadData[11],					/* InformationNumber */
							pCom->ReadData[12],					/* Messwert			 */
							(RTCtime_typ*) &TimeBuf,			/* Zeitstempel 		 */
							pDeviceActiv->set_event);			/* Event für Leitsystem */

		if(pDeviceActiv->GI_Ok==INIT_GI_START) pDeviceActiv->GI_Ok = INIT_GI_LAEUFT;
	}

	/* Rueckmeldetelegramm bei der Vorgabe von Kommandos */
	if((pANL->Command.Function) && (pANL->Command.DeviceAtWork==pANL->DeviceAtWork)) {
		command_response(pANL);
	}
	return 0;
}

/****************************************************************************/
/*		Time-tagged message 7.3.1.2	with relative time						*/
/****************************************************************************/

USINT check_m2(AnlagenType *pANL)
{

	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);
/*	Variable Structur Qualifier
	SQ  .  .  .  nr nr nr nr
				 ++ ++ ++ ++----------: number of information elements
				 						hier 1
	++--------------------------------: SQ-Bit	1 = feste Länge
												0 = variable Länge	*/
	if(pCom->ReadData[7] != 0x81) {
		Empfangsfehler(11,pANL);
		return -1;
	}
	/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(12,pANL);
		return -1;
	}

	if((pCom->ReadData[8]!=21)  && (pCom->ReadData[8]!=20)){	/* Cause of Transmission: 21 = negativ acknowledgement of command */
																/* Cause of Transmission: 20 = positiv acknowledgement of command */
		get_4OctetBinaryTime(&(pCom->ReadData[17]),(RTCtime_typ*) &TimeBuf, (RTCtime_typ*) &RPSTime);

		/*	Digitalwert abspeichern	*/

		save_digitalwert(	&(pANL->dm),						/* Adresse Datenmodul-Infostruktur */
							pANL->LinienNummer,					/* Liniennummer */
							pDeviceActiv,						/* Zeiger auf aktives Geraet */
							pCom->ReadData[10],					/* FunctionType		 */
							pCom->ReadData[11],					/* InformationNumber */
							pCom->ReadData[12],					/* Messwert			 */
							(RTCtime_typ*) &TimeBuf,			/* Zeitstempel 		 */
							pDeviceActiv->set_event);			/* Event für Leitsystem */
	}

	/* Rueckmeldetelegramm bei der Vorgabe von Kommandos */
	if((pANL->Command.Function) && (pANL->Command.DeviceAtWork==pANL->DeviceAtWork)) {
		command_response(pANL);
	}
	return 0;
}

/****************************************************************************/
/*		Measurands 1 7.3.1.3			      								*/
/****************************************************************************/
USINT check_m3(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	 0  nr nr nr nr nr nr nr
		++ ++ ++ ++ ++ ++ ++----------: number of information elements	*/

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(92,pANL);
		return -1;
	}

	EmpfangeneMesswerte = pCom->ReadData[7] & 0x7f;


	save_analogwert( &(pANL->dm),									/* Adresse Datenmodul-Infostruktur */
					 pANL->LinienNummer,							/* Liniennummer */
					 pDeviceActiv,									/* Zeiger auf aktives Geraet */
					 pCom->ReadData[10],							/* FunctionType		 */
					 pCom->ReadData[11],							/* InformationNumber */
					 EmpfangeneMesswerte,							/* Anzahl Messwerte  */
					 (USINT *) &(pCom->ReadData[12]),				/* Messwerte		 */
					 pDeviceActiv->set_event);						/* Event für Leitsystem */
	return 0;
}

/****************************************************************************/
/*		Identification Message 7.3.1.5	      								*/
/****************************************************************************/
USINT check_m5(AnlagenType *pANL)
{

	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	SQ  .  .  .  nr nr nr nr
				 ++ ++ ++ ++----------: number of information elements
				 						hier 1
	++--------------------------------: SQ-Bit	1 = feste Länge
												0 = variable Länge	*/
	if(pCom->ReadData[7] != 0x81) {
		Empfangsfehler(51,pANL);
		return -1;
	}

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(52,pANL);
		return -1;
	}

// Beginn 02.07.3| 04-12-13 | KOB
// COL Kompatibilitätslever, nun 2 und 3
//	if(pCom->ReadData[12] != 2) { 
	if((pCom->ReadData[12] != 2) && (pCom->ReadData[12] != 3)) {
		Empfangsfehler(53,pANL);
		return -1;
	}
// Ende 02.07.3| 04-12-13 | KOB


/* Beginn 02.05.1| 02-09-08 | KOB */

/*	Function Type (hängt vom Schutzgerät ab), siehe Gerätehandbuch

		160 (0xa0)	overcurrent protection
					7JS512 (Überstromzeitschutz)
		192	(0xc0)	line differential protection
					7SD503 (Leitungsdifferentialschutz)
		240	(0xf0)	6MD63	*/


/* Beginn 02.05.1| 02-09-08 | KOB
   Überprüfung des Function Types wird weggelassen -> alles wird akzeptiert */

/*
	if(pCom->ReadData[10] != 160 && pCom->ReadData[10] != 192 && pCom->ReadData[10] != 240) {
		Empfangsfehler(54,pANL);
		return -1;
	}
*/

/* Ende 02.05.1| 02-09-08 | KOB  */


/*	Information Number

		3	reset cu (KE)  IEC 870-5-130 page 37
		4	start/restart
		5	power on	*/

	if(pCom->ReadData[11] == 2 || pCom->ReadData[11] == 3 || pCom->ReadData[11] == 4 || pCom->ReadData[11] == 5) {

// Beginn 02.07.3| 04-12-13 | KOB Zustand ACD Bit ignorieren
//		if(!pCom->class1_res_adc1) {
// Ende 02.07.3| 04-12-13 | KOB

/*		if(pDeviceActiv->KE_InitOK==INIT_KE_LAEUFT)	{
*/			pDeviceActiv->KE_InitOk = READY;
			#ifdef DEBUG
				dbg_msg(MESSAGE,7,0,pANL->LinienNummer,pDeviceActiv->Adresse , 0, 0, 0, 0);
			#endif
		}
//	}
	else {
		Empfangsfehler(55,pANL);
		return -1;
	}
	return 0;
}

/****************************************************************************/
/*		Time Synchronistion    7.3.1.6	      								*/
/****************************************************************************/
USINT check_m6(AnlagenType *pANL)
{

	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	USINT	ok,error;
	unsigned short milliseconds;
	DATE_AND_TIME	DT_Device,DT_Synch;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	SQ  .  .  .  nr nr nr nr
				 ++ ++ ++ ++----------: number of information elements
				 						hier 1
	++--------------------------------: SQ-Bit	1 = feste Länge
												0 = variable Länge	*/
	if(pCom->ReadData[7] != 0x81) {
		Empfangsfehler(61,pANL);
		return -1;
	}

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(62,pANL);
		return -1;
	}

/*	Function Type 	*/

	if(pCom->ReadData[10] != 0xff) {
		Empfangsfehler(63,pANL);
		return -1;
	}

/*	Information Number

		0	time sync.  IEC 870-5-130 page 37	*/

	if(pCom->ReadData[11] != 0) {
		Empfangsfehler(64,pANL);
		return -1;
	}

	/* Uhrzeitvergleich */

	/* Zeitstempel vom Geraet in DTStructure verwandeln */

// Beginn 02.07.3| 04-12-13 | nicht verwendete Bits der zurueckgemeldeten Zeit ausmaskieren
// Fehler: Das Byte in dem der Wert fuer den Tag enthalten ist, enthaelt auch den Wochentag
//         Bei Rueckmeldung des Wochentages war der Wert fuer den Tag falsch
	pDeviceActiv->TDevice.year 		= (pCom->ReadData[18] & 0x7F) + 2000;
	pDeviceActiv->TDevice.month		= pCom->ReadData[17] & 0xF;
	pDeviceActiv->TDevice.day		= pCom->ReadData[16] & 0x1F;
	pDeviceActiv->TDevice.wday		= (pCom->ReadData[16] >> 5) & 0x7;
	pDeviceActiv->TDevice.hour		= pCom->ReadData[15] & 0x1F;
	pDeviceActiv->TDevice.minute	= pCom->ReadData[14] & 0x3F;
	milliseconds = (unsigned short) pCom->ReadData[13] * 256 + (unsigned short) pCom->ReadData[12];
	pDeviceActiv->TDevice.second	= milliseconds / 1000;
	pDeviceActiv->TDevice.millisec	= milliseconds % 1000;
	pDeviceActiv->TDevice.microsec	= 0;
// Ende 02.07.3| 04-12-13 | nicht verwendete Bits der zurueckgemeldeten Zeit ausmaskieren


	/* DTStructure in DATE_AND_TIME verwandeln */
	pDeviceActiv->TDevice.wday = 0;	// Wai 15.05.2014 wegen Inkompatibilität von DTStructure_TO_DT
	pDeviceActiv->TSynch.wday = 0;	
	DT_Device	= DTStructure_TO_DT( (UDINT) &(pDeviceActiv->TDevice));
	DT_Synch	= DTStructure_TO_DT( (UDINT) &(pDeviceActiv->TSynch));

	/* Zeitdifferenz berechnen */
	if(DT_Device>DT_Synch) {
		pDeviceActiv->TDiff = DiffDT(DT_Device,DT_Synch);
	}
	else {
		pDeviceActiv->TDiff = DiffDT(DT_Synch,DT_Device);
	}

	ok		= FALSE;
	error	= FALSE;
	
	if(pDeviceActiv->TDiff <= TIME_DIFF_SYNCH_MAX) {
		ok		= TRUE;
		pDeviceActiv->TDiffCounter = 0;
	}
	else  {
		if(pDeviceActiv->TDiffCounter < 1) {
			pDeviceActiv->TDiffCounter = pDeviceActiv->TDiffCounter + 1;
		}
		else {
			error = TRUE;
		}
	}

	if(pDeviceActiv->ZeitsynchAktiv) {
		pDeviceActiv->ZeitsynchAktiv		= 0;
		pDeviceActiv->ZeitLetzteZeitsync	= 0;

		if(ok==TRUE) {
			if(pDeviceActiv->StateTimeSynchError != STATE_ERROR_OFF) {
				save_TimeSynchError( &(pANL->dm),					/* Adresse Datenmodul-Infostruktur */
										pANL->LinienNummer,			/* Nummer der Linie */
										pDeviceActiv,				/* Zeiger auf aktives Geraet */
						 				STATE_ERROR_OFF,			/* Messwert		(Doppelmeldung 1=aus 2=ein)	 */
		 								&RPSTime,					/* Zeitstempel 		 */
						 				TRUE);						/* Event für Leitsystem */
				pDeviceActiv->StateTimeSynchError = STATE_ERROR_OFF;
			}
		}
		if(error==TRUE) {
			if(pDeviceActiv->StateTimeSynchError != STATE_ERROR_ON) {
				save_TimeSynchError( &(pANL->dm),					/* Adresse Datenmodul-Infostruktur */
										pANL->LinienNummer,			/* Nummer der Linie */
										pDeviceActiv,				/* Zeiger auf aktives Geraet */
						 				STATE_ERROR_ON,				/* Messwert		(Doppelmeldung 1=aus 2=ein)	 */
		 								&RPSTime,					/* Zeitstempel 		 */
						 				TRUE);						/* Event für Leitsystem */

				pDeviceActiv->StateTimeSynchError = STATE_ERROR_ON;
			}
		}
	}

	return 0;
}

/****************************************************************************/
/*		Termination of general interrogation 7.3.1.7						*/
/****************************************************************************/
USINT check_m8(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	SQ  .  .  .  nr nr nr nr
				 ++ ++ ++ ++----------: number of information elements
				 						hier 1
	++--------------------------------: SQ-Bit	1 = feste Länge
												0 = variable Länge	*/
	if(pCom->ReadData[7] != 0x81) {
		Empfangsfehler(81,pANL);
		return -1;
	}

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(82,pANL);
		return -1;
	}

/*	Function Type 	*/

	if(pCom->ReadData[10] != 0xff) {
		Empfangsfehler(83,pANL);
		return -1;
	}

/*	Information Number

		0	time sync.  IEC 870-5-130 page 37	*/

	if(pCom->ReadData[11] != 0) {
		Empfangsfehler(84,pANL);
		return -1;
	}
	pDeviceActiv->GI_Ok 	= READY;
	pDeviceActiv->set_event	= TRUE;


/* Übergabe ans Leitsystem */

	#ifdef DEBUG
		dbg_msg(MESSAGE,8,0,pANL->LinienNummer,pDeviceActiv->Adresse , 0, 0, 0, 0);
	#endif

/*	set_global_event_station (idMessageServer, RPSTime, (UINT) pDeviceActiv->Adresse);*/
	return 0;
}

/****************************************************************************/
/*		Measurands 2 7.3.1.8			      								*/
/****************************************************************************/
USINT check_m9(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	 0  nr nr nr nr nr nr nr
		++ ++ ++ ++ ++ ++ ++----------: number of information elements	*/

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(92,pANL);
		return -1;
	}

	EmpfangeneMesswerte = pCom->ReadData[7] & 0x7f;


	save_analogwert( &(pANL->dm),									/* Adresse Datenmodul-Infostruktur */
					 pANL->LinienNummer,							/* Liniennummer */
					 pDeviceActiv,									/* Zeiger auf aktives Geraet */
					 pCom->ReadData[10],							/* FunctionType		 */
					 pCom->ReadData[11],							/* InformationNumber */
					 EmpfangeneMesswerte,							/* Anzahl Messwerte  */
					 (USINT *) &(pCom->ReadData[12]),				/* Messwerte		 */
					 pDeviceActiv->set_event);						/* Event für Leitsystem */
	return 0;
}

/****************************************************************************/
/*	Privates Telegramm: TYPE IDENTIFICATION 79      						*/
/****************************************************************************/
USINT check_m79(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	 0  nr nr nr nr nr nr nr
		++ ++ ++ ++ ++ ++ ++----------: number of information elements	*/

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(92,pANL);
		return -1;
	}

	EmpfangeneMesswerte = pCom->ReadData[7] & 0x7f;


	save_analogwert_priv( &(pANL->dm),									/* Adresse Datenmodul-Infostruktur */
					 		pANL->LinienNummer,							/* Liniennummer */
					 		pDeviceActiv,								/* Zeiger auf aktives Geraet */
					 		pCom->ReadData[10],							/* FunctionType		 */
					 		pCom->ReadData[11],							/* InformationNumber */
					 		EmpfangeneMesswerte,						/* Anzahl Messwerte  */
					 		(USINT *) &(pCom->ReadData[12]),			/* Messwerte		 */
					 		pDeviceActiv->set_event);					/* Event für Leitsystem */
	return 0;
}

/****************************************************************************/
/*	Privates Telegramm: TYPE IDENTIFICATION 205      						*/
/****************************************************************************/
USINT check_m205(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

/*	Variable Structur Qualifier
	 0  nr nr nr nr nr nr nr
		++ ++ ++ ++ ++ ++ ++----------: number of information elements	*/

/*  Deviceadresse */

	if(pCom->ReadData[9] != pDeviceActiv->Adresse) {
		Empfangsfehler(92,pANL);
		return -1;
	}

	EmpfangeneMesswerte = pCom->ReadData[7] & 0x7f;


	save_counter_2( &(pANL->dm),								/* Adresse Datenmodul-Infostruktur */
					 pANL->LinienNummer,						/* Liniennummer */
					 pDeviceActiv,								/* Zeiger auf aktives Geraet */
					 pCom->ReadData[10],						/* FunctionType		 */
					 pCom->ReadData[11],						/* InformationNumber */
					 EmpfangeneMesswerte,						/* Anzahl Messwerte  */
					 (USINT *) &(pCom->ReadData[12]),			/* Messwerte		 */
					 pDeviceActiv->set_event);					/* Event für Leitsystem */

	return 0;
}

/****************************************************************************/
/*		Telegramm beendet, weitere Verarbeitung								*/
/****************************************************************************/
void tel_ok(AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	pDeviceActiv->LetzterACD			= pCom->class1_res_adc1;
	pDeviceActiv->Verbindungsversuche	= 0;
	pDeviceActiv->ZeitLetzteVerbindung	= 0;
	pDeviceActiv->Class1Request++;
	if((pDeviceActiv->GI_Ok!=READY) || ((pANL->DistData.Function!=0) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork))) {
		if(pDeviceActiv->Class1Request > 10) {
			pDeviceActiv->Class1Request = 0;
			GetNewDevice(pANL);
		}
	}
	else {
		if(pDeviceActiv->Class1Request > 3) {
			pDeviceActiv->Class1Request = 0;
			GetNewDevice(pANL);
		}
	}
	pCom->write_read_activ = WRITE;
	pCom->sendewiederholung = 0;

	if(pDeviceActiv->VerbindungsStoerung == 1) {
		save_kopplungsstoerung(	&(pANL->dm),						/* Adresse Datenmodul-Infostruktur */
								pANL->LinienNummer,					/* Nummer der Linie */
								pDeviceActiv,						/* Zeiger auf aktives Geraet */
								1,									/* Messwert	(Doppelmeldung 1=aus 2=ein) */
								&RPSTime,							/* Zeitstempel 		 */
								TRUE);								/* Event für Leitsystem */
		pDeviceActiv->VerbindungsStoerung = 0;
	}


}

/****************************************************************************/
/*		CRC bilden             			      								*/
/****************************************************************************/
USINT MakeCRC(char *data, USINT start, USINT laenge)
{
	USINT crc;
	UINT n;

	crc = 0;
	data += start;
	for(n = start; n < start + laenge; n++)
		crc += *data++;
	return crc;
}

/****************************************************************************/
/*		nächstes Device aussuchen		      								*/
/****************************************************************************/
void GetNewDevice(AnlagenType *pANL)
{
	short n,m;

	m = pANL->DeviceAtWork;
	for(n = 0; n <= DEVICES_PRO_LINE-1; n++) {
		m++;
		if(m > DEVICES_PRO_LINE) m = 1;
		if(pANL->DeviceActiv[m-1].Enabled) {
			 pANL->DeviceAtWork = m;
			 break;
		 }
	}
}

/****************************************************************************/
/*		Empfangsfehler           		      								*/
/****************************************************************************/
void Empfangsfehler(UINT nr, AnlagenType *pANL)
{
	ComType *pCom;
	DeviceActivType *pDeviceActiv;
	unsigned short	MaxVersuche;

	pCom			= &(pANL->com);
	pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

	#ifdef DEBUG
		if(nr==0xFFFF) {					/* Timeout */
			dbg_msg(TIMEOUT,0,0,pANL->LinienNummer,pDeviceActiv->Adresse , 0, 0, 0, 0);
		}
		else {								/* Formatfehler */
			dbg_msg(FEHLERMELDUNG,(UDINT) nr,0,pANL->LinienNummer,pDeviceActiv->Adresse, 0, 0, 0, 0);
		}
	#endif

	EFehler = nr;
	pDeviceActiv->LetzterVerbindungsfehler = nr;

	pDeviceActiv->Verbindungsversuche++;

	if(pDeviceActiv->KE_InitOk!=READY)	MaxVersuche	= IEC_Config.ConnAnz;
	else								MaxVersuche = IEC_Config.RetryAnz;


	if(pDeviceActiv->Verbindungsversuche >= MaxVersuche) {

	/*		Meldung an Leitsystem, daß Device ausgefallen  */

		save_kopplungsstoerung(	&(pANL->dm),						/* Adresse Datenmodul-Infostruktur */
								pANL->LinienNummer,					/* Nummer der Linie */
								pDeviceActiv,						/* Zeiger auf aktives Geraet */
								2,									/* Messwert	(Doppelmeldung 1=aus 2=ein) */
								&RPSTime,	  						/* Zeitstempel 		 */
								TRUE);								/* Event für Leitsystem */

		/* Stoerschriebuebertragung dieses Geraetes abbrechen */

		if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork)) {
			if(nr==0xFFFF) {					/* Timeout */
				save_disturbance_error(pANL,DIST_ERR_LINK_DOWN,pANL->DistData.Function, 0 ,0);
			}
			else {								/* Formatfehler */
				save_disturbance_error(pANL,DIST_ERR_RECEIVE_ERROR,pANL->DistData.Function, nr ,0);
			}
			clear_dist_data_request(pANL);
		}

		pDeviceActiv->Enabled				= 0;
		pANL->DeviceAtWork					= 0;
		pCom->write_read_activ				= WRITE;
		pCom->sendewiederholung				= 0;
		pDeviceActiv->VerbindungsStoerung	= 1;

		pDeviceActiv->VerbindungsStoerungen++;

		GetNewDevice(pANL);
	}
	else {
		pCom->write_read_activ	= WRITE;
		/* Bei den ersten 3 Wiederholungsversuchen wird das gleiche Telegramm nochmals gesendet */
		/* Anschließend wird ein neues Telegramm erzeugt und versendet */
		if(pDeviceActiv->Verbindungsversuche<3) {
			pCom->sendewiederholung = 1;
		}
		else {
			pCom->sendewiederholung = 0;
			/* Stoerschriebtelegramm wiederholen */
			if((pANL->DistData.Function!=NO_FUNCTION) && (pANL->DeviceAtWork==pANL->DistData.DeviceAtWork)) {
				if(pANL->DistData.WaitForResponse) {
					pANL->DistData.DataSend			= 1;
					pANL->DistData.WaitForResponse	= 0;
				}
			}
			if(pDeviceActiv->GI_Ok == INIT_GI_START) pDeviceActiv->GI_Ok = FALSE;
		}
	}
}
/****************************************************************************/
/*		7 Octet Binary Time in RTCtime_typ wandeln			      			*/
/****************************************************************************/
void get_7OctetBinaryTime(USINT *data, RTCtime_typ *TimeBuf, RTCtime_typ *SysTime)
{
	unsigned char  cW[2];
	unsigned short milliseconds;

	TimeBuf->year		= (unsigned short) (*(data+6) & 0x7f);
	TimeBuf->year		= TimeBuf->year + ((SysTime->year/100) * 100);
	TimeBuf->month		= *(data+5) & 0x0f;
	TimeBuf->day		= *(data+4) & 0x1f;
	TimeBuf->hour		= *(data+3) & 0x1f;
	TimeBuf->minute		= *(data+2) & 0x3f;

	cW[0]									= *(data+0);
	cW[1]									= *(data+1);
	milliseconds 							= *((unsigned short *) &cW);

	TimeBuf->second		= milliseconds / 1000;
	TimeBuf->millisec	= milliseconds % 1000;
	TimeBuf->microsec	= 0;
}

/****************************************************************************/
/*		4 Octet Binary Time in RTCtime_typ wandeln			      			*/
/****************************************************************************/
void get_4OctetBinaryTime(USINT *data, RTCtime_typ *TimeBuf, RTCtime_typ *SysTime)
{
	unsigned char  cW[2];
	unsigned short int millisecond;
	RTCtime_typ DateNew;

	cW[0]					= *(data+0);
	cW[1]					= *(data+1);
	millisecond 			= *((unsigned short *) &cW);

	TimeBuf->millisec		= millisecond % 1000;
	TimeBuf->second			= millisecond / 1000;
	TimeBuf->minute			= *(data+2) & 0x3F;
	TimeBuf->hour			= *(data+3) & 0x1F;

/*	TimeBuf->invalid		= *(data+2)>>7;
	TimeBuf->summertime		= *(data+3)>>7;
*/

	TimeBuf->day		= SysTime->day;
	TimeBuf->month		= SysTime->month;
	TimeBuf->year		= SysTime->year;

	if((TimeBuf->hour==0) && (SysTime->hour==23)) {
		the_next_day(TimeBuf,&DateNew);
		TimeBuf->day	= DateNew.day;
		TimeBuf->month	= DateNew.month;
		TimeBuf->year	= DateNew.year;
	}
	else
	if((TimeBuf->hour==23) && (SysTime->hour==0)) {
		the_day_before(TimeBuf,&DateNew);
		TimeBuf->day	= DateNew.day;
		TimeBuf->month 	= DateNew.month;
		TimeBuf->year	= DateNew.year;
	}
}

/****************************************************************************/
/*		Pruefen, ob Geraeteadresse zu einem aktiven Geraet gehoert			*/
/****************************************************************************/
BOOL DeviceIsActive(AnlagenType *pANL, USINT DeviceAdr)
{

	short n;

	for(n = 0; n <= DEVICES_PRO_LINE-1; n++) {
		if((pANL->DeviceActiv[n].Enabled) && (pANL->DeviceActiv[n].Adresse == DeviceAdr)) {
			return(1);
		}
	}
	return(0);
}

/*** eof *****************************************************************************/
