#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/* bksysbrtcp.c --------------------------------------------------------------- */

#include <bur/plc.h>      /*some usefull macros for B&R pcc programming*/

#include <stdlib.h>			/* Standard-Library */

#include "bksys.h"


/*-----------------------------------------------------------------------------
// CliGetClientAnzMax
// Ermittelt die maximal zulaessige Clientanzahl
// Last Edit:
//		10.06.03/RES: Zeigerpruefung pCliAnzMax eingefuegt
//					  Returnwert geaendert
//		05.06.03/RES: Erstversion erstellt
//
// inputs:
//
// outputs: pCliAnzMax	Maximale Clientanzahl
//
// return:    0 kein Fehler
//			 -1 Ungueltiger pCliAnzMax-Ptr
//			-10 Kein gueltiger Globals-Ptr
//			-11 Kein gueltiger CAB-Zeiger
//---------------------------------------------------------------------------*/
short CliGetClientAnzMax( unsigned char *pCliAnzMax )
{
	Globals_Typ 	*pGlobals;		/* Zeiger auf Globaldatenstruktur */
	CabHeader_Typ	*pCab;			/* Zeiger auf CAB */

	/* Zeiger auf Struktur mit Zeigern auf Globaldaten initialisieren */
	GetGlobalsPtr( &pGlobals );

	if( pGlobals != NULL ) {	/* Gueltiger Zeiger in pGlobals ? */
		if( pCliAnzMax != NULL ) {	/* Gueltiger Zeiger in pCliAnzMax ? */
			pCab = pGlobals->pCab;
			if( pCab != NULL ) {	/* Gueltiger Zeiger in pCab ? */
				*pCliAnzMax = pCab->ClientAnz;
			}
			else {
				return( -11 );		/* Kein gueltiger CAB-Zeiger */
			}
		}
		else {
			return( -1 );		/* Kein gueltiger pCliAnzMax-Ptr */
		}
	}
	else {
		return ( -10 );		/* Kein gueltiger Globals-Ptr */
	}
	return( 0 );
}

/*-----------------------------------------------------------------------------
// CliGetDtAnz
// Ermittelt die Anzahl Datentypen
// Last Edit:
//		10.06.03/RES: Zeigerpruefung pCliAnzMax eingefuegt
//					  Returnwert geaendert
//		05.06.03/RES: Erstversion erstellt
//
// inputs:
//
// outputs: pDtAnz	Anzahl Datentypen
//
// return:    0 kein Fehler
//			 -1 Ungueltiger pDtAnz-Ptr
//---------------------------------------------------------------------------*/
short CliGetDtAnz( unsigned char *pDtAnz )
{
	if( pDtAnz != NULL ) {	/* Gueltiger Zeiger in pDtAnz ? */
		*pDtAnz = DT_ANZ;
	}
	else {
		return( -1 );		/* Kein gueltiger pDtAnz-Ptr */
	}
	return( 0 );
}


