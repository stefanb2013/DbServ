#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  DbServ
*  Teil:     Header-File
*  Datei:    dbsfunc.c
*  Typ:      GNU C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.01
*  Datum:    05-07-2004
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Globale Funktionen.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.01 | 05-07-04 | MKrz | Fehlerbeseitigung in den Funktionen
*        |          |        GetEventPipeInfo(), GetWritePipeInfo, GetEventItem(), ScanWriteItem(), GetWriteItem(); instance wird überprüft
*  01.00 | 14-07-03 | MKrz | Modul-Erstellung
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>		/* RPS-spezifische Typendeklarationen */
#include <bur\plctypes.h>	/* RPS-Datentypen */

#include <sys_lib.h>		/* B&R-Libraries */
#include <fileio.h>

#include <string.h>			/* ANSI-C-Libraries */

#include "platform.h"		/* CPU-Plattform */
#include "dbserv.h"			/* eigene */


/*=============================================================================
// Funktion: GetIdBrServer
//-----------------------------------------------------------------------------
// Anzahl der eingerichteten Datentypen.
// Input:	ppidBrServer:	System-ID (Rückmeldung)
//			pbrstate:	B&R-Fehlermeldung (Rückmeldung)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetIdBrServer (void **ppidBrServer, UINT *pbrstate) {

	static	UDINT	pidBrServer = NULL, lenidBrServer = 0;
	static	UINT	statePV_xgetadr = 0;


	if (ppidBrServer != NULL && pbrstate != NULL) {
		if (pidBrServer == NULL || statePV_xgetadr != 0) {
			#ifdef BRSERVER
				statePV_xgetadr = PV_xgetadr ("dbserv:idBrServer", &pidBrServer, &lenidBrServer);
			#else
				statePV_xgetadr = PV_xgetadr ("bktcp:idBrServer", &pidBrServer, &lenidBrServer);
			#endif
		}
		*pbrstate = statePV_xgetadr;
		if (statePV_xgetadr == 0 && lenidBrServer == sizeof(void*)) {
			*ppidBrServer = (void*)*(UDINT*)pidBrServer;
			if (*ppidBrServer == NULL) {
				return cEC_NOT_READY_SERVER;
			}
			else {
				return cEC_OK;
			}
		}
		else {
			*ppidBrServer = NULL;
			return cEC_INVALID_SERVER;
		}
	}
	else {
		return cEC_INVALID_HANDLE;
	}
} /* GetIdBrServer */


/*=============================================================================
// Funktion: GetPortNumber
//-----------------------------------------------------------------------------
// Anzahl der eingerichteten Datentypen.
// Input:	idBrServer: System-ID
//			pPortNumber:	Port-Nummer (Rückmeldung)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetPortNumber (void *idBrServer, UINT *pPortNumber) {


	if (idBrServer != NULL) {
		if (pPortNumber != NULL) {
			*pPortNumber = ((ACCESS_SERVER*)idBrServer)->PortNumber;
			return cEC_OK;
		}
		else {
			return cEC_INVALID_HANDLE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetPortNumber */


/*=============================================================================
// Funktion: GetTimeoutValue
//-----------------------------------------------------------------------------
// Anzahl der eingerichteten Datentypen.
// Input:	idBrServer: System-ID
//			pTimeoutValue:	Timeout-Wert (Rückmeldung)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetTimeoutValue (void *idBrServer, UINT *pTimeoutValue) {


	if (idBrServer != NULL) {
		if (pTimeoutValue != NULL) {
			*pTimeoutValue = ((ACCESS_SERVER*)idBrServer)->TimeoutValue;
			return cEC_OK;
		}
		else {
			return cEC_INVALID_HANDLE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetTimeoutValue */


/*=============================================================================
// Funktion: GetEventInstanceInfo
//-----------------------------------------------------------------------------
// Anzahl der eingerichteten Instanzen.
// Input:	idBrServer: System-ID
//			pinstance:	Anzahl der Instanzen (Rückmeldung)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetEventInstanceInfo (void *idBrServer, UINT *pEventInstance) {

	ACCESS_SERVER	*pAccessServer;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (pEventInstance != NULL) {
			*pEventInstance = pAccessServer->countEventInstance;
			return cEC_OK;
		}
		else {
			return cEC_INVALID_HANDLE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetEventInstanceInfo */


/*=============================================================================
// Funktion: GetWriteInstanceInfo
//-----------------------------------------------------------------------------
// Anzahl der eingerichteten Instanzen.
// Input:	idBrServer: System-ID
//			pinstance:	Anzahl der Instanzen (Rückmeldung)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetWriteInstanceInfo (void *idBrServer, UINT *pWriteInstance) {

	ACCESS_SERVER	*pAccessServer;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (pWriteInstance != NULL) {
			*pWriteInstance = pAccessServer->countWriteInstance;
			return cEC_OK;
		}
		else {
			return cEC_INVALID_HANDLE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetWriteInstanceInfo */

/*=============================================================================
// Funktion: GetTypeInfo
//-----------------------------------------------------------------------------
// Anzahl der eingerichteten Datentypen.
// Input:	idBrServer: System-ID
//			ptype:		Anzahl der Instanzen (Rückmeldung)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetTypeInfo (void *idBrServer, UINT *ptype) {

	ACCESS_SERVER	*pAccessServer;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (ptype != NULL) {
			*ptype = pAccessServer->countType;
			return cEC_OK;
		}
		else {
			return cEC_INVALID_HANDLE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetTypeInfo */


/*=============================================================================
// Funktion: GetItemInfo
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			plant:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

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
					USINT** ppData_ievent,		UDINT* plength_ievent) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								/* externer Datenbereich */
								if (ppData_extern != NULL && plength_extern != 0) {
									*ppData_extern = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
									*plength_extern = pItemDesc->lenItem;
								}
								/* interner Datenbereich */
								if (ppData_intern != NULL && plength_intern != 0) {
									*ppData_intern = pAccessPoint->pAccessData + pAccessType->offIntern;
									*plength_intern = pAccessType->lenIntern;
								}
								/* instanzierter externer Datenbereich */
								if (ppData_iextern != NULL && plength_iextern != 0) {
									*ppData_iextern = pAccessPoint->pAccessData + pAccessType->offiExtern + instance * pAccessType->leniExternNext + pItemDesc->offItem;
									*plength_iextern = pItemDesc->lenItem;
								}
								/* Gültigkeits Datenbereich */
								if (ppData_valid != NULL && plength_valid != 0) {
									*ppData_valid = pAccessPoint->pAccessData + pAccessType->offValid + item * pAccessType->lenValidAlign;
									*plength_valid = pAccessType->lenValid;
								}
								/* Zeit Datenbereich */
								if (ppData_time != NULL && plength_time != 0) {
									*ppData_time = pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign;
									*plength_time = pAccessType->lenTime;
								}
								/* instanzierter Zeit Datenbereich */
								if (ppData_itime != NULL && plength_itime != 0) {
									*ppData_itime = pAccessPoint->pAccessData + pAccessType->offiTime + instance * pAccessType->leniTimeNext + item * pAccessType->leniTimeAlign;
									*plength_itime = pAccessType->leniTime;
								}
								/* Writelink Datenbereich */
								if (ppData_writelink != NULL && plength_writelink != 0) {
									*ppData_writelink = pAccessPoint->pAccessData + pAccessType->offWritelink + item * pAccessType->lenWritelinkAlign;
									*plength_writelink = pAccessType->lenWritelink;
								}
								/* instanzierter Eventlink Datenbereich */
								if (ppData_ieventlink != NULL && plength_ieventlink != 0) {
									*ppData_ieventlink = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
									*plength_ieventlink = pAccessType->leniEventlink;
								}
								/* instanzierter Hysteresen Datenbereich */
								if (ppData_ihyst != NULL && plength_ihyst != 0) {
									*ppData_ihyst = pAccessPoint->pAccessData + pAccessType->offiHyst + instance * pAccessType->leniHystNext + pItemDesc->offItem;
									*plength_ihyst = pItemDesc->lenItem;
								}
								/* instanzierter Event Datenbereich */
								if (ppData_ievent != NULL && plength_ievent != 0) {
									*ppData_ievent = pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;
									*plength_ievent = pAccessType->leniEvent;
								}
								return cEC_OK;
							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType[type].countPoint) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessDevice[device].countType) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device < pAccessUnit[unit].countDeviceNo && pAccessUnit[unit].pAccessDevice[device].pAccessType != NULL) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnitNo && pAccessServer->pAccessUnit[unit].pAccessDevice != NULL) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessServer->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetItemInfo */


/*=============================================================================
// Funktion: GetEventLinkInfo
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
//			timestamp:	Zeitstempel
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetEventLinkInfo (void *idBrServer,
						UINT unit, UINT device, UINT type, UINT point, UINT item,
						UINT *pcountInstanceAll, UINT *pcountInstanceNoTimestamp, UINT *pcountInstanceTimestamp, USINT *ptsItem) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;
	UINT	instance, countInstanceAll, countInstanceNoTimestamp, countInstanceTimestamp;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
			pAccessUnit = &pAccessServer->pAccessUnit[unit];
			if (device == pAccessServer->noDevice) {
				if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
					pAccessType = &pAccessUnit->pAccessType[type];
					pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
					if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
						pAccessPoint = &pAccessType->pAccessPoint[point];
						if (item < pTypeDesc->countItem) {
							pItemDesc = &pTypeDesc->pItemDesc[item];

							/* Link Datenbereich */
							countInstanceAll = 0, countInstanceNoTimestamp = 0, countInstanceTimestamp = 0;
							for (instance=0; instance<pAccessServer->countEventInstance; instance++) {
								pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
								if (*pAccessEventLinkItem == cITEM_LINKED_WITHOUT_TIMESTAMP) {
									countInstanceNoTimestamp++;
									countInstanceAll++;
								}
								else if (*pAccessEventLinkItem == cITEM_LINKED_WITH_TIMESTAMP) {
									countInstanceTimestamp++;
									countInstanceAll++;
								}
							} /* (instance=0; instance<pAccessServer->countEventInstance; instance++) */
							if (pcountInstanceAll != NULL) {
								*pcountInstanceAll = countInstanceAll;
							}
							if (pcountInstanceNoTimestamp != NULL) {
								*pcountInstanceNoTimestamp = countInstanceNoTimestamp;
							}
							if (pcountInstanceTimestamp != NULL) {
								*pcountInstanceTimestamp = countInstanceTimestamp;
							}
							if (ptsItem != NULL) {
								*ptsItem = pItemDesc->tsItem;
							}
							return cEC_OK;
						} /* (item < pTypeDesc->countItem) */
						else {
							return cEC_INVALID_ITEM;
						}
					} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
					else {
						return cEC_INVALID_POINT;
					}
				} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (device == pAccessServer->noDevice) */
			else {
				return cEC_INVALID_DEVICE;
			}
		} /* (unit < pAccessServer->countUnitNo && pAccessServer->pAccessUnit[unit].pAccessType != NULL) */
		else {
			return cEC_INVALID_UNIT;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetEventLinkInfo */


/*=============================================================================
// Funktion: GetEventPipeInfo
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			system:		Anlagen-Nummer
//			station:	Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetEventPipeInfo (void *idBrServer, UINT instance, UINT type,
						UINT *pipe_entry, UINT *pipe_level_abs, USINT *pipe_level_rel) {

	ACCESS_SERVER		*pAccessServer;
	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderRead;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (type < pAccessServer->countType) {
				pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
				if (pAccessEventPipe->pEventPipeInstance != NULL) {
					pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];
					pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
					if (pEventPipeHeaderRead != NULL) {
						*pipe_entry = pEventPipeHeaderRead->pipe_entry;
						*pipe_level_abs = pEventPipeHeaderRead->pipe_level;
						*pipe_level_rel = 0; /* default */
						if (pEventPipeHeaderRead->pipe_entry > 0) {
							*pipe_level_rel = 100 * pEventPipeHeaderRead->pipe_level / pEventPipeHeaderRead->pipe_entry;
						}
						return cEC_OK;
					} /* (pEventPipeHeader != NULL) */
					return cEC_NO_DATA_IN_EVENTBUFFER;
				} /* (pAccessEventPipe != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (type < pAccessSystem->countType) */
			else {
				return cEC_INVALID_TYPE;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idMessageServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetEventPipeInfo */


/*=============================================================================
// Funktion: GetWritePipeInfo
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			system:		Anlagen-Nummer
//			station:	Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetWritePipeInfo (void *idBrServer, UINT instance,
						UINT *pipe_entry, UINT *pipe_level_abs, USINT *pipe_level_rel) {

	ACCESS_SERVER		*pAccessServer;
	ACCESS_WRITE_PIPE	*pAccessWritePipe;
	WRITE_PIPE_HEADER	*pWritePipeHeader;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {
			pAccessWritePipe = pAccessServer->pAccessWritePipe;
			pWritePipeHeader = (WRITE_PIPE_HEADER*)((USINT*)pAccessWritePipe->pWritePipeHeader + instance * pAccessWritePipe->length_pipe_next);
			*pipe_entry = pWritePipeHeader->pipe_entry;
			*pipe_level_abs = pWritePipeHeader->pipe_level;
			*pipe_level_rel = 0; /* default */
			if (pWritePipeHeader->pipe_entry > 0) {
				*pipe_level_rel = 100 * pWritePipeHeader->pipe_level / pWritePipeHeader->pipe_entry;
			}
			return cEC_OK;
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idMessageServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetWritePipeInfo */


/*=============================================================================
// Funktion: GetFubInstance
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
//			timestamp:	Zeitstempel
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetFubInstance (void *idBrServer,
						UINT unit, UINT device, UINT type, UINT point, UINT item,
						USINT **ppFubInstance) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_FUB_INSTANCE	*pAccessFubInstance;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
			pAccessUnit = &pAccessServer->pAccessUnit[unit];
			if (device == pAccessServer->noDevice) {
				if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
					pAccessType = &pAccessUnit->pAccessType[type];
					pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
					if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
						pAccessPoint = &pAccessType->pAccessPoint[point];
						if (item < pTypeDesc->countItem) {
							pItemDesc = &pTypeDesc->pItemDesc[item];

							/* FubInstanz Datenbereich */
							if (ppFubInstance != NULL) {
								pAccessFubInstance = (ACCESS_FUB_INSTANCE*)(pAccessPoint->pAccessData + pAccessType->offFubInstance + item * pAccessType->lenFubInstanceAlign);
								if (pAccessFubInstance != NULL) {
									*ppFubInstance = *pAccessFubInstance;
								}
								return cEC_OK;
							} /* (pFubInstance != NULL) */
							else {
								return cEC_INVALID_HANDLE;
							}
						} /* (item < pTypeDesc->countItem) */
						else {
							return cEC_INVALID_ITEM;
						}
					} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
					else {
						return cEC_INVALID_POINT;
					}
				} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (device == pAccessServer->noDevice) */
			else {
				return cEC_INVALID_DEVICE;
			}
		} /* (unit < pAccessServer->countUnit) */
		else {
			return cEC_INVALID_UNIT;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetFubInstance */


/*=============================================================================
// Funktion: LinkEventInstance
//-----------------------------------------------------------------------------
// System mit allen zugehörigen Datenpunkten für die Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT LinkEventInstance (void *idBrServer, UINT instance) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;

	unsigned short	unit, type, point, item;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			for (unit=0; unit<pAccessServer->countUnit; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					for (type=0; type<pAccessUnit->countType; type++) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (pAccessType->pAccessPoint != NULL) {
							for (point=0; point<pAccessType->countPoint; point++) {
								pAccessPoint = &pAccessType->pAccessPoint[point];
								if (pAccessPoint->pAccessData != NULL) {
									for (item=0; item<pTypeDesc->countItem ; item++) {
										/* Link Datenbereich */
										pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
										if (*pAccessEventLinkItem == cITEM_UNLINKED) {
											*pAccessEventLinkItem = cITEM_LINKED_WITHOUT_TIMESTAMP;
										}
									} /* (item = 0; item<pTypeDesc->countItem ; item++) */
								} /* (pAccessPoint->pAccessData != NULL) */
							} /* (point=0; point<pAccessType->countPoint; point++) */
						} /* (pAccessType->pAccessPoint != NULL) */
					} /* (type=0; type<pAccessUnit->countType; type++) */
				} /* (pAccessUnit->pAccessType != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnitNo; unit++) */
			return cEC_OK;
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* LinkEventInstance */


