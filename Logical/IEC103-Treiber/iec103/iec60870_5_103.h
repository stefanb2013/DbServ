/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Hauptprogramm
*  Datei:    iec60870_5_103.h
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 03.00.4
*  Datum:    07-02-19
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion: Globale Headerdatei fuer IEC-Treiber
*------------------------------------------------------------------------------
*  Rev.   | Datum    | Name | Kommentar
*  -------+----------+------+---------------------------------------------------
*  01.00  | 26-11-03 | KOB  | Datei-Erstellung
*         |          |      |
*  01.06  | 10-01-07 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Type Identification 3 : Measurands 1
*         |          |      | (check_m3, iec__103_sub.c)
*         |          |      | Type Identification 79: Privates Telegramm zur Übertragung von Zählwerten
*         |          |      | (check_m79, iec__103_sub.c)
*         |          |      | Auswertung der im privaten Telegramm empfangenen Zählwerte
*         |          |      | (save_analogwert_priv, iec__103_serv.c)
*         |          |      |
*  02.05.2| 02-09-08 | KOB  | Funktionen wie unter 01.06 beschrieben eingefügt.
*  02.05.2| 02-09-08 | KOB  | Funktionen wie unter 01.06 beschrieben eingefügt.
*  03.00.3| 04-04-14 | KOB  | TIME_DIFF_SYNCH_MAX von 2 auf 10 geändert
*         |          |      | DeviceActivType erweitert um die Elemente
*         |          |      | TDiff und TDiffCounter;
*  03.00.4| 07-02-19 | KOB  | Neue Konstante für Fehlermeldungen der Commando-Schreibfunktion
*         |          |      | Neuer Prototyp DeviceIsActive
******************************************************************************/
#include <bur/plctypes.h>
#include <dvframe.h>
#include <sys_lib.h>
#include <dataobj.h>
#include <brsystem.h>
#include <astime.h>
#include "bksys.h"


/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/

/* ------------------------------ */
/* Quelle der Konfigurationsdaten */
/* ------------------------------ */

/* Je nach Definition werden die Konfigurationsdaten in den beschriebenen Datenmodulen erwartet */

/* Quelle der Konfigurationsdaten: dplist und typlist (DbServ) */
#define INIT_MODE_DM_DPLIST
/* Quelle der Konfigurationsdaten: lines,devax,wertx,mruc,norm (Ursprungsversion) */
#undef	INIT_MODE_DM_WERT

/* -------------------------------------------*/
/* Grenzwert fuer Linien und Geraete pro Linie */
/* -------------------------------------------*/

#define MAX_LINES							12				/* Maximal moegliche Anzahl Linien */
#define DEVICES_PRO_LINE					32				/* Anzahl moeglicher Geraete pro Linie */

/* -------------------------------------------*/
/* Buffer fuer Stoerschriebdaten			  */
/* -------------------------------------------*/

#define NR_OF_BUFFERS						6				/* Maximale Anzahl von Buffern fuer die Uebertragung von Stoerschrieben */
#define BUFFER_SIZE							150000			/* Groesse eines Buffers fuer die Uebertragung von Stoerschrieben */

#define MAX_ANALOGWERTE						16				/* Maximale Anzahl Analogwerte in einem Telegramm */
/* -------------------------------------------*/
/* Fehlerkennungen Initialisierung            */
/* -------------------------------------------*/

#define INI_ERR_DO_LINES					1				/* Fehler Datenmodul lines,   siehe: DOI_lines.status */
#define INI_ERR_DO_MRUK						2				/* Fehler Datenmodul mruk,    siehe: DOI_mruk.status	*/
#define INI_ERR_DO_NORM						3				/* Fehler Datenmodul norm,    siehe: DOI_norm.status	*/
#define INI_ERR_DEVICE_OPEN					100				/* Fehler Device Open,		  siehe: stComIni[].FrameXOpenStruct */
#define INI_ERR_DO_DEVA						103				/* Fehler Datenmodul deva_,   siehe: DOI_deva[].status */
#define INI_ERR_DO_DEVA_READ				104				/* Leseehler Datenmodul deva_,siehe: DOR_deva[].status */
#define INI_ERR_DO_WERT						105				/* Fehler Datenmodul wert_,   siehe: DOI_wert[].status */
#define INI_ERR_DO_STOER					106				/* Fehler Datenmodul stoer_,  siehe: DOI_stoer[].status */


