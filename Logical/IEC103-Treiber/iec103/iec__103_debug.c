#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Debugfunktionen
*  Datei:    iec__103_debug.c
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
*  01.01 | 04-12-13 | KOB  | neue Debug Möglichkeit (Variablen: IEC_Info_Clear,IEC_Info_Enable,IEC_InfoNumber,IEC_Info)
*        |          |      | Aufschlüsselung der IEC Telegrammsteuerelemente und Eintrag in Buffer
*        |          |      |
******************************************************************************/
#include <bur\plc.h>		/* RPS-spezifische Typendeklarationen */
#include <string.h>
#include <dvframe.h>
#include <asbrstr.h>
#include <sys_lib.h>
#include <bur/plctypes.h>

#include "iec__103_debug.h"
#include "iec60870_5_103.h"		/* Definition der globalen Variablen */

_LOCAL Debug_IEC_Info_typ IEC_Info[20];
_LOCAL UINT IEC_InfoNumber;
_LOCAL UINT IEC_InfoAdresse;
_LOCAL UINT IEC_InfoLinie;

_LOCAL USINT dbgBuf[400];
_LOCAL USINT dbgBufD[400];
_LOCAL USINT dbgBufA[400];
_LOCAL USINT dbgBufS[400];
_LOCAL USINT dbgBufR[400];

_LOCAL USINT timBuf[12];

_LOCAL FRM_xopen_typ	DebugXOpenStruct;
_LOCAL XOPENCONFIG		DebugConfigStruct;
_LOCAL FRM_close_typ	DebugCloseStruct;
_LOCAL FRM_gbuf_typ		DebugGBufStruct;

_LOCAL DebugEnable_typ	DebugEnable;
_LOCAL DebugConfig_typ	DebugConfig;

_LOCAL USINT buf[4];

char hextab[16] = {							/* Hextabelle */
	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};

char crlf[3] = {13,10,0};


/**********************************************************
*		initialization of debugging-interface
**********************************************************/

void debug_initialize(void)
{

	/* initialize config structure for debug-message */

	DebugConfigStruct.idle		= 4;
	DebugConfigStruct.delimc	= 0;
	DebugConfigStruct.delim[0]	= 0;
	DebugConfigStruct.delim[1]	= 0;
	DebugConfigStruct.tx_cnt	= 8;
	DebugConfigStruct.rx_cnt	= 2;
	DebugConfigStruct.tx_len	= 256;
	DebugConfigStruct.rx_len	= 256;
	DebugConfigStruct.argc		= 0;
	DebugConfigStruct.argv		= 0;

	DebugXOpenStruct.device		= (UDINT) "SL4.SS1.IF1";

	/* fuer m68k */
	/* DebugXOpenStruct.mode		= (UDINT) "RS232, 19200, N, 8, 1";*/

	/* fuer i386 */
	DebugXOpenStruct.mode		= (UDINT) "/PHY=RS232 /BD=19200 /PA=N /DB=8 /SB=1";

	DebugXOpenStruct.config		= (UDINT) &DebugConfigStruct;
	DebugXOpenStruct.enable		= 1;

	DebugXOpenStruct.ident = 0;
}


void debug_open_device(USINT enable)
{
	if(enable) {
		if(!DebugXOpenStruct.ident)	FRM_xopen(&DebugXOpenStruct);
	}
	else {
		if(DebugXOpenStruct.ident) {
			DebugCloseStruct.enable	= 1;
			DebugCloseStruct.ident	= DebugXOpenStruct.ident;
			FRM_close(&DebugCloseStruct);
			DebugXOpenStruct.ident = 0;
		}
	}
}

/**********************************************************
*		print a character in hex-format
**********************************************************/

void hex_ascii(USINT b1,char *buffer)
{
	*(buffer+1) = hextab[b1 & 0xF];
	*(buffer) 	= hextab[b1>>4];
}

/**********************************************************
*		print the time in ascii-format
**********************************************************/

