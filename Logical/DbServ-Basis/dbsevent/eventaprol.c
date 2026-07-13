#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Burklimat 2000
*  Teil:     Parser-Funktionen
*  Datei:    inetbk2.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 02.00
*  Datum:    06-07-2000
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Globale Include-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  00.00 | 23-06-99 | MKrz | Datei-Erstellung
*  02.00 | 06-07-00 | MKrz | Logbucheinträge, Fehlerbeseitigung für Typ-Section
*  03.00 | 11-10-16 | MKrz | Fehlerstruktur für Eventfehlererkennung (BIE)
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>		/* RPS-spezifische Typendeklarationen */
#include <bur\plctypes.h>	/* RPS-Datentypen */
#include <sys_lib.h>		/* Burtrap */
#include <string.h>			/* String-Library */
#include <fileio.h>

#include "eventaprol.h"

#include "dbserv.h"

/***********************************************************************/
/* Variablendeklaration                                                */
/***********************************************************************/

/**** globale Variablen ************************************************/

_LOCAL	EVENT_DOWN		event_down;
_LOCAL	EVENT_UP		event_up;

/**** lokale Variablen *************************************************/

_LOCAL	ACCESS_SERVER	*myidBrServer;
_LOCAL_RETAIN	UINT			statePV_xgetadr	;
static	UDINT			pv_adresse, data_len;

_LOCAL	UINT			statusGetValueItem;

_LOCAL	RTCtime_typ		RTCtime;
typedef struct {
	RTCtime_typ			RTCtime;
	UINT				statusGetValueItem;
	EVENT_DOWN			event_down;
} ERROR_EVENT_DOWN;
typedef struct {
	BOOL				Reset;
	UDINT				NumberOfEntries;
	UDINT				indexEntry;
	ERROR_EVENT_DOWN	ErrorEventDown[32];
} ERRORS_EVENT_DOWN;
_LOCAL_RETAIN	ERRORS_EVENT_DOWN	ErrorsEventDown	;

/**** interne Variablen ************************************************/

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	USINT			*dData, *sData;

	USINT			*pDataExtern, *pDataTime;
	UDINT			lengthExtern, lengthTime;

	UINT				stateEvRead;
	USINT				EventBuffer[256];
	UDINT				lenEventMessage;
	EVENT_PIPE_DATA*	pMSEventHeadData;

	UINT			countType, startType, i, j;
	UINT			item, offset;

/*		unsigned char		ErrStatisticsYField[EC_MAX / 8 + 1];*/

/***********************************************************************/
/* Initialisierungs-Unterprogramm                                      */
/***********************************************************************/

void _INIT init(void)
{
	myidBrServer = NULL;
	statePV_xgetadr = PV_xgetadr("dbserv:idBrServer", &pv_adresse, &data_len);
	if (statePV_xgetadr == 0) {
		myidBrServer = (ACCESS_SERVER*)*(UDINT*)pv_adresse;
	}
/*	memset(ErrStatisticsYField, 0, sizeof (ErrStatisticsYField));*/

} /* _INIT init(void) */

/***********************************************************************/
/* zyklisches Hauptprogramm                                            */
/***********************************************************************/

enum {
	EVENT_NONE,
	EVENT_READ_REQUEST,
	EVENT_WRITE_REQUEST,
	EVENT_EVENT_REQUSET
};