/* -------------------------------------------*/
/* Fehlerkennungen Stoerschriebuebermittlung  */
/* -------------------------------------------*/

#define DIST_ERR_FAULTNUMBER				1001
#define DIST_ERR_TELEGRAM					1002
#define DIST_ERR_TRY_MAX_TAGS				1003
#define DIST_ERR_TRY_MAX_CHANNEL			1004
#define DIST_ERR_ABORT						1005
#define DIST_ERR_TAG_ABORT					1006
#define DIST_ERR_CHANNEL_ABORT				1007
#define DIST_ERR_TIMEOUT					1008
#define DIST_ERR_VAL_STARTPOS				1009
#define DIST_ERR_VAL_ENDPOS					1010
#define DIST_ERR_NR_OF_CHANNELS				1011
#define DIST_ERR_CHANNEL_TYPE				1012
#define DIST_ERR_MEMORY						1013
#define DIST_ERR_LINK_DOWN					1014
#define DIST_ERR_RECEIVE_ERROR				1015

/* -------------------------------------------*/
/* Fehlerkennungen fuer Kommandos             */
/* -------------------------------------------*/

#define COMMAND_ERR_TIMEOUT					2008
#define COMMAND_ERR_NAK						2020
#define COMMAND_ERR_FUNCTION_NR				2021
#define COMMAND_ERR_INFORMATION_NR			2022
#define COMMAND_ERR_VALUE					2023
#define COMMAND_ERR_RETURNINFORMATION		2024
#define COMMAND_ERR_DISABLED				2025
#define COMMAND_ERR_DEVICE_NOT_READY		2026
#define COMMAND_ERR_DEVICE_ADR				2027
#define COMMAND_ERR_FUNCTION_ACTIVE			2028

/* --------------------------------------------*/
/* Allgemeine Konstanten                       */
/* --------------------------------------------*/

#define FALSE			0
#define TRUE			1

#define ACK				254
#define NAK				255

#define STATE_ERROR_UNDEFINED	0
#define STATE_ERROR_OFF			1
#define STATE_ERROR_ON			2


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

#define TIME_DIFF_SYNCH_MAX		10		/* Erlaubte Zeitdifferenz nach Uhrzeitsynchronisation [s] */
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

/* -------------------------------------------------------------------------*/
/* Kennzeichnung fuer das Speichern von Daten der Stoerschriebuebermittlung */
/* -------------------------------------------------------------------------*/

#define DISTURBANCE_START					1
#define DISTURBANCE_TAGS_START				2
#define DISTURBANCE_TAGS					3
#define DISTURBANCE_VALUES_START			4
#define DISTURBANCE_CHANNEL_START			5
#define DISTURBANCE_VALUES					6
#define DISTURBANCE_END						7

/* -------------------------------------------------------------------------*/
/* Befehlskennungen (Type of Order TOO) nach IEC 60870-5-103 7.2.6.26       */
/* -------------------------------------------------------------------------*/

#define DIST_DATA_TRANS_SUCCESSFULLY		64
#define DIST_DATA_TRANS_NOT_SUCCESSFULLY	65
#define ABORTION_OF_DIST_DATA				3
#define CHANNEL_TRANS_SUCCESSFULLY			66
#define CHANNEL_TRANS_NOT_SUCCESSFULLY		67
#define ABORTION_OF_CHANNEL					9
#define TAGS_TRANS_SUCCESSFULLY				68
#define TAGS_TRANS_NOT_SUCCESSFULLY			69
#define ABORTION_OF_TAGS					17

/* --------------------------------------------------------*/
/* Funktionskennungen bei der Uebertragung von Kommandos   */
/* --------------------------------------------------------*/

#define SEND_COMMAND						1
#define REPEAT_COMMAND						2
#define WAIT_FOR_COMMAND_ACK				3
#define WAIT_FOR_COMMAND_EXE				4

/***********************************************************************/
/* Datentypdeklaration                                                 */
/***********************************************************************/

