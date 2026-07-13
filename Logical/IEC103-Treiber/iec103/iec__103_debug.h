/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Debugfunktionen
*  Datei:    iec__103_debug.h
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    26-11-2003
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion: Header fuer Debugfunktionen
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 26-11-03 | KOB  | Datei-Erstellung
*  01.01 | 04-12-13 | KOB  | neue Debug Möglichkeit (Variablen: IEC_Info_Clear,IEC_Info_Enable,IEC_InfoNumber,IEC_Info)
*        |          |      | Aufschlüsselung der IEC Telegrammsteuerelemente und Eintrag in Buffer
*        |          |      |
******************************************************************************/
#define DEBUG	; Falls keine DEBUG-Information notwendig ist, dann DEBUG nicht definieren.

#define DIGITALMELDUNG		0
#define ANALOGMELDUNG		1
#define FEHLERMELDUNG		2
#define TIMEOUT				3
#define MESSAGE				4
#define SENDBUFFER			5
#define READBUFFER			6
#define KE_INIT_OK			7
#define GI_INIT_OK			8
#define KOPPLUNGSSTOERUNG	9
#define TIMESYNCHERROR		10

typedef struct {
	USINT OpenDevice;
	USINT DebugEnable;
	USINT ShowMessages;
	USINT ShowTXBuffer;
	USINT ShowRXBuffer;
	USINT ShowErrors;
	USINT ShowDigitalValues;
	USINT ShowAnalogValues;
	INT	  LinienNummer;
	USINT AdresseStart;
	USINT AdresseEnde;
	USINT FunctionType;
	USINT InformationNr;
	USINT AnalogNr;
	USINT DataInHex;
	USINT KeinBuffer;
	USINT DebugDelayAdresse;
	UINT  DebugDelayTime;
	UINT  DebugDelayCounter;
} DebugConfig_typ;


typedef struct {
	USINT EnableMessages;
	USINT EnableTXBuffer;
	USINT EnableRXBuffer;
	USINT EnableErrors;
	USINT EnableDigitalValues;
	USINT EnableAnalogValues;
} DebugEnable_typ;

typedef struct {
	USINT Read;
	USINT Write;
	USINT Adresse;
	USINT Control;
	USINT TypeIdentification;
	USINT StructureIdentifier;
	USINT CauseOfTransmission;
	USINT FunctionType;
	USINT InformationNumber;
	USINT ScanNumber;
	USINT PrimaryFCB;
	USINT PrimaryFCV;
	USINT PrimaryFunction;
	USINT SecondaryACD;
	USINT SecondaryDCF;
	USINT SecondaryFunction;
} Debug_IEC_Info_typ;



void dbg_msg(	USINT code,
				UDINT info1,
				UINT  info2,
				UINT  LinienNummer,
				USINT Adresse,
				USINT FunctionType,
				USINT InformationNumber,
				INT   Messwert,
				USINT *data
				);


// Prototypen
void dbg_buf_iec_clear();
void dgb_msg_send(UDINT adrData, UINT len);
void dbg_iec(Debug_IEC_Info_typ *pIecInfo, UDINT adrData, UINT len);
void dbg_buf_iec_short(USINT Type, UDINT adrData, UINT len);
void dbg_buf_iec_short(USINT Type, UDINT adrData, UINT len);
void dbg_buf_iec_short_ack(USINT Type, UDINT adrData, UINT len);
void dbg_buf_iec_long(USINT Type, UDINT adrData, UINT len);



/*** eof *****************************************************************************/



