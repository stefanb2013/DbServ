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

_LOCAL_RETAIN	UINT	statePV_xgetadr	;
_LOCAL	UDINT	pv_adresse, data_len;

typedef struct {
	UINT	instance;
	UINT	unit;
	UINT	device;
	UINT	type;
	UINT	point;
	UINT	item;
	UINT	infoerror;
} DBS_TEST;
_LOCAL	DBS_TEST	dbstest;

_LOCAL	UINT	wTest;
_LOCAL	USINT	bTest;

_LOCAL	BOOL	fLink, fWrite, fRead;

_LOCAL	USINT	WriteBuffer[32];
_LOCAL	UDINT	Length, LengthTimeStamp;

/***********************************************************************************************/
_INIT void init(void)
{
	myidBrServer = NULL;
	statePV_xgetadr = PV_xgetadr ("dbserv:idBrServer", &pv_adresse, &data_len);
	if (statePV_xgetadr == 0) {
		myidBrServer = (ACCESS_SERVER*)*(UDINT*)pv_adresse;
	}
} /* _INIT void init(void) */

/***********************************************************************************************/
_CYCLIC	void cyclic(void)
{

	wTest = (bTest & 0x0F) << 12;


	if (myidBrServer == NULL) {
		if (statePV_xgetadr == 0) {
			myidBrServer = (ACCESS_SERVER*)*(UDINT*)pv_adresse;
		}
		else {
			statePV_xgetadr = PV_xgetadr("dbserv:idBrServer", &pv_adresse, &data_len);
		}
	} /* (myidBrServer == NULL) */
	else {




#ifdef xxx
		CopyPipeToFile (myidBrServer);



		/* ------------- */
		/* Wert nach PAB */
		/* ------------- */
		InValue = (InValue + 1) % 2;
		dbstest.infoerror = SetValueItem(myidBrServer,
											dbstest.unit, dbstest.device, dbstest.type, dbstest.point, dbstest.item,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);


#endif


	if (fLink == TRUE) {
		dbstest.infoerror = LinkWriteItem(myidBrServer, dbstest.instance,
											dbstest.unit, dbstest.device, dbstest.type, dbstest.point, dbstest.item,
											NULL);
		fLink = FALSE;
	}

	if (fWrite == TRUE) {
		dbstest.infoerror = SetWriteItem(myidBrServer,
											dbstest.unit, dbstest.device, dbstest.type, dbstest.point, dbstest.item,
											(USINT*)&bTest, sizeof(bTest));
		fWrite = FALSE;
	}

	if (fRead == TRUE) {
		dbstest.infoerror = GetWriteItem(myidBrServer, dbstest.instance,
											&WriteBuffer[0], sizeof(WriteBuffer),
											&Length, &LengthTimeStamp);
		fRead = FALSE;
	}

/*
UINT SetWriteItem (void *idBrServer,
					UINT unit, UINT device, UINT type, UINT point, UINT item,
					USINT* pValue, UDINT lengthValue) {
*/

	}
} /* _CYCLIC void cyclic(void) */