void time_ascii(RTCtime_typ *Zeit, char *timBuf)
{
	if(Zeit->minute<10) {
		*timBuf = '0';
		brsitoa(Zeit->minute,(UDINT) timBuf+1);
	}
	else {
		brsitoa(Zeit->minute,(UDINT) timBuf);
	}
	*(timBuf+2) = ':';

	if(Zeit->second<10) {
		*(timBuf+3) = '0';
	 	brsitoa(Zeit->second,(UDINT) timBuf+4);
	}
	else {
		brsitoa(Zeit->second,(UDINT) timBuf+3);
	}
	*(timBuf+5) = ':';


	*(timBuf+6) = '0';
	*(timBuf+7) = '0';

	if(Zeit->millisec<10) {
	 	brsitoa(Zeit->millisec,(UDINT) timBuf+8);
	}
	else {
		if(Zeit->millisec<100) {
		 	brsitoa(Zeit->millisec,(UDINT) timBuf+7);
		}
		else {
		 	brsitoa(Zeit->millisec,(UDINT) timBuf+6);
		}
	}

	*(timBuf+9)		= ' ';
	*(timBuf+10)	= 0;
}

/**********************************************************
*		build a debug-message
**********************************************************/

void dbg_msg(	USINT	code,
				UDINT	info1,
				UINT 	info2,
				UINT	LinienNummer,
				USINT	Adresse,
				USINT	FunctionType,
				USINT	InformationNumber,
				INT	 	Messwert,
				USINT 	*data)

