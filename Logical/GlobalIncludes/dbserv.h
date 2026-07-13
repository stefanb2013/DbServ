
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  DbServ
*  Teil:     Parser-Funktionen
*  Datei:    dbserv.h
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.03
*  Datum:    04-08-2003
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Globale Include-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.03 | 26-07-04 | MKrz | Typzuordnung verbessert
*  01.01 | 06-07-04 | MKrz | Fehlermeldungen angepasst
*  01.00 | 04-08-03 | MKrz | Datei-Erstellung
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/

#define FALSE	0	/* logisch falsch */
#define TRUE	1	/* logisch wahr */

#ifndef ERR_FUB_BUSY
	#define ERR_FUB_BUSY	0xFFFF
#endif

/* Instanznummern Schreiben */
enum {	cINSTANCE_DBSLIB,
		cINSTANCE_IODRV,
		cINSTANCE_IEC103,
		cINSTANCE_L2DP	};

/* Parser-Zustände */
enum {	cCD_PARSER_STATE_DEF_TCP,			/* Parser-Zustand  0: Tcp-Definitionen */

		cCD_PARSER_STATE_DEF_BUF,			/* Parser-Zustand  1: Buf-Definitionen */

		cCD_PARSER_STATE_COUNT_TYPE,		/* Parser-Zustand  2: Zählen der Datentypen */
		cCD_PARSER_STATE_DEF_TYPE,			/* Parser-Zustand  3: Anmelden der Datentypen */

		cCD_PARSER_STATE_COMPARE_DO_TYPE,	/* Parser-Zustand  4: Abgleich der Datentypdefinitionen */
		cCD_PARSER_STATE_DEL_DO_TYPE,		/* Parser-Zustand  5: Löschen des Datenobjektes für Datentypdefinitionen */
		cCD_PARSER_STATE_CREATE_DO_TYPE,	/* Parser-Zustand  6: Anlegen des Datenobjektes für Datentypdefinitionen */

		cCD_PARSER_STATE_COUNT_DP,			/* Parser-Zustand  7: Zählen der Datenpunkte */
		cCD_PARSER_STATE_DEF_DP,			/* Parser-Zustand  8: Anmelden der Datenpunkte */

		cCD_PARSER_STATE_COMPARE_DO_POINT,	/* Parser-Zustand  9: Abgleich der Datentypdefinitionen */
		cCD_PARSER_STATE_DEL_DO_POINT,		/* Parser-Zustand 10: Löschen des Datenobjektes für Datentypdefinitionen */
		cCD_PARSER_STATE_CREATE_DO_POINT,	/* Parser-Zustand 11: Anlegen des Datenobjektes für Datentypdefinitionen */

		cCD_PARSER_STATE_ALLOC_ACCESS_DATA,	/* Parser-Zustand 12: Prozessabbild allokieren */

		cCD_PARSER_STATE_COMPARE_DO_DATA,	/* Parser-Zustand 13: Abgleich der Datentypdefinitionen */
		cCD_PARSER_STATE_DEL_DO_DATA,		/* Parser-Zustand 14: Löschen des Datenobjektes für Datentypdefinitionen */
		cCD_PARSER_STATE_CREATE_DO_DATA,	/* Parser-Zustand 15: Anlegen des Datenobjektes für Datentypdefinitionen */

		cCD_PARSER_STATE_WORK_DATA,			/* Parser-Zustand 16: */

		cCD_PARSER_STATE_COMPARE_DO_EVENT,	/* Parser-Zustand 17: Abgleich der Datentypdefinitionen */
		cCD_PARSER_STATE_DEL_DO_EVENT,		/* Parser-Zustand 18: Löschen des Datenobjektes für Datentypdefinitionen */
		cCD_PARSER_STATE_CREATE_DO_EVENT,	/* Parser-Zustand 19: Anlegen des Datenobjektes für Datentypdefinitionen */

		cCD_PARSER_STATE_GET_DEVMEMINFO,	/* Parser-Zustand 20: */
		cCD_PARSER_STATE_WORK_EVENT,		/* Parser-Zustand 21: */

