/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  I/O-Treiber
*  Teil:     Headerfile
*  Datei:    iodrv.h
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
*        |          |      |
******************************************************************************/
#include <bur/plctypes.h>
#include <dvframe.h>
#include <sys_lib.h>
#include <dataobj.h>
#include <brsystem.h>
#include "bksys.h"


/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/

/* -------------------------------------------*/
/* Grenzwert fuer Linien und Geraete pro Linie */
/* -------------------------------------------*/

#define MAX_LINES							1				/* Maximal moegliche Anzahl Linien */
#define DEVICES_PRO_LINE					128				/* Anzahl moeglicher Geraete pro Linie */


/* --------------------------------------------*/
/* Allgemeine Konstanten                       */
/* --------------------------------------------*/

#define FALSE			0
#define TRUE			1

#define ACK				254
#define NAK				255

#define WRITE			0
#define READ			1

#define INIT_KE			1
#define INIT_KE_LAEUFT	2
#define ZEIT_SYNC		3
#define INIT_GI			4
#define INIT_GI_START   5
#define INIT_GI_LAEUFT	6
#define CLASS_1_REQ		7
#define CLASS_2_REQ		8
#define READY			99

#define TYPE_CONNECTION_ERROR	255		/* Typ für Datenpunkt : Verbindungsfehler */
#define INFO_CONNECTION_ERROR	255		/* Info für Datenpunkt: Verbindungsfehler */

/* -------------------------------------------------------------*/
/* Kennzeichnung für Einzelschritte der Störschriebübermittlung */
/* -------------------------------------------------------------*/

#define NO_FUNCTION							0
#define SELECTION_OF_FAULT	 				1
#define REQ_DISTURBANCE_TRANSMISSION		2
#define REQ_FOR_TAGS						3
#define REQ_FOR_A_CHANNEL					4
#define ACK_FOR_TAGS						5
#define ACK_FOR_A_CHANNEL					6
#define ACK_FOR_DISTURBANCE_DATA			7
#define REQ_FOR_LIST_OF_FAULTS				8
#define ABORT_DISTURBANCE_TRANSMISSION		9
#define ABORT_CHANNEL_TRANSMISSION			10

/* -------------------------------------------------------------------------*/
/* Kennung fuer Stoerschriebbuffer und Stoerschriebinformation des Geraetes */
/* -------------------------------------------------------------------------*/

#define NODATA								0
#define WRITE_BUFFER						1
#define DATA_READY							2
#define WAIT_FOR_QUIT						3
#define DELETE_DATA							4
#define SEND_DELETE_TELEGRAM				5
#define CLEAR_DATA_AGAIN					6


/***********************************************************************/
/* Datentypdeklaration                                                 */
/***********************************************************************/

/* ----------------------------------------*/
/* Konfigurationsvariable des IODRV-Treibers */
/* ----------------------------------------*/
typedef struct
{
	unsigned long			TimeOut;					/* Timeout bei Telegrammempfang (in ms) */
	unsigned short			RetryAnz;					/* Anzahl Telegrammwiederholungen bei bestehender Verbindung */
	unsigned short			ConnAnz;					/* Anzahl Telegrammwiederholung bei neu aufzubauender Verbindung */
	unsigned long			WaitIntv;					/* Wartezeit für erneuten Verbindungsaufbau (in ms) */
	unsigned long			TimeTS;						/* Intervallzeit fuer Uhrzeitsynchronisation (in ms) */
	unsigned short			EnableComReset;				/* Freigabe fuer das Schliessen und Wieder-Oeffnen der Schnittstelle bei Schreibfehlern */
	unsigned long			TimeClass2Poll;				/* Abtastzeit Class 2 Telegramme (Analogwerte) (in ms) */
	unsigned long			TimeClass2PollFast;			/* Abtastzeit Class 2 Telegramme (Analogwerte) während Übertragung von Störschrieben (in ms) */
	unsigned short			DistDataEnable;				/* Freigabe fuer Stoerschriebuebertragung */
	unsigned long			DistDataTimeoutMax;			/* Timeout fuer den Telegrammempfang bei Stoerschriebuebertragung in (ms) */
	unsigned long			DisturbanceDelay;			/* Zeit die ein Lesen von Stoerschrieben der gleichen Station verzoegert wird in (ms) */
	unsigned short			CommandEnable;				/* Freigabe fuer Kommandouebertragung */
	unsigned long			CommandTimeoutMax;			/* Timeout fuer die Uebertragung von Kommandos in (ms) */
} IODRV_Config_typ;