/* ----------------------------------------*/
/* Konfigurationsvariable des IEC-Treibers */
/* ----------------------------------------*/
typedef struct
{
	unsigned long			TimeOut;					/* Timeout bei Telegrammempfang (in ms) */
	unsigned short			RetryAnz;					/* Anzahl Telegrammwiederholungen bei bestehender Verbindung */
	unsigned short			ConnAnz;					/* Anzahl Telegrammwiederholung bei neu aufzubauender Verbindung */
	unsigned long			WaitIntv;					/* Wartezeit für erneuten Verbindungsaufbau (in ms) */
	unsigned long			TimeTS;						/* Intervallzeit fuer Uhrzeitsynchronisation (in ms) */
	unsigned long			TimeOutTimeSynch;			/* Timeout fuer Rueckmeldetelegramm Zeitsynchronisation (ms) */
	unsigned short			EnableComReset;				/* Freigabe fuer das Schliessen und Wieder-Oeffnen der Schnittstelle bei Schreibfehlern */
	unsigned long			TimeClass2Poll;				/* Abtastzeit Class 2 Telegramme (Analogwerte) (in ms) */
	unsigned long			TimeClass2PollFast;			/* Abtastzeit Class 2 Telegramme (Analogwerte) während Übertragung von Störschrieben (in ms) */
	unsigned short			DistDataEnable;				/* Freigabe fuer Stoerschriebuebertragung */
	unsigned long			DistDataTimeoutMax;			/* Timeout fuer den Telegrammempfang bei Stoerschriebuebertragung in (ms) */
	unsigned long			DisturbanceDelay;			/* Zeit die ein Lesen von Stoerschrieben der gleichen Station verzoegert wird in (ms) */
	unsigned short			CommandEnable;				/* Freigabe fuer Kommandouebertragung */
	unsigned long			CommandTimeoutMax;			/* Timeout fuer die Uebertragung von Kommandos in (ms) */
} IEC_Config_typ;



/* Verwaltung der Unterstationen */

/*---------------------------------------------*/
/* Datenmodulstrukturen 		               */
/*---------------------------------------------*/


/*---------------------------------------------*/
/* NUR FUER URSPRUNGSVARIANTE DES IEC-TREIBERS */
/*---------------------------------------------*/

/* Eintrag im Datenmodul deva.. */
typedef struct {
	USINT	Nummer;
	USINT	Adresse;
	USINT	Aktiv;
	USINT	TimeSynch;
	UINT	AnlagenNr;
	UINT	Frei;
} DeviceType;

/* Eintrag im Datenmodul stoer.. */
typedef struct {
	USINT	Device;
	USINT	Type;
	USINT	F1;
	USINT	F2;
	UINT	Datentyp;
	UINT	DatenpunktNr;
} StoerDM_typ;

/* Eintrag im Datenmodul wert.. */
typedef struct {
	USINT	Device;
	USINT	Type;
	USINT	Info;
	USINT	WertCode;
	UINT	Datentyp;
	UINT	DatenpunktNr;
	UINT	Info1;
	UINT	Info2;
} WertType;

/* Struktur mruk */
typedef struct {
	USINT	Type;
	USINT	Info;
} MrukType;

/* Eintrag im Datenmodul norm */
typedef struct {
	INT		x1;
	INT		y1;
	INT		x2;
	INT		y2;
} NormierungType;

/*-------------------------------------------------------*/
/* BIS HIER: NUR FUER URSPRUNGSVARIANTE DES IEC-TREIBERS */
/*-------------------------------------------------------*/






/*--------------------------*/
/* NUR FUER VARIANTE DbSERV */
/*--------------------------*/

/* Struktur eines Datenpunkt Items im Speicher */
typedef struct {
	USINT	AccessCounter;					/* Zaehle fuer Zugriffe auf das Item */
	USINT	ErrorInfo;						/* Fehlerinformation */
	USINT	DeviceAdr;						/* Geraetadresse */
	USINT	KopplungsStoerung;				/* Item fuer Kopplungsstoerung*/
	USINT	TimeSynchError;					/* Item fuer Fehler bei Zeitsynchronisation: 0=undefiniert, 1= AUS (kein Fehler), 2= EIN (Fehler) */
	USINT	IECType;						/* IEC-Type */
	USINT	IECInfoNr;						/* IEC-Informations Nummer */
	USINT	IECAnalogNr;					/* IEC-Nummer des Analogwertes */
	USINT	MRuc;							/* MRuc verwenden 0=nein/1=ja */
	USINT	MRucType;						/* IEC-Type fuer MRuc */
	USINT	MRucInfo;						/* IEC-Informations Nummer fuer MRuc */
	USINT	Mode;							/* Mode fuer Digitalmeldungen */
	USINT	Normierung;						/* Normierung 0=nein / 1=ja */
	USINT	ItemTypNr;						/* Datentypnummer des Items */
	USINT	Invalid;						/* Invalid Bit des Analogwertes auswerten 0=nein 1=ja */
	USINT	Overflow;						/* Overflow Bit des Analogwertes auswerten 0=nein 1=ja */
	UINT	NormierungIdx;					/* Index fuer Normierung */
	UINT	DPNr;							/* Datenpunktnummer */
	UINT	DPTypNr;						/* Datentypnummer */
	UINT	ItemNr;							/* Item Nummer */
	DINT	ValueDevice;					/* Wert,der vom Geraet empfangen wurde */
	DINT	ValueServer;					/* Wert, der an den Server uebergeben wurde */
} IEC_DP_Info_typ;