/*=============================================================================
// Funktion: UnlinkEventInstance
//-----------------------------------------------------------------------------
// System mit allen zugehörigen Datenpunkten für die Ereignisbehandlung abmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT UnlinkEventInstance (void *idBrServer, UINT instance) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;

	unsigned short	unit, type, point, item;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderWrite, *pEventPipeHeaderRead;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			for (unit=0; unit<pAccessServer->countUnit; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					for (type=0; type<pAccessUnit->countType; type++) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (pAccessType->pAccessPoint != NULL) {
							for (point=0; point<pAccessType->countPoint; point++) {
								pAccessPoint = &pAccessType->pAccessPoint[point];
								if (pAccessPoint->pAccessData != NULL) {
									for (item=0; item<pTypeDesc->countItem ; item++) {
										/* Unlink Datenbereich */
										pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
										if (*pAccessEventLinkItem != cITEM_UNLINKED) {
											*pAccessEventLinkItem = cITEM_UNLINKED;
										}
									} /* (item = 0; item<pTypeDesc->countItem ; item++) */
								} /* (pAccessPoint->pAccessData != NULL) */
							} /* (point=0; point<pAccessType->countPoint; point++) */

							/* aus Queue austragen */
							pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
							pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];

							pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[0];
							pEventPipeHeaderWrite->iPipeRead = 0;
							pEventPipeHeaderWrite->iPipeWrite = 0;

							pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[1];
							pEventPipeHeaderWrite->iPipeRead = 0;
							pEventPipeHeaderWrite->iPipeWrite = 0;

							pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
							pEventPipeHeaderRead->iPipeRead = 0;
							pEventPipeHeaderRead->iPipeWrite = 0;

						} /* (pAccessType->pAccessPoint != NULL) */
					} /* (type=0; type<pAccessUnit->countType; type++) */
				} /* (pAccessUnit->pAccessType != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnit; unit++) */
			return cEC_OK;
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* UnlinkEventInstance */


/*=============================================================================
// Funktion: UnlinkEventInstanceWithoutFileBuffer
//-----------------------------------------------------------------------------
// System mit allen zugehörigen Datenpunkten für die Ereignisbehandlung abmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT UnlinkEventInstanceWithoutFileBuffer (void *idBrServer, UINT instance) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;

	unsigned short	unit, type, point, item;
	unsigned char	flag;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderWrite, *pEventPipeHeaderRead;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			flag = FALSE; /* default */
			for (unit=0; unit<pAccessServer->countUnit; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					for (type=0; type<pAccessUnit->countType; type++) {
						pAccessType = &pAccessUnit->pAccessType[type];
						if (pAccessType->pAccessPoint != NULL) {
							pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
							pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
							if (pAccessEventPipe->WriteToFile == FALSE) {
								pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];
								#warning WriteToFile
								/*if (pEventPipeInstance->WriteToFile == FALSE) {*/
								for (point=0; point<pAccessType->countPoint; point++) {
									pAccessPoint = &pAccessType->pAccessPoint[point];
									if (pAccessPoint->pAccessData != NULL) {
										for (item=0; item<pTypeDesc->countItem ; item++) {
											/* Unlink Datenbereich */
											pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
											if (*pAccessEventLinkItem != cITEM_UNLINKED) {
												*pAccessEventLinkItem = cITEM_UNLINKED;
											}
										} /* (item = 0; item<pTypeDesc->countItem ; item++) */
									} /* (pAccessPoint->pAccessData != NULL) */
								} /* (point=0; point<pAccessType->countPoint; point++) */

								/* aus Queue austragen */
								pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
								pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];

								pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[0];
								pEventPipeHeaderWrite->iPipeRead = 0;
								pEventPipeHeaderWrite->iPipeWrite = 0;

								pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[1];
								pEventPipeHeaderWrite->iPipeRead = 0;
								pEventPipeHeaderWrite->iPipeWrite = 0;

								pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
								pEventPipeHeaderRead->iPipeRead = 0;
								pEventPipeHeaderRead->iPipeWrite = 0;
							} /* (pEventPipeInstance->WriteToFile == FALSE) */
							else {
								flag = TRUE;
							}
						} /* (pAccessType->pAccessPoint != NULL) */
					} /* (type=0; type<pAccessUnit->countType; type++) */
				} /* (pAccessUnit->pAccessType != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnit; unit++) */
			if (flag == FALSE) {
				return cEC_OK;
			}
			else {
				return cEC_ITEM_NOT_UNLINKED_FILE_BUFFER;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* UnlinkEventInstanceWithoutFileBuffer */


/*=============================================================================
// Funktion: LinkEventItem
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
//			timestamp:	Zeitstempel
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/
UINT LinkEventItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT timestamp) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];

						pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
						pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];

						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								/* Link Datenbereich */
								if (timestamp == FALSE || (timestamp == TRUE && pItemDesc->tsItem == TRUE)) {
									pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
									if (*pAccessEventLinkItem == cITEM_UNLINKED) {
										*pAccessEventLinkItem = (timestamp == FALSE) ? cITEM_LINKED_WITHOUT_TIMESTAMP : cITEM_LINKED_WITH_TIMESTAMP;
										return cEC_OK;
									}
									else {
										#warning WriteToFile
										if (pAccessEventPipe->WriteToFile == FALSE) {
											return cEC_ITEM_ALREADY_LINKED;
										}
										else {
											return cEC_ITEM_ALREADY_LINKED_FILE_BUFFER;
										}
									}
								} /* (timestamp == FALSE || (timestamp == TRUE && pItemDesc->tsItem == TRUE)) */
								else {
									return cEC_ITEM_NO_TIMESTAMP;
								}

							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* LinkEventItem */


/*=============================================================================
// Funktion: UnlinkEventItem
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehnadlung abmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT UnlinkEventItem (void *idBrServer, UINT instance,
						UINT unit, UINT device, UINT type, UINT point, UINT item) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderWrite;
	EVENT_PIPE_DATA 	EventPipeData;
	USINT				*pEventPipeData;

	UDINT			lengthExtern, lengthTimeStamp;
	UINT			i, iPipeRead, iPipeReadTmp;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								/* Unlink Datenbereich */
								pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
								if (*pAccessEventLinkItem != cITEM_UNLINKED) {
									*pAccessEventLinkItem = cITEM_UNLINKED;

									/* aus Queue austragen */
									pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
									pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];
									pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[0];
									iPipeRead = pEventPipeHeaderWrite->iPipeRead;
									while (iPipeRead != pEventPipeHeaderWrite->iPipeWrite) {
										/* Eintrag in Event-Pipe */
										pEventPipeData = (USINT*)&EventPipeData;
										iPipeReadTmp = iPipeRead;
										/* Header kopieren */
										for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
											pEventPipeData[i] = pEventPipeHeaderWrite->pPipeData[iPipeRead++ % pEventPipeHeaderWrite->pipe_size];
										}
										lengthExtern = EventPipeData.length;
										lengthTimeStamp = (EventPipeData.linkstate == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
										if (EventPipeData.linkstate != cITEM_UNLINKED &&
											EventPipeData.unit == unit && EventPipeData.device == device &&
											EventPipeData.type == type && EventPipeData.point == point && EventPipeData.item == item) {

											EventPipeData.linkstate = cITEM_UNLINKED;
											EventPipeData.length += lengthTimeStamp;
											/* Header zurückkopieren */
											for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
												pEventPipeHeaderWrite->pPipeData[iPipeReadTmp++ % pEventPipeHeaderWrite->pipe_size] = pEventPipeData[i];
											}
										}
										iPipeRead = (iPipeRead + lengthExtern + lengthTimeStamp) % pEventPipeHeaderWrite->pipe_size;
									} /* (iPipeRead != pEventPipeHeaderWrite->iPipeWrite) */
									return cEC_OK;
								} /* (pAccessLinkItem[instance] != cITEM_UNLINKED) */
								else {
									return cEC_ITEM_ALREADY_UNLINKED;
								}

							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* UnlinkEventItem */


/*=============================================================================
// Funktion: UnlinkEventItemWithoutLTB
//-----------------------------------------------------------------------------
// Nur Datenpunkt für Ereignisbehandlung ohne Langzeitpufferung abmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT UnlinkEventItemWithoutFileBuffer (void *idBrServer, UINT instance,
										UINT unit, UINT device, UINT type, UINT point, UINT item) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderWrite;
	EVENT_PIPE_DATA 	EventPipeData;
	USINT				*pEventPipeData;

	UDINT			lengthExtern, lengthTimeStamp;
	UINT			i, iPipeRead, iPipeReadTmp;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
								pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];
								pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[0];
								#warning WriteToFile
								if (pAccessEventPipe->WriteToFile == FALSE) {

									/* Unlink Datenbereich */
									pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
									if (*pAccessEventLinkItem != cITEM_UNLINKED) {
										*pAccessEventLinkItem = cITEM_UNLINKED;

										/* aus Queue austragen */
										iPipeRead = pEventPipeHeaderWrite->iPipeRead;
										while (iPipeRead != pEventPipeHeaderWrite->iPipeWrite) {
											/* Eintrag in Event-Pipe */
											pEventPipeData = (USINT*)&EventPipeData;
											iPipeReadTmp = iPipeRead;
											/* Header kopieren */
											for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
												pEventPipeData[i] = pEventPipeHeaderWrite->pPipeData[iPipeRead++ % pEventPipeHeaderWrite->pipe_size];
											}
											lengthExtern = EventPipeData.length;
											lengthTimeStamp = (EventPipeData.linkstate == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
											if (EventPipeData.linkstate != cITEM_UNLINKED &&
												EventPipeData.unit == unit && EventPipeData.device == device &&
												EventPipeData.type == type && EventPipeData.point == point && EventPipeData.item == item)
											{
												EventPipeData.linkstate = cITEM_UNLINKED;
												EventPipeData.length += lengthTimeStamp;
												/* Header zurückkopieren */
												for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
													pEventPipeHeaderWrite->pPipeData[iPipeReadTmp++ % pEventPipeHeaderWrite->pipe_size] = pEventPipeData[i];
												}
											}
											iPipeRead = (iPipeRead + lengthExtern + lengthTimeStamp) % pEventPipeHeaderWrite->pipe_size;
										} /* (iPipeRead != pEventPipeHeaderWrite->iPipeWrite) */
										return cEC_OK;
									} /* (pAccessLinkItem[instance] != cITEM_UNLINKED) */
									else {
										return cEC_ITEM_ALREADY_UNLINKED;
									}
								} /* (pEventPipeInstance->WriteToFile == FALSE) */
								else {
									return cEC_ITEM_NOT_UNLINKED_FILE_BUFFER;
								}
							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* UnlinkEventItemWithoutFileBuffer */