void _CYCLIC cyclic(void)
{

	/* Einlesen der Echtzeituhr */
	RTC_gettime(&RTCtime);
	if (ErrorsEventDown.Reset == 1) {
		memset(&ErrorsEventDown, 0, sizeof(ErrorsEventDown));
	}

	if (myidBrServer == NULL) {

		/* ------------*/
		/* Id-BrServer */
		/* ----------- */
		if (statePV_xgetadr == 0) {
			myidBrServer = (ACCESS_SERVER*)*(UDINT*)pv_adresse;
		}
		else {
			statePV_xgetadr = PV_xgetadr("dbserv:idBrServer", &pv_adresse, &data_len);
		}
	} /* (myidBrServer == NULL) */
	else {

		if (event_down.Auftragscode != EVENT_NONE) {
			if (event_up.Auftragscode == EVENT_NONE) {

				/* ======= */
				/* DP-Read */
				/* ======= */
				if (event_down.Auftragscode == EVENT_READ_REQUEST) {

					/* Item bestimmen */
					pAccessServer = (ACCESS_SERVER*)myidBrServer;
					if (event_down.Anlage < pAccessServer->countUnit && pAccessServer->pAccessUnit[event_down.Anlage].pAccessType != NULL) {
						pAccessUnit = &pAccessServer->pAccessUnit[event_down.Anlage];
						if (cDBS_DEFAULT_DEVICE == pAccessServer->noDevice) {
							if (event_down.Datentyp < pAccessUnit->countType && pAccessUnit->pAccessType[event_down.Datentyp].pAccessPoint != NULL) {
								pAccessType = &pAccessUnit->pAccessType[event_down.Datentyp];
								pTypeDesc   = &pAccessServer->pAccessTypeDesc[event_down.Datentyp];
								if (event_down.Datenpunkt < pAccessType->countPoint && pAccessType->pAccessPoint[event_down.Datenpunkt].pAccessData != NULL) {
									item = 0, offset = 0;
									while (item < pTypeDesc->countItem && offset < event_down.Offset) {
										pItemDesc = &pTypeDesc->pItemDesc[item];
										offset   += pItemDesc->lenItem;
										item++;
									}
								}
							}
						}
					}

					/* möglichst mit Zeitstempel linken */
					if (cEC_ITEM_NO_TIMESTAMP == LinkEventItem(myidBrServer, 0, event_down.Anlage, cDBS_DEFAULT_DEVICE, event_down.Datentyp, event_down.Datenpunkt, item, 1)) {
						LinkEventItem(myidBrServer, 0, event_down.Anlage, cDBS_DEFAULT_DEVICE, event_down.Datentyp, event_down.Datenpunkt, item, 0);
					}

					/* Station & Typ & Punkt suchen */
					statusGetValueItem = GetValueItem(myidBrServer, 0,
														event_down.Anlage, cDBS_DEFAULT_DEVICE, event_down.Datentyp, event_down.Datenpunkt, item,
														&pDataExtern, &lengthExtern,
														&pDataTime, &lengthTime);
					if (statusGetValueItem == 0)
					{
						/* read back */
						event_up.Anlage			= event_down.Anlage;
						event_up.Unterstation	= event_down.Unterstation;
						event_up.Datentyp		= event_down.Datentyp;
						event_up.Datenpunkt		= event_down.Datenpunkt;
						event_up.Offset			= event_down.Offset;
						event_up.Anzahl			= event_down.Anzahl;
						if (lengthTime == 0) {
							memset(&event_up.RTCTime, 0, sizeof(RTCtime_typ));
						}
						else {
							memcpy(&event_up.RTCTime, pDataTime, sizeof(RTCtime_typ));
						}
						dData = (unsigned char*)&event_up + sizeof(EVENT_UP_HEAD) + event_down.Anzahl;
						sData = pDataExtern;
						for (i=0; i<event_down.Anzahl; i++) {
							*--dData = *sData++;
						}
						/* Antwort gültig */
						event_up.Auftragscode = EVENT_READ_REQUEST;
					}
					else {
						/* Datenpunkt nicht gefunden */
						/* ErrStatisticsY (ErrStatisticsYField, EC_READ_UNKNOWN_POINT, NULL, ERRLOG, ERR, EC_READ_UNKNOWN_POINT + EC_INET_BASE, 1000 * BKInetReadType + BKInetReadPoint, ERR_INET_PRE, ET_READ_UNKNOWN_POINT);*/

						ErrorsEventDown.ErrorEventDown[ErrorsEventDown.indexEntry].RTCtime				= RTCtime;
						ErrorsEventDown.ErrorEventDown[ErrorsEventDown.indexEntry].statusGetValueItem	= statusGetValueItem;
						ErrorsEventDown.ErrorEventDown[ErrorsEventDown.indexEntry].event_down			= event_down;
						ErrorsEventDown.indexEntry = (ErrorsEventDown.indexEntry + 1) % (sizeof(ErrorsEventDown.ErrorEventDown)/sizeof(ErrorsEventDown.ErrorEventDown[0]));
						if (ErrorsEventDown.NumberOfEntries < sizeof(ErrorsEventDown.ErrorEventDown)/sizeof(ErrorsEventDown.ErrorEventDown[0])) {
							ErrorsEventDown.NumberOfEntries++;
						}

					}
					event_down.Auftragscode = EVENT_NONE;
				}
				/* ======== */
				/* DP-Write */
				/* ======== */
				else if (event_down.Auftragscode == EVENT_WRITE_REQUEST) {

					/* Item bestimmen */
					pAccessServer = (ACCESS_SERVER*)myidBrServer;
					if (event_down.Anlage < pAccessServer->countUnit && pAccessServer->pAccessUnit[event_down.Anlage].pAccessType != NULL) {
						pAccessUnit = &pAccessServer->pAccessUnit[event_down.Anlage];
						if (cDBS_DEFAULT_DEVICE == pAccessServer->noDevice) {
							if (event_down.Datentyp < pAccessUnit->countType && pAccessUnit->pAccessType[event_down.Datentyp].pAccessPoint != NULL) {
								pAccessType = &pAccessUnit->pAccessType[event_down.Datentyp];
								pTypeDesc   = &pAccessServer->pAccessTypeDesc[event_down.Datentyp];
								if (event_down.Datenpunkt < pAccessType->countPoint && pAccessType->pAccessPoint[event_down.Datenpunkt].pAccessData != NULL) {
									item = 0, offset = 0;
									while (item < pTypeDesc->countItem && offset < event_down.Offset) {
										pItemDesc = &pTypeDesc->pItemDesc[item];
										offset   += pItemDesc->lenItem;
										item++;
									}
								}
							}
						}
					}
					if (1==1) {
						/* write pi */
						sData = (unsigned char*)&event_down + sizeof(EVENT_DOWN);
						dData = pDataExtern + event_down.Offset;
						for (i=0; i<event_down.Anzahl; i++) {
							*dData++ = *sData++;
						}
						/* read back */
						event_up.Anlage			= event_down.Anlage;
						event_up.Unterstation	= event_down.Unterstation;
						event_up.Datentyp		= event_down.Datentyp;
						event_up.Datenpunkt		= event_down.Datenpunkt;
						event_up.Offset			= event_down.Offset;
						event_up.Anzahl			= event_down.Anzahl;
						if (lengthTime == 0) {
							memset(&event_up.RTCTime, 0, sizeof(RTCtime_typ));
						}
						else {
							memcpy(&event_up.RTCTime, pDataTime, sizeof(RTCtime_typ));
						}
						dData = (unsigned char*)&event_up + sizeof(EVENT_UP_HEAD);
						sData = pDataExtern + event_down.Offset;
						for (i=0; i<event_down.Anzahl; i++) {
							*dData++ = *sData++;
						}
						/* Antwort gültig */
						event_up.Auftragscode = EVENT_WRITE_REQUEST;
					}
					else {
						/* Datenpunkt nicht gefunden */
						/* ErrStatisticsY (ErrStatisticsYField, EC_READ_UNKNOWN_POINT, NULL, ERRLOG, ERR, EC_READ_UNKNOWN_POINT + EC_INET_BASE, 1000 * BKInetReadType + BKInetReadPoint, ERR_INET_PRE, ET_READ_UNKNOWN_POINT);*/
					}
					event_down.Auftragscode = EVENT_NONE;
				}
				/* unbekannter Dienst */
				else {
					event_down.Auftragscode = EVENT_NONE;
				}
			} /* (event_up.Auftragscode == 0) */
		} /* (event_down.Auftragscode != 0) */
		else {

			if (event_up.Auftragscode == EVENT_NONE) {

				/* =============================== */
				/* Events über alle Typen abfragen */
				/* =============================== */
				GetTypeInfo(myidBrServer, &countType);
				for (i=0; i<countType; i++) {
					if (GetEventItem(myidBrServer, 0, (startType + i) % countType, EventBuffer, sizeof(EventBuffer),&lenEventMessage, &lengthTime) == 0) {
						pMSEventHeadData		= (EVENT_PIPE_DATA*)EventBuffer;
						event_up.Anlage			= pMSEventHeadData->unit;
						event_up.Unterstation	= cDBS_DEFAULT_DEVICE;
						event_up.Datentyp		= pMSEventHeadData->type;
						event_up.Datenpunkt		= pMSEventHeadData->point;

						/* offset */
						pAccessServer = (ACCESS_SERVER*)myidBrServer;
						if (pMSEventHeadData->unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[pMSEventHeadData->unit].pAccessType != NULL) {
							pAccessUnit = &pAccessServer->pAccessUnit[pMSEventHeadData->unit];
							if (pMSEventHeadData->device == pAccessServer->noDevice) {
								if (pMSEventHeadData->type < pAccessUnit->countType && pAccessUnit->pAccessType[pMSEventHeadData->type].pAccessPoint != NULL) {
									pAccessType = &pAccessUnit->pAccessType[pMSEventHeadData->type];
									pTypeDesc   = &pAccessServer->pAccessTypeDesc[pMSEventHeadData->type];
									if (pMSEventHeadData->point < pAccessType->countPoint && pAccessType->pAccessPoint[pMSEventHeadData->point].pAccessData != NULL) {
										if (pMSEventHeadData->item < pTypeDesc->countItem) {
											pItemDesc = &pTypeDesc->pItemDesc[pMSEventHeadData->item];
										}
									}
								}
							}
						}
						event_up.Offset = pItemDesc->offItem;
						event_up.Anzahl = pItemDesc->lenItem;
						if (lengthTime == 0) {
							memset(&event_up.RTCTime, 0, sizeof(RTCtime_typ));
						}
						else {
							memcpy(&event_up.RTCTime, (unsigned char*)pMSEventHeadData + sizeof(EVENT_PIPE_DATA) + pMSEventHeadData->length, sizeof(RTCtime_typ));
						}
						dData = (unsigned char*)&event_up   + sizeof(EVENT_UP_HEAD) + pMSEventHeadData->length;
						sData = (unsigned char*)EventBuffer + sizeof(EVENT_PIPE_DATA);
						for (j=0; j<pMSEventHeadData->length; j++) {
							*--dData = *sData++;
						}
						/* Antwort gültig */
						event_up.Auftragscode = EVENT_EVENT_REQUSET;
						break;
					} /* (stateEvRead == 0) */
				} /* (i=0; i<pAccessSystem->countType; i++) */

				/* Event-Abfrage um einen Typ weiterschalten */
				startType = (startType + i + 1) % countType;

			} /* (event_up.Auftragscode == 0) */
		} /* (event_down.Auftragscode != 0) */
	}
} /* _CYCLIC cyclic(void) */