/* Struktur eines Normierungseintrages im Speicher */
typedef struct {
	DINT	x1;								/* Eingangswert minimal */
	DINT	y1;								/* Ausgangswert minimal */
	DINT	x2;								/* Eingangswert maximal */
	DINT	y2;								/* Ausgangswert maximal */
} IEC_Normierung_typ;

/*------------------------------------*/
/* BIS HIER: NUR FUER VARIANTE DbSERV */
/*------------------------------------*/


/*-----------------------------------------------*/
/* Strukturen fuer die Verwaltung eines Geraetes */
/*-----------------------------------------------*/

/* Datentyp: Four Octet binary time */

typedef struct {
	USINT	MilliSeconds1;
	USINT	MilliSeconds2;
	USINT	Minutes;
	USINT	Hours;
} FourOctetBinaryTime_typ;

/* Datentyp: Seven Octet binary time */

typedef struct {
	USINT	MilliSeconds1;
	USINT	MilliSeconds2;
	USINT	Minutes;
	USINT	Hours;
	USINT	Day;
	USINT	Month;
	USINT	Year;
	USINT	not_used;
} SevenOctetBinaryTime_typ;

/* Information ueber die Uebertragung eines Stoerschriebes */
typedef struct {
	UINT						Number;						/* Nummer des Stoerschriebes (vom Geraet) */
	USINT						Status;						/* Status des Stoerschriebes (vom Geraet) */
	RTCtime_typ 				Time;						/* Zeitstempel des Stoerschriebes (vom Geraet) */
	SevenOctetBinaryTime_typ	IECTime;					/* Zeitstempel in IEC-Form */
	USINT						Clear;						/* Stoerschrieb kann geloescht werden */
	USINT						Transmitted;				/* Stoerschrieb wurde uebertragen */
	USINT						TypeOfValues;				/* Typ der zu übertragenden Daten */
} FaultInformationType;

/* Information ueber den Verbindungsstatus und den Telegrammverkehr */
typedef struct {
	USINT					Enabled;					/* Verbindung zum Geraet ist aktiviert */
	USINT					Adresse;					/* Geraeteadresse */
	USINT					Verbindungsversuche;
	USINT					ZeitsynchAktiv;
	USINT					StateTimeSynchError;
	UDINT					TimeoutTimeSynch;
	USINT					FCB_Toggle;
	USINT					KE_InitOk;
	UDINT					Timeout_KE;
	UINT					AdresseLeitsystem;
	UINT					AnlagenNr;
	UDINT					ZeitLetzteVerbindung;
	UDINT					ZeitLetzteZeitsync;
	USINT					Class1Request;
	USINT					TimoutNext;
	USINT					GI_Ok;
	UDINT					Timeout_GI;
	USINT					LetzterACD;
	UDINT					Class2Verz;
	USINT					Class1Verz;
	USINT					set_event;
	USINT					VerbindungsStoerung;
	UDINT					Timeouts;
	UDINT					VerbindungsStoerungen;
	UINT					LetzterVerbindungsfehler;
	USINT					NrOfFaults;					/* Anzahl vorhandener Stoerschriebe */
	USINT					FaultFunctionType;			/* Funktionsnummer */
	IEC_DP_Info_typ			*pFaultDPInfo;				/* Zeiger auf Datenpunkt-Info-Struktur im Speicher */
	FaultInformationType	FaultInfo[2];
	UINT					DPInfoFirstEntry;			/* Index des ersten Eintrages im Speicher fuer dieses Device */
	UINT					DPInfoLastEntry;			/* Index del letzten Eintrages im Speicher fuer dieses Device */
	DTStructure				TSynch;						/* Uhrzeit die zuletzt an das Geraet gesendet wurde */
	DTStructure				TDevice;					/* Uhrzeit die vom Geraet empfangen wurde */
	UDINT					TDiff;						/* Zeitdifferenz zwischen gesendeter und empfangener Uhrzeit */
	UINT					TDiffCounter;				/* Fehlerzaehler, Zeitdifferenz zu gross */
} DeviceActivType;