		cCD_PARSER_STATE_READY,				/* Parser-Zustand 22: Fertig */
		cCD_PARSER_STATE_ERROR,				/* Parser-Zustand 23: Fehler */
		cCD_PARSER_STATE_ERROR_READY	};	/* Parser-Zustand 24: Fertig mit Fehler */

/* Default-Werte */
#define cTCP_DEFAULT_PORT			20001		/* Default Port */
#define cTCP_DEFAULT_TIMEOUT		20			/* Default Timeout */

#define cDBS_DEFAULT_UNIT			1			/* Default Anlage */
#define cDBS_DEFAULT_DEVICE			1			/* Default Gerät */

/* Max-Werte */
#define cMAX_UNIT_DEF				65535		/* maximaler Wert */
#define cMAX_TYPE_DEF				65535		/* maximaler Wert */
#define cMAX_POINT_DEF				65535		/* maximaler Wert */

/* Bezeichner */
#define cKW_DBS_DM_DP_LIST			"dplist"	/* Datenmodul-Name 1 */
#define cKW_DBS_DM_TYP_LIST			"typlist"	/* Datenmodul-Name 2 */

#define cKW_ITEM_SECTION			"ITEM"		/* Schlüsselwort für Item-Section */
#define cKW_ALL_SECTION				"ALL"		/* Schlüsselwort für All-Section */
#define cKW_DBS_SECTION				"DBS"		/* Schlüsselwort für Dbs-Section */

#define cKW_BUF_SECTION				"BUF"		/* Schlüsselwort für Buf-Section */


#define cKW_DBS_CLIENT				"Client"	/* Instanzen */
#define cKW_DBS_CLIENT_COUNT		"Anz"		/* Instanzen-Anzahl */
#define cKW_DBS_APP					"App"		/* Applikation */
#define cKW_DBS_APP_COUNT			"Anz"		/* Applikation-Anzahl */
#define cKW_DBS_UNIT				"Anlage"	/* Anlage */
#define cKW_DBS_UNIT_NUMBER			"Nr"		/* Anlagen-Nummer */
#define cKW_DBS_TYPE				"Typ"		/* Datentyp */
#define cKW_DBS_TYPE_NUMBER			"Nr"		/* Datentyp-Nummer */


#define cKW_DBS_TYPE_FILE_SIZE		"FbSize"	/* Datentyp-Filebuffer Grösse */
#define cKW_DBS_TYPE_FILE_MODE		"FbMode"	/* Datentyp-Filebuffer Modus */

#define cKW_DBS_BUF_ATTRIBUTE		"Buf"		/* Datentyp-Filebuffer Modus */
#define cKW_DBS_BUF_EVENTS			"Events"	/* Datentyp-Filebuffer Modus */
#define cKW_DBS_BUF_TYPE			"Typ"		/* Datentyp-Filebuffer Modus */


#define cKW_DBS_ITEM				"Item"		/* Datentyp-Item */
#define cKW_DBS_ITEM_NUMBER			"Nr"		/* Datentyp-Item-Nummer */
#define cKW_DBS_ITEM_NAME			"Name"		/* Datentyp-Item-Name */
#define cKW_DBS_TYPE_TIMESTAMP		"TS"		/* Datentypbezeichner für Zeitstempel */
#define cKW_DBS_POINT				"Dp"		/* Datenpunkt */
#define cKW_DBS_POINT_NUMBER		"Nr"		/* Datenpunkt-Nummer */
#define cKW_DBS_FUB					"Fub"		/* Funktionsblock */

#define cKW_DBS_TYPE_BOOL			"_BOOL"		/* Datentypbezeichner für BOOL */
#define cKW_DBS_TYPE_USINT			"_USINT"	/* Datentypbezeichner für USINT */
#define cKW_DBS_TYPE_SINT			"_SINT"		/* Datentypbezeichner für SINT */
#define cKW_DBS_TYPE_UINT			"_UINT"		/* Datentypbezeichner für UINT */
#define cKW_DBS_TYPE_INT			"_INT"		/* Datentypbezeichner für INT */
#define cKW_DBS_TYPE_UDINT			"_UDINT"	/* Datentypbezeichner für UDINT */
#define cKW_DBS_TYPE_DINT			"_DINT"		/* Datentypbezeichner für DINT */
#define cKW_DBS_TYPE_STREAM			"_STREAM"	/* Datentypbezeichner für STREAM */

