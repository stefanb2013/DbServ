#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/* bksysgetinfo.c ------------------------------------------------------------- */

#include <bur/plc.h>      /*some usefull macros for B&R pcc programming*/

#include <stdlib.h>

#include "bksys.h"


/*-----------------------------------------------------------------------------
// GetDpInfo
// Sucht nach Datenpunkt mit angegebenem Namen und liefert interne Adresse und
// zugehoerigen Alias-Zeiger zurueck
// LastEdit:
//		22.02.02/RES	Rueckgabewert von GetDpInfo() geaendert (2=Leiche gefunden)
//		17.05.00/RES	PabSize von UCHAR auf USHORT geaendert (wg. BkZs)
//		22.09.99/RES	pPabHead und pDtDescr immer aktuell ermitteln
//		20.07.99/RES	Datenpunktname als Zeichenkette direkt im PAB
//		10.06.99/RES	Erstversion
//
// inputs:	pDpName		Datenpunktname (Zeichenkette)
// outputs:	pDtIdx		Datentyp-Index
//			pDpIdx		Datenpunkt-Index
//			ppAlias		Zeiger auf zugehoerige Alias-Struktur
// return:	 1 = Datenpunkt gefunden => Rueckgabe-Infos gueltig
//			 2 = Leiche gefunden => Rueckgabe-Infos gueltig
//			 0 = Keinen passenden Datenpunkt gefunden => Rueckgabe-Infos ungueltig
//			<0 = Bk2000-Systemfehler => Rueckgabe-Infos ungueltig
//---------------------------------------------------------------------------*/
short GetDpInfo( char *pDpName, unsigned char *pDtIdx, unsigned short *pDpIdx, unsigned char **ppAlias )
{
	short sFound = 0;
	unsigned char *pPab;
	unsigned char DtIdx;
	unsigned short DpIdx;
	unsigned short PabSize;
	unsigned char DpNameOff;
	Globals_Typ *pGlobals = NULL;
	PabHeader_Typ *pPabHead = NULL;
	DtProps_Typ *pDtProps = NULL;

	/* Zeiger auf Globaldatenstruktur initialisieren */
	GetGlobalsPtr( &pGlobals );
	if( pGlobals != NULL ) {	/* Gueltige Adresse in pGlobals ? */
		pPabHead = pGlobals->pPab;
		if( pPabHead == NULL ) {	/* Keine gueltige Adresse in pPab ? */
			return( -2 );
		}
		pDtProps = pGlobals->pDtProps;
		if( pDtProps == NULL ) {	/* Keine gueltige Adresse in pPab ? */
			return( -3 );
		}
	}
	else {	/* Keine gueltige Adresse in pGlobals */
		return( -1 );
	}

	/* Datenpunkt mittels Namen suchen */
	for( DtIdx = 0; !sFound && DtIdx < DT_ANZ; DtIdx++ ) {
		pPab = pPabHead->pDtBase[DtIdx];
		PabSize = pDtProps[DtIdx].DtPabLen;
		DpNameOff = pDtProps[DtIdx].DpNameOff;
		for( DpIdx = 0; DpIdx < pPabHead->DpAnz[DtIdx]; DpIdx++, pPab += PabSize ) {
			if( strcmp( pDpName, (char *)(pPab + DpNameOff) ) == 0 ) {
				*pDtIdx = *( pPab + pDtProps[DtIdx].DtIdxOff );
				*pDpIdx = *(unsigned short *)( pPab + pDtProps[DtIdx].DpIdxOff );
				*ppAlias = *(unsigned char **)( pPab + pDtProps[DtIdx].AliasPtrOff );
				if( *ppAlias != NULL ) {
					sFound = 1;	/* Datenpunkt mit passendem Namen gefunden */
				}
				else {
					sFound = 2;	/* Leiche im PAB gefunden */
				}
				break;
			}
		}
	}
	return( sFound );
}

/*-----------------------------------------------------------------------------
// GetDtDescInfos
// Liefert Zeiger und maximalen Eintragsindex von Datentyp-Deskriptoren und
// Item-Konvertierungtabellen
// Last Edit:
//		09.06.99/RES
//		06.06.99/RES
//
// inputs:	DtIdx			Datentyp-Index
// outputs: ppDtProps		Zeiger auf Datentyp-Eigenschaften
//			ppEaDesc		Zeiger auf Elementart-Descriptor
//			pEaDescMaxIdx	Hoechster Index in Elementart-Descriptor
//			ppDtDesc		Zeiger auf Datentyp-Descriptor
//			pDtDescMaxIdx	Hoechster Index in Datentyp-Descriptor
//			ppItemConv		Zeiger auf Item-Konvertierungstabelle
//			pItemConvMaxIdx	Hoechster Index in Item-Konvertierungstabelle
// return:	 0 = Rueckgabe-Infos gueltig
//			<0 = Fehler => Rueckgabe-Infos ungueltig
//---------------------------------------------------------------------------*/
short GetDtDescInfos( unsigned char DtIdx, DtProps_Typ **ppDtProps,
					  EaDesc_Typ **ppEaDesc, unsigned char *pEaDescMaxIdx,
					  DtDesc_Typ **ppDtDesc, unsigned char *pDtDescMaxIdx,
					  ItemConv_Typ **ppItemConv, unsigned char *pItemConvMaxIdx )
{
	static Globals_Typ *pGlobals = NULL;

	short sRet = 0;

	/* Zeiger auf Struktur mit Zeigern auf Globaldaten initialisieren */
	if( pGlobals == NULL ) {
		GetGlobalsPtr( &pGlobals );
	}

	if( pGlobals != NULL ) {
		/* Output-Informationen ermitteln */
		if( DtIdx < DT_ANZ ) {	/* Gueltiger Datentyp ? */
			/* Zeiger auf Struktur mit Datentyp-Eigenschaften */
			if( ppDtProps != NULL ) {
				*ppDtProps = pGlobals->pDtProps + DtIdx;
			}
			/* Zeiger auf Elementart-Deskriptor und zugehoerigen MaxIndex ermitteln */
			if( ppEaDesc != NULL ) {
				*ppEaDesc = pGlobals->pEaDesc[DtIdx];
			}
			if( pEaDescMaxIdx != NULL ) {
				*pEaDescMaxIdx = pGlobals->pDtProps[DtIdx].EaMaxItemIdx;
			}
			/* Zeiger auf Datentyp-Deskriptor und zugehoerigen MaxIndex ermitteln */
			if( ppDtDesc != NULL ) {
				*ppDtDesc = pGlobals->pDtDesc[DtIdx];
			}
			if( pDtDescMaxIdx != NULL ) {
				*pDtDescMaxIdx = pGlobals->pDtProps[DtIdx].DtMaxItemIdx;
			}
			/* Zeiger auf Itemkonvertierungstabelle und zugehoerigen MaxIndex ermitteln */
			if( ppItemConv != NULL ) {
				*ppItemConv = pGlobals->pItemConv[DtIdx];
			}
			if( pItemConvMaxIdx != NULL ) {
				*pItemConvMaxIdx = pGlobals->pDtProps[DtIdx].ItemConvMaxIdx;
			}
		}
		else {	/* Datentyp ungueltig */
			sRet = -1;
		}
	}
	else {	/* pGlobals == NULL */
		sRet = -2;
	}
	return( sRet );
}