/*-----------------------------------------------------------------------------
// CliItemLink
// Setzt zum angegebenen Item das Linkflag fuer den angegebenen Client auf den
// angegebenen Status.
// Gleichzeitig wird das Logon-Flag des Clients im CAB-Header gesetzt und
// der Wert von LastVal im CAB mit dem aktuellen Itemwert initialisiert.
//
// LastEdit:
//		24.07.03/RES	Bei Link=1 nun Aktualisierung von LastVal durch AktVal (aus Alias-PV) eingefuegt
//		10.06.03/RES	Bei Link=1 nun Force-Flag=Event setzen
//		07.06.03/RES	Bei Link=1 Force-Flag=Aktualisieren (ohne Event) eingefuegt
//		02.05.03/RES	Erstversion erstellt
//
// inputs:	Client		Client-Index
//			DtIdx		Datentyp-Index
//			DpIdx		Datenpunkt-Index
//			ItemIdx		Item-Index
//			Link		Neuer Link-Status
// outputs:
// return:	  1 Item erfolgreich gelinkt
//			  0 Item hat bereits den gewuenschten Link-Status
//			 -1 Item hat keinen CAB-Eintrag
//			 -2 Unzulaessiger Client
//			 -3 Ungueltiger Datentyp
//			 -4 Unbekannter Datenpunkt
//			 -5 Unbekanntes Item
//			-10 Bk2000-Systemfehler => Globaldatenstruktur nicht gefunden
//			-11 Bk2000-Systemfehler => CAB-Header nicht gefunden
//			-12 Bk2000-Systemfehler => Dt-Props nicht gefunden
//			-13 Bk2000-Systemfehler => Dt-Desc nicht gefunden
//---------------------------------------------------------------------------*/
short CliItemLink( unsigned char Client, unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx, unsigned char Link )
{
	unsigned short		sRet = 0;
	Globals_Typ			*pGlobals = NULL;
	DtProps_Typ 		*pDtProps;
	DtDesc_Typ 			*pDtDesc;
	CabHeader_Typ		*pCabHead = NULL;
	unsigned char		*pPab;
	CabItemEntry_Typ	*pCab;

	/* Zeiger auf Globaldatenstruktur initialisieren */
	GetGlobalsPtr( &pGlobals );
	if( pGlobals != NULL ) {	/* Gueltiger Zeiger in pGlobals ? */
		pCabHead = pGlobals->pCab;
		if( pCabHead == NULL ) {	/* Keine gueltige Adresse in pCab ? */
			return( -11 );
		}
		/* Parameter überprüfen */
		if( Client >= pCabHead->ClientAnz ) {
			return( -2 );	/* Unzulaessiger Client */
		}
		if ( DtIdx >= DT_ANZ )	{
			return( -3 );	/* Ungueltiger Datentyp */
		}
		pDtProps = pGlobals->pDtProps + DtIdx;
		if( pDtProps == NULL ) {	/* Keine gueltige Adresse in pDtProps ? */
			return( -12 );
		}
		pDtDesc = pGlobals->pDtDesc[DtIdx];
		if( pDtDesc == NULL ) {		/* Keine gueltige Adresse in pDtDesc ? */
			return( -13 );
		}
		if ( DpIdx >= pGlobals->pPab->DpAnz[DtIdx] )	{
			return( -4 );	/* Unbekannter Datenpunkt */
		}
		if ( ItemIdx > pDtProps->DtMaxItemIdx )	{
			return( -5 );	/* Unbekanntes Item */
		}

		/* Linkflag zu Item und Client setzen ------------------------------------ */
		if( pDtDesc[ItemIdx].ItemNrCab != 0 ) {		/* Hat Item CAB-Item ? */
			pPab = pGlobals->pPab->pDtBase[DtIdx] + DpIdx * pDtProps->DtPabLen;	/* Zeiger auf DP in PAB */
			pCab = *( CabItemEntry_Typ **)( pPab + pDtProps->CabPtrOff );		/* Zeiger auf Item in CAB */
			pCab += ( pDtDesc[ItemIdx].ItemNrCab - 1 ) + ( Client * pDtProps->CabItemAnz );
			if( pCab->Link == Link ) {
				return( 0 );			/* Item hat bereits den gewuenschten Link-Status */
			}
			else {
				if( Link ) {					/* Aktionen bei Link */
					if( CliAktValToCabVal( pDtProps, pDtDesc, pPab, pCab, ItemIdx ) < 0 ) {		/* Aktuellen Wert aus Alias-PV als LastVal ins CAB schreiben */
						return( -4 );					/* Keine Alias-PV, d.h. Leiche im PAB => Fehler "Unbekannter Datenpunkt" */
					}
					pCab->Link = 1;						/* Link-Status setzen (nur, wenn aktueller Wert kopiert werden konnte) */
					pCabHead->CliLogon[Client] = 1;		/* Anmeldeflag des Clients bei Link setzen */
				}
				else {							/* Aktionen bei Unlink */
					pCab->Link = 0;						/* Link-Status loeschen */
					pCab->Force = 0;					/* Forceflag bei unlink loeschen */
				}
				sRet = 1;
			}
		}
		else {
			sRet = -1;	/* Item hat keinen CAB-Eintrag */
		}
	}
	else {
		sRet = -10;		/* Kein gueltiger Globals-Ptr */
	}
	return( sRet );
}