/* Typ-IDs */
enum {	cDBS_IDTYPE_UNDEF,
		cDBS_IDTYPE_BOOL,
		cDBS_IDTYPE_USINT,
		cDBS_IDTYPE_SINT,
		cDBS_IDTYPE_UINT,
		cDBS_IDTYPE_INT,
		cDBS_IDTYPE_UDINT,
		cDBS_IDTYPE_DINT,
		cDBS_IDTYPE_STREAM	};

/* Typ-Eigenschaften */
enum {	cDBS_TYPE_UNSIGNED,					/* vorzeichenloser Typ */
		cDBS_TYPE_SIGNED,					/* vorzeichenbehafteter Typ */
		cDBS_TYPE_STREAM	};				/* Zeiger auf Datenstrom */

/* Bezeichner */
#define cKW_TCP_PORT			"Client"	/* Port */
#define cKW_TCP_PORT_NUMBER		"Port"		/* Port-Nummer */

#define cKW_TCP_TIMEOUT			"Client"	/* Timeout */
#define cKW_TCP_TIMEOUT_NUMBER	"Timeout"	/* Timeout-Wert */

#define cKW_TCP_STATION			"UST"		/* Schlüsselwort für Station */
#define cKW_TCP_SECTION			"TCP"		/* Schlüsselwort für Station */

/* Fehler-Nummern */
#define EC_MEM_ALLOC_NEED			0	/* Benötigter Mem-Speicher */
#define ET_MEM_ALLOC_NEED			"MEM="

#define EC_TMP_ALLOC_NEED			1	/* Benötigter Tmp-Speicher */
#define ET_TMP_ALLOC_NEED			"kB, TMP="

#define EC_DPS_FOUND				2	/* gefundene Datenpunkte */
#define ET_DPS_FOUND				"gef.DP="

#define EC_DPS_NOT_FOUND			3	/* nicht gefundene Datenpunkte */
#define ET_DPS_NOT_FOUND			", n.gef.DP="

#define EC_ITEM_NOT_FOUND			4	/* nicht gefundenes Item */
#define ET_ITEM_NOT_FOUND			", n.gef.ITEM="

#define EC_ITEM_INVALID_FOUND		5	/* nicht gefundenes Item */
#define ET_ITEM_INVALID_FOUND		"unerw.gef.ITEM="

#define EC_TCP_READY				6	/* TCP bereit */
#define ET_TCP_READY				" bereit"

#define EC_TCP_NOT_READY			7	/* TCP nicht bereit */
#define ET_TCP_NOT_READY			" nicht bereit"

#define EC_DA_NOT_FOUND				8	/* Datenmodul nicht gefunden */
#define ET_DA_NOT_FOUND				"Datenmodul nicht vorhanden"

#define EC_TMP_ALLOC				9	/* Fehler bei Allokieren von Speicher */
#define ET_TMP_ALLOC				"Fehler bei Allokieren von Speicher"

#define EC_ITEM_NO_INVALID			10	/* Item-Nummer falsch */
#define ET_ITEM_NO_INVALID			":ITEM Nr falsch: "

#define EC_ITEM_ALREADY_EXIST		11	/* Item-Name falsch */
#define ET_ITEM_ALREADY_EXIST		":ITEM Name falsch: "

#define EC_DP_FOUND					12	/* Datenpunkt gefunden */
#define ET_DP_FOUND					":DP gefunden: "

#define EC_DP_NOT_FOUND				13	/* Datenpunkt nicht gefunden */
#define ET_DP_NOT_FOUND				":DP nicht gefunden:"

#define EC_TCP_VERS					14	/* Zeichen für Vollversion */
#define ET_TCP_VERS					"V"

#define EC_TCP_NOT_VERS				15	/* Zeichen für Vorabversion */
#define ET_TCP_NOT_VERS				"B"

#define EC_PARSER_READY				16	/* Parser bereit */
#define ET_PARSER_READY				":Parser bereit"

#define EC_PARSER_STATE				17	/* Fehler im Parser-Zustand */
#define ET_PARSER_STATE				":Syntax-Fehler -"

