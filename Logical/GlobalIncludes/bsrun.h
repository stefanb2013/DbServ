
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  BrServer
*  Teil:     Header-File
*  Datei:    bsrun.h
*  Typ:      GNU C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    04-08-2003
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Generelle Definitionen.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 14-07-03 | MKrz | Datei-Erstellung
*        |          |      |
******************************************************************************/


/* --------------------- */
/* logische Definitionen */
/* --------------------- */
#define FALSE	0
#define TRUE	1

/* ---------------------------- */
/* Zustände für Initialisierung */
/* ---------------------------- */
#define cSTATE_INIT_COUNT		0
#define cSTATE_INIT_ALLOC		1
#define cSTATE_INIT_READY		2

/* ------------------- */
/* Fehler-Definitionen */
/* ------------------- */
#define cEC_OK						0

#define cEC_INVALID_SERVER			1000
#define cEC_INVALID_INSTANCE		1001
#define cEC_INVALID_UNIT			1002
#define cEC_INVALID_DEVICE			1003
#define cEC_INVALID_TYPE			1004
#define cEC_INVALID_POINT			1005
#define cEC_INVALID_ITEM			1006
#define cEC_INVALID_VALUE			1007

#define cEC_INVALID_TYPE_FOR_HYST	1100
#define cEC_INVALID_POINT_FOR_HYST	1101

#define cEC_NO_DATA_IN_EVENTBUFFER	1200
#define cEC_EVENTBUFFER_TO_SMALL	1201
#define cEC_INVALID_LENGTH_IN_EVENT	1202

#define cEC_ITEM_ALREADY_LINKED		1300
#define cEC_ITEM_ALREADY_UNLINKED	1301
#define cEC_ITEM_NO_TIMESTAMP		1302

#define cEC_INVALID_LENGTH			1400
#define cEC_INVALID_HANDLE			1401

#define cEC_ITEM_NOT_WRITEABLE		1500
#define cEC_NO_DATA_IN_WRITEBUFFER	1501
#define cEC_WRITE_PIPE_FULL			1502

/* ------------------- */
/* Status-Definitionen */
/* ------------------- */
enum {	cITEM_UNLINKED,
		cITEM_LINKED_WITHOUT_TIMESTAMP,
		cITEM_LINKED_WITH_TIMESTAMP	};

#define cMAX_INSTANCE	16

/* ------------------------------ */
/* Datentypen für Initialisierung */
/* ------------------------------ */
typedef struct {
	UINT	word_0;
	UINT	word_1;
} UNIT_DEFINITION;

typedef struct {
	UINT	word_0;
	UINT	word_1;
} DEVICE_DEFINITION;

typedef struct {
	USINT	length_extern;
	USINT	signed_extern;
	USINT	length_intern;
	USINT	signed_intern;
	USINT	time_stamp;
	USINT	hyst;
	USINT	typeforhyst;
	USINT	init;
	UINT	init_value;
	UINT	pipe_entry;
} TYPE_DEFINITION;

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
	EVENT_PIPE_HEADER*	pEventPipeHeader;
	UDINT				length_pipe_next;
	TYPE_DEFINITION		TypeDefinition;
} ACCESS_EVENT_PIPE;

typedef struct {
	UINT	pipe_entry;
	UINT	pipe_size;
	UINT	pipe_level;
	USINT*	pPipeData;
	UINT	iPipeRead;
	UINT	iPipeWrite;
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
typedef USINT	ACCESS_ITEM_VALID;
typedef	USINT	ACCESS_ITEM_EVENT_LINK;

typedef USINT	POINT_LINK_STATE;
typedef USINT	POINT_VALID_STATE;
typedef USINT	POINT_RELEASE_STATE;

typedef	UINT	ACCESS_ITEM_WRITE_LINK;
typedef	UDINT	ACCESS_ITEM_WRITE_HANDLE;

typedef struct {
	UDINT	noEvent;
} POINT_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} TYPE_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} DEVICE_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} UNIT_EVENT_STATE;

typedef struct {
	UDINT	countEvent;
} SERVER_EVENT_STATE;

