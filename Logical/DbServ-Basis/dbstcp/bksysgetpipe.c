#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/* bksysgetpipe.c ------------------------------------------------------------- */

#include <bur/plc.h>      /*some usefull macros for B&R pcc programming*/

#include <stdlib.h>

#include "bksys.h"


/*-----------------------------------------------------------------------------
// EvGetPipePtr
// Zeiger auf Event-Pipe ermitteln
// Last Edit:
//		13.07.99/RES
//		30.04.99/RES
//
// inputs:	UserID	Identity des zugehoerigen Dienstes
//          DtIdx	Datentyp-Index
//
// outputs: ppPipe	Zeiger auf Event-Pipe
//
// return:  Status: 0=kein Fehler, d.h. Pipe-Zeiger gueltig
//			       -1=unzulaessiger DtIdx
//				   -2=kein passender Event-Pipeblock gefunden
//				   -3=kein gueltiger Globals-Ptr
//---------------------------------------------------------------------------*/
short EvGetPipePtr( unsigned char UserID, unsigned char DtIdx, PipeHead_Typ** ppPipe )
{
	static Globals_Typ *pGlobals = NULL;

	short sRet = 0;
	EvPbHead_Typ* pEvBlock;

	/* Zeiger auf Struktur mit Zeigern auf Globaldaten initialisieren */
	if( pGlobals == NULL ) {
		GetGlobalsPtr( &pGlobals );
	}

	if( pGlobals != NULL ) {	/* Gueltiger Zeiger in pGlobals ? */
		if( DtIdx < DT_ANZ ) {	/* DtIdx zulaessig ? */
			/* Zugehoerigen Event-PipeBlock ermitteln */
			for( pEvBlock = pGlobals->pEvPipes; pEvBlock != NULL; pEvBlock = pEvBlock->pNext ) {
				if( pEvBlock->UserID == UserID ) {
					*ppPipe = pEvBlock->pDtPipe[DtIdx];
					break;	/* Event-Block gefunden ! */
				}
			}
			if( pEvBlock == NULL ) {
				sRet = NOEVBLKERR_NUM;	/* Kein EvPipeBlock mit angegebener UserID gefunden */
				ErrStatistics("", WARN, EC_SYS_BASE + NOEVBLKERR_NUM, 3, EC_SYS_BASE, NOEVBLKERR_TEXT );
			}
		}
		else {
			sRet = DTERR_NUM;	/* unzulaessiger DtIdx */
			ErrStatistics("", WARN, EC_SYS_BASE + DTERR_NUM, 3, EC_SYS_BASE, DTERR_TEXT );
		}
	}
	else {
		sRet = -3;
	}
	return( sRet );
}

/*-----------------------------------------------------------------------------
// CliGetPipePtr
// Zeiger auf Event-Pipe ermitteln
// Last Edit:
//		19.05.03/RES: Erstversion aus EvGetPipePtr() erzeugt
//
// inputs:	Client	Client
//          DtIdx	Datentyp-Index
//
// outputs: ppPipe	Zeiger auf Event-Pipe
//
// return:    0 kein Fehler, d.h. Pipe-Zeiger gueltig
//			 -1 Unzulaessiger Client
//			 -2 Unzulaessiger DtIdx
//			-10 Kein gueltiger Globals-Ptr
//			-11 Kein gueltiger Zeiger auf Pipeblock-Header
//			-12 Kein gueltiger PipeHead-Ptr
//---------------------------------------------------------------------------*/
short CliGetPipePtr( unsigned char Client, unsigned char DtIdx, CliPipeHead_Typ** ppPipe )
{
	static Globals_Typ	*pGlobals = NULL;

	short 				sRet = 0;
	CliPbHead_Typ 		*pPbHead;			/* Zeiger auf Client-Pipeblock-Header */

	/* Zeiger auf Struktur mit Zeigern auf Globaldaten initialisieren */
	if( pGlobals == NULL ) {
		GetGlobalsPtr( &pGlobals );
	}

	if( pGlobals != NULL ) {	/* Gueltiger Zeiger in pGlobals ? */
		if( Client < pGlobals->pCab->ClientAnz ) {
			pPbHead = pGlobals->pCab->pClientPb[Client];	/* Zeiger auf PipeBlock des angegebenen Clients ermitteln */
			if( pPbHead != 0 ) {
				if( DtIdx < DT_ANZ ) {	/* DtIdx zulaessig ? */
					*ppPipe = pPbHead->pDtPipe[DtIdx];	/* Zeiger auf Pipe-Header ermitteln */
					if( *ppPipe != 0 ) {
						sRet = 0;	/* Gueltiger PipeHead-Ptr gefunden */
					}
					else {
						sRet = -12;	/* Kein gueltiger PipeHead-Ptr */
					}
				}
				else {
					sRet = -2;	/* Unzulaessiger DtIdx */
				}
			}
			else {
				sRet = -11;	/* Kein gueltiger Zeiger auf Pipeblock-Header */
			}
		}
		else {
			sRet = -1;	/* Unzulaessiger Client */
		}
	}
	else {
		sRet = -10;	/* Kein gueltiger Globals-Ptr */
	}
	return( sRet );
}