#define EC_PARSER_STATIONTYPPOINT	18	/* Fehler im Parser-Zustand: Station/Typ/Punkt */
#define ET_PARSER_STATIONTYPPOINT	":DP-Attr unbekannt"

#define EC_PARSER_SECTION			19	/* Fehler im Parser-Zustand: Section */
#define ET_PARSER_SECTION			":DP-Attr unbekannt"

#define EC_PARSER_DATA				20	/* Fehler im Parser-Zustand: Data */
#define ET_PARSER_DATA				":DP-Attr unbekannt"

#define EC_NO_PGLOBALS				21	/* Fehler, da kein gültiger pGlobals-Pointer */
#define ET_NO_PGLOBALS				":kein pGlobals"

#define EC_PORT_ALREADY_EXISTS		22	/* Fehler, da Port bereits vergeben */
#define ET_PORT_ALREADY_EXISTS		":Port bereits vorh."

#define EC_TIMEOUT_ALREADY_EXISTS	23	/* Fehler, da Tiemout bereits vergeben */
#define ET_TIMEOUT_ALREADY_EXISTS	":Timeout bereits vorh"

#define EC_TYPE_ALREADY_EXIST		24	/* Fehler, da Datentyp bereits existiert */
#define ET_TYPE_ALREADY_EXIST		":TYP Name falsch"

#define EC_TYPE_NO_INVALID			25	/* Fehler, da Datenpunkt bereits existiert */
#define ET_TYPE_NO_INVALID			":TYP Nr falsch"

#define EC_POINT_ALREADY_EXIST		26	/* Fehler, da Datenpunkt bereits existiert */
#define ET_POINT_ALREADY_EXIST		":Dp bereits vorhanden"

#define EC_NO_LINK_FUB_DBSERV		27	/* Fehler, da Datenpunkt bereits existiert */
#define ET_NO_LINK_FUB_DBSERV		"Verkettung unvollständig"

#define EC_NO_LINK_DBSERV_FUB		28	/* Fehler, da Datenpunkt bereits existiert */
#define ET_NO_LINK_DBSERV_FUB		"Verkettung unvollständig"

#define EC_NO_TIMESTAMP_FUB			29	/* Fehler, da Datenpunkt bereits existiert */
#define ET_NO_TIMESTAMP_FUB			"Fub ohne Zeitstempel"

#define EC_BUF_EVENTS				30	/* Fehler, da Datenpunkt bereits existiert */
#define ET_BUF_EVENTS				"Archiv EventAnz="

#define EC_MEM_EVENTS				31	/* Fehler, da Datenpunkt bereits existiert */
#define ET_MEM_EVENTS				"Archiv Mem="


#define EC_BOOL_PREFIX				50
#define ET_BOOL_PREFIX				"BOOL"

#define EC_BOOL_TS_PREFIX			51
#define ET_BOOL_TS_PREFIX			"BOOL_TS"

#define EC_USINT_PREFIX				52
#define ET_USINT_PREFIX				"USINT"

#define EC_USINT_TS_PREFIX			53
#define ET_USINT_TS_PREFIX			"USINT_TS"

#define EC_UINT_PREFIX				54
#define ET_UINT_PREFIX				"UINT"

#define EC_INT_PREFIX				55
#define ET_INT_PREFIX				"INT"

#define EC_UDINT_PREFIX				56
#define ET_UDINT_PREFIX				"UDINT"

#define EC_DINT_PREFIX				57
#define ET_DINT_PREFIX				"DINT"

#define EC_MAX						60	/* Anzahl Fehlereinträge */

/***********************************************************************/
/* Datentypdeklaration                                                 */
/***********************************************************************/

typedef plcbit BIT;			/* Datentyp BIT */

/* -------------------------------- */
/* Datentypen für Item-Deskriptoren */
/* -------------------------------- */
typedef struct {
	unsigned short	offItem;
	unsigned short	lenItem;
	unsigned char	sItem;
	unsigned char	tsItem;
	unsigned char	FileBuffer;
	unsigned char	idTypeItem;
	unsigned char	*pNameItem;
} ITEM_DESC;