/*-----------------------------------------------------------------------------
// CliItemLinkAll
// Setzt fuer den angegebenen Client alle Items auf den angegebenen Status
// Gleichzeitig wird das Logon-Flag des Clients im CAB-Header gesetzt
// LastEdit:
//		25.07.03/RES	Bei Link=1 nun Aktualisierung von LastVal durch AktVal (aus Alias-PV) eingefuegt
//		10.06.03/RES	Bei Link=1 nun Force-Flag=Event setzen
//		07.06.03/RES	Bei Link=1 Force-Flag=Aktualisieren (ohne Event) eingefuegt
//		06.06.03/RES	Erstversion erstellt
//
// inputs:	Client		Client-Index
//			Link		Neuer Link-Status
// outputs:
// return:	  1 Alle Items des Clients erfolgreich gelinkt / -unlinked
//			 -1 Unzulaessiger Client
//			-10 Bk2000-Systemfehler => Globaldatenstruktur nicht gefunden
//			-11 Bk2000-Systemfehler => CAB-Header nicht gefunden
//---------------------------------------------------------------------------*/
short CliItemLinkAll( unsigned char Client, unsigned char Link )
{
	unsigned short		sRet = 0;
	Globals_Typ			*pGlobals = NULL;
	DtProps_Typ 		*pDtProps;
	DtDesc_Typ 			*pDtDesc;
	char				*pCabItemConv;
	CabHeader_Typ		*pCabHead = NULL;
	CliPipeHead_Typ		*pPipe;
	unsigned char		DtIdx;					/* Schleifenzaehler Datentypen */
	unsigned short		DpIdx;					/* Schleifenzaehler Datenpunkte */
	unsigned char		CabItemIdx;				/* Schleifenzaehler CAB-Items */
	CabItemEntry_Typ	*pCabDt, *pCabCli, *pCabDp, *pCab;
	unsigned char		*pPab;

	/* Zeiger auf Globaldatenstruktur initialisieren */
	GetGlobalsPtr( &pGlobals );
	if( pGlobals != NULL ) {	/* Gueltiger Zeiger in pGlobals ? */
		pCabHead = pGlobals->pCab;
		if( pCabHead == NULL ) {	/* Keine gueltige Adresse in pCab ? */
			return( -11 );
		}
		/* Parameter überprüfen */
		if( Client >= pCabHead->ClientAnz ) {
			return( -1 );	/* Unzulaessiger Client */
		}

		/* Bei Unlink: Zuerst Anmeldeflag des Clients loeschen */
		if( Link == 0 ) {
			pCabHead->CliLogon[Client] = 0;
		}

		/* Linkflag aller Items des Clients setzen ------------------------------------ */
		for( DtIdx = 0; DtIdx < DT_ANZ; DtIdx++ ) {		/* Schleife ueber alle Datentypen */
			pDtProps = pGlobals->pDtProps + DtIdx;
			if( pDtProps == NULL ) {		/* Keine gueltige Adresse in pDtProps ? */
				continue;
			}
			pDtDesc = pGlobals->pDtDesc[DtIdx];
			if( pDtDesc == NULL ) {			/* Keine gueltige Adresse in pDtDesc ? */
				continue;
			}
			pCabItemConv = pGlobals->pCabItemConv[DtIdx];
			if( pCabItemConv == NULL ) {	/* Keine gueltige Adresse in pCabItemConv ? */
				continue;
			}
			pCabDt = (CabItemEntry_Typ *)pCabHead->pDtBase[DtIdx];	/* Zeiger auf Datentyp im CAB */
			if( pCabDt != 0 ) {
				pCabCli = pCabDt + Client * pDtProps->CabItemAnz;	/* Zeiger auf Client im CAB */
				for( DpIdx = 0, pCabDp = pCabCli; DpIdx < pCabHead->DpAnz[DtIdx]; DpIdx++ ) {	/* Schleife ueber alle Datenpunkte dieses Datentyps */
					pPab = pGlobals->pPab->pDtBase[DtIdx] + DpIdx * pDtProps->DtPabLen;		/* Zeiger auf DP in PAB */
					for( CabItemIdx = 0, pCab = pCabDp; CabItemIdx < pDtProps->CabItemAnz; CabItemIdx++, pCab++ ) {	/* Schleife ueber alle Items dieses Datentyps */
						if( Link ) {				/* Aktionen bei Link */
							if( CliAktValToCabVal( pDtProps, pDtDesc, pPab, pCab, pCabItemConv[CabItemIdx] ) == 0 ) {	/* Aktuellen Wert aus Alias-PV als LastVal ins CAB schreiben */
								pCab->Link = 1;				/* Link-Status setzen (nur, wenn aktueller Wert kopiert werden konnte) */
							}
						}
						else {						/* Aktionen bei Unlink */
							pCab->Link = 0;					/* Link-Status loeschen */
							pCab->Force = 0;				/* Forceflag bei unlink loeschen */
						}
					}
					pCabDp += pCabHead->ClientAnz * pDtProps->CabItemAnz;
				}
			}
		}

		if( Link ) {	/* Bei Link: Zuletzt Anmeldeflag des Clients setzen */
			pCabHead->CliLogon[Client] = 1;
		}
		else {	/* Bei Unlink: Zuletzt alle Pipes des Clients leeren */
			for( DtIdx = 0; DtIdx < DT_ANZ; DtIdx++ ) {		/* Schleife ueber alle Datentypen */
				if( CliGetPipePtr( Client, DtIdx, &pPipe ) == 0 ) {
					pPipe->pRead = pPipe->pFirst;	/* Pipe leeren */
					pPipe->pWrite = pPipe->pFirst;	/* Pipe leeren */
				}
			}
		}
		sRet = 1;	/* Alles erledigt */
	}
	else {
		sRet = -10;		/* Kein gueltiger Globals-Ptr */
	}
	return( sRet );
}