{

	int i;


	DebugEnable.EnableMessages		= DebugConfig.DebugEnable & DebugConfig.ShowMessages;
	DebugEnable.EnableTXBuffer		= DebugConfig.DebugEnable & DebugConfig.ShowTXBuffer;
	DebugEnable.EnableRXBuffer		= DebugConfig.DebugEnable & DebugConfig.ShowRXBuffer;
	DebugEnable.EnableErrors		= DebugConfig.DebugEnable & DebugConfig.ShowErrors;
	DebugEnable.EnableDigitalValues = DebugConfig.DebugEnable & DebugConfig.ShowDigitalValues;
	DebugEnable.EnableAnalogValues	= DebugConfig.DebugEnable & DebugConfig.ShowAnalogValues;


	if( ((DebugConfig.LinienNummer<0) || (DebugConfig.LinienNummer==LinienNummer)) &&
		(((DebugConfig.AdresseStart==0) && (DebugConfig.AdresseEnde==0)) ||
		    ((DebugConfig.AdresseStart<=Adresse) && (DebugConfig.AdresseEnde>=Adresse))) ){

		time_ascii(&RPSTime,(char *) &timBuf);

		switch(code) {
			case READBUFFER:
				if(DebugEnable.EnableRXBuffer) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBufR[0]));
						strcat(dbgBufR," ");
					}
					else {
						strcpy(dbgBufR,"  ");
					}
					strcat(dbgBufR,timBuf);
					strcat(dbgBufR,"RX: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBufR[strlen(dbgBufR)]));
					strcat(dbgBufR," Data: ");
					strcpy(buf,"   ");
					if(info1>0) {
						for(i=0;i<info1;i++) {
							hex_ascii(*(data+i),buf);
							strcat(dbgBufR,buf);
						}
					}

					if(DebugXOpenStruct.ident) {
						strcat(dbgBufR,crlf);
						dgb_msg_send((UDINT) dbgBufR, strlen(dbgBufR));
					}
				}
			break;

			case SENDBUFFER:
				if(DebugEnable.EnableTXBuffer) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBufS[0]));
						strcat(dbgBufS," ");
					}
					else {
						strcpy(dbgBufS,"  ");
					}

					strcat(dbgBufS,timBuf);
					strcat(dbgBufS,"TX: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBufS[strlen(dbgBufS)]));
					strcat(dbgBufS," Data: ");
					strcpy(buf,"   ");
					if(info1>0) {
						for(i=0;i<info1;i++) {
							hex_ascii(*(data+i),buf);
							strcat(dbgBufS,buf);
						}
					}
					if(DebugXOpenStruct.ident) {
						strcat(dbgBufS,crlf);
						dgb_msg_send((UDINT) dbgBufS, strlen(dbgBufS));
					}
				}
			break;

			case MESSAGE:
				if(DebugEnable.EnableMessages) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBuf[0]));
						strcat(dbgBuf," ");
					}
					else {
						strcpy(dbgBuf,"  ");
					}
					strcat(dbgBuf,timBuf);
					strcat(dbgBuf,"MSG: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBuf[strlen(dbgBuf)]));
					switch(info1) {
						case 0:
							strcat(dbgBuf," RESET_KE");
						break;
						case 1:
							strcat(dbgBuf," CLASS_1_REQUEST 1");
						break;
						case 2:
							strcat(dbgBuf," ZEIT_SYNCH");
						break;
						case 3:
							strcat(dbgBuf," INIT_GI");
						break;
						case 4:
							strcat(dbgBuf," CLASS_1_REQUEST 4");
						break;
						case 5:
							strcat(dbgBuf," CLASS_1_REQUEST 5");
						break;
						case 6:
							strcat(dbgBuf," CLASS_2_REQUEST");
						break;
						case 7:
							strcat(dbgBuf," KE_INIT_OK");
						break;
						case 8:
							strcat(dbgBuf," GI_INIT_OK");
						break;
						case 9:
							strcat(dbgBuf," DISTURBANCE DATA REQUEST");
						break;
						case 10:
							strcat(dbgBuf," CLEAR DISTURBANCE DATA");
						break;
						case 11:
							strcat(dbgBuf," COMMAND");
						break;
					}

					if(DebugXOpenStruct.ident) {
						strcat(dbgBuf,crlf);
						dgb_msg_send((UDINT) dbgBuf, strlen(dbgBuf));
					}
				}
			break;

			case TIMEOUT:
				if(DebugEnable.EnableErrors) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBuf[0]));
						strcat(dbgBuf," ");
					}
					else {
						strcpy(dbgBuf,"  ");
					}
					strcat(dbgBuf,timBuf);
					strcat(dbgBuf,"TOUT: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBuf[strlen(dbgBuf)]));
					if(DebugXOpenStruct.ident) {
						strcat(dbgBuf,crlf);
						dgb_msg_send((UDINT) dbgBuf, strlen(dbgBuf));
					}
				}
			break;

			case FEHLERMELDUNG:
				if(DebugEnable.EnableErrors) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBuf[0]));
						strcat(dbgBuf," ");
					}
					else {
						strcpy(dbgBuf,"  ");
					}
					strcat(dbgBuf,timBuf);
					strcat(dbgBuf,"ERR: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBuf[strlen(dbgBuf)]));
					strcat(dbgBuf," Nr=");
					brsitoa((int) info1,(UDINT) &(dbgBuf[strlen(dbgBuf)]));
					if(DebugXOpenStruct.ident) {
						strcat(dbgBuf,crlf);
						dgb_msg_send((UDINT) dbgBuf, strlen(dbgBuf));
					}
				}
			break;

			case KOPPLUNGSSTOERUNG:
				if(DebugEnable.EnableDigitalValues) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBufD[0]));
						strcat(dbgBufD," ");
					}
					else {
						strcpy(dbgBufD,"  ");
					}

					strcat(dbgBufD,timBuf);
					strcat(dbgBufD,"VERBINDUNGSSTATUS: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
					strcat(dbgBufD," WERT=");
					brsitoa(Messwert,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
					if(DebugXOpenStruct.ident) {
						strcat(dbgBufD,crlf);
						dgb_msg_send((UDINT) dbgBufD, strlen(dbgBufD));
					}
				}

			case TIMESYNCHERROR:
				if(DebugEnable.EnableDigitalValues) {
					if(DebugConfig.KeinBuffer) {
						brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBufD[0]));
						strcat(dbgBufD," ");
					}
					else {
						strcpy(dbgBufD,"  ");
					}

					strcat(dbgBufD,timBuf);
					strcat(dbgBufD,"STATUS ZEIT SYNCH: Adr=");
					brsitoa(Adresse,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
					strcat(dbgBufD," WERT=");
					brsitoa(Messwert,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
					if(DebugXOpenStruct.ident) {
						strcat(dbgBufD,crlf);
						dgb_msg_send((UDINT) dbgBufD, strlen(dbgBufD));
					}
				}
			break;

			case DIGITALMELDUNG:
				if(DebugEnable.EnableDigitalValues) {
					if( ((DebugConfig.FunctionType==0)  || (DebugConfig.FunctionType==FunctionType)) &&
						((DebugConfig.InformationNr==0) || (DebugConfig.InformationNr==InformationNumber)))
					{
						if(DebugConfig.KeinBuffer) {
							brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBufD[0]));
							strcat(dbgBufD," ");
						}
						else {
							strcpy(dbgBufD,"  ");
						}

						strcat(dbgBufD,timBuf);
						strcat(dbgBufD,"DIG: Adr=");
						brsitoa(Adresse,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
						strcat(dbgBufD," FUNC=");
						brsitoa(FunctionType,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
						strcat(dbgBufD," INFO=");
						brsitoa(InformationNumber,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
						strcat(dbgBufD," WERT=");
						brsitoa(Messwert,(UDINT) &(dbgBufD[strlen(dbgBufD)]));
						if(DebugXOpenStruct.ident) {
							strcat(dbgBufD,crlf);
							dgb_msg_send((UDINT) dbgBufD, strlen(dbgBufD));
						}
					}
				}
			break;

			case ANALOGMELDUNG:
				if(DebugEnable.EnableAnalogValues) {
					if( ((DebugConfig.FunctionType==0)  || (DebugConfig.FunctionType==FunctionType)) &&
						((DebugConfig.InformationNr==0) || (DebugConfig.InformationNr==InformationNumber)) &&
						((DebugConfig.AnalogNr==0)      || (DebugConfig.AnalogNr==info1)) )
					{
						if(DebugConfig.KeinBuffer) {
							brsitoa((INT) DebugConfig.KeinBuffer,(DINT) &(dbgBufA[0]));
							strcat(dbgBufA," ");
						}
						else {
							strcpy(dbgBufA,"  ");
						}
						strcat(dbgBufA,timBuf);
						strcat(dbgBufA,"ANL: Adr=");
						brsitoa(Adresse,(UDINT) &(dbgBufA[strlen(dbgBufA)]));
						strcat(dbgBufA," FUNC=");
						brsitoa(FunctionType,(UDINT) &(dbgBufA[strlen(dbgBufA)]));
						strcat(dbgBufA," INFO=");
						brsitoa(InformationNumber,(UDINT) &(dbgBufA[strlen(dbgBufA)]));
						strcat(dbgBufA," NR=");
						brsitoa(info1,(UDINT) &(dbgBufA[strlen(dbgBufA)]));
						strcat(dbgBufA," WERT=");
						brsitoa(Messwert,(UDINT) &(dbgBufA[strlen(dbgBufA)]));
						if(DebugXOpenStruct.ident) {
							strcat(dbgBufA,crlf);
							dgb_msg_send((UDINT) dbgBufA, strlen(dbgBufA));
						}
					}
				}
			break;
		}
	}
}