/* ------------------------------- */
/* Datentypen für Typ-Deskriptoren */
/* ------------------------------- */
typedef struct {
	unsigned short	dbsType;
	unsigned short	sizeType;
	unsigned short	sizeIntType;
	unsigned char	*pTypeName;
	unsigned short	countItem;
	unsigned short	countTimeItem;
	ITEM_DESC		*pItemDesc;
	unsigned short	countRelatedPoints;
	unsigned char	FileBuffer;
	unsigned char	ModeFileBuffer;
	unsigned long	EventFileBuffer;
	unsigned long	SizeFileBuffer;
} TYPE_DESC;

/* --------------------------------- */
/* Datentypen für Punkt-Deskriptoren */
/* --------------------------------- */
typedef struct {
	unsigned short	dbsUnit;
	unsigned short	dbsType;
	unsigned short	dbsPoint;
	unsigned char	*pPointName;
} POINT_DESC;

/* -------------------------------------- */
/* Datentypen für Fubinstanz-Deskriptoren */
/* -------------------------------------- */
typedef struct {
	unsigned short	dbsUnit;
	unsigned short	dbsType;
	unsigned short	dbsPoint;
	unsigned short	dbsItem;
	unsigned char	*pFubInstance;
} FUB_DESC;

/* ------------------------- */
/* Datentypen für Eventpipes */
/* ------------------------- */
typedef struct {
	UDINT	countEvent;
	UDINT	firstEvent;
	UDINT	lastEvent;
} PIPE_EVENT_STATE;

typedef struct {
	PIPE_EVENT_STATE	stateEvent;
	UINT	pipe_entry;
	UINT	pipe_size;
	UINT	pipe_level;
	USINT*	pPipeData;
	UINT	iPipeRead;
	UINT	iPipeWrite;
} EVENT_PIPE_HEADER;

typedef struct {
	USINT	linkstate;
	USINT	eventtype;
	UINT	unit;
	UINT	device;
	UINT	type;
	UINT	point;
	UINT	item;
	UDINT	length;
	USINT*	pData;
} EVENT_PIPE_DATA;

typedef struct {
	EVENT_PIPE_HEADER*	pEventPipeHeaderWrite[2];
	EVENT_PIPE_HEADER*	pEventPipeHeaderRead;
	UINT				BufferEventIndex;
	UINT				BufferFileIndex;
	BOOL				WriteDataBusy;
	BOOL				ReadDataBusy;
	BOOL				GetEventsFromReadPipe;

	UDINT				xlength_pipe_next;
/*
	BOOL				WriteToFile;
	USINT				ModeFileBuffer;
	UDINT				SizeFileBuffer;
*/
	USINT				FileDevice[32];
	USINT				FileName[32];
	UDINT				fileIdent;
	UDINT				fileLen;
	UDINT				fileOffsetWrite;
	UDINT				fileOffsetRead;
	UINT				fileStep;
	UINT				fileState;
	UINT				fileSysError;

	FileOpen_typ		iFileOpen;
	FileCreate_typ		iFileCreate;
	FileWrite_typ		iFileWrite;
	FileRead_typ		iFileRead;
	FileClose_typ		iFileClose;
	FileDelete_typ		iFileDelete;
} EVENT_PIPE_INSTANCE;

typedef struct {
	BOOL					WriteToFile;
	USINT					ModeFileBuffer;
	UDINT					SizeFileBuffer;
	EVENT_PIPE_INSTANCE*	pEventPipeInstance;
} ACCESS_EVENT_PIPE;

typedef struct {
	UINT	pipe_entry;
	UINT	pipe_size;
	UINT	pipe_level;
	USINT*	pPipeData;
	UINT	iPipeRead;
	UINT	iPipeWrite;

	UDINT	TickCount;
	UINT	iLastPipeRead;
	UINT	unit;
	UINT	device;
	UINT	type;
	UINT	point;
	UINT	item;
} WRITE_PIPE_HEADER;

typedef struct {
	USINT	linkstate;
	USINT	eventtype;
	UINT	unit;
	UINT	device;
	UINT	type;
	UINT	point;
	UINT	item;
	UDINT	length;
	USINT*	pData;
} WRITE_PIPE_DATA;