/*-----------------------------------------------------------------------------
// CliSetItemHyst()
// Setzt zum angegebenen Item die Hysterese fuer den angegebenen Client
// LastEdit:
//		07.06.03/RES	Hysteresezeiger-Pruefung eingefuegt
//		02.05.03/RES	Erstversion erstellt
//
// inputs:	Client		Client-Index
//			DtIdx		Datentyp-Index
//			DpIdx		Datenpunkt-Index
//			ItemIdx		Item-Index
//			Len			Datenlaenge des Hysteresewertes
//			pHyst		Zeiger auf Hysteresewert
// outputs:
// return:	 1 = Item-Hyst erfolgreich eingetragen
//			-1 = Item hat keinen CAB-Eintrag
//			-2 = Unzulaessiger Client
//			-3 = Ungueltiger Datentyp
//			-4 = Unbekannter Datenpunkt
//			-5 = Unbekanntes Item
//			-6 = Falsche Datenlaenge
//			-7 = Ungueltiger pHyst
//			-10 = Bk2000-Systemfehler => Globaldatenstruktur nicht gefunden
//			-11 = Bk2000-Systemfehler => CAB-Header nicht gefunden
//			-12 = Bk2000-Systemfehler => Dt-Props nicht gefunden
//			-13 = Bk2000-Systemfehler => Dt-Desc nicht gefunden
//---------------------------------------------------------------------------*/
short CliSetItemHyst( unsigned char Client, unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx, unsigned char Len, unsigned char *pHyst )
{
	unsigned short		sRet = 0;
	Globals_Typ			*pGlobals = NULL;
	DtProps_Typ 		*pDtProps;
	DtDesc_Typ 			*pDtDesc;
	CabHeader_Typ		*pCabHead = NULL;
	unsigned char		*pPab;
	CabItemEntry_Typ	*pCab;

	GetGlobalsPtr( &pGlobals );								/* Zeiger auf Globaldatenstruktur initialisieren */
	if( pGlobals != NULL ) {
		pCabHead = pGlobals->pCab;
		if( pCabHead == NULL ) {							/* Keine gueltige Adresse in pCab ? */
			return( -11 );
		}
		if( Client >= pCabHead->ClientAnz ) {
			return( -2 );									/* Unzulaessiger Client */
		}
		if ( DtIdx >= DT_ANZ )	{
			return( -3 );									/* Ungueltiger Datentyp */
		}
		pDtProps = pGlobals->pDtProps + DtIdx;
		if( pDtProps == NULL ) {							/* Keine gueltige Adresse in pDtProps ? */
			return( -12 );
		}
		pDtDesc = pGlobals->pDtDesc[DtIdx];
		if( pDtDesc == NULL ) {								/* Keine gueltige Adresse in pDtDesc ? */
			return( -13 );
		}
		if ( DpIdx >= pGlobals->pPab->DpAnz[DtIdx] )	{
			return( -4 );									/* Unbekannter Datenpunkt */
		}
		if ( ItemIdx > pDtProps->DtMaxItemIdx )	{
			return( -5 );									/* Unbekanntes Item */
		}
		if ( Len != abs( pDtDesc[ItemIdx].ItemLen ) )	{
			return( -6 );									/* Falsche Datenlaenge */
		}
		if( pHyst == NULL ) {
			return( -7 );									/* Ungueltiger Zeiger auf Hysteresewert */
		}

		/* Hysteresewert zu Item und Client eintragen ------------------------------- */
		if( pDtDesc[ItemIdx].ItemNrCab != 0 ) {		/* Hat Item CAB-Item ? */
			pPab = pGlobals->pPab->pDtBase[DtIdx] + DpIdx * pDtProps->DtPabLen;	/* Zeiger auf DP in PAB */
			pCab = *( CabItemEntry_Typ ** )( pPab + pDtProps->CabPtrOff );		/* Zeiger auf Item in CAB */
			pCab += ( pDtDesc[ItemIdx].ItemNrCab - 1 ) + ( Client * pDtProps->CabItemAnz );
			switch( Len ) {
				case 1:
					pCab->Hyst = ( unsigned long )*pHyst;
					break;
				case 2:
					pCab->Hyst = ( unsigned long )*( unsigned short * )pHyst;
					break;
				case 4:
					pCab->Hyst = *( unsigned long * )pHyst;
					break;
				default:
					return( -6 );							/* Falsche Datenlaenge */
			}
			sRet = 1;
		}
		else {
			sRet = -1;										/* Item hat keinen CAB-Eintrag */
		}
	}
	else {
		sRet = -10;		/* Kein gueltiger Globals-Ptr */
	}

	return( sRet );
}