/* Struktur zur Verwaltung der Schnittstelle und des Sende- und Empfangsbuffers */
typedef struct {
	USINT					write_read_activ;
	UDINT					receive_timeout;
	UDINT					timeout;
	UDINT					Ident; 						/* pointer for ident info */
	USINT					WriteData[30];
	USINT					WriteDataLen;
	USINT					sendewiederholung;
	USINT					class1_res_adc1;
	USINT					ReadData[256]; 				/* arrays for write and read data */
	UDINT					RBufferAdr;
	UINT					OpenError;
	UINT					WriteErrorCount;			/* Zaehler fuer fehlerhafte Schreibversuche */
	UINT					WriteError;					/* Letzte Fehlernummer bei fehlerhaftem Schreibversuch */
	UINT					FirstWriteError;			/* Fehlernummer des ersten fehlerhaften Schreibversuches */
	UINT					LastWriteError;				/* Fehlernummer des letzten fehlerhaften Schreibversuches */
	UINT					ReOpenCount;				/* Zaehler, wieoft das Device neu geoeffnet wurde */
} ComType;

/* Struktur zur Verwaltung der verwendeten Datenmodule */
typedef struct{
	UDINT					AdrDM_stoer;
	UINT					MaxWerteDM_stoer;
	UDINT					AdrDM_wert;
	UINT					MaxWerteDM_wert;
	UDINT					AdrDM_mruk;
	UINT					MaxWerteDM_mruk;
	UDINT					AdrDM_norm;
	UINT					MaxWerteDM_norm;
} DMInfoType;

/* Struktur mit Infos ueber die verwendete Schnittstelle */
typedef struct {
	USINT 					StringDevice[32]; 			/* initialize strings for FRM_xopen */
	USINT 					StringMode[80]; 			/* initialize strings for FRM_xopen */
	FRM_xopen_typ			FrameXOpenStruct;
	XOPENCONFIG				XOpenConfigStruct;
} ComIniType;


/* Strukturen zur Uebertragung von Stoerschrieben */
/* Struktur des Headers fuer die Uebergabe an das Leitsystem */
typedef struct {
	UINT						FaultNumber;				/* Fehleridentifizierungsnummer */
	UINT						Status;
	SevenOctetBinaryTime_typ	FaultTime;					/* Zeitpunkt der Störung in IEC-Form */
	UINT						GridFaultNumber;			/* Netzstörungsnummer */
	USINT						NrOfStoredChannels;			/* Anzahl übertragener Kanäle */
	USINT						Res1;						/* Frei */
	UINT						NrOfElementsPerChannel; 	/* Anzahl Werte pro Kanal */
	UINT						IntervallTime;				/* Abtastzeit */
	FourOctetBinaryTime_typ		DistStartTime;				/* Startzeitpunkt der Aufzeichnung in IEC-Form */
	UINT						NrOfTags;					/* Anzahl übertragener Tags */
} DisturbanceHeadType;

/* Struktur eines Tags fuer die Uebergabe ans das Leitsystem */
typedef struct {
	UINT					DatenTyp;
	UINT					DatenpunktNr;
	UINT					Item;
	UINT					TagPosition;
	USINT					Value;
	USINT					Res1;
} DisturbanceTagType;

/* Struktur der Informationen (Teil 1) eines Kanales fuer die Uebergabe ans das Leitsystem */
typedef struct {
	UINT					DatenTyp;
	UINT					DatenpunktNr;
	UINT					Item;
} DisturbanceChannelInfoType;

/* Struktur der Informationen (Teil 2) eines Kanales fuer die Uebergabe ans das Leitsystem */
typedef struct {
	unsigned long			RPV;
	unsigned long			RSV;
	unsigned long			RFA;
} DisturbanceChannelFaktorType;

typedef struct {
	UINT					ErrorNr;					/* Letzte Fehluebertragung: Fehlerkennung */
	UINT					Function;					/* Letzte Fehluebertragung: Funktion */
	UINT					TypeOfOrder;				/* Letzte Fehluebertragung: Type of Order */
	UINT					ChannelNr;					/* Letzte Fehluebertragung: Kanalnummer */
}	DistTransErrorType;