typedef struct {
	WRITE_PIPE_HEADER*	pWritePipeHeader;
	UDINT				length_pipe_next;
} ACCESS_WRITE_PIPE;

/* ---------------------------------------- */
/* Datentypen für Zugriff auf Prozessabbild */
/* ---------------------------------------- */
typedef struct {
	unsigned char	ItemEventState;
} ITEM_EVENT_STATE;

typedef struct {
	UDINT	noEvent;
} POINT_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} TYPE_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} UNIT_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} SERVER_EVENT_STATE;

/* ---------------------------------------- */
/* Datentypen für Zugriff auf Prozessabbild */
/* ---------------------------------------- */
typedef USINT	ACCESS_ITEM_VALID;
typedef USINT	ACCESS_ITEM_EVENT_LINK;
typedef USINT	ACCESS_ITEM_WRITE_LINK;
typedef UDINT	ACCESS_ITEM_WRITE_HANDLE;
typedef USINT	ACCESS_ITEM_RELEASE;

typedef USINT	ACCESS_ITEM_EVENT_STATE;

typedef USINT*	ACCESS_FUB_INSTANCE;

typedef unsigned char	ACCESS_DATA;

typedef struct {
	unsigned short	countItem;
	ACCESS_DATA		*pAccessData;
} ACCESS_POINT;

typedef struct {
	unsigned short	countPoint;
	ACCESS_POINT	*pAccessPoint;

	unsigned long	offExtern;
	unsigned long	lenExtern;

	unsigned long	offIntern;
	unsigned long	lenIntern;

	unsigned long	offiExtern;
	unsigned long	leniExtern;
	unsigned long	leniExternAlign;
	unsigned long	leniExternNext;		/* Abstand für Instanzen */

	unsigned long	offValid;
	unsigned long	lenValid;
	unsigned long	lenValidAlign;		/* Abstand für Items */

	unsigned long	offTime;
	unsigned long	lenTime;
	unsigned long	lenTimeAlign;		/* Abstand für Items */

	unsigned long	offiTime;
	unsigned long	leniTime;
	unsigned long	leniTimeAlign;		/* Abstand für Items */
	unsigned long	leniTimeNext;		/* Abstand für Instanzen */

	unsigned long	offWritelink;
	unsigned long	lenWritelink;
	unsigned long	lenWritelinkAlign;	/* Abstand für Items */

	unsigned long	offWritehandle;
	unsigned long	lenWritehandle;
	unsigned long	lenWritehandleAlign;/* Abstand für Items */

	unsigned long	offiEventlink;
	unsigned long	leniEventlink;
	unsigned long	leniEventlinkAlign;	/* Abstand für Items */
	unsigned long	leniEventlinkNext;	/* Abstand für Instanzen */

	unsigned long	offiReleasestream;
	unsigned long	leniReleasestream;
	unsigned long	leniReleasestreamAlign;	/* Abstand für Items */
	unsigned long	leniReleasestreamNext;	/* Abstand für Instanzen */

	unsigned long	offiHyst;
	unsigned long	leniHyst;
	unsigned long	leniHystAlign;
	unsigned long	leniHystNext;		/* Abstand für Instanzen */

	unsigned long	offiEvent;
	unsigned long	leniEvent;
	unsigned long	leniEventAlign;		/* Abstand für Items */
	unsigned long	leniEventNext;		/* Abstand für Instanzen */

	unsigned long	offFubInstance;
	unsigned long	lenFubInstance;
	unsigned long	lenFubInstanceAlign;
} ACCESS_TYPE;

typedef struct {
	unsigned short	countType;
	ACCESS_TYPE*	pAccessType;
} ACCESS_UNIT;

typedef struct {
	unsigned short		countEventInstance;
	unsigned short		countWriteInstance;
	unsigned short		countType;
	unsigned short		countUnit;
	ACCESS_UNIT*		pAccessUnit;
	TYPE_DESC*			pAccessTypeDesc;
	POINT_DESC*			pAccessPointDesc;
	unsigned short		noDevice;
	UNIT_EVENT_STATE*	pUnitEventState;
	ACCESS_EVENT_PIPE*	pAccessEventPipe;
	ACCESS_WRITE_PIPE*	pAccessWritePipe;
	unsigned short		PortNumber;
	unsigned short		TimeoutValue;
	unsigned long		TimeOutWritePipe;
	unsigned long		TickCount;
} ACCESS_SERVER;