/*-----------------------------------------------------------------------------
// CliForceWriteEvent()
// Erzwingt zum angegebenen Item einen Event mit aktuellem Wert fuer alle gelinkten Clients
// LastEdit:
//		04.08.03/RES	Umbenannt nach CliForceWriteEvent() und Client aus Argumentliste entfernt
//		27.07.03/RES	Erstversion erstellt
//
// inputs:	DtIdx		Datentyp-Index
//			DpIdx		Datenpunkt-Index
//			ItemIdx		Item-Index
// outputs:
// return:	 1 = Event erfolgreich angefordert
//			-1 = Item hat keinen CAB-Eintrag
//			-3 = Ungueltiger Datentyp
//			-4 = Unbekannter Datenpunkt
//			-5 = Unbekanntes Item
//			-10 = Bk2000-Systemfehler => Globaldatenstruktur nicht gefunden
//			-11 = Bk2000-Systemfehler => CAB-Header nicht gefunden
//			-12 = Bk2000-Systemfehler => Dt-Props nicht gefunden
//			-13 = Bk2000-Systemfehler => Dt-Desc nicht gefunden
//---------------------------------------------------------------------------*/
short CliForceWriteEvent( unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx )
{
	unsigned short		sRet = 0;
	Globals_Typ			*pGlobals = NULL;
	DtProps_Typ 		*pDtProps;
	DtDesc_Typ 			*pDtDesc;
	CabHeader_Typ		*pCabHead = NULL;
	unsigned char		*pPab;
	CabItemEntry_Typ	*pCab;
	unsigned char		Client;

	GetGlobalsPtr( &pGlobals );								/* Zeiger auf Globaldatenstruktur initialisieren */
	if( pGlobals != NULL ) {
		pCabHead = pGlobals->pCab;
		if( pCabHead == NULL ) {							/* Keine gueltige Adresse in pCab ? */
			return( -11 );
		}
		if ( DtIdx >= DT_ANZ )	{
			return( -3 );									/* Ungueltiger Datentyp */
		}
		pDtProps = pGlobals->pDtProps + DtIdx;
		if( pDtProps == NULL ) {							/* Keine gueltige Adresse in pDtProps ? */
			return( -12 );
		}
		pDtDesc = pGlobals->pDtDesc[DtIdx];
		if( pDtDesc == NULL ) {								/* Keine gueltige Adresse in pDtDesc ? */
			return( -13 );
		}
		if ( DpIdx >= pGlobals->pPab->DpAnz[DtIdx] ) {
			return( -4 );									/* Unbekannter Datenpunkt */
		}
		if ( ItemIdx > pDtProps->DtMaxItemIdx )	{
			return( -5 );									/* Unbekanntes Item */
		}

		/* Hysteresewert zu Item fuer alle gelinkten Clients eintragen ------------------------------- */
		if( pDtDesc[ItemIdx].ItemNrCab != 0 ) {		/* Hat Item CAB-Item ? */
			pPab = pGlobals->pPab->pDtBase[DtIdx] + DpIdx * pDtProps->DtPabLen;	/* Zeiger auf DP in PAB */
			pCab = *( CabItemEntry_Typ ** )( pPab + pDtProps->CabPtrOff );		/* Zeiger auf Item in CAB */
			pCab += pDtDesc[ItemIdx].ItemNrCab - 1;
			for( Client = 0; Client < pCabHead->ClientAnz; Client++ ) {
				if( pCab->Link ) {
					pCab->Force = 3;	/* Event mit AKTUELLEM Wert anfordern */
				}
				pCab += pDtProps->CabItemAnz;	/* Zeiger auf Abblid fuer naechsten Client */
			}
			sRet = 1;
		}
		else {
			sRet = -1;										/* Item hat keinen CAB-Eintrag */
		}
	}
	else {
		sRet = -10;		/* Kein gueltiger Globals-Ptr */
	}

	return( sRet );
}