/* Verwaltung der Unterstationen */
/* Struktur eines Datenpunkt Items im Speicher */
typedef struct {
	UINT	AnlagenNr;						/* Anlagennummer eines Geräts */
	UINT	DPTypNr;						/* Datentypnummer */
	UINT	DPNr;							/* Datenpunktnummer */
	UINT	ItemNr;							/* Item Nummer */
	USINT	ItemTypNr;						/* Datentypnummer des Items */
	USINT	*DPName;						/* Datenpunktname */
	UINT	DPNameLen;						/* Länge des DP-Namens */
	USINT	*ItemName;						/* Item Name */
	UINT	ItemNameLen;					/* Länge des Item-Namens */

	USINT	Invers;							/* DP invertieren */
	USINT	Quitt;							/* DP quittierpflichtig */
	USINT	Dir;							/* Schreib-/Leseattribut */

	STRING	PVName[64];						/* Name der PV */
	UDINT	PVAdrs;							/* Adresse der PV */
	UDINT	PVDataLen;						/* Länge der PV */
	DINT	ValueIO;						/* Wert,der vom Geraet empfangen wurde */
	DINT	ValueServer;					/* Wert, der an den Server uebergeben wurde */

	USINT	AccessCounter;					/* Zaehle fuer Zugriffe auf das Item */
	USINT	ErrorInfo;						/* Fehlerinformation */

} IODRV_DP_Info_typ;
/*---------------------------------------------*/
/* Datenmodulstrukturen 		               */
/*---------------------------------------------*/




/*-----------------------------------------------*/
/* Strukturen fuer die Verwaltung eines Geraetes */
/*-----------------------------------------------*/

typedef struct {
	USINT	Nummer;
	USINT	Adresse;
	USINT	Aktiv;
	USINT	TimeSynch;
	UINT	AnlagenNr;
	UINT	Frei;
} DeviceType;




typedef union
{
	BOOL 	b;
	SINT 	i1;
	USINT	ui1;
	INT		i2;
	UINT	ui2;
	DINT	i4;
	UDINT	ui4;
	float r4;
} PAR_TYPE;


/**********************************************************
*						Prototypen
**********************************************************/
/*
USINT save_analogwert(	DMInfoType *pDMInfo,  IODRV_DeviceActivType *pDeviceActiv,
						USINT FunctionType, USINT InformationNumber, USINT AnzahlMesswerte,
						USINT *AdresseMesswerte, USINT set_event);

void save_analogwert_error(	DMInfoType *pDMInfo, IODRV_DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT set_event);

USINT save_digitalwert( DMInfoType *pDMInfo, IODRV_DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT Messwert,
						RTCtime_typ *TimeBuf, USINT set_event);

USINT save_kopplungsstoerung( DMInfoType *pDMInfo, IODRV_DeviceActivType *pDeviceActiv, USINT Messwert, RTCtime_typ *TimeBuf, USINT set_event);

*/
void debug_initialize(void);




void get_idBrServer();


/**********************************************************
*				Globale Variablen Declaration
**********************************************************/


/**********************************************************
*				Lokale Variablen Declaration
**********************************************************/

_LOCAL	UDINT							myidBrServer;
_LOCAL_RETAIN	UINT							statePV_xgetadr	;
_LOCAL  UINT							StatusSetValueItem;


_LOCAL	RTInfo_typ						rt_info;
_LOCAL  UINT							CycleTime;

_LOCAL RTCtime_typ 						TimeBuf;

_LOCAL USINT 							Error;
_LOCAL UINT								idstat,brstate;

_LOCAL BOOL								initPAB;

_LOCAL_RETAIN Stats_Typ						SYS_Statistic	;


/*** eof *****************************************************************************/