/***********************************************************************/
/* DbServ-Funktionen                                                  */
/***********************************************************************/
unsigned char IsANumber(unsigned char *pData);
int ltoa(long value, char *buffer);
int ultoa(unsigned long value, char *buffer);


/*============================= Ende der Datei ==============================*/


/* ------------------- */
/* Fehler-Definitionen */
/* ------------------- */
#define cEC_OK								0

#define cEC_INVALID_SERVER					1000
#define cEC_NOT_READY_SERVER				1010
#define cEC_INVALID_INSTANCE				1001
#define cEC_INVALID_UNIT					1002
#define cEC_INVALID_DEVICE					1003
#define cEC_INVALID_TYPE					1004
#define cEC_INVALID_POINT					1005
#define cEC_INVALID_ITEM					1006
#define cEC_INVALID_VALUE					1007

#define cEC_INVALID_TYPE_FOR_HYST			1100
#define cEC_INVALID_POINT_FOR_HYST			1101

#define cEC_NO_DATA_IN_EVENTBUFFER			1200
#define cEC_EVENTBUFFER_TO_SMALL			1201

#define cEC_ITEM_ALREADY_LINKED				1300
#define cEC_ITEM_ALREADY_UNLINKED			1301
#define cEC_ITEM_NO_TIMESTAMP				1302
#define cEC_ITEM_ALREADY_LINKED_FILE_BUFFER	1303
#define cEC_ITEM_NOT_UNLINKED_FILE_BUFFER	1304

#define cEC_INVALID_LENGTH					1400
#define cEC_INVALID_HANDLE					1401

#define cEC_ITEM_NOT_WRITEABLE				1500
#define cEC_NO_DATA_IN_WRITEBUFFER			1501
#define cEC_WRITE_PIPE_FULL					1502

#define cEC_FUB_NOT_LINKED_DBSERV			1600
#define cEC_DBSERV_NOT_LINKED_FUB			1601

#define cEC_FUB_WITHOUT_TIMESTAMP			1700

/* ------------------- */
/* Status-Definitionen */
/* ------------------- */
enum {	cITEM_UNLINKED,
		cITEM_LINKED_WITHOUT_TIMESTAMP,
		cITEM_LINKED_WITH_TIMESTAMP	};

/* ----------------------- */
/* Funktionen für Brserver */
/* ----------------------- */
UINT GetIdBrServer (void **pidBrServer, UINT *pbrstate);
UINT GetPortNumber (void *idBrServer, UINT *pPortNumber);
UINT GetTimeoutValue (void *idBrServer, UINT *pTimeoutValue);
UINT GetEventInstanceInfo (void *idBrServer, UINT *pEventInstance);
UINT GetWriteInstanceInfo (void *idBrServer, UINT *pWriteInstance);
UINT GetTypeInfo (void *idBrServer, UINT *ptype);
UINT GetItemInfo (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT** ppData_extern,		UDINT* plength_extern,
					USINT** ppData_intern,		UDINT* plength_intern,
					USINT** ppData_iextern,		UDINT* plength_iextern,
					USINT** ppData_valid,		UDINT* plength_valid,
					USINT** ppData_time,		UDINT* plength_time,
					USINT** ppData_itime,		UDINT* plength_itime,
					USINT** ppData_writelink,	UDINT* plength_writelink,
					USINT** ppData_ieventlink,	UDINT* plength_ieventlink,
					USINT** ppData_ihyst,		UDINT* plength_ihyst,
					USINT** ppData_ievent,		UDINT* plength_ievent);
UINT GetEventLinkInfo (void *idBrServer,
						UINT unit, UINT device, UINT type, UINT point, UINT item,
						UINT *pcountInstanceAll, UINT *pcountInstanceNoTimestamp, UINT *pcountInstanceTimestamp, USINT *ptsItem);
UINT GetEventPipeInfo (void *idBrServer, UINT instance, UINT type,
						UINT *pipe_entry, UINT *pipe_level_abs, USINT *pipe_level_rel);
