#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  DbServ
*  Teil:     Monitor
*  Datei:    bsrun.h
*  Typ:      GNU C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    26-05-2003
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Task-Source-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 14-07-03 | MKrz | Modul-Erstellung
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>		/* RPS-spezifische Typendeklarationen */
#include <bur\plctypes.h>	/* RPS-Datentypen */
#include <dataobj.h>		/* SYS_LIB */
#include <sys_lib.h>		/* SYS_LIB */
#include <fileio.h>
#include <string.h>

#include "dbserv.h"

/*************************************************************************************************/
/*	globale Variablen                                                                            */
/*************************************************************************************************/

/*************************************************************************************************/
/*	lokale Variablen                                                                             */
/*************************************************************************************************/
_LOCAL	ACCESS_SERVER	*myidBrServer;
_LOCAL_RETAIN	UINT			statePV_xgetadr	;
static	UDINT			pv_adresse, data_len;

/*
_LOCAL	USINT	*pData_extern, *pData_iextern, *pData_intern, *pData_valid, *pData_time, *pData_itime, *pData_writelink, *pData_ieventlink, *pData_ihyst, *pData_ievent;
_LOCAL	UDINT 	length_extern, length_intern, length_iextern, length_valid, length_time, length_itime, length_writelink, length_ieventlink, length_ihyst, length_ievent;
_LOCAL	UINT	countInstanceAll, countInstanceNoTimestamp, countInstanceTimestamp;
_LOCAL	USINT	tsItem;
_LOCAL	UINT	pipe_entry, pipe_level_abs;
_LOCAL	USINT	pipe_level_rel;
_LOCAL	USINT	bValue;
_LOCAL	UINT	wValue;
_LOCAL	UDINT	lValue;
*/

typedef struct {
	USINT	bValue;
	UINT	wValue;
	UDINT	lValue;
} DATA_VALUE;
/*_LOCAL	DATA_VALUE	DataValue;*/
typedef struct {
	USINT	*pData_extern, *pData_iextern, *pData_intern, *pData_valid, *pData_time, *pData_itime, *pData_writelink, *pData_ieventlink, *pData_ihyst, *pData_ievent;
	UDINT	length_extern, length_intern, length_iextern, length_valid, length_time, length_itime, length_writelink, length_ieventlink, length_ihyst, length_ievent;
} DATA_EXTERN;
/*_LOCAL	DATA_EXTERN	DataExtern;*/
typedef struct {
	UINT	countInstanceAll, countInstanceNoTimestamp, countInstanceTimestamp;
	USINT	tsItem;
} DATA_INSTANCE;
/*_LOCAL	DATA_INSTANCE	DataInstance;*/
typedef struct {
	UINT	pipe_entry, pipe_level_abs;
	USINT	pipe_level_rel;
} DATA_PIPE;
/*_LOCAL	DATA_PIPE	DataPipe;*/
typedef struct {
	UINT			instance;
	UINT			unit;
	UINT			device;
	UINT			type;
	UINT			point;
	UINT			item;
	UINT			infoerror;
	USINT			valid;			/* WAI 18.05.07 */
	DATA_VALUE		DataValue;
	DATA_EXTERN		DataExtern;
	DATA_INSTANCE	DataInstance;
	DATA_PIPE		DataPipe;
} DBS_MONITOR;
_LOCAL_RETAIN	DBS_MONITOR	dbsmon	;


/***********************************************************************************************/
_INIT void init(void)
{
	myidBrServer = NULL;
	statePV_xgetadr = PV_xgetadr("dbserv:idBrServer", &pv_adresse, &data_len);
	if (statePV_xgetadr == 0) {
		myidBrServer = (ACCESS_SERVER*)*(UDINT*)pv_adresse;
	}
} /* _INIT void init(void) */

/***********************************************************************************************/
_CYCLIC	void cyclic(void)
{
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

		/* ------------------------ */
		/* Datenpunkt-Informationen */
		/* ------------------------ */
		dbsmon.infoerror = GetItemInfo(myidBrServer, dbsmon.instance,
										dbsmon.unit, dbsmon.device, dbsmon.type, dbsmon.point, dbsmon.item,
										&dbsmon.DataExtern.pData_extern,		&dbsmon.DataExtern.length_extern,
										&dbsmon.DataExtern.pData_intern,		&dbsmon.DataExtern.length_intern,
										&dbsmon.DataExtern.pData_iextern,		&dbsmon.DataExtern.length_iextern,
										&dbsmon.DataExtern.pData_valid,			&dbsmon.DataExtern.length_valid,
										&dbsmon.DataExtern.pData_time,			&dbsmon.DataExtern.length_time,
										&dbsmon.DataExtern.pData_itime,			&dbsmon.DataExtern.length_itime,
										&dbsmon.DataExtern.pData_writelink,		&dbsmon.DataExtern.length_writelink,
										&dbsmon.DataExtern.pData_ieventlink,	&dbsmon.DataExtern.length_ieventlink,
										&dbsmon.DataExtern.pData_ihyst,			&dbsmon.DataExtern.length_ihyst,
										&dbsmon.DataExtern.pData_ievent,		&dbsmon.DataExtern.length_ievent);
		if (dbsmon.infoerror == cEC_OK) {
			switch (dbsmon.DataExtern.length_extern) {
				case 1:
					dbsmon.DataValue.bValue = *(USINT*)dbsmon.DataExtern.pData_extern;
					break;
				case 2:
					dbsmon.DataValue.wValue =  *(UINT*)dbsmon.DataExtern.pData_extern;
					break;
				case 4:
					dbsmon.DataValue.lValue = *(UDINT*)dbsmon.DataExtern.pData_extern;
					break;
			}
			if(dbsmon.DataExtern.pData_valid)	dbsmon.valid = *(dbsmon.DataExtern.pData_valid);
		} /* (infoerror == cEC_OK) */

		/* ------------------ */
		/* Link-Informationen */
		/* ------------------ */
		dbsmon.infoerror = GetEventLinkInfo(myidBrServer,
												dbsmon.unit, dbsmon.device, dbsmon.type, dbsmon.point, dbsmon.item,
												&dbsmon.DataInstance.countInstanceAll,
												&dbsmon.DataInstance.countInstanceNoTimestamp,
												&dbsmon.DataInstance.countInstanceTimestamp,
												&dbsmon.DataInstance.tsItem);

		/* ------------------ */
		/* Pipe-Informationen */
		/* ------------------ */
		GetEventPipeInfo(myidBrServer,
							dbsmon.instance, dbsmon.type,
							&dbsmon.DataPipe.pipe_entry,
							&dbsmon.DataPipe.pipe_level_abs,
							&dbsmon.DataPipe.pipe_level_rel);
	}
} /* _CYCLIC void cyclic(void) */