/* Verwaltung der Uebertragung eines Stoerschriebes */
typedef struct {
	UINT					Function;					/* Momentane Übertragungsfunktion */
	USINT					DataSend;					/* Auftrag Daten senden */
	USINT					WaitForResponse;			/* Warten auf Antwort */
	USINT					TypeOfOrder;				/* Befehlskennung (abort, acknowledge,..) */
	USINT					DeviceAtWork;				/* Gerätenummer (1..) fuer Uebertragung (aus Array DeviceActiv) */
	USINT					FunctionType;				/* Funktionsnummer */
	USINT					TypeOfValues;				/* Typ der zu übertragenden Daten */
	UINT					NrOfReceivedTags;			/* Anzahl empfangener Tags */
	UINT					NrOfReceivedChannels;		/* Anzahl empfangender Kanäle */
	UINT					NrOfChannels;				/* Anzahl zu übertragender Kanäle */
	UINT					ActualChannelTyp;			/* Kennnummer des zu übertragenden Kanales */
	UINT					LastChannelTyp;				/* Kennnummer des zuletzt uebertragenen Kanales */
	UINT					EstimateChannelPos;			/* Erwartete Startposition */
	USINT					NewChannel;					/* Ein neuer Kanal wird uebertragen */
	USINT					StoreChannel;				/* Analogkanal speichern */
	UINT					ChannelDataError;			/* Fehlermerker, falls beim Uebertragen eines Kanales Fehler auftreten */
	UDINT					TimeOut;					/* Timeoutzaehler */
	USINT					Trys;						/* Versuche Daten zu uebertragen */
	USINT					FaultIdx;					/* Index zu uebertragenden Stoerschriebes */
	USINT					BufferIdx;					/* Bufferindex fuer die Stoerschriebdaten */
	UINT					Delay;						/* Verzoegerung beim Empfangen von Stoerschrieben gleicher Stationen */
	DistTransErrorType		ActError;					/* Fehlerspeicher fuer laufende Uebertragung */
	DistTransErrorType		LastError;					/* Fehlerspeicher der letzten Uebertragung */
} DisturbanceDataType;

/* Struktur zur Ausloesung von Kommandos */
typedef struct {
	UINT					Function;					/* Momentane Übertragungsfunktion */
	USINT					DataSend;					/* Auftrag Daten senden */
	USINT					WaitForResponse;			/* Warten auf Antwort */
	USINT					DeviceAdress;				/* GeräteAdresse (alternativ DeviceAtWork) */
	USINT					DeviceAtWork;				/* Gerätenummer (alternativ DeviceAdress) (1..) fuer Uebertragung (aus Array DeviceActiv) */
	USINT					FunctionType;				/* Funktionsnummer */
	USINT					InformationNumber;			/* Informationsnummer */
	USINT					Val;						/* Wert, der zu schreiben ist */
	USINT					ReturnVal;					/* Wert, der vom Geraet zurueckgemeldet wurde */
	USINT					ReturnInformation;			/* Rueckmeldungswert */
	UINT					TimeOut;					/* Timeoutzaehler */
	USINT					Trys;						/* Versuche Daten zu uebertragen */
	USINT					Ready;						/* Kommando uebertragen */
	UINT					Error;						/* Fehlermeldung */
} CommandType;

/* Struktur zur Vewaltung einer gesamten Linie */
typedef struct {

	UINT					LinienNummer;
	UINT					SNr;
	DeviceType 				DeviceTAB[DEVICES_PRO_LINE];
	DeviceActivType 		DeviceActiv[DEVICES_PRO_LINE];
	DisturbanceDataType		DistData;
	DisturbanceHeadType		DistHead;
	CommandType				Command;
	USINT 					DeviceAtWork;
	USINT					Error;
	USINT					ComStart;
	DMInfoType				dm;
	ComType					com;
} AnlagenType;


/* Struktur zum Speichern von Kontrollelementen der Telegramme */

typedef struct {
	USINT				ACD;
	USINT				DFC;
	USINT				Function;
} ControlFieldPrimary_typ;

/* Strukturen fuer Testzwecke */
typedef struct {
	USINT 					enable;
	USINT					ready;
	UINT					LinienNummer;
	USINT 					DeviceAdresse;
	USINT 					FunctionType;
	USINT 					InformationNumber;
	USINT 					Messwert;
} TestTypeDig;

typedef struct {
	USINT 					enable;
	USINT					ready;
	UINT					LinienNummer;
	USINT 					DeviceAdresse;
	USINT 					FunctionType;
	USINT 					InformationNumber;
	USINT 					AnzahlMesswerte;
	INT 					Messwerte[MAX_ANALOGWERTE];
} TestTypeAna;