typedef struct {
	UINT	countPoint;

	USINT*	pData_extern;
	UINT	length_extern;
	UINT	length_extern_align;

	USINT*	pData_intern;
	UINT	length_intern;
	UINT	length_intern_align;

	USINT*	pData_iextern;
	UINT	length_iextern;
	UINT	length_iextern_align;
	UINT	length_iextern_next;

	USINT*	pData_valid;
	UINT	length_valid;
	UINT	length_valid_align;

	USINT*	pData_time;
	UINT	length_time;
	UINT	length_time_align;

	USINT*	pData_itime;
	UINT	length_itime;
	UINT	length_itime_align;
	UINT	length_itime_next;

	USINT*	pData_writelink;
	UINT	length_writelink;
	UINT	length_writelink_align;

	USINT*	pData_writehandle;
	UINT	length_writehandle;
	UINT	length_writehandle_align;

	USINT*	pData_ieventlink;
	UINT	length_ieventlink;
	UINT	length_ieventlink_align;
	UINT	length_ieventlink_next;

	USINT*	pData_ireleasestream;
	UINT	length_ireleasestream;
	UINT	length_ireleasestream_align;
	UINT	length_ireleasestream_next;

	USINT*	pData_ihyst;
	UINT	length_ihyst;
	UINT	length_ihyst_align;
	UINT	length_ihyst_next;

	USINT*	pData_ievent;
	UINT	length_ievent;
	UINT	length_ievent_align;
	UINT	length_ievent_next;
} ACCESS_TYPE;

typedef struct {
	UINT				countType;
	UINT				device_no;
	ACCESS_TYPE*		pAccessType;
	TYPE_EVENT_STATE*	pTypeEventState;
} ACCESS_DEVICE;

typedef struct {
	UINT				countDeviceNo;
	UINT				unit_no;
	ACCESS_DEVICE*		pAccessDevice;
	DEVICE_EVENT_STATE*	pDeviceEventState;
} ACCESS_UNIT;

typedef struct {
	UINT				countEventInstance;
	UINT				countWriteInstance;
	UINT				countType;
	UINT				countUnitNo;
	ACCESS_UNIT*		pAccessUnit;
	UNIT_EVENT_STATE*	pUnitEventState;
	ACCESS_EVENT_PIPE*	pAccessEventPipe;
	ACCESS_WRITE_PIPE*	pAccessWritePipe;
	UINT				PortNumber;
	UINT				TimeoutValue;
} ACCESS_SERVER;

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
						UINT *pcountInstanceAll, UINT *pcountInstanceNoTimestamp, UINT *pcountInstanceTimestamp);
UINT GetEventPipeInfo (void *idBrServer, UINT instance, UINT type,
						UINT *pipe_entry, UINT *pipe_level_abs, USINT *pipe_level_rel);
UINT GetWritePipeInfo (void *idBrServer, UINT instance,
						UINT *pipe_entry, UINT *pipe_level_abs, USINT *pipe_level_rel);
UINT LinkEventInstance (void *idBrServer, UINT instance);
UINT UnlinkEventInstance (void *idBrServer, UINT instance);
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
UINT SetEventItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UINT lengthValue,
					USINT eventtype, USINT* pDataExtern, RTCtime_typ RTCTime);
UINT SetEventItemStream (void *idBrServer,
							UINT unit, UINT device, UINT type, UINT point, UINT item,
							USINT* pValue, UDINT lengthValue,
							USINT* pFlag,
							RTCtime_typ RTCTime, USINT setTimeStampNow,
							USINT enableEvent);
UINT ReleaseEventItemStream (void *idBrServer, UINT instance,
								UINT unit, UINT device, UINT type, UINT point, UINT item);
UINT GetEventItem (void *idBrServer, UINT instance,
					UINT type, USINT* pEventBuffer, UDINT lengthEventBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp);
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
UINT GetWriteItem (void *idBrServer, UINT instance,
					USINT* pWriteBuffer, UDINT lengthWriteBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp);

/*============================= Ende der Datei ==============================*/