/*=============================================================================
// Funktion: SetHystItem
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT SetHystItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	USINT			*pDest;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								if (pValue != NULL) {
									/* Hysterese eintragen */
									if (pItemDesc->lenItem == lengthValue) {
										if (lengthValue > 0) {
											pDest = pAccessPoint->pAccessData + pAccessType->offiHyst + instance * pAccessType->leniHystNext;
											/* konsistent kopieren */
											switch (lengthValue) {
												case 1:
													*(unsigned char*)pDest = *(unsigned char*)pValue;
													break;
												case 2:
													*(unsigned short*)pDest = *(unsigned short*)pValue;
													break;
												case 4:
													*(unsigned long*)pDest = *(unsigned long*)pValue;
													break;
												default:
													return cEC_INVALID_LENGTH;
													break;
											}
										}
										return cEC_OK;
									}
									else {
										return cEC_INVALID_LENGTH;
									}
								} /* (pValue != NULL) */
								else {
									return cEC_INVALID_HANDLE;
								}

							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* SetHystItem */


/*=============================================================================
// Funktion: SetValueItem
//-----------------------------------------------------------------------------
// Wert in das Prozessabbild eintragen.
// Input:	idBrServer: System-ID
//			unit:		Anlagen-Nummer
//			device:		Geräte-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT SetValueItem (void *idBrServer,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue,
					RTCtime_typ RTCTime, USINT setTimeStampNow,
					USINT enableEvent) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_VALID		*pAccessItemValid;
	ACCESS_ITEM_EVENT_STATE	*pAccessEventItem;

	USINT	*pSrc, *pDest;
	UINT	instance, intreturnValue, returnValue;

	UDINT	appValue, hystValue, oldValue;
	USINT	setEvent;


	returnValue = cEC_OK, intreturnValue = cEC_OK;
	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
			pAccessUnit = &pAccessServer->pAccessUnit[unit];
			if (device == pAccessServer->noDevice) {
				if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
					pAccessType = &pAccessUnit->pAccessType[type];
					pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
					if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
						pAccessPoint = &pAccessType->pAccessPoint[point];
						if (item < pTypeDesc->countItem) {
							pItemDesc = &pTypeDesc->pItemDesc[item];

							if (pValue != NULL) {

								/* -------------------------- */
								/* Wert aus Applikation holen */
								/* -------------------------- */
								if (pItemDesc->sItem == FALSE) {
									switch (lengthValue) {
										case 1:
											appValue = (UDINT)*(USINT*)pValue;
											break;
										case 2:
											appValue = (UDINT)*(UINT*)pValue;
											break;
										case 4:
											appValue = (UDINT)*(UDINT*)pValue;
											break;
										default:
											return cEC_INVALID_LENGTH;
											break;
									}
								} /* (pItemDesc->sItem == FALSE) */
								else {
									switch (lengthValue) {
										case 1:
											appValue = (DINT)*(SINT*)pValue;
											break;
										case 2:
											appValue = (DINT)*(INT*)pValue;
											break;
										case 4:
											appValue = (DINT)*(DINT*)pValue;
											break;
										default:
											return cEC_INVALID_LENGTH;
											break;
									}
								}

								/* ------------------- */
								/* über alle Instanzen */
								/* ------------------- */
								pAccessItemValid = pAccessPoint->pAccessData + pAccessType->offValid + item * pAccessType->lenValidAlign;
								for (instance=0; instance<pAccessServer->countEventInstance; instance++) {

									pAccessEventItem = pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;

									/* -------------------------------- */
									/* Änderungskennung mit Hysterese ? */
									/* -------------------------------- */
									hystValue = 0; /* default */
									pSrc = pAccessPoint->pAccessData + pAccessType->offiHyst + instance * pAccessType->leniHystNext + pItemDesc->offItem;
									if (pItemDesc->sItem == FALSE) {
										switch (pItemDesc->lenItem) {
											case 1:
												hystValue = (UDINT)*(USINT*)pSrc;
												break;
											case 2:
												hystValue = (UDINT)*(UINT*)pSrc;
												break;
											case 4:
												hystValue = (UDINT)*(UDINT*)pSrc;
												break;
											default:
												return cEC_INVALID_LENGTH;
												break;
										}
									} /* (pItemDesc->sItem == FALSE) */
									else {
										switch (pItemDesc->lenItem) {
											case 1:
												hystValue = (DINT)*(SINT*)pSrc;
												break;
											case 2:
												hystValue = (DINT)*(INT*)pSrc;
												break;
											case 4:
												hystValue = (DINT)*(DINT*)pSrc;
												break;
											default:
												return cEC_INVALID_LENGTH;
												break;
										}
									}

									/* ------------------------------------------ */
									/* Wert aus instanziertem Prozessabbild holen */
									/* ------------------------------------------ */
									pSrc = pAccessPoint->pAccessData + pAccessType->offiExtern + instance * pAccessType->leniExternNext + pItemDesc->offItem;
									if (pItemDesc->sItem == FALSE) {
										switch (pItemDesc->lenItem) {
											case 1:
												oldValue = (UDINT)*(USINT*)pSrc;
												break;
											case 2:
												oldValue = (UDINT)*(UINT*)pSrc;
												break;
											case 4:
												oldValue = (UDINT)*(UDINT*)pSrc;
												break;
											default:
												return cEC_INVALID_LENGTH;
												break;
										}
									} /* (pItemDesc->sItem == FALSE) */
									else {
										switch (pItemDesc->lenItem) {
											case 1:
												oldValue = (DINT)*(SINT*)pSrc;
												break;
											case 2:
												oldValue = (DINT)*(INT*)pSrc;
												break;
											case 4:
												oldValue = (DINT)*(DINT*)pSrc;
												break;
											default:
												return cEC_INVALID_LENGTH;
												break;
										}
									}

									/* --------- */
									/* Vergleich */
									/* --------- */
									setEvent = FALSE;
									if (pItemDesc->sItem == FALSE) {
										if ((UDINT)appValue > (UDINT)oldValue + (UDINT)hystValue || (UDINT)appValue + (UDINT)hystValue < (UDINT)oldValue) {
											setEvent = TRUE;
										}
									}
									else {
										if ((DINT)appValue > (DINT)oldValue + (DINT)hystValue || (DINT)appValue + (DINT)hystValue < (DINT)oldValue) {
											setEvent = TRUE;
										}
									}
									if (*pAccessItemValid == FALSE || setEvent == TRUE || *pAccessEventItem == TRUE) {

										/* ------------------------------------------------------ */
										/* neuen Wert in das instanzierte Prozessabbild schreiben */
										/* ------------------------------------------------------ */
										pDest = pAccessPoint->pAccessData + pAccessType->offiExtern + instance * pAccessType->leniExternNext + pItemDesc->offItem;
										if (pItemDesc->sItem == FALSE) {
											switch (pItemDesc->lenItem) {
												case 1:
													*(USINT*)pDest = (USINT)appValue;
													break;
												case 2:
													*(UINT*)pDest = (UINT)appValue;
													break;
												case 4:
													*(UDINT*)pDest = (UDINT)appValue;
													break;
												default:
													return cEC_INVALID_LENGTH;
													break;
											}
										}
										else {
											switch (pItemDesc->lenItem) {
												case 1:
													*(SINT*)pDest = (SINT)appValue;
													break;
												case 2:
													*(INT*)pDest = (INT)appValue;
													break;
												case 4:
													*(DINT*)pDest = (DINT)appValue;
													break;
												default:
													return cEC_INVALID_LENGTH;
													break;
											}
										}
										/* Event setzen */
										if (enableEvent == TRUE) {
											if (pItemDesc->lenItem >= 1 && pItemDesc->lenItem <= 4) {
												#ifdef INTEL
													intreturnValue = SetEventItem(idBrServer, instance, unit, device, type, point, item, (USINT*)&appValue, pItemDesc->lenItem, 0, NULL, RTCTime);
												#else
													intreturnValue = SetEventItem(idBrServer, instance, unit, device, type, point, item, (USINT*)&appValue + 4 - pItemDesc->lenItem, pItemDesc->lenItem, 0, NULL, RTCTime);
												#endif
												if (intreturnValue != cEC_OK) {
													returnValue = intreturnValue;
												}
											}
											else {
												return cEC_INVALID_LENGTH;
											}
										} /* (enableEvent == TRUE) */
									} /* (setEvent == TRUE) */
								} /* (instance=0; i<pAccessServer->countEventInstance; instance++) */

								/* ----------------------------------------- */
								/* neuen Wert in das Prozessabbild schreiben */
								/* ----------------------------------------- */
								pDest = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
								if (pItemDesc->sItem == FALSE) {
									switch (pItemDesc->lenItem) {
										case 1:
											*(USINT*)pDest = (USINT)appValue;
											break;
										case 2:
											*(UINT*)pDest = (UINT)appValue;
											break;
										case 4:
											*(UDINT*)pDest = (UDINT)appValue;
											break;
										default:
											return cEC_INVALID_LENGTH;
											break;
									}
								} /* (pItemDesc->sItem == FALSE) */
								else {
									switch (pItemDesc->lenItem) {
										case 1:
											*(SINT*)pDest = (SINT)appValue;
											break;
										case 2:
											*(INT*)pDest = (INT)appValue;
											break;
										case 4:
											*(DINT*)pDest = (DINT)appValue;
											break;
										default:
											return cEC_INVALID_LENGTH;
											break;
									}
								}
								/* Ereigniszeit aktualisieren */
								if (
									pItemDesc->tsItem == TRUE
									&&
									(
										*pAccessItemValid == FALSE || setTimeStampNow == TRUE || setEvent == TRUE
									)
								) {
									memcpy(pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign, &RTCTime, pAccessType->lenTime);
								}
								*pAccessItemValid = TRUE;
								return returnValue;
							} /* (pValue != NULL) */
							else {
								return cEC_INVALID_HANDLE;
							}

						} /* (item < pTypeDesc->countItem) */
						else {
							return cEC_INVALID_ITEM;
						}
					} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
					else {
						return cEC_INVALID_POINT;
					}
				} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (device == pAccessServer->noDevice) */
			else {
				return cEC_INVALID_DEVICE;
			}
		} /* (unit < pAccessServer->countUnit) */
		else {
			return cEC_INVALID_UNIT;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* SetValueItem */


/*=============================================================================
// Funktion: GetValueItem
//-----------------------------------------------------------------------------
// Wert aus dem Prozessabbild lesen.
// Input:	idBrServer: System-ID
//			unit:		Anlagen-Nummer
//			device:		Geräte-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetValueItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT** ppData_extern, UDINT* plength_extern,
					USINT** ppData_time, UDINT* plength_time) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_ITEM_VALID	*pAccessItemValid;

	UINT			i;
	USINT			*pData_extern, *pData_iextern, *pData_time, *pData_itime;
	UDINT			length_extern, length_time;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								/* externer Datenbereich */
								if (ppData_extern != NULL && plength_extern != 0) {
									*ppData_extern = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
									*plength_extern = pItemDesc->lenItem;
								}
								/* Zeit Datenbereich */
								if (ppData_time != NULL && plength_time != 0) {
									*ppData_time = pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign;
									*plength_time = pAccessType->lenTime;
								}
								pAccessItemValid = pAccessPoint->pAccessData + pAccessType->offValid + item * pAccessType->lenValidAlign;
								if (1==1 || *pAccessItemValid == TRUE) {
									/* ----------------------------------------- */
									/* instanziertes Prozessabbild aktualisieren */
									/* ----------------------------------------- */
									pData_extern = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
									length_extern = pItemDesc->lenItem;
									pData_iextern = pAccessPoint->pAccessData + pAccessType->offiExtern + instance * pAccessType->leniExternNext + pItemDesc->offItem;
									for (i=0; i<length_extern; i++) {
										pData_iextern[i] = pData_extern[i];
									}
									pData_time = pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign;
									length_time = pAccessType->lenTime;
									pData_itime = pAccessPoint->pAccessData + pAccessType->offiTime + instance * pAccessType->leniTimeNext + item * pAccessType->leniTimeAlign;
									for (i=0; i<length_time; i++) {
										pData_itime[i] = pData_time[i];
									}
									return cEC_OK;
								} /* (*pAccessItemValid == TRUE) */
								else {
									return cEC_INVALID_VALUE;
								}

							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetValueItem */


/*=============================================================================
// Funktion: GetValueItemNoUpdate
//-----------------------------------------------------------------------------
// Wert aus dem Prozessabbild lesen.
// Input:	idBrServer: System-ID
//			unit:		Anlagen-Nummer
//			device:		Geräte-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetValueItemNoUpdate (void *idBrServer,
							UINT unit, UINT device, UINT type, UINT point, UINT item,
							USINT** ppData_extern, UDINT* plength_extern,
							USINT** ppData_time, UDINT* plength_time) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_ITEM_VALID	*pAccessItemValid;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
			pAccessUnit = &pAccessServer->pAccessUnit[unit];
			if (device == pAccessServer->noDevice) {
				if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
					pAccessType = &pAccessUnit->pAccessType[type];
					pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
					if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
						pAccessPoint = &pAccessType->pAccessPoint[point];
						if (item < pTypeDesc->countItem) {
							pItemDesc = &pTypeDesc->pItemDesc[item];

							/* externer Datenbereich */
							if (ppData_extern != NULL && plength_extern != 0) {
								*ppData_extern = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
								*plength_extern = pItemDesc->lenItem;
							}
							/* Zeit Datenbereich */
							if (ppData_time != NULL && plength_time != 0) {
								*ppData_time = pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign;
								*plength_time = pAccessType->lenTime;
							}
							pAccessItemValid = pAccessPoint->pAccessData + pAccessType->offValid + item * pAccessType->lenValidAlign;
							if (*pAccessItemValid == TRUE) {
								return cEC_OK;
							} /* (*pAccessItemValid == TRUE) */
							else {
								return cEC_INVALID_VALUE;
							}

						} /* (item < pTypeDesc->countItem) */
						else {
							return cEC_INVALID_ITEM;
						}
					} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
					else {
						return cEC_INVALID_POINT;
					}
				} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (device == pAccessServer->noDevice) */
			else {
				return cEC_INVALID_DEVICE;
			}
		} /* (unit < pAccessServer->countUnit) */
		else {
			return cEC_INVALID_UNIT;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetValueItemNoUpdate */


/*=============================================================================
// Funktion: SetEventItem
//-----------------------------------------------------------------------------
// Löst ein Ereignis auf ein Item aus.
// Input:	idBrServer: System-ID
//			unit:		Anlagen-Nummer
//			device:		Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT SetEventItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT *pValue, UINT lengthValue,
					USINT eventtype, USINT *pDataExtern, RTCtime_typ RTCTime) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;
	ACCESS_ITEM_EVENT_STATE	*pAccessEventItem;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderWrite;
	EVENT_PIPE_DATA 	EventPipeData;
	USINT				*pEventPipeData, *pEventPipeTime;

	UDINT			lengthExtern, lengthPipeData, lengthTimeStamp;
	UINT			iPipeWrite, i;
	BOOL			flag;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
								if (*pAccessEventLinkItem != cITEM_UNLINKED) {

									pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
									pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];
									pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferEventIndex];

									pItemDesc = &pTypeDesc->pItemDesc[item];
									lengthExtern = pItemDesc->lenItem;
									lengthTimeStamp = (*pAccessEventLinkItem == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
									lengthPipeData = (pValue != NULL) ? lengthValue : 0;

									flag = FALSE; /* default */
									if (
										pEventPipeHeaderWrite != NULL &&
										pEventPipeHeaderWrite->stateEvent.countEvent == 0 &&
										(
											(
												pEventPipeHeaderWrite->iPipeWrite >= pEventPipeHeaderWrite->iPipeRead &&
												(UINT)(pEventPipeHeaderWrite->pipe_size - (pEventPipeHeaderWrite->iPipeWrite - pEventPipeHeaderWrite->iPipeRead)) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthPipeData
											)
											||
											(
												pEventPipeHeaderWrite->iPipeWrite < pEventPipeHeaderWrite->iPipeRead &&
												(UINT)(pEventPipeHeaderWrite->iPipeRead - pEventPipeHeaderWrite->iPipeWrite) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthPipeData
											)
										)
									) {
										flag = TRUE;
									}
									#warning WriteToFile
									else if (
										pAccessEventPipe->WriteToFile == TRUE &&
										(pEventPipeInstance->BufferEventIndex + 1) % 2 != pEventPipeInstance->BufferFileIndex
									) {
										pEventPipeInstance->BufferEventIndex = (pEventPipeInstance->BufferEventIndex + 1) % 2;
										pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferEventIndex];
										pEventPipeHeaderWrite->iPipeWrite = 0;
										pEventPipeHeaderWrite->iPipeRead = 0;
										flag = TRUE;
									}

									/*if (pEventPipeHeaderWrite != NULL &&
										pEventPipeHeaderWrite->stateEvent.countEvent == 0 &&
										((pEventPipeHeaderWrite->iPipeWrite >= pEventPipeHeaderWrite->iPipeRead &&
										(UINT)(pEventPipeHeaderWrite->pipe_size - (pEventPipeHeaderWrite->iPipeWrite - pEventPipeHeaderWrite->iPipeRead)) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthPipeData) ||
										(pEventPipeHeaderWrite->iPipeWrite < pEventPipeHeaderWrite->iPipeRead &&
										(UINT)(pEventPipeHeaderWrite->iPipeRead - pEventPipeHeaderWrite->iPipeWrite) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthPipeData))) {*/

									if (flag == TRUE) {

										pAccessEventItem = pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;
										*pAccessEventItem = FALSE;

										if (pValue != NULL || pDataExtern != NULL) {
											if ((pValue != NULL && lengthValue == lengthExtern) || pDataExtern != NULL) {
												/* Eintrag in Event-Pipe */
												iPipeWrite = pEventPipeHeaderWrite->iPipeWrite;

												/* Header vorbesetzen */
												EventPipeData.linkstate = *pAccessEventLinkItem;
												EventPipeData.eventtype = eventtype;
												EventPipeData.unit = unit;
												EventPipeData.device = device;
												EventPipeData.type = type;
												EventPipeData.point= point;
												EventPipeData.item = item;
												EventPipeData.length = lengthValue;
												EventPipeData.pData = pDataExtern;
												pEventPipeData = (USINT*)&EventPipeData;
												/* Header eintragen */
												for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
													pEventPipeHeaderWrite->pPipeData[iPipeWrite++ % pEventPipeHeaderWrite->pipe_size] = pEventPipeData[i];
												}
												if (pValue != NULL) {
													/* Daten eintragen */
													for (i=0; i<lengthValue; i++) {
														pEventPipeHeaderWrite->pPipeData[iPipeWrite++ % pEventPipeHeaderWrite->pipe_size] = pValue[i];
													}
												}
												/* optional Zeitstempel eintragen */
												if (lengthTimeStamp > 0) {
													pEventPipeTime = (USINT*)&RTCTime;
													for (i=0; i<lengthTimeStamp; i++) {
														pEventPipeHeaderWrite->pPipeData[iPipeWrite++ % pEventPipeHeaderWrite->pipe_size] = pEventPipeTime[i];
													}
												}
												/* Ereigniszeit aktualisieren */
												if (pItemDesc->tsItem == TRUE) {
													memcpy(pAccessPoint->pAccessData + pAccessType->offiTime + instance * pAccessType->leniTimeNext + item * pAccessType->leniTimeAlign, &RTCTime, pAccessType->leniTime);
												}
												pEventPipeHeaderWrite->iPipeWrite = (iPipeWrite % pEventPipeHeaderWrite->pipe_size); /* Index erst nach Zugriff updaten !!! */
												pEventPipeHeaderWrite->pipe_level++; /* Füllstand erhöhen */

											} /* ((pValue != NULL && lengthValue == lengthExtern) || pDataExtern != NULL) */
											else {
												return cEC_INVALID_LENGTH;
											}
										} /* (pValue != NULL || pDataExtern != NULL) */
										else {
											return cEC_INVALID_HANDLE;
										}
									}
									else {
										#warning WriteToFile
										if (pAccessEventPipe->WriteToFile == TRUE) {
											pEventPipeInstance->BufferEventIndex = (pEventPipeInstance->BufferEventIndex + 1) % 2;
										}
										else {
											/* Event auf Datenpunkt */
											/*pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;
											if ((*(UDINT*)(pAccessType[type].pData_ievent + point * pAccessType[type].length_ievent_next + instance * pAccessType[type].length_ievent_align)) == 0) {
												if (pEventPipeHeaderWrite->stateEvent.lastEvent == 0) {
													pEventPipeHeaderWrite->stateEvent.firstEvent = 1;
												}
												pEventPipeHeaderWrite->stateEvent.lastEvent++;
												if (pEventPipeHeaderWrite->stateEvent.lastEvent == 0) {
													pEventPipeHeaderWrite->stateEvent.lastEvent++;
												}
												(*(UDINT*)(pAccessType[type].pData_ievent + point * pAccessType[type].length_ievent_next + instance * pAccessType[type].length_ievent_align)) = pEventPipeHeaderWrite->stateEvent.lastEvent;
												pEventPipeHeaderWrite->stateEvent.countEvent++;
												pAccessDevice[device].pTypeEventState[pAccessServer->countEventInstance * type + instance].countEvent++;
												pAccessUnit[unit].pDeviceEventState[pAccessServer->countEventInstance * device + instance].countEvent++;
												pAccessServer->pUnitEventState[pAccessServer->countEventInstance * unit + instance].countEvent++;
											}*/
											pAccessEventItem = pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;
											*pAccessEventItem = TRUE;
											/* Ereigniszeit aktualisieren */
											if (pItemDesc->tsItem == TRUE) {
												memcpy(pAccessPoint->pAccessData + pAccessType->offiTime + instance * pAccessType->leniTimeNext + item * pAccessType->leniTimeAlign, &RTCTime, pAccessType->leniTime);
											}
										}
									}
								} /* (*pAccessEventLinkItem != cITEM_UNLINKED) */
								else {
									pAccessEventItem = pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;
									*pAccessEventItem = FALSE;
								}
								return cEC_OK;
							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* SetEventItem */