typedef struct {
	USINT 					enable;
	USINT 					Adresse;
} TestTypeGlobal;






typedef struct {
	USINT					DataReady;
	USINT					ClearData;
	USINT					BufferIdx;
} DisturbanceDataServer;


typedef struct {
	USINT					Code;
	USINT					SetEventStream;
	DeviceActivType			*DevicePointer;
	UINT					FaultNumber;
	UDINT					DataLen;
	UDINT					DataAdr;
	UDINT					Size;
	UINT					status;
} DisturbanceBufferInfo;

typedef struct {
	UINT					NrOfBuffers;
	DisturbanceBufferInfo	Info[NR_OF_BUFFERS];
} DisturbanceBuffer;


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
void Zeitmessung(AnlagenType *pANL);

void Device_Reaktivieren(AnlagenType *pANL);

USINT Data_write(AnlagenType *pANL);

void Data_read(AnlagenType *pANL);

USINT save_analogwert(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType, USINT InformationNumber, USINT AnzahlMesswerte,
						USINT *AdresseMesswerte, USINT set_event);

USINT save_analogwert_priv(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
							USINT FunctionType, USINT InformationNumber, USINT AnzahlMesswerte,
							USINT *AdresseMesswerte, USINT set_event);

USINT save_counter_2(DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
							USINT FunctionType, USINT InformationNumber, USINT AnzahlMesswerte,
							USINT *AdresseMesswerte, USINT set_event);

void save_analogwert_error(	DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT set_event);

USINT save_digitalwert( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv,
						USINT FunctionType,USINT InformationNumber, USINT Messwert,
						RTCtime_typ *TimeBuf, USINT set_event);

USINT save_kopplungsstoerung( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv, USINT Messwert, RTCtime_typ *TimeBuf, USINT set_event);

USINT save_TimeSynchError( DMInfoType *pDMInfo, UINT LinienNummer, DeviceActivType *pDeviceActiv, USINT Messwert, RTCtime_typ *TimeBuf, USINT set_event);

void get_4OctetBinaryTime(USINT *data, RTCtime_typ *TimeBuf, RTCtime_typ *SysTime);

void get_7OctetBinaryTime(USINT *data, RTCtime_typ *TimeBuf, RTCtime_typ *SysTime);

void debug_initialize(void);

void debug_open_device(USINT enable);

short int send_buffer(ComType *pCom, UINT LinienNummer, USINT Adresse);

void clear_buffer(ComType *pCom);

USINT MakeCRC(char *data, USINT start, USINT laenge);

void Reset_KE(UINT LinienNummer, DeviceActivType *pDeviceActiv, ComType *pCom);

void Zeit_Sync(UINT LinienNummer,DeviceActivType *pDeviceActiv, ComType *pCom);

void Init_GI(UINT LinienNummer,DeviceActivType *pDeviceActiv, ComType *pCom);

void Class_1_Request(UINT LinienNummer,DeviceActivType *pDeviceActiv, ComType *pCom);

void Class_2_Request(UINT LinienNummer,DeviceActivType *pDeviceActiv, ComType *pCom);

void disturbance_data_request(AnlagenType *pANL);

void disturbance_data_response(AnlagenType *pANL,USINT code, USINT info);

void disturbance_data_timeout(AnlagenType *pANL);

void clear_dist_data_request(AnlagenType *pANL);

signed short save_disturbance_data(UINT code, AnlagenType *pANL);

void check_device_dist_data(AnlagenType *pANL);

void check_dist_data_transmission();

void abort_disturbance_data(AnlagenType *pANL);

void save_disturbance_error(AnlagenType *pANL,UINT Error, UINT Function, UINT TypeOfOrder, UINT Channel);

void command_request(AnlagenType *pANL);

void command_timeout(AnlagenType *pANL);

void command_response(AnlagenType *pANL);

void GetNewDevice(AnlagenType *pANL);

void Empfangsfehler(UINT nr, AnlagenType *pANL);

void timesynch_timeout(AnlagenType *pANL);


USINT check_m1(AnlagenType *pANL);

USINT check_m2(AnlagenType *pANL);

USINT check_m3(AnlagenType *pANL);

USINT check_m5(AnlagenType *pANL);

USINT check_m6(AnlagenType *pANL);

USINT check_m8(AnlagenType *pANL);

USINT check_m9(AnlagenType *pANL);