/*-----------------------------------------------------------------------------
// CliEvRead
// Event-Message aus Client-Event-Pipe lesen
// Last Edit:
//		08.08.03/RES: Pipe-Eintraege auf gleiche Laenge abgestimmt => schnelleres Schreiben und v.a. Lesen einer Meldung
//					  ( dabei Argumentliste geaendert )
//		07.06.03/RES: Meldungs- und Längenzeiger - Pruefung eingefuegt
//		05.06.03/RES: Erstversion aus EvRead() erzeugt
//
// inputs:	Client		Client
//			DtIdx		Datentyp-Index
//
// outputs:	pDpIdx		Datenpunkt-Index
//			pItemIdx	Item-Index
//			pItemLen	Item-Laenge
//			pValue		Item-Wert
//
// return:	  1 Meldung aus Pipe gelesen
// 			  0 keine Meldung in Pipe
//			 -1 Unzulaessiger Client
//			 -2 Unzulaessiger DtIdx
//			 -3 Unzulaessige Item-Laenge
//			<-9 Systemfehler --> siehe CliEvGetPipePtr()
//---------------------------------------------------------------------------*/
short CliEvRead( unsigned char Client, unsigned char DtIdx, unsigned short *pDpIdx, unsigned char *pItemIdx, unsigned char *pItemLen, unsigned long *pValue )
{
	short 				sRet = 0;
	CliPipeHead_Typ		*pPipe;

	if( ( sRet = CliGetPipePtr( Client, DtIdx, &pPipe ) ) == 0 ) {
		/* Daten aus Pipe in Message-Puffer kopieren */
		if( pPipe->pRead != pPipe->pWrite ) {	/* Meldung in Pipe ? */
			sRet = 1;
			*pDpIdx = pPipe->pRead->ItemAdr.DpIdx;
			*pItemIdx = pPipe->pRead->ItemAdr.ItemIdx;
			*pItemLen = pPipe->pRead->ItemAdr.ItemLen;
			switch( *pItemLen ) {
				case 1:
					*( unsigned char *)pValue = ( unsigned char )pPipe->pRead->ItemVal;
					break;
				case 2:
					*( unsigned short *)pValue = ( unsigned short )pPipe->pRead->ItemVal;
					break;
				case 4:
					*pValue = pPipe->pRead->ItemVal;
					break;
				default:
					sRet = -3;
					break;
			}
			/* Lesezeiger versetzen */
			if( pPipe->pRead == pPipe->pLast ) {
				pPipe->pRead = pPipe->pFirst;
			}
			else {
				pPipe->pRead++;
			}
		} /* Meldung in Pipe */
		else {	/* Keine Meldung in Pipe */
			sRet = 0;	/* Pipe ist leer */
		}
	}
	return( sRet );
}