void dgb_msg_send(UDINT adrData, UINT len)
{


	UINT send_len;

/*						DebugWriteStruct.ident 	= DebugXOpenStruct.ident;
						DebugWriteStruct.buffer = (UDINT) dbgBuf;
						DebugWriteStruct.buflng = strlen(dbgBuf);
						DebugWriteStruct.enable = 1;
						FRM_write(&DebugWriteStruct); 	*/				/* write data to interface */


	DebugGBufStruct.ident  = DebugXOpenStruct.ident;
	DebugGBufStruct.enable = 1;
	FRM_gbuf(&DebugGBufStruct);
	if(DebugGBufStruct.status==0) {
		if(DebugGBufStruct.buflng>len) 	send_len = len;
		else							send_len = DebugGBufStruct.buflng;
		memcpy((UDINT *) DebugGBufStruct.buffer,(UDINT *) adrData,send_len);
		DebugWriteStruct.ident 	= DebugXOpenStruct.ident;
		DebugWriteStruct.buffer = DebugGBufStruct.buffer;
		DebugWriteStruct.buflng = send_len;
		DebugWriteStruct.enable = 1;
		FRM_write(&DebugWriteStruct); 					/* write data to interface */
		DebugConfig.KeinBuffer  = 0;
	}
	else
		DebugConfig.KeinBuffer++;
}