UINT GetWritePipeInfo (void *idBrServer, UINT instance,
						UINT *pipe_entry, UINT *pipe_level_abs, USINT *pipe_level_rel);
UINT GetFubInstance (void *idBrServer,
						UINT unit, UINT device, UINT type, UINT point, UINT item,
						USINT **ppFubInstance);
UINT LinkEventInstance (void *idBrServer, UINT instance);
UINT UnlinkEventInstance (void *idBrServer, UINT instance);
UINT UnlinkEventInstanceWithoutFileBuffer (void *idBrServer, UINT instance);
UINT LinkEventUnit (void *idBrServer, UINT instance,
					UINT unit);
UINT UnLinkEventUnit (void *idBrServer, UINT instance,
						UINT unit);
UINT LinkEventDevice (void *idBrServer, UINT instance,
						UINT unit, UINT device);
UINT UnLinkEventDevice (void *idBrServer, UINT instance,
						UINT unit, UINT device);
UINT LinkEventType (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type);
UINT UnlinkEventType (void *idBrServer, UINT instance,
						UINT unit, UINT device, UINT type);
UINT LinkEventItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT timestamp);
UINT UnlinkEventItem (void *idBrServer, UINT instance,
						UINT unit, UINT device, UINT type, UINT point, UINT item);
UINT UnlinkEventItemWithoutFileBuffer (void *idBrServer, UINT instance,
										UINT unit, UINT device, UINT type, UINT point, UINT item);
UINT SetHystItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue);
UINT SetValueItem (void *idBrServer,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue,
					RTCtime_typ RTCTime, USINT setTimeStampNow,
					USINT enableEvent);
UINT GetValueItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT** ppData_extern, UDINT* plength_extern,
					USINT** ppData_time, UDINT* plength_time);
UINT GetValueItemNoUpdate (void *idBrServer,
							UINT unit, UINT device, UINT type, UINT point, UINT item,
							USINT** ppData_extern, UDINT* plength_extern,
							USINT** ppData_time, UDINT* plength_time);
UINT SetEventItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT *pValue, UINT lengthValue,
					USINT eventtype, USINT *pDataExtern, RTCtime_typ RTCTime);
UINT SetEventItemStream (void *idBrServer,
							UINT unit, UINT device, UINT type, UINT point, UINT item,
							USINT *pValue, UDINT lengthValue,
							USINT *pFlag,
							RTCtime_typ RTCTime, USINT setTimeStampNow,
							USINT enableEvent);
UINT ReleaseEventItemStream (void *idBrServer, UINT instance,
								UINT unit, UINT device, UINT type, UINT point, UINT item);
UINT GetEventItem (void *idBrServer, UINT instance,
					UINT type, USINT *pEventBuffer, UDINT lengthEventBuffer,
					UDINT* pLength, UDINT *pLengthTimeStamp);
UINT WorkEvent (void *idBrServer);
UINT LinkWriteInstance (void *idBrServer, UINT instance);
UINT UnlinkWriteInstance (void *idBrServer, UINT instance);
UINT LinkWriteUnit (void *idBrServer, UINT instance,
					UINT unit);
UINT UnlinkWriteUnit (void *idBrServer, UINT instance,
						UINT unit);
UINT LinkWriteDevice (void *idBrServer, UINT instance,
						UINT unit, UINT device);
UINT UnlinkWriteDevice (void *idBrServer, UINT instance,
						UINT unit, UINT device);
UINT LinkWriteType (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type);
UINT UnlinkWriteType (void *idBrServer, UINT instance,
						UINT unit, UINT device, UINT type);
UINT LinkWriteItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					UDINT handle);
UINT UnlinkWriteItem (void *idBrServer, UINT instance,
						UINT unit, UINT device, UINT type, UINT point, UINT item);
UINT SetWriteItem (void *dBrServer,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue);
UINT ScanWriteItem (void *idBrServer, UINT instance,
					USINT* pWriteBuffer, UDINT lengthWriteBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp);
UINT GetWriteItem (void *idBrServer, UINT instance,
					USINT* pWriteBuffer, UDINT lengthWriteBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp);
UINT CopyPipeToFile (void *idBrServer);

/*============================= Ende der Datei ==============================*/