/*=============================================================================
// Funktion: SetEventItemStream
//-----------------------------------------------------------------------------
// Wert in das Prozessabbild eintragen.
// Input:	idBrServer: System-ID
//			unit:		Anlagen-Nummer
//			device:		Geräte-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT SetEventItemStream (void *idBrServer,
							UINT unit, UINT device, UINT type, UINT point, UINT item,
							USINT* pValue, UDINT lengthValue,
							USINT* pFlag,
							RTCtime_typ RTCTime, USINT setTimeStampNow,
							USINT enableEvent) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_VALID	*pAccessItemValid;
	ACCESS_ITEM_RELEASE	*pAccessReleaseStreamItem;

	UINT	instance, intreturnValue, returnValue;
	USINT	*pData_extern;


	returnValue = cEC_OK, intreturnValue = cEC_OK;
	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
			pAccessUnit = &pAccessServer->pAccessUnit[unit];
			if (device == pAccessServer->noDevice) {
				if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
					pAccessType = &pAccessUnit->pAccessType[type];
					pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
					if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
						pAccessPoint = &pAccessType->pAccessPoint[point];
						if (item < pTypeDesc->countItem) {
							pItemDesc = &pTypeDesc->pItemDesc[item];

							if (pValue != NULL) {
								for (instance=0; instance<pAccessServer->countEventInstance; instance++) {
									pAccessReleaseStreamItem = pAccessPoint->pAccessData + pAccessType->offiReleasestream + instance * pAccessType->leniReleasestream + item * pAccessType->leniReleasestream;
									*pAccessReleaseStreamItem = FALSE;
									intreturnValue = SetEventItem(idBrServer, instance, unit, device, type, point, item, NULL, lengthValue, 0, pValue, RTCTime);
									if (intreturnValue != cEC_OK) {
										returnValue = intreturnValue;
									}
								} /* (instance=0; instance<pAccessServer->countEventInstance; instance++) */

								pData_extern = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
								((UDINT*)pData_extern)[0] = (UDINT)pValue;
								((UDINT*)pData_extern)[1] = (UDINT)pFlag;
								if (pFlag != NULL) {
									*pFlag = TRUE;
								}
								else {
									return cEC_INVALID_HANDLE;
								}

								/* Ereigniszeit aktualisieren */
								pAccessItemValid = pAccessPoint->pAccessData + pAccessType->offValid + item * pAccessType->lenValidAlign;
								if ((*pAccessItemValid == FALSE || setTimeStampNow == TRUE) && pItemDesc->tsItem == TRUE) {
									memcpy(pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign, &RTCTime, pAccessType->lenTime);
								}
								*pAccessItemValid = TRUE;
								return returnValue;
							} /* (pValue != NULL) */
							else {
								return cEC_INVALID_HANDLE;
							}
						} /* (item < pTypeDesc->countItem) */
						else {
							return cEC_INVALID_ITEM;
						}
					} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
					else {
						return cEC_INVALID_POINT;
					}
				} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (device == pAccessServer->noDevice) */
			else {
				return cEC_INVALID_DEVICE;
			}
		} /* (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) */
		else {
			return cEC_INVALID_UNIT;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* SetEventItemStream */


/*=============================================================================
// Funktion: ReleaseEventItemStream
//-----------------------------------------------------------------------------
// Wert in das Prozessabbild eintragen.
// Input:	idBrServer: System-ID
//			unit:		Anlagen-Nummer
//			device:		Geräte-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT ReleaseEventItemStream (void *idBrServer, UINT instance,
								UINT unit, UINT device, UINT type, UINT point, UINT item) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;
	ACCESS_ITEM_RELEASE		*pAccessReleaseStreamItem;

	USINT			*pData_extern, *pFlag, flag;
	UINT			i;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];
								pAccessReleaseStreamItem = pAccessPoint->pAccessData + pAccessType->offiReleasestream + instance * pAccessType->leniReleasestream + item * pAccessType->leniReleasestream;
								*pAccessReleaseStreamItem = TRUE;
								flag = TRUE; /* default */
								for (i=0; i<pAccessServer->countEventInstance; i++) {
									pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + i * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
									pAccessReleaseStreamItem = pAccessPoint->pAccessData + pAccessType->offiReleasestream + i * pAccessType->leniReleasestream + item * pAccessType->leniReleasestream;
									if (*pAccessReleaseStreamItem == FALSE && *pAccessEventLinkItem != cITEM_UNLINKED) {
										flag = FALSE;
									}
								}
								if (flag == TRUE) {
									pData_extern = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
									pFlag = (USINT*)((UDINT*)pData_extern)[1];
									if (pFlag != NULL) {
										*pFlag = FALSE;
									}
									else {
										return cEC_INVALID_HANDLE;
									}
								}
								return cEC_OK;
							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessServer->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* ReleaseEventItemStream */


/*=============================================================================
// Funktion: GetEventItem
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			type:		Datentyp-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetEventItem (void *idBrServer, UINT instance,
					UINT type, USINT* pEventBuffer, UDINT lengthEventBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp) {

	ACCESS_SERVER		*pAccessServer;
	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderRead;

	UINT				i, iPipeRead;
	UDINT				lengthExtern, lengthPipeData, lengthTimeStamp;

	EVENT_PIPE_DATA 	EventPipeData;
	USINT				*pEventPipeData;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countEventInstance) {
			if (type < pAccessServer->countType) {

				pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
				if (pAccessEventPipe->pEventPipeInstance != NULL) {
					pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];

					/* Event-Queue selektieren */
					pEventPipeHeaderRead = NULL;
					#warning WriteToFile
					if (pAccessEventPipe->WriteToFile == FALSE) {
						pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferEventIndex];
					}
					else if (pAccessEventPipe->WriteToFile == TRUE &&
						pEventPipeInstance->ReadDataBusy == FALSE)
					{
						if (pEventPipeInstance->fileIdent == NULL) {
							pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferEventIndex];
						}
						else if (pEventPipeInstance->ReadDataBusy == FALSE) {
							pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
						}
					}

					/* Event-Queue selektieren */
					pEventPipeHeaderRead = NULL;
					#warning WriteToFile
					if (pAccessEventPipe->WriteToFile == FALSE ||
						pEventPipeInstance->fileIdent == NULL)
					{
						pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferEventIndex];
					}
					else if (pAccessEventPipe->WriteToFile == TRUE &&
							pEventPipeInstance->fileIdent != NULL &&
							pEventPipeInstance->ReadDataBusy == FALSE)
					{
						pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
					}


					/* Event-Queue selektieren */
					if (pEventPipeInstance->GetEventsFromReadPipe == TRUE && pEventPipeInstance->fileIdent == NULL) {
						pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
						if (pEventPipeHeaderRead->iPipeRead == pEventPipeHeaderRead->iPipeWrite) {
							pEventPipeInstance->GetEventsFromReadPipe = FALSE;
						}
					}
					pEventPipeHeaderRead = NULL;
					#warning WriteToFile
					if (pAccessEventPipe->WriteToFile == FALSE ||
						pEventPipeInstance->GetEventsFromReadPipe == FALSE)
					{
						pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferEventIndex];
					}
					else if (pAccessEventPipe->WriteToFile == TRUE &&
							pEventPipeInstance->GetEventsFromReadPipe == TRUE &&
							pEventPipeInstance->ReadDataBusy == FALSE)
					{
						pEventPipeHeaderRead = pEventPipeInstance->pEventPipeHeaderRead;
					}


					/* Event holen */
					if (pEventPipeHeaderRead != NULL) {
						while (pEventPipeHeaderRead->iPipeRead != pEventPipeHeaderRead->iPipeWrite) {
							/* Eintrag in Event-Pipe */
							iPipeRead = pEventPipeHeaderRead->iPipeRead;
							/* Header kopieren */
							pEventPipeData = (USINT*)&EventPipeData;
							for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
								pEventPipeData[i] = pEventPipeHeaderRead->pPipeData[iPipeRead++ % pEventPipeHeaderRead->pipe_size];
							}
							lengthExtern = EventPipeData.length;
							lengthPipeData = (EventPipeData.pData == NULL) ? lengthExtern : 0;
							lengthTimeStamp = (EventPipeData.linkstate == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
							if (EventPipeData.linkstate == cITEM_UNLINKED) {
								iPipeRead = iPipeRead + lengthPipeData + lengthTimeStamp;
								pEventPipeHeaderRead->iPipeRead = (iPipeRead % pEventPipeHeaderRead->pipe_size); /* Index erst nach Zugriff updaten !!! */
								pEventPipeHeaderRead->pipe_level--; /* Füllstand vermindern */
								continue;
							}
							if (pLength != NULL) {
								*pLength = lengthExtern;
							}
							if (pLengthTimeStamp != NULL) {
								*pLengthTimeStamp = lengthTimeStamp;
							}
							/* Buffergröße überprüfen */
							if (sizeof(EVENT_PIPE_DATA) + lengthPipeData + lengthTimeStamp <= lengthEventBuffer) {
								/* Header austragen */
								for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
									pEventBuffer[i] = pEventPipeData[i];
								}
								/* Daten austragen */
								for (i=0; i<lengthPipeData + lengthTimeStamp; i++) {
									pEventBuffer[i + sizeof(EVENT_PIPE_DATA)] = pEventPipeHeaderRead->pPipeData[iPipeRead++ % pEventPipeHeaderRead->pipe_size];
								}
								pEventPipeHeaderRead->iPipeRead = (iPipeRead % pEventPipeHeaderRead->pipe_size); /* Index erst nach Zugriff updaten !!! */
								pEventPipeHeaderRead->pipe_level--; /* Füllstand vermindern */
								return cEC_OK;
							}
							else {
								return cEC_EVENTBUFFER_TO_SMALL;
							}
						} /* (pEventPipeHeader->iPipeRead != pEventPipeHeader->iPipeWrite) */
					} /* (pEventPipeHeader != NULL) */
					return cEC_NO_DATA_IN_EVENTBUFFER;
				}
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (type < pAccessServer->countType) */
			else {
				return cEC_INVALID_TYPE;
			}
		} /* (instance < pAccessSystem->countEventInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetEventItem */


/*=============================================================================
// Funktion: WorkEvent
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

#ifdef FERTIG

UINT WorkEvent (void *idBrServer) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;

	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;
	ACCESS_ITEM_EVENT_STATE	*pAccessEventItem;

	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_HEADER	*pEventPipeHeader;
	EVENT_PIPE_DATA 	EventPipeData;
	USINT				*pEventPipeData, *pEventPipeTime;

	UDINT		lengthExtern, lengthPipeData, lengthTimeStamp;
	UINT		iPipeWrite, i;

	UINT		instance, unit, type, point, item;
	USINT		*pValue;
	UINT		lengthValue;
	USINT		eventtype;
	USINT		*pDataExtern;
	RTCtime_typ	RTCTime;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		for (instance=0; instance<pAccessServer->countEventInstance; instance++) {
			for (unit=0; unit<pAccessServer->countUnit; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					if (pAccessUnit->pUnitEventState[instance].countEvent > 0) {
						for (type=0; type<pAccessUnit->countType; type++) {
							pAccessType = &pAccessUnit->pAccessType[type];
							pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
							if (pAccessType->pAccessPoint != NULL) {
								if (pAccessType->pTypeEventState[instance].countEvent > 0) {
									for (point=0; point<pAccessType->countPoint; point++) {
										pAccessPoint = &pAccessType->pAccessPoint[point];
										if (pAccessPoint->pAccessData != NULL) {
											if (pAccessPoint->pPointEventState[instance].countEvent > 0) {
												for (item=0; item<pTypeDesc->countItem ; item++) {
													pItemDesc = &pTypeDesc->pItemDesc[item];

													/* Event Datenbereich */
													pAccessEventItem = pAccessPoint->pAccessData + pAccessType->offiEvent + instance * pAccessType->leniEventNext + item * pAccessType->leniEventAlign;
													if (*pAccessEventItem == TRUE) {
														/* Link Datenbereich */
														pAccessEventLinkItem = pAccessPoint->pAccessData + pAccessType->offiEventlink + instance * pAccessType->leniEventlinkNext + item * pAccessType->leniEventlinkAlign;
														if (*pAccessEventLinkItem != cITEM_UNLINKED) {

															/* Daten wie für Funktions-Prototypen */
															pValue = pAccessPoint->pAccessData + pAccessType->offExtern + pItemDesc->offItem;
															lengthValue = pItemDesc->lenItem;
															eventtype = 0;
															pDataExtern = NULL;
															if (pAccessType->lenTime > 0) {
																memcpy(&RTCTime, pAccessPoint->pAccessData + pAccessType->offTime + item * pAccessType->lenTimeAlign, pAccessType->lenTime);
															}
															else {
																memset(&RTCTime, 0, sizeof(RTCTime));
															}

															/* SetEventItem */
															pAccessEventPipe = pAccessServer->pAccessEventPipe;
															lengthExtern = pItemDesc->lenItem;
															lengthTimeStamp = (*pAccessEventLinkItem == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
															pEventPipeHeader = (EVENT_PIPE_HEADER*)((USINT*)pAccessEventPipe[type].pEventPipeHeader + instance * pAccessEventPipe[type].length_pipe_next);
															lengthPipeData = (pValue != NULL) ? lengthValue : 0;
															if (pEventPipeHeader != NULL &&
																pEventPipeHeader->stateEvent.countEvent == 0 &&
																((pEventPipeHeader->iPipeWrite >= pEventPipeHeader->iPipeRead &&
																(UINT)(pEventPipeHeader->pipe_size - (pEventPipeHeader->iPipeWrite - pEventPipeHeader->iPipeRead)) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthPipeData) ||
																(pEventPipeHeader->iPipeWrite < pEventPipeHeader->iPipeRead &&
																(UINT)(pEventPipeHeader->iPipeRead - pEventPipeHeader->iPipeWrite) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthPipeData))) {

																if (pValue != NULL || pDataExtern != NULL) {
																	if ((pValue != NULL && lengthValue == lengthExtern) || pDataExtern != NULL) {
																		/* Eintrag in Event-Pipe */
																		iPipeWrite = pEventPipeHeader->iPipeWrite;

																		/* Header vorbesetzen */
																		EventPipeData.linkstate = *pAccessEventLinkItem;
																		EventPipeData.eventtype = eventtype;
																		EventPipeData.unit = unit;
																		EventPipeData.device = device;
																		EventPipeData.type = type;
																		EventPipeData.point= point;
																		EventPipeData.item = item;
																		EventPipeData.length = lengthValue;
																		EventPipeData.pData = pDataExtern;
																		pEventPipeData = (USINT*)&EventPipeData;
																		/* Header eintragen */
																		for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
																			pEventPipeHeader->pPipeData[iPipeWrite++ % pEventPipeHeader->pipe_size] = pEventPipeData[i];
																		}
																		if (pValue != NULL) {
																			/* Daten eintragen */
																			for (i=0; i<lengthValue; i++) {
																				pEventPipeHeader->pPipeData[iPipeWrite++ % pEventPipeHeader->pipe_size] = pValue[i];
																			}
																		}
																		/* optional Zeitstempel eintragen */
																		if (lengthTimeStamp > 0) {
																			pEventPipeTime = (USINT*)&RTCTime;
																			for (i=0; i<lengthTimeStamp; i++) {
																				pEventPipeHeader->pPipeData[iPipeWrite++ % pEventPipeHeader->pipe_size] = pEventPipeTime[i];
																			}
																		}
																		/* Ereigniszeit aktualisieren */
																		if (pItemDesc->tsItem == TRUE) {
																			memcpy(pAccessPoint->pAccessData + pAccessType->offiTime + instance * pAccessType->leniTimeNext + item * pAccessType->leniTimeAlign, &RTCTime, pAccessType->leniTime);
																		}
																		pEventPipeHeader->iPipeWrite = (iPipeWrite % pEventPipeHeader->pipe_size); /* Index erst nach Zugriff updaten !!! */
																		pEventPipeHeader->pipe_level++; /* Füllstand erhöhen */

																	} /* ((pValue != NULL && lengthValue == lengthExtern) || pDataExtern != NULL) */
																	else {
																		return cEC_INVALID_LENGTH;
																	}
																} /* (pValue != NULL || pDataExtern != NULL) */
																else {
																	return cEC_INVALID_HANDLE;
																}
															}
														} /* (*pAccessEventLinkItem != cITEM_UNLINKED) */

														*pAccessEventItem = FALSE;
														pAccessPoint->pPointEventState[instance].countEvent--;
														pAccessType->pTypeEventState[instance].countEvent--;
														pAccessUnit->pUnitEventState[instance].countEvent--;

													} /* (*pAccessEventItem == TRUE) */
												} /* (item=0; item<pTypeDesc->countItem ; item++) */
											} /* (pAccessPoint->pPointEventState[instance] > 0) */
										} /* (pAccessPoint->pAccessData != NULL) */
									} /* (point=0; point<pAccessType->countPoint; point++) */
								} /* (pAccessType->pTypeEventState[instance].countEvent > 0) */
							} /* (pAccessType->pAccessPoint != NULL) */
						} /* (type=0; type<pAccessUnit->countType; type++) */
					} /* (pAccessUnit->pUnitEventState[instance].countEvent > 0) */
				} /* (pAccessUnit->pAccessType != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnit; unit++) */
		} /* (instance=0; instance<pAccessServer->countEventInstance; instance++) */
		return cEC_OK;
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* WorkEvent */