void dbg_iec(Debug_IEC_Info_typ *pIecInfo, UDINT adrData, UINT len)
{

	if(IEC_InfoAdresse == pIecInfo->Adresse) {

		if(IEC_InfoNumber>19) IEC_InfoNumber = 0;
		memcpy((void *) &IEC_Info[IEC_InfoNumber], pIecInfo, sizeof(Debug_IEC_Info_typ));
	
	
		if(IEC_Info[IEC_InfoNumber].Read == 1) {
	
			IEC_Info[IEC_InfoNumber].PrimaryFCB			= 0;
			IEC_Info[IEC_InfoNumber].PrimaryFCV			= 0;
			IEC_Info[IEC_InfoNumber].PrimaryFunction	= 0;
	
			if(IEC_Info[IEC_InfoNumber].Control & 0x20) {
				IEC_Info[IEC_InfoNumber].SecondaryACD		= 1;
			}
			else {
				IEC_Info[IEC_InfoNumber].SecondaryACD		= 0;
			}
			if(IEC_Info[IEC_InfoNumber].Control & 0x10) {
				IEC_Info[IEC_InfoNumber].SecondaryDCF		= 1;
			}
			else {
				IEC_Info[IEC_InfoNumber].SecondaryDCF		= 0;
			}
	
			IEC_Info[IEC_InfoNumber].SecondaryFunction	= IEC_Info[IEC_InfoNumber].Control & 0x0F;
		}
		else {
			if(IEC_Info[IEC_InfoNumber].Control & 0x20) {
				IEC_Info[IEC_InfoNumber].PrimaryFCB	= 1;
			}
			else {
				IEC_Info[IEC_InfoNumber].PrimaryFCB	= 0;
			}
	
			if(IEC_Info[IEC_InfoNumber].Control & 0x10) {
				IEC_Info[IEC_InfoNumber].PrimaryFCV	= 1;
			}
			else {
				IEC_Info[IEC_InfoNumber].PrimaryFCV	= 0;
			}
			IEC_Info[IEC_InfoNumber].PrimaryFunction	= IEC_Info[IEC_InfoNumber].Control & 0x0F;
	
			IEC_Info[IEC_InfoNumber].SecondaryACD		= 0;
			IEC_Info[IEC_InfoNumber].SecondaryDCF		= 0;
			IEC_Info[IEC_InfoNumber].SecondaryFunction	= 0;
		}
	
		IEC_InfoNumber++;
	}
}

void dbg_buf_iec_short(USINT Type, UDINT adrData, UINT len)
{
	char *data;
	Debug_IEC_Info_typ iec_info;

	if(Type == READ) {
		iec_info.Read	= 1;
		iec_info.Write	= 0;
	}
	else {
		iec_info.Read	= 0;
		iec_info.Write	= 1;
	}

	data = (char *) adrData;
	iec_info.Adresse 				= *(data+2);
	iec_info.Control				= *(data+1);
	iec_info.TypeIdentification		= 0;
	iec_info.StructureIdentifier	= 0;
	iec_info.CauseOfTransmission	= 0;
	iec_info.FunctionType			= 0;
	iec_info.InformationNumber		= 0;
	iec_info.ScanNumber				= 0;

	dbg_iec((Debug_IEC_Info_typ *) &iec_info, adrData, len);
}

void dbg_buf_iec_long(USINT Type, UDINT adrData, UINT len)
{
	char *data;
	Debug_IEC_Info_typ iec_info;

	if(Type == READ) {
		iec_info.Read	= 1;
		iec_info.Write	= 0;
	}
	else {
		iec_info.Read	= 0;
		iec_info.Write	= 1;
	}

	data = (char *) adrData;
	iec_info.Adresse 				= *(data+5);
	iec_info.Control				= *(data+4);
	iec_info.TypeIdentification		= *(data+6);
	iec_info.StructureIdentifier	= *(data+7);
	iec_info.CauseOfTransmission	= *(data+8);
	iec_info.FunctionType			= *(data+10);
	iec_info.InformationNumber		= *(data+11);
	iec_info.ScanNumber				= *(data+12);

	dbg_iec((Debug_IEC_Info_typ *) &iec_info, adrData, len);

}

void dbg_buf_iec_short_ack(USINT Type, UDINT adrData, UINT len)
{
	char *data;
	Debug_IEC_Info_typ iec_info;

	if(Type == READ) {
		iec_info.Read	= 1;
		iec_info.Write	= 0;
	}
	else {
		iec_info.Read	= 0;
		iec_info.Write	= 1;
	}

	data = (char *) adrData;
	iec_info.Adresse 				= 0;
	iec_info.Control				= *(data+0);
	iec_info.TypeIdentification		= 0;
	iec_info.StructureIdentifier	= 0;
	iec_info.CauseOfTransmission	= 0;
	iec_info.FunctionType			= 0;
	iec_info.InformationNumber		= 0;
	iec_info.ScanNumber				= 0;

	dbg_iec((Debug_IEC_Info_typ *) &iec_info, adrData, len);

}

void dbg_buf_iec_clear()
{
	memset((void *) &IEC_Info, 0, sizeof(IEC_Info));
	IEC_InfoNumber = 0;
}

/*** eof *****************************************************************************/


