#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Kommandouebertragung
*  Datei:    iec__103_command.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.01
*  Datum:    30-01-2019
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion:
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 26-11-03 | KOB  | Datei-Erstellung
*  01.01 | 30-01-19 | KOB  | Es wird nur auf die Rückmeldung Kommando aktzeptiert /
*        |          |      | nicht aktzeptiert gewartet.
*        |          |      | Die Rückmeldung, Kommando ausgeführt wird nicht mehr
*        |          |      | abgewartet.
******************************************************************************/

#include <bur/plctypes.h>
#include <sys_lib.h>			/* SYS_LIB */
#include "iec__103_date.h"
#include "iec60870_5_103.h"		/* Definition der globalen Variablen */
#include "iec__103_debug.h"

/**********************************************************
*			Routinen zum Ausloesen von Kommandos
**********************************************************/
/****************************************************************************/
/*		       Command data Request 			      				     	*/
/****************************************************************************/
void command_request(AnlagenType *pANL)
{
		ComType *pCom;
		DeviceActivType *pDeviceActiv;

		pCom			= &(pANL->com);
		pDeviceActiv	= &(pANL->DeviceActiv[pANL->DeviceAtWork-1]);

		clear_buffer(pCom);

		/*ASDU 20: General Command */

		pCom->WriteData[0] = 0x68;				/* Startbyte Langsatz */
		pCom->WriteData[1] = 10;				/* Länge              */
		pCom->WriteData[2] = 10;
		pCom->WriteData[3] = 0x68;

		if(!pCom->sendewiederholung) {
			pDeviceActiv->FCB_Toggle ^= 1;
			pANL->Command.ReturnInformation++;
		}

		pCom->WriteData[4] = 0x53 + (pDeviceActiv->FCB_Toggle << 5);

		/*             0%01x10011;	  	see iec 870-5-2 Page 35
		                     ----		Function		= 3 	user data
		                    -....		FCV				= 1
		                   -.....		FCB						Wert laut vorheriger Berechnung
		                  -......		PRM				= 1		Message from master
		*/


		pCom->WriteData[5]	= pDeviceActiv->Adresse;

		pCom->WriteData[6]	= 20;								/* Type Identification   					iec 870-5-103 page 31 	*/
		pCom->WriteData[7]	= 0x81;								/* Variable Structur Identifier	*/
		pCom->WriteData[8]	= 20;								/* Cause of Transmission 					iec 870-5-103 page 33 Table 6 */
		pCom->WriteData[9]	= pDeviceActiv->Adresse;
		pCom->WriteData[10] = pANL->Command.FunctionType;		/* Funktion Type         					iec 870-5-103 page 35 	*/
		pCom->WriteData[11] = pANL->Command.InformationNumber;	/* Information Number      					iec 870-5-103 page 37 	*/
		pCom->WriteData[12] = pANL->Command.Val;				/* DCO	Double Command	(1=OFF,2=ON)		iec 870-5-103 7.2.6.4  	*/
		pCom->WriteData[13]	= pANL->Command.ReturnInformation; 	/* RII  Return information identifier		iec 870-5-103 7.2.6.19	*/

		pCom->WriteData[14] = MakeCRC(pCom->WriteData, 4, 10);
		pCom->WriteData[15] = 0x16;		/* Endezeichen */
		pCom->WriteData[16] = 0;
		pCom->WriteDataLen	= 16;


		send_buffer(pCom,pANL->LinienNummer,pDeviceActiv->Adresse);

		pANL->Command.DataSend			= 0;
		pANL->Command.WaitForResponse	= 1;
		pANL->Command.Ready				= 0;
		pANL->Command.Error				= 0;
		pANL->Command.Function			= WAIT_FOR_COMMAND_ACK;
		pCom->timeout					= 0;
		pCom->write_read_activ			= READ;

}
/****************************************************************************/
/*		Timeout beim Empfang von Kommandotelegrammen			    		*/
/****************************************************************************/
void command_timeout(AnlagenType *pANL)
{

	/* Timeoutzaehler */
	if(pANL->Command.Function!=0) {
		pANL->Command.TimeOut += CycleTime;
	}
	else {
		pANL->Command.TimeOut = 0;
	}

	/* Timeoutzeit ueberschritten ? */
	if(pANL->Command.TimeOut>IEC_Config.CommandTimeoutMax) {

		pANL->Command.Trys++;
		if(pANL->Command.Trys>3) {
			pANL->Command.Error		= COMMAND_ERR_TIMEOUT;
			pANL->Command.Function	= NO_FUNCTION;
			pANL->Command.TimeOut	= 0;
			pANL->Command.Trys		= 0;
		}
		else {
			pANL->Command.DataSend	= 1;
			pANL->Command.TimeOut	= 0;
			pANL->Command.Function	= REPEAT_COMMAND;
		}
	}
}

/****************************************************************************/
/*		Rueckmeldetelegramm auswerten										*/
/****************************************************************************/
void command_response(AnlagenType *pANL)
{
	ComType *pCom;
	UINT 	Error;

	pCom	= &(pANL->com);

	if((pANL->Command.FunctionType==pCom->ReadData[10]) && (pANL->Command.InformationNumber	== pCom->ReadData[11])) {

		pANL->Command.WaitForResponse	= 0;

		switch(pCom->ReadData[8]) {
			case 21:			/* Cause of Transmission: 21 = negativ acknowledgement of command */
				pANL->Command.Function	= NO_FUNCTION;
				pANL->Command.Error		= COMMAND_ERR_NAK;			/* Fehlermeldung */
				pANL->Command.Ready		= 1;						/* Uebertragung beendet */
				pANL->Command.Trys		= 0;
				break;

			case 20:			/* Cause of Transmission: 20 = positiv acknowledgement of command */
				Error = 0;
				if(pANL->Command.Val				!= pCom->ReadData[12]) Error = COMMAND_ERR_VALUE;
				if(pANL->Command.ReturnInformation	!= pCom->ReadData[17]) Error = COMMAND_ERR_RETURNINFORMATION;

				if(!Error) {

//					pANL->Command.Function	= WAIT_FOR_COMMAND_EXE;
//					pANL->Command.TimeOut	= 0;

					pANL->Command.Error		= 0;
					pANL->Command.Function	= NO_FUNCTION;
					pANL->Command.TimeOut	= 0;
					pANL->Command.Trys		= 0;
					pANL->Command.Ready		= 1;						/* Uebertragung beendet */
			
				}
				else {
					pANL->Command.Trys++;
					if(pANL->Command.Trys>3) {
						pANL->Command.Error		= Error;
						pANL->Command.Function	= NO_FUNCTION;
						pANL->Command.TimeOut	= 0;
						pANL->Command.Trys		= 0;
						pANL->Command.Ready		= 1;						/* Uebertragung beendet */
					}
					else {
						pANL->Command.DataSend	= 1;
						pANL->Command.TimeOut	= 0;
						pANL->Command.Function	= REPEAT_COMMAND;
					}
				}
				break;

			case 12:			/* Cause of Transmission: remote operation */
				pANL->Command.ReturnVal = pCom->ReadData[12];
				pANL->Command.Error		= 0;
				pANL->Command.Function	= NO_FUNCTION;
				pANL->Command.TimeOut	= 0;
				pANL->Command.Trys		= 0;
				pANL->Command.Ready		= 1;						/* Uebertragung beendet */
				break;
		}
	}
}

/*** eof *****************************************************************************/