/*UINT WorkEvent (void *idBrServer) {*/

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_ITEM_EVENT_LINK	*pAccessEventLinkItem;
	ACCESS_EVENT_PIPE		*pAccessEventPipe;
	EVENT_PIPE_HEADER		*pEventPipeHeader;

	UDINT					lengthExtern, lengthTimeStamp;
	UINT					i, instance, unit, device, type, point, iPipeWrite;

	EVENT_PIPE_DATA 		EventPipeData;
	USINT					*pEventPipeData, *pData, *pEventPipeTime;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		for (instance=0; instance<pAccessServer->countEventInstance; instance++) {
			for (unit=0; unit<pAccessServer->countUnitNo; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					if (pAccessServer->pUnitEventState[pAccessServer->countEventInstance * unit + instance].countEvent != 0) {
						for (device=0; device<pAccessUnit[unit].countDeviceNo; device++) {
							pAccessDevice = pAccessUnit[unit].pAccessDevice;
							if (device == pAccessDevice[device].device_no && pAccessDevice[device].pAccessType != NULL) {
								if (pAccessUnit[unit].pDeviceEventState[pAccessServer->countEventInstance * device + instance].countEvent != 0) {
									for (type=0; type<pAccessDevice[device].countType; type++) {
										if (pAccessDevice[device].pTypeEventState[pAccessServer->countEventInstance * type + instance].countEvent != 0) {

											pAccessType = pAccessDevice[device].pAccessType;
											pAccessEventPipe = pAccessServer->pAccessEventPipe;
											pEventPipeHeader = (EVENT_PIPE_HEADER*)((USINT*)pAccessEventPipe[type].pEventPipeHeader + instance * pAccessEventPipe[type].length_pipe_next);

											for (point=0; point<pAccessType[type].countPoint; point++) {

												pAccessEventLinkItem = pAccessType[type].pData_ieventlink + point * pAccessType[type].length_ieventlink_next;
												lengthExtern = pAccessEventPipe[type].TypeDefinition.length_extern;
												lengthTimeStamp = (pAccessEventLinkItem[instance] == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;

												/* genügend Platz in der Event-Pipe ? */
												if (pEventPipeHeader != NULL &&
													pEventPipeHeader->stateEvent.countEvent != 0 &&
													((pEventPipeHeader->iPipeWrite >= pEventPipeHeader->iPipeRead &&
													(UINT)(pEventPipeHeader->pipe_size - (pEventPipeHeader->iPipeWrite - pEventPipeHeader->iPipeRead)) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthExtern) ||
													(pEventPipeHeader->iPipeWrite < pEventPipeHeader->iPipeRead &&
													(UINT)(pEventPipeHeader->iPipeRead - pEventPipeHeader->iPipeWrite) > sizeof(EVENT_PIPE_DATA) + lengthTimeStamp + lengthExtern))) {

													/* Event auf Datenpunkt vorhanden ? */
													if (*(UDINT*)(pAccessType[type].pData_ievent + point * pAccessType[type].length_ievent_next + instance * pAccessType[type].length_ievent_align) != 0 &&
														*(UDINT*)(pAccessType[type].pData_ievent + point * pAccessType[type].length_ievent_next + instance * pAccessType[type].length_ievent_align) == pEventPipeHeader->stateEvent.firstEvent) {

														/* Datenpunkt noch gelinkt ? */
														if (pAccessEventLinkItem[instance] != cITEM_UNLINKED) {

															/* Eintrag in Event-Pipe */
															iPipeWrite = pEventPipeHeader->iPipeWrite;

															/* Header eintragen */
															EventPipeData.linkstate = pAccessEventLinkItem[instance];
															EventPipeData.eventtype = 0;
															EventPipeData.unit = unit;
															EventPipeData.device = device;
															EventPipeData.type = type;
															EventPipeData.point= point;
															EventPipeData.item = 0;
															EventPipeData.length = lengthExtern;
															EventPipeData.pData = NULL;
															pEventPipeData = (USINT*)&EventPipeData;
															for (i=0; i<sizeof(EVENT_PIPE_DATA); i++) {
																pEventPipeHeader->pPipeData[iPipeWrite++ % pEventPipeHeader->pipe_size] = pEventPipeData[i];
															}

															/* Daten eintragen */
															pData = pAccessType[type].pData_extern + point * pAccessType[type].length_extern_align;
															for (i=0; i<lengthExtern; i++) {
																pEventPipeHeader->pPipeData[iPipeWrite++ % pEventPipeHeader->pipe_size] = pData[i];
															}
															pEventPipeHeader->iPipeWrite = (iPipeWrite % pEventPipeHeader->pipe_size); /* Index erst nach Zugriff updaten !!! */
															pEventPipeHeader->pipe_level++; /* Füllstand erhöhen */

															/* optional Zeitstempel eintragen */
															if (lengthTimeStamp > 0) {
																pEventPipeTime = pAccessType[type].pData_time + point * pAccessType[type].length_time_align;
																for (i=0; i<lengthTimeStamp; i++) {
																	pEventPipeHeader->pPipeData[iPipeWrite++ % pEventPipeHeader->pipe_size] = pEventPipeTime[i];
																}
															}
														} /* (pAccessItemLink[instance] != cITEM_UNLINKED) */

														/* Event auf Pipe-Typ austragen */
														pEventPipeHeader->stateEvent.firstEvent++;
														if (pEventPipeHeader->stateEvent.firstEvent > pEventPipeHeader->stateEvent.lastEvent) {
															pEventPipeHeader->stateEvent.firstEvent = 0;
															pEventPipeHeader->stateEvent.lastEvent = 0;
														}
														/* Event auf Punkt, Typ, Station, System austragen */
														*(UDINT*)(pAccessType[type].pData_ievent + point * pAccessType[type].length_ievent_next + instance * pAccessType[type].length_ievent_align) = 0;
														pEventPipeHeader->stateEvent.countEvent--;

														pAccessDevice[device].pTypeEventState[pAccessServer->countEventInstance * type + instance].countEvent--;
														pAccessUnit[unit].pDeviceEventState[pAccessServer->countEventInstance * device + instance].countEvent--;
														pAccessServer->pUnitEventState[pAccessServer->countEventInstance * unit + instance].countEvent--;

														/*if (pAccessServer->stateEvent[instance].countEvent == 0) {*/
															/* sicherheitshalber alle Pipe-Events bezüglich first/last austragen */
															/*for (m=0; m<pAccessServer->countType; m++) {
																pAccessEventPipe[m].stateEvent.firstEvent = 0;
																pAccessEventPipe[m].stateEvent.lastEvent = 0;
															}*/
														/*} (pAccessSystem->stateEvent.countEvent == 0) */
													} /* ... */
												} /* ... */
												else {
													break;
												}
											} /* (point=0; point<pAccessType[type].countPoint; point++) */
										} /* (pAccessDevice[device].pTypeEventState[pAccessServer->countEventInstance * type + instance].countEvent != 0) */
									} /* (type=0; type<pAccessDevice[device].countType; type++) */
								} /* (pAccessUnit[unit].pDeviceEventState[pAccessServer->countEventInstance * device + instance].countEvent != 0) */
							} /* (device == pAccessDevice[device].device_no && pAccessDevice[device].pAccessType != NULL) */
						} /* (device=0; device<pAccessUnit.countDeviceNo; device++) */
					} /* (pAccessServer->pUnitEventState[pAccessServer->countInstance * unit + instance].countEvent != 0) */
				} /* (unit == pAccessUnit[unit].unit_no && pAccessUnit[unit].pAccessDevice != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnitNo; unit++) */
		} /* (instance=0; instance<pAccessServer->countEventInstance; instance++) */
		return cEC_OK;
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* work_event */
#endif

/*=============================================================================
// Funktion: LinkWriteInstance
//-----------------------------------------------------------------------------
// System mit allen zugehörigen Datenpunkten für die Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT LinkWriteInstance (void *idBrServer, UINT instance) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ACCESS_ITEM_WRITE_LINK	*pAccessWriteLinkItem;

	unsigned short	unit, type, point, item;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {
			for (unit=0; unit<pAccessServer->countUnit; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					for (type=0; type<pAccessUnit->countType; type++) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (pAccessType->pAccessPoint != NULL) {
							for (point=0; point<pAccessType->countPoint; point++) {
								pAccessPoint = &pAccessType->pAccessPoint[point];
								if (pAccessPoint->pAccessData != NULL) {
									for (item=0; item<pTypeDesc->countItem ; item++) {
										/* Link Datenbereich */
										pAccessWriteLinkItem = pAccessPoint->pAccessData + pAccessType->offWritelink + item * pAccessType->lenWritelinkAlign;
										if (*pAccessWriteLinkItem == 0) {
											*pAccessWriteLinkItem = instance + 1;
										}
									} /* (item = 0; item<pTypeDesc->countItem ; item++) */
								} /* (pAccessPoint->pAccessData != NULL) */
							} /* (point=0; point<pAccessType->countPoint; point++) */
						} /* (pAccessType->pAccessPoint != NULL) */
					} /* (type=0; type<pAccessUnit->countType; type++) */
				} /* (pAccessUnit->pAccessType != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnit; unit++) */
			return cEC_OK;
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* LinkWriteInstance */


/*=============================================================================
// Funktion: UnlinkWriteInstance
//-----------------------------------------------------------------------------
// System mit allen zugehörigen Datenpunkten für die Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT UnlinkWriteInstance (void *idBrServer, UINT instance) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ACCESS_ITEM_WRITE_LINK	*pAccessWriteLinkItem;

	unsigned short	unit, type, point, item;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {
			for (unit=0; unit<pAccessServer->countUnit; unit++) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (pAccessUnit->pAccessType != NULL) {
					for (type=0; type<pAccessUnit->countType; type++) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (pAccessType->pAccessPoint != NULL) {
							for (point=0; point<pAccessType->countPoint; point++) {
								pAccessPoint = &pAccessType->pAccessPoint[point];
								if (pAccessPoint->pAccessData != NULL) {
									for (item=0; item<pTypeDesc->countItem ; item++) {
										/* Unlink Datenbereich */
										pAccessWriteLinkItem = pAccessPoint->pAccessData + pAccessType->offWritelink + item * pAccessType->lenWritelinkAlign;
										if (*pAccessWriteLinkItem > 0) {
											*pAccessWriteLinkItem = 0;
										}
									} /* (item = 0; item<pTypeDesc->countItem ; item++) */
								} /* (pAccessPoint->pAccessData != NULL) */
							} /* (point=0; point<pAccessType->countPoint; point++) */
						} /* (pAccessType->pAccessPoint != NULL) */
					} /* (type=0; type<pAccessUnit->countType; type++) */
				} /* (pAccessUnit->pAccessType != NULL) */
			} /* (unit=0; unit<pAccessServer->countUnit; unit++) */
			return cEC_OK;
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* UnlinkWriteInstance */


/*=============================================================================
// Funktion: LinkWriteItem
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehandlung anmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT LinkWriteItem (void *idBrServer, UINT instance,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					UDINT handle) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_ITEM_WRITE_LINK		*pAccessWriteLinkItem;
	ACCESS_ITEM_WRITE_HANDLE	*pAccessWriteHandleItem;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								/* Link Datenbereich */
								pAccessWriteLinkItem = pAccessPoint->pAccessData + pAccessType->offWritelink + item * pAccessType->lenWritelinkAlign;
								pAccessWriteHandleItem = (ACCESS_ITEM_WRITE_HANDLE*)(pAccessPoint->pAccessData + pAccessType->offWritehandle + item * pAccessType->lenWritehandleAlign);
								if (*pAccessWriteLinkItem == 0) {
									*pAccessWriteLinkItem = instance + 1;
									*pAccessWriteHandleItem = handle;
									return cEC_OK;
								}
								else {
									return cEC_ITEM_ALREADY_LINKED;
								}

							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* LinkWriteItem */


/*=============================================================================
// Funktion: UnlinkWriteItem
//-----------------------------------------------------------------------------
// Datenpunkt für Ereignisbehnadlung abmelden.
// Input:	idBrServer: System-ID
//			instance:	Instanz-Nummer
//			unit:		Anlagen-Nummer
//			device:		Gerät-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			item:		Strukturelement-Nummer
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT UnlinkWriteItem (void *idBrServer, UINT instance,
						UINT unit, UINT device, UINT type, UINT point, UINT item) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_ITEM_WRITE_LINK		*pAccessWriteLinkItem;
	ACCESS_ITEM_WRITE_HANDLE	*pAccessWriteHandleItem;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {
			if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
				pAccessUnit = &pAccessServer->pAccessUnit[unit];
				if (device == pAccessServer->noDevice) {
					if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
						pAccessType = &pAccessUnit->pAccessType[type];
						pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
						if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
							pAccessPoint = &pAccessType->pAccessPoint[point];
							if (item < pTypeDesc->countItem) {
								pItemDesc = &pTypeDesc->pItemDesc[item];

								/* Unlink Datenbereich */
								pAccessWriteLinkItem = pAccessPoint->pAccessData + pAccessType->offWritelink + item * pAccessType->lenWritelinkAlign;
								pAccessWriteHandleItem = (ACCESS_ITEM_WRITE_HANDLE*)(pAccessPoint->pAccessData + pAccessType->offWritehandle + item * pAccessType->lenWritehandleAlign);
								if (*pAccessWriteLinkItem > 0) {
									*pAccessWriteLinkItem = 0;
									*pAccessWriteHandleItem = 0;
									return cEC_OK;
								}
								else {
									return cEC_ITEM_ALREADY_UNLINKED;
								}

							} /* (item < pTypeDesc->countItem) */
							else {
								return cEC_INVALID_ITEM;
							}
						} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
						else {
							return cEC_INVALID_POINT;
						}
					} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
					else {
						return cEC_INVALID_TYPE;
					}
				} /* (device == pAccessServer->noDevice) */
				else {
					return cEC_INVALID_DEVICE;
				}
			} /* (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) */
			else {
				return cEC_INVALID_UNIT;
			}
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* UnlinkWriteItem */