/*-----------------------------------------------------------------------------
// CliAktValToCabVal
// Setzt Cab.LastVal auf AktVal aus Alias-PV
// = Hilfsfunktion fuer CliItemLink() und CliItemLinkAll()
//
// LastEdit:
//		24.07.03/RES	Erstversion erstellt
//
// inputs:	pDtProps	Zeiger auf DT-Props des aktuellen Datentyps
//			pDtDesc		Zeiger auf DT-Desc des aktuellen Datentyps
//			pPab		Zeiger auf Datenpunkt in PAB
//			pCab		Zeiger auf Item in CAB
//			ItemIdx		Item-Index
// outputs:
// return:	  0 Aktuellen Wert aus Alias-PV erfolgreich auf Cab.LastVal kopiert
//			 -1 Keine Alias-PV, d.h. Leiche im PAB
//---------------------------------------------------------------------------*/
short CliAktValToCabVal( DtProps_Typ *pDtProps, DtDesc_Typ *pDtDesc, unsigned char *pPab, CabItemEntry_Typ *pCab, unsigned char ItemIdx )
{
	unsigned char	*pAlias;
	unsigned char	*pAktVal;

	/* Aktuellen Wert aus Alias-PV als LastVal ins CAB schreiben */
	pAlias = *(unsigned char **)( pPab + pDtProps->AliasPtrOff );
	if( pAlias == NULL ) {
		return( -1 );		/* Keine Alias-PV, d.h. Leiche im PAB */
	}
	pAktVal = pAlias + pDtDesc[ItemIdx].ItemOff;
	switch( abs( pDtDesc[ItemIdx].ItemLen ) ) {
		case 1:
			pCab->LastVal = ( unsigned long )*pAktVal;
			break;
		case 2:
			pCab->LastVal = ( unsigned long )*( unsigned short * )pAktVal;
			break;
		case 4:
			pCab->LastVal = *( unsigned long * )pAktVal;
			break;
	}
	return( 0 );
}