USINT check_m23(AnlagenType *pANL);

USINT check_m26(AnlagenType *pANL);

USINT check_m27(AnlagenType *pANL);

USINT check_m28(AnlagenType *pANL);

USINT check_m29(AnlagenType *pANL);

USINT check_m30(AnlagenType *pANL);

USINT check_m31(AnlagenType *pANL);

USINT check_m79(AnlagenType *pANL);

USINT check_m205(AnlagenType *pANL);

void tel_ok(AnlagenType *pANL);


void get_idBrServer();

void disturbance_simulation();

BOOL DeviceIsActive(AnlagenType *pANL, USINT DeviceAddr);

/**********************************************************
*				Globale Variablen Declaration
**********************************************************/


/**********************************************************
*				Lokale Variablen Declaration
**********************************************************/

_LOCAL	IEC_Config_typ					IEC_Config;

_LOCAL	UDINT							myidBrServer;
_LOCAL_RETAIN	UINT							statePV_xgetadr;
_LOCAL	UDINT							pv_adresse, data_len;
_LOCAL  UINT							StatusSetValueItem;
_LOCAL	UINT							SetEventStreamStatus;
_LOCAL	UINT							RelEventStreamStatus;


_LOCAL	RTInfo_typ						rt_info;
_LOCAL  UINT							CycleTime;

_LOCAL AnlagenType						stANL[MAX_LINES];
_LOCAL  ComIniType						stComIni[MAX_LINES];
_LOCAL RTCtime_typ						RPSTime;
_LOCAL USINT							RPSTimeOK;

_LOCAL DisturbanceDataServer			DistDataServer;
_LOCAL UINT								DistDataType;
_LOCAL UINT								DistDataPoint;

_LOCAL FRM_rbuf_typ						FrameReleaseBufferStruct;
_LOCAL FRM_close_typ 					FrameCloseStruct;
_LOCAL FRM_write_typ					FrameWriteStruct;
_LOCAL FRM_read_typ						FrameReadStruct;
_LOCAL FRM_write_typ					DebugWriteStruct;

_LOCAL RTCtime_typ 						TimeBuf;

_LOCAL USINT 							Error,ReadError;
_LOCAL UINT								StatusRead, StatusReleaseBuffer;
_LOCAL UDINT 							*ReadBuffer; 								/* pointer to read buffer */
_LOCAL UINT  							*ReadBufferLen;
_LOCAL UINT 							ReadBufferLength;


_LOCAL UINT								EFehler;
_LOCAL USINT							EmpfangeneMesswerte,InfoElemente;

_LOCAL	DatObjInfo_typ					DOI_wert[MAX_LINES],DOI_deva[MAX_LINES],DOI_stoer[MAX_LINES],DOI_mruk,DOI_norm,DOI_lines;
_LOCAL	DatObjRead_typ					DOR_wert[MAX_LINES],DOR_deva[MAX_LINES],DOR_stoer[MAX_LINES],DOR_mruk,DOR_norm,DOR_lines;

_LOCAL	DisturbanceTagType				DisturbanceTagInfo;
_LOCAL	DisturbanceChannelInfoType		DisturbanceChannelInfo;
_LOCAL	DisturbanceChannelFaktorType	DisturbanceChannelFaktor;

_LOCAL	unsigned long					ChannelDataLen,CopySize,Offset,Anzahl,Adresse;
_LOCAL	unsigned short					StartNr;
_LOCAL	USINT							FunctionType,InformationNumber,NrOfTags,aktTag;
_LOCAL	USINT							Messwert,WertLS0,WertLS1,found;

_LOCAL	DisturbanceBuffer				DataBuffer;

_LOCAL UINT								ActiveLines;
_LOCAL USINT							aktANL,maxANL;

_LOCAL ControlFieldPrimary_typ			ControlFieldPrimary;

_LOCAL TestTypeDig						DigTest;
_LOCAL TestTypeAna						AnaTest;
_LOCAL TestTypeGlobal					GlobalTest;


_LOCAL	DatObjInfo_typ					DOISimu;
_LOCAL	BOOL							DisturbanceSimulation;

_LOCAL_RETAIN Stats_Typ						SYS_Statistic	;

_LOCAL	USINT							Overflow[MAX_ANALOGWERTE];
_LOCAL	USINT							Invalid[MAX_ANALOGWERTE];

_LOCAL USINT IEC_Info_Clear;
_LOCAL USINT IEC_Info_Enable;


/*** eof *****************************************************************************/