/*=============================================================================
// Funktion: SetWriteItem
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			system:		Anlagen-Nummer
//			station:	Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT SetWriteItem (void *idBrServer,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue) {

	ACCESS_SERVER	*pAccessServer;
	ACCESS_UNIT		*pAccessUnit;
	ACCESS_TYPE		*pAccessType;
	ACCESS_POINT	*pAccessPoint;
	TYPE_DESC		*pTypeDesc;
	ITEM_DESC		*pItemDesc;
	ACCESS_WRITE_PIPE			*pAccessWritePipe;
	WRITE_PIPE_HEADER			*pWritePipeHeader;
	ACCESS_ITEM_WRITE_LINK		*pAccessWriteLinkItem;
	ACCESS_ITEM_WRITE_HANDLE	*pAccessWriteHandleItem;

	UDINT			lengthExtern;
	UINT			instance, iPipeWrite, i;

	WRITE_PIPE_DATA	WritePipeData;
	USINT			*pWritePipeData;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (unit < pAccessServer->countUnit && pAccessServer->pAccessUnit[unit].pAccessType != NULL) {
			pAccessUnit = &pAccessServer->pAccessUnit[unit];
			if (device == pAccessServer->noDevice) {
				if (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) {
					pAccessType = &pAccessUnit->pAccessType[type];
					pTypeDesc = &pAccessServer->pAccessTypeDesc[type];
					if (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) {
						pAccessPoint = &pAccessType->pAccessPoint[point];
						if (item < pTypeDesc->countItem) {
							pItemDesc = &pTypeDesc->pItemDesc[item];
							pAccessWriteLinkItem = pAccessPoint->pAccessData + pAccessType->offWritelink + item * pAccessType->lenWritelinkAlign;
							pAccessWriteHandleItem = (ACCESS_ITEM_WRITE_HANDLE*)(pAccessPoint->pAccessData + pAccessType->offWritehandle + item * pAccessType->lenWritehandleAlign);
							if (*pAccessWriteLinkItem > 0) {
								instance = *pAccessWriteLinkItem - 1;
								pAccessWritePipe = pAccessServer->pAccessWritePipe;
								lengthExtern = pItemDesc->lenItem;
								pWritePipeHeader = (WRITE_PIPE_HEADER*)((USINT*)pAccessWritePipe->pWritePipeHeader + instance * pAccessWritePipe->length_pipe_next);
								if (pWritePipeHeader != NULL
									&&
									(
										(
											pWritePipeHeader->iPipeWrite >= pWritePipeHeader->iPipeRead &&
											(UINT)(pWritePipeHeader->pipe_size - (pWritePipeHeader->iPipeWrite - pWritePipeHeader->iPipeRead)) > sizeof(WRITE_PIPE_DATA) + lengthValue
										)
										||
										(
											pWritePipeHeader->iPipeWrite < pWritePipeHeader->iPipeRead &&
											(UINT)(pWritePipeHeader->iPipeRead - pWritePipeHeader->iPipeWrite) > sizeof(WRITE_PIPE_DATA) + lengthValue
										)
									)
								) {
									if (pValue != NULL) {
										if (lengthValue == lengthExtern) {
											/* Eintrag in Event-Pipe */
											iPipeWrite = pWritePipeHeader->iPipeWrite;

											/* Header eintragen */
											WritePipeData.linkstate = cITEM_LINKED_WITHOUT_TIMESTAMP;
											WritePipeData.eventtype = 0;
											WritePipeData.unit = unit;
											WritePipeData.device = device;
											WritePipeData.type = type;
											WritePipeData.point= point;
											WritePipeData.item = item;
											WritePipeData.length = lengthValue;
											WritePipeData.pData = (USINT*)*pAccessWriteHandleItem;
											pWritePipeData = (USINT*)&WritePipeData;
											/* Header eintragen */
											for (i=0; i<sizeof(WRITE_PIPE_DATA); i++) {
												pWritePipeHeader->pPipeData[iPipeWrite++ % pWritePipeHeader->pipe_size] = pWritePipeData[i];
											}
											if (pValue != NULL) {
												/* Daten eintragen */
												for (i=0; i<lengthValue; i++) {
													pWritePipeHeader->pPipeData[iPipeWrite++ % pWritePipeHeader->pipe_size] = pValue[i];
												}
											}
											pWritePipeHeader->iPipeWrite = (iPipeWrite % pWritePipeHeader->pipe_size); /* Index erst nach Zugriff updaten !!! */
											pWritePipeHeader->pipe_level++; /* Füllstand erhöhen */
											return cEC_OK;
										} /* (lengthValue == lengthExtern) */
										else {
											return cEC_INVALID_LENGTH;
										}
									} /* (pValue != NULL) */
									else {
										return cEC_INVALID_HANDLE;
									}
								} /* ... */
								else {
									return cEC_WRITE_PIPE_FULL;
								}
							} /* (*pAccessWriteLinkItem > 0) */
							else {
								return cEC_ITEM_NOT_WRITEABLE;
							}
						} /* (item < pTypeDesc->countItem) */
						else {
							return cEC_INVALID_ITEM;
						}
					} /* (point < pAccessType->countPoint && pAccessType->pAccessPoint[point].pAccessData != NULL) */
					else {
						return cEC_INVALID_POINT;
					}
				} /* (type < pAccessUnit->countType && pAccessUnit->pAccessType[type].pAccessPoint != NULL) */
				else {
					return cEC_INVALID_TYPE;
				}
			} /* (device == pAccessServer->noDevice) */
			else {
				return cEC_INVALID_DEVICE;
			}
		} /* (unit < pAccessServer->countUnitNo) */
		else {
			return cEC_INVALID_UNIT;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* SetWriteItem */


/*=============================================================================
// Funktion: ScanWriteItem
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			system:		Anlagen-Nummer
//			station:	Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT ScanWriteItem (void *idBrServer, UINT instance,
					USINT* pWriteBuffer, UDINT lengthWriteBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp) {

	ACCESS_SERVER		*pAccessServer;
	ACCESS_WRITE_PIPE	*pAccessWritePipe;
	WRITE_PIPE_HEADER	*pWritePipeHeader;

	UINT				i, iPipeRead;
	UDINT				lengthExtern, lengthTimeStamp;

	WRITE_PIPE_DATA 	WritePipeData;
	USINT				*pWritePipeData;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {

			pAccessWritePipe = pAccessServer->pAccessWritePipe;
			pWritePipeHeader = (WRITE_PIPE_HEADER*)((USINT*)pAccessWritePipe->pWritePipeHeader + instance * pAccessWritePipe->length_pipe_next);
			if (pWritePipeHeader != NULL) {
				while (pWritePipeHeader->iPipeRead != pWritePipeHeader->iPipeWrite) {
					/* Eintrag in Event-Pipe */
					iPipeRead = pWritePipeHeader->iPipeRead;
					/* Header kopieren */
					pWritePipeData = (USINT*)&WritePipeData;
					for (i=0; i<sizeof(WRITE_PIPE_DATA); i++) {
						pWritePipeData[i] = pWritePipeHeader->pPipeData[iPipeRead++ % pWritePipeHeader->pipe_size];
					}
					lengthExtern = WritePipeData.length;
					lengthTimeStamp = (WritePipeData.linkstate == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
					if (WritePipeData.linkstate == cITEM_UNLINKED) {
						iPipeRead = iPipeRead + lengthExtern + lengthTimeStamp;
						pWritePipeHeader->iPipeRead = (iPipeRead % pWritePipeHeader->pipe_size); /* Index erst nach Zugriff updaten !!! */
						pWritePipeHeader->pipe_level--; /* Füllstand vermindern */
						continue;
					}
					if (pLength != NULL) {
						*pLength = lengthExtern;
					}
					if (pLengthTimeStamp != NULL) {
						*pLengthTimeStamp = lengthTimeStamp;
					}
					/* Buffergröße überprüfen */
					if (sizeof(WRITE_PIPE_DATA) + lengthExtern + lengthTimeStamp <= lengthWriteBuffer) {
						/* Header austragen */
						for (i=0; i<sizeof(WRITE_PIPE_DATA); i++) {
							pWriteBuffer[i] = pWritePipeData[i];
						}
						/* Daten austragen */
						for (i=0; i<lengthExtern + lengthTimeStamp; i++) {
							pWriteBuffer[i + sizeof(WRITE_PIPE_DATA)] = pWritePipeHeader->pPipeData[iPipeRead++ % pWritePipeHeader->pipe_size];
						}
						return cEC_OK;
					}
					else {
						return cEC_EVENTBUFFER_TO_SMALL;
					}
				} /* (pEventPipeHeader->iPipeRead != pEventPipeHeader->iPipeWrite) */
			} /* (pWritePipeHeader != NULL) */
			return cEC_NO_DATA_IN_WRITEBUFFER;
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* ScanWriteItem */


/*=============================================================================
// Funktion: GetWriteItem
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			system:		Anlagen-Nummer
//			station:	Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

UINT GetWriteItem (void *idBrServer, UINT instance,
					USINT* pWriteBuffer, UDINT lengthWriteBuffer,
					UDINT* pLength, UDINT* pLengthTimeStamp) {

	ACCESS_SERVER		*pAccessServer;
	ACCESS_WRITE_PIPE	*pAccessWritePipe;
	WRITE_PIPE_HEADER	*pWritePipeHeader;

	UINT				i, iPipeRead;
	UDINT				lengthExtern, lengthTimeStamp;

	WRITE_PIPE_DATA 	WritePipeData;
	USINT				*pWritePipeData;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		if (instance < pAccessServer->countWriteInstance) {

			pAccessWritePipe = pAccessServer->pAccessWritePipe;
			pWritePipeHeader = (WRITE_PIPE_HEADER*)((USINT*)pAccessWritePipe->pWritePipeHeader + instance * pAccessWritePipe->length_pipe_next);
			if (pWritePipeHeader != NULL) {
				while (pWritePipeHeader->iPipeRead != pWritePipeHeader->iPipeWrite) {
					/* Eintrag in Event-Pipe */
					iPipeRead = pWritePipeHeader->iPipeRead;
					/* Header kopieren */
					pWritePipeData = (USINT*)&WritePipeData;
					for (i=0; i<sizeof(WRITE_PIPE_DATA); i++) {
						pWritePipeData[i] = pWritePipeHeader->pPipeData[iPipeRead++ % pWritePipeHeader->pipe_size];
					}
					lengthExtern = WritePipeData.length;
					lengthTimeStamp = (WritePipeData.linkstate == cITEM_LINKED_WITH_TIMESTAMP) ? sizeof(RTCtime_typ) : 0;
					if (WritePipeData.linkstate == cITEM_UNLINKED) {
						iPipeRead = iPipeRead + lengthExtern + lengthTimeStamp;
						pWritePipeHeader->iPipeRead = (iPipeRead % pWritePipeHeader->pipe_size); /* Index erst nach Zugriff updaten !!! */
						pWritePipeHeader->pipe_level--; /* Füllstand vermindern */
						continue;
					}
					if (pLength != NULL) {
						*pLength = lengthExtern;
					}
					if (pLengthTimeStamp != NULL) {
						*pLengthTimeStamp = lengthTimeStamp;
					}
					/* Buffergröße überprüfen */
					if (sizeof(WRITE_PIPE_DATA) + lengthExtern + lengthTimeStamp <= lengthWriteBuffer) {
						/* Header austragen */
						for (i=0; i<sizeof(WRITE_PIPE_DATA); i++) {
							pWriteBuffer[i] = pWritePipeData[i];
						}
						/* Daten austragen */
						for (i=0; i<lengthExtern + lengthTimeStamp; i++) {
							pWriteBuffer[i + sizeof(WRITE_PIPE_DATA)] = pWritePipeHeader->pPipeData[iPipeRead++ % pWritePipeHeader->pipe_size];
						}
						pWritePipeHeader->iPipeRead = (iPipeRead % pWritePipeHeader->pipe_size); /* Index erst nach Zugriff updaten !!! */
						pWritePipeHeader->pipe_level--; /* Füllstand vermindern */
						return cEC_OK;
					}
					else {
						return cEC_EVENTBUFFER_TO_SMALL;
					}
				} /* (pEventPipeHeader->iPipeRead != pEventPipeHeader->iPipeWrite) */
			} /* (pWritePipeHeader != NULL) */
			return cEC_NO_DATA_IN_WRITEBUFFER;
		} /* (instance < pAccessSystem->countWriteInstance) */
		else {
			return cEC_INVALID_INSTANCE;
		}
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* GetWriteItem */


/*=============================================================================
// Funktion: CopyPipeToFile
//-----------------------------------------------------------------------------
// Zugriff auf das Prozessabbild.
// Input:	idBrServer: System-ID
//			system:		Anlagen-Nummer
//			station:	Stations-Nummer
//			type:		Datentyp-Nummer
//			point:		Datenpunkt-Nummer
//			ppData_extern:	Zeiger auf externen Datenbereich (Rückgabewert)
//			plength_extern:	Länge des externen Datenbereichs (Rückgabewert)
//			ppData_intern:	Zeiger auf internen Datenbereich (Rückgabewert)
//			plength_intern:	Länge des internen Datenbereichs (Rückgabewert)
//			ppData_time:	Zeiger auf Zeitbereich (Rückgabewert)
//			plength_time:	Länge des Zeitbereichs (Rückgabewert)
//			ppData_event:	Zeiger auf Ereignis-Kennung (Rückgabewert)
//			plength_event:	Länge der Ereignis-Kennung (Rückgabewert)
// Return:	== 0 -> kein Fehler aufgetreten.
//			!= 0 -> Fehlerstatus der Funktion.
//=============================================================================*/

/*
	ltoa  converts a signed long  value into ASCII Format
	ultoa converts a unsigned long  value into ASCII Format

	both returns the length of the converted string.
*/

int ultoa(unsigned long value, char *buffer) {

	int string_len=0, i, j;
	char swap;


	do {
		buffer[string_len++]=(value % 10) + '0'; /*calculate next digit*/
	} while((value = value/10)); /*while there are more digits*/

	/*swap digits*/
	i=0;
	j=string_len-1;

	while (i<j) {
		swap = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = swap;
		i++; j--;
	}
	buffer[string_len]=0; /*NULL termination of string*/
	return string_len;    /*Return lenght of string*/
}

