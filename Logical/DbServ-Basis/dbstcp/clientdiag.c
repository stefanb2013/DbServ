#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/* clientdiag.c -------------------- */

#include <bur/plc.h>      /*some usefull macros for B&R pcc programming*/

#include <stdlib.h>

#include "bksys.h"


/*-----------------------------------------------------------------------------
// ClientDiagnose
// Berechnet diverse Diagnose-Informationen zu ClientPipes
// Last Edit:
//		13.08.03/RES:	Erstversion erstellt
//
// inputs:
// outputs:
// return:
//---------------------------------------------------------------------------*/
void ClientDiagnose( void )
{
	static	unsigned char	ClientIdx = 0;					/* Client-Index */
	static	unsigned short	Count10msOld;					/* 10ms-Taktzaehler */
	static	unsigned long	AnyPipeEntryOld[CLI_ANZ_MAX];	/* Pipe-Eintragszaehler der letzten Berechnung */
	static	unsigned long	EntryCountOld;					/* Pipe-Eintragszaehler der letzten Berechnung */

	Globals_Typ 			*pGlobals;						/* Zeiger auf Globaldatenstruktur */
	CliPipeDiagAll_Typ		*pCliPipeDiag;					/* Zeiger ClientPipe-Diagnose-Struktur */
	unsigned short			Delta10ms;						/* Vergangene Zeit in 10ms seit letzter sekuendlicher Berechnung */
	unsigned char			ClientIdxTmp;					/* Client-Index fuer temporaere Verwendung */
	unsigned char			DtIdx;							/* Datentyp-Index */
	unsigned long			EntryDiff;						/* Eintragsdifferenz */

	CliPbHead_Typ 			*pPbHead;						/* Zeiger auf Client-Pipeblock-Header */
	CliPipeHead_Typ			*pPipe;
	long					lUsed;


	/* Zeiger auf Globaldatenstruktur initialisieren */
	GetGlobalsPtr( &pGlobals );
	if( pGlobals == NULL ) {			/* Gueltige Adresse in pGlobals ? */
		return;		/* Kein gueltiger pGlobals */
	}

	/* ClientPipe-Diagnose-Monitor -------------------------------------------- */
	pCliPipeDiag = pGlobals->pCliPipeDiag;		/* Interne Datenstruktur ueber dynamische DIAG-PV (lokale Pointer PV) sichtbar machen */

	/* Initialisierung */
	if( pCliPipeDiag->Init ) {
		pCliPipeDiag->Init = 0;
		memset( pCliPipeDiag, 0, sizeof( CliPipeDiagAll_Typ ) );
	}

	/* Sekuendliche Berechnungen ---------------------------------------------- */
	Delta10ms = pGlobals->Count10ms - Count10msOld;
	if( Delta10ms >= 100 ) {
		Count10msOld = pGlobals->Count10ms;		/* Zeit abgelaufen => aktuellen 10ms Count merken */
		EntryDiff = pCliPipeDiag->EntryCount - EntryCountOld;				/* Eintragsdifferenz ermitteln */
		pCliPipeDiag->EntryCountPerSec = EntryDiff * 100 / Delta10ms;
		EntryCountOld = pCliPipeDiag->EntryCount;
		for( ClientIdxTmp = 0; ClientIdxTmp < pGlobals->pCab->ClientAnz; ClientIdxTmp++ ) {
			EntryDiff = pCliPipeDiag->Client[ClientIdxTmp].AnyPipeEntry - AnyPipeEntryOld[ClientIdxTmp];		/* Eintragsdifferenz ermitteln */
			pCliPipeDiag->Client[ClientIdxTmp].AnyPipeEntryPerSec = EntryDiff * 100 / Delta10ms;
			AnyPipeEntryOld[ClientIdxTmp] = pCliPipeDiag->Client[ClientIdxTmp].AnyPipeEntry;
		}
	}

	/* Zyklische clientweise Berechnungen ------------------------------------- */
	for( DtIdx = 0; DtIdx < DT_ANZ; DtIdx++ ) {
		lUsed = 0;
		pPbHead = pGlobals->pCab->pClientPb[ClientIdx];	/* Zeiger auf PipeBlock des angegebenen Clients ermitteln */
		if( pPbHead != 0 ) {
			pPipe = pPbHead->pDtPipe[DtIdx];	/* Zeiger auf Pipe-Header ermitteln */
			if( pPipe != 0 ) {
				/* Genutzten Platz in der Pipe berechnen */
				lUsed = pPipe->pWrite - pPipe->pRead;
				if( lUsed < 0 ) {
					lUsed += pPipe->pLast - pPipe->pFirst + 1;						/* Absolutwert */
				}
				if( pPipe->pLast > pPipe->pFirst ) {
					lUsed = ( lUsed * 100 ) / ( pPipe->pLast - pPipe->pFirst );		/* Prozentangabe */
				}
				else {
					lUsed = 0;		/* Nur sicherheitshalber !! */
				}
			}
		}
		pCliPipeDiag->Client[ClientIdx].DtPipeUse[DtIdx] = lUsed;
	}
	ClientIdx++;		/* Naechsten Client fuer naechsten Zyklus anwaehlen */
	if( ClientIdx >= pGlobals->pCab->ClientAnz ) {
		ClientIdx = 0;
	}
}


