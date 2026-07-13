#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  I/O-Treiber
*  Teil:     Unterprogramme
*  Datei:    iodrv_sub.c
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
#include <sys_lib.h>		/* SYS_LIB */
#include <fileio.h>
#include "iodrv.h"
#include "dbserv.h"
#include "platform.h"
/****************************************************************************/
/*		BR-Server ID besorgen												*/
/****************************************************************************/
void get_idBrServer() {

	if (myidBrServer == 0) {
		idstat = GetIdBrServer((void*)&myidBrServer,&brstate);
	}
}