int ltoa(long value, char *buffer) {

	int string_len=0, neg=0, i, j;
	char swap;
	unsigned long absval;  /*we have to use unsigned long because, with signed long
	                         0x80000000 would not work.*/

	if (value<0) {
		absval = -value;
		neg=1;
		buffer[0]='-';
		string_len=1;
	}
	else {
		absval = value;
	}
	do {
		buffer[string_len++]=(absval % 10) + '0'; /*calculate next digit*/
	} while((absval = absval/10)); /*while there are more digits*/

	/*swap digits*/
	i=neg;  /*begin at pos 1 if negative*/
	j=string_len-1;

	while (i<j) {
		swap = buffer[i];
		buffer[i] = buffer[j];
		buffer[j] = swap;
		i++; j--;
	}
	buffer[string_len]=0; /*NULL termination of string*/
	return string_len;    /*Return lenght of string*/
}

UINT CopyPipeToFile (void *idBrServer) {

	ACCESS_SERVER		*pAccessServer;
	ACCESS_EVENT_PIPE	*pAccessEventPipe;
	EVENT_PIPE_INSTANCE	*pEventPipeInstance;
	EVENT_PIPE_HEADER	*pEventPipeHeaderWrite, *pEventPipeHeaderRead;
	UINT				instance, type;


	if (idBrServer != NULL) {
		pAccessServer = (ACCESS_SERVER*)idBrServer;
		for (instance = 0; instance < pAccessServer->countEventInstance; instance++) {
			for (type = 0; type < pAccessServer->countType; type++) {
				pAccessEventPipe = &pAccessServer->pAccessEventPipe[type];
				if (pAccessEventPipe->pEventPipeInstance != NULL) {
					pEventPipeInstance = &pAccessEventPipe->pEventPipeInstance[instance];
					#warning WriteToFile
					if (pAccessEventPipe->WriteToFile == TRUE) {

						pEventPipeHeaderWrite = pEventPipeInstance->pEventPipeHeaderWrite[pEventPipeInstance->BufferFileIndex];
						pEventPipeHeaderRead  = pEventPipeInstance->pEventPipeHeaderRead;

						if (pEventPipeHeaderWrite != NULL && pEventPipeHeaderRead != NULL) {

							/* ---------------- */
							/* File - Schreiben */
							/* ---------------- */
							switch (pEventPipeInstance->fileStep) {

								/* ------- */
								/* default */
								/* ------- */
								case 0:
									if (pEventPipeInstance->BufferEventIndex != pEventPipeInstance->BufferFileIndex)
									{
										if (1==0 && pEventPipeHeaderRead->iPipeWrite == pEventPipeHeaderRead->iPipeRead) {
										}
										else {
											pEventPipeInstance->fileState = 65535;

											pEventPipeInstance->WriteDataBusy = TRUE;
											pEventPipeInstance->ReadDataBusy = TRUE;

											pEventPipeInstance->WriteDataBusy = FALSE;
											pEventPipeInstance->ReadDataBusy = FALSE;

											strcpy(pEventPipeInstance->FileDevice, "CompactFlash");
											strcpy(pEventPipeInstance->FileName, "Pipe_Type");
											ultoa(type, &pEventPipeInstance->FileName[strlen(pEventPipeInstance->FileName)]);
											strcat(pEventPipeInstance->FileName, "_Inst");
											ultoa(instance, &pEventPipeInstance->FileName[strlen(pEventPipeInstance->FileName)]);
											strcat(pEventPipeInstance->FileName, ".dat");

											pEventPipeInstance->fileOffsetWrite = 0;
											pEventPipeInstance->GetEventsFromReadPipe = TRUE;

											pEventPipeInstance->fileStep = 1;
										}
									}
									break;

								/* ----------- */
								/* File öffnen */
								/* ----------- */
								case 1:
									pEventPipeInstance->iFileOpen.enable = 1;
									pEventPipeInstance->iFileOpen.pDevice = (unsigned long)pEventPipeInstance->FileDevice;
									pEventPipeInstance->iFileOpen.pFile   = (unsigned long)pEventPipeInstance->FileName;
									pEventPipeInstance->iFileOpen.mode = FILE_RW;
									FileOpen(&pEventPipeInstance->iFileOpen);
									pEventPipeInstance->fileIdent = pEventPipeInstance->iFileOpen.ident;
									pEventPipeInstance->fileLen = pEventPipeInstance->iFileOpen.filelen;
									if (pEventPipeInstance->iFileOpen.status == fiERR_FILE_NOT_FOUND) {
										pEventPipeInstance->fileStep = 2;
									}
									else if (pEventPipeInstance->iFileOpen.status == 0) {
										pEventPipeInstance->fileStep = 3;
									}
									else if (pEventPipeInstance->iFileOpen.status != 65535) {
										pEventPipeInstance->fileState = pEventPipeInstance->iFileOpen.status;
										pEventPipeInstance->fileStep = 8;
									}
									break;

								/* ------------ */
								/* File anlegen */
								/* ------------ */
								case 2:
									pEventPipeInstance->iFileCreate.enable = 1;
									pEventPipeInstance->iFileCreate.pDevice = (unsigned long)pEventPipeInstance->FileDevice;
									pEventPipeInstance->iFileCreate.pFile   = (unsigned long)pEventPipeInstance->FileName;
									FileCreate(&pEventPipeInstance->iFileCreate);
									pEventPipeInstance->fileIdent = pEventPipeInstance->iFileCreate.ident;
									if (pEventPipeInstance->iFileCreate.status == 0) {
										pEventPipeInstance->fileStep = 3;
									}
									else if (pEventPipeInstance->iFileCreate.status != 65535) {
										pEventPipeInstance->fileState = pEventPipeInstance->iFileCreate.status;
										pEventPipeInstance->fileStep = 8;
									}
									break;

								/* -------------- */
								/* File schreiben */
								/* -------------- */
								case 3:
									if (pEventPipeInstance->iFileRead.status == 0)
									{
										pEventPipeInstance->iFileWrite.enable = 1;
										pEventPipeInstance->iFileWrite.ident = pEventPipeInstance->fileIdent;
										pEventPipeInstance->iFileWrite.offset = pEventPipeInstance->fileOffsetWrite;
										pEventPipeInstance->iFileWrite.pSrc = (unsigned long)pEventPipeHeaderWrite;
										pEventPipeInstance->iFileWrite.len = sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderWrite->pipe_size;
										FileWrite(&pEventPipeInstance->iFileWrite);
										if (pEventPipeInstance->iFileWrite.status == 0) {
											pEventPipeHeaderWrite->iPipeRead = pEventPipeHeaderWrite->iPipeWrite;
											pEventPipeInstance->WriteDataBusy = FALSE;
											switch (pAccessEventPipe->ModeFileBuffer) {
												/* bis Puffergrösse */
												case 0:
													if (pEventPipeInstance->fileOffsetWrite +  sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderWrite->pipe_size < pAccessEventPipe->SizeFileBuffer) {
														pEventPipeInstance->fileOffsetWrite += sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderWrite->pipe_size;
													}
													break;
												/* Ringpuffer */
												case 1:
													if (pEventPipeInstance->fileOffsetWrite +  sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderWrite->pipe_size < pAccessEventPipe->SizeFileBuffer) {
														pEventPipeInstance->fileOffsetWrite += sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderWrite->pipe_size;
													}
													else {
														pEventPipeInstance->fileOffsetWrite = 0;
													}
													break;
												/* unendlich */
												case 2:
													pEventPipeInstance->fileOffsetWrite += sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderWrite->pipe_size;
													break;
											}
											pEventPipeInstance->BufferFileIndex = (pEventPipeInstance->BufferFileIndex + 1) % 2;
											pEventPipeInstance->fileStep = 4;
										}
										else if (pEventPipeInstance->iFileWrite.status != 65535) {
											pEventPipeInstance->WriteDataBusy = FALSE;
											pEventPipeInstance->fileState = pEventPipeInstance->iFileWrite.status;
											pEventPipeInstance->fileStep = 8;
										}
										else {
											pEventPipeInstance->WriteDataBusy = TRUE;
										}
									}
									break;

								/* --------------------- */
								/* Auf neue Daten warten */
								/* --------------------- */
								case 4:
									if (pEventPipeInstance->BufferEventIndex != pEventPipeInstance->BufferFileIndex) {
										pEventPipeInstance->fileStep = 3;
									}
									break;

								/* ----------- */
								/* File Fehler */
								/* ----------- */
								case 8:
									pEventPipeInstance->fileSysError = FileIoGetSysError();
									pEventPipeInstance->ReadDataBusy = FALSE;
									pEventPipeInstance->fileIdent = NULL;
									pEventPipeInstance->fileState = 0;
									pEventPipeInstance->fileStep = 0;
									break;

							} /* (*pfileStep) */

							/* ------------ */
							/* File - Lesen */
							/* ------------ */
							switch (pEventPipeInstance->fileStep) {

								/* ---------- */
								/* File lesen */
								/* ---------- */
								case 3:
								case 4:
									if (
										pEventPipeInstance->iFileWrite.status == 0
										&&
										(
											(
												pEventPipeHeaderRead->iPipeRead == pEventPipeHeaderRead->iPipeWrite &&
												pEventPipeInstance->fileOffsetWrite >= sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderRead->pipe_size
											)
											||
											pEventPipeInstance->ReadDataBusy == TRUE
										)
									) {
										pEventPipeInstance->iFileRead.enable = 1;
										pEventPipeInstance->iFileRead.ident = pEventPipeInstance->fileIdent;
										pEventPipeInstance->iFileRead.offset = pEventPipeInstance->fileOffsetRead;
										pEventPipeInstance->iFileRead.pDest = (unsigned long)pEventPipeHeaderRead;
										pEventPipeInstance->iFileRead.len = sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderRead->pipe_size;
										FileRead(&pEventPipeInstance->iFileRead);
										if (pEventPipeInstance->iFileRead.status == 0) {
											/* Pointer relozieren */
											pEventPipeHeaderRead->pPipeData = (unsigned char*)&pEventPipeHeaderRead[1];
											pEventPipeInstance->ReadDataBusy = FALSE;
											pEventPipeInstance->fileOffsetRead += sizeof(EVENT_PIPE_HEADER) + pEventPipeHeaderRead->pipe_size;
											if (pEventPipeInstance->fileOffsetRead >= pEventPipeInstance->fileOffsetWrite) {
												pEventPipeInstance->fileOffsetRead  = 0;
												pEventPipeInstance->fileOffsetWrite = 0;
												pEventPipeInstance->fileStep = 6;
											}
										}
										else if (pEventPipeInstance->iFileRead.status != 65535) {
											pEventPipeInstance->ReadDataBusy = FALSE;
											pEventPipeInstance->fileState = pEventPipeInstance->iFileRead.status;
											pEventPipeInstance->fileStep = 8;
										}
										else {
											pEventPipeInstance->ReadDataBusy = TRUE;
										}
									}
									else {
										#warning ! ??? !
										pEventPipeInstance->ReadDataBusy = FALSE;
									}
									break;

								/* -------------- */
								/* File schließen */
								/* -------------- */
								case 6:
									pEventPipeInstance->iFileClose.enable = 1;
									pEventPipeInstance->iFileClose.ident = pEventPipeInstance->fileIdent;
									FileClose(&pEventPipeInstance->iFileClose);
									if (pEventPipeInstance->iFileClose.status == 0) {
										pEventPipeInstance->fileStep = 7;
									}
									else if (pEventPipeInstance->iFileClose.status != 65535) {
										pEventPipeInstance->fileState = pEventPipeInstance->iFileClose.status;
										pEventPipeInstance->fileStep = 8;
									}
									break;

								/* ------------ */
								/* File löschen */
								/* ------------ */
								case 7:
									pEventPipeInstance->iFileDelete.enable = 1;
									pEventPipeInstance->iFileDelete.pDevice = (unsigned long)pEventPipeInstance->FileDevice;
									pEventPipeInstance->iFileDelete.pName = (unsigned long)pEventPipeInstance->FileName;
									FileDelete(&pEventPipeInstance->iFileDelete);
									if (pEventPipeInstance->iFileDelete.status == 0) {
										pEventPipeInstance->WriteDataBusy = FALSE;
										pEventPipeInstance->ReadDataBusy = FALSE;
										pEventPipeInstance->fileIdent = NULL;
										pEventPipeInstance->fileState = 0;
										pEventPipeInstance->fileStep = 0;
									}
									else if (pEventPipeInstance->iFileDelete.status != 65535) {
										pEventPipeInstance->fileState = pEventPipeInstance->iFileDelete.status;
										pEventPipeInstance->fileStep = 8;
									}
									break;

								/* ----------- */
								/* File Fehler */
								/* ----------- */
								case 8:
									pEventPipeInstance->fileSysError = FileIoGetSysError();
									pEventPipeInstance->WriteDataBusy = FALSE;
									pEventPipeInstance->ReadDataBusy = FALSE;
									pEventPipeInstance->fileIdent = NULL;
									pEventPipeInstance->fileState = 0;
									pEventPipeInstance->fileStep = 0;
									break;

							} /* (*pfileStep) */
						} /* (pEventPipeHeaderWrite != NULL && pEventPipeHeaderRead != NULL) */
					} /* (pEventPipeInstance->WriteToFile == TRUE) */
				} /* (pAccessEventPipe != NULL) */
			} /* (type = 0; type < pAccessServer->countType; type++) */
		} /* (instance = 0; instance < pAccessServer->countEventInstance; instance++) */
		return 0;
	} /* (idBrServer != NULL) */
	else {
		return cEC_INVALID_SERVER;
	}
} /* CopyPipeToFile */

/*============================= Ende der Datei ==============================*/


