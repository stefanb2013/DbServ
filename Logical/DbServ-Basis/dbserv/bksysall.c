#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/* bksysall.c ----------------------------------------------------------------- */

#include <bur/plc.h>      /*some usefull macros for B&R pcc programming*/
#include <sys_lib.h>

#include <stdlib.h>
#include <string.h>

#include "bksys.h"


/*-----------------------------------------------------------------------------
// ErrStatistics
// Fehlerbehandlungsroutine
// Last Edit:
//		28.06.00/RES:	Bei mehrzeiligen Eintraegen wird ab Zeile 2 die Zeilennummer eingetragen
//		12.05.00/RES:	ErrNum=0 wird nicht mehr statistisch erfasst
//						pErrPre eingefuehrt
//						Aufteilung langer ErrTexte auf mehrere Logbuch-Eintraege
//						wobei bedingte und unbedingte Umbrueche definiert werden koennen
//		23.12.99/RES
//		30.04.99/RES
//
// inputs:	ErrStat		Fehlerstatus: 0=Warn, 1=Err, 2=Err+Log, 3=Err+Log+HALT
//			ErrNum		Fehlernummer
//			ErrInfo		Fehlerinfo
//			pErrPre		Praefix zu Fehlertext (fuer jede Eintragszeile)
//			pErrText	Zeiger auf Fehlertext
//
// outputs:
//
// return:
//---------------------------------------------------------------------------*/
void ErrStatistics( unsigned char *pTaskName,
					unsigned char ErrStat, unsigned short ErrNum, unsigned long ErrInfo,
					unsigned char *pErrPre, unsigned char *pErrText )
{
	static Stats_Typ *pStats = NULL;
	static short ErrFlag = 0;
	unsigned short LogErrNo;
	long lPtr, lVarLen;
	char szText[ERRTEXT_MAXLEN + 1];				/* Puffer fuer Fehlertext */
	char *pRead, *pWrite, *pStart, *pReadTemp;		/* Diverse Textzeiger */
	char buffer[64];
	short sByteAnz, sRest, sTextLen, sLine, sReadyText, sReadyLine, sSearchCount, sSearchCountMax;
	short sCutFlag = 0;		/* Trenn-Flag fuer Anzeige des Trennzeichens am Zeilenanfang */

	LogErrNo = EC_BK2_BASE + ErrNum;	/* Fehlernummer fuer Logbuch und LastErr sowie LastWarn */

	if( pStats == NULL ) {	/* Zeiger noch nicht bekannt ? */
		/* Zeiger auf Statistikstruktur ermitteln */
		if (strlen(pTaskName) > 0) {
			strcpy(buffer, pTaskName);
			strcat(buffer, ":");
		}
		else {
			buffer[0] = 0;
		}
		strcat(buffer, "SYS_Statistic");
		if( !PV_xgetadr( buffer, &lPtr, &lVarLen ) ) {
			pStats = (Stats_Typ *)lPtr;
		}
		else {	/* SYS_Statistic nicht bekannt */
			if( ErrFlag == 0 ) {
				ERRxwarning( EC_BK2_BASE + EC_SYS_BASE + NOVALIDSTATSPTR_NUM, 0, NOVALIDSTATSPTR_TEXT );
				ErrFlag = 1;
			}
		}
	}
	if( pStats != NULL ) {	/* Zeiger bekannt ? */
		switch(ErrStat) {
			case WARN:	/* Warnmeldung */
				if( ErrNum > 0 ) {
					pStats->LastWarn = LogErrNo;
				}
				break;
			case ERR:	/* Fehlermeldung ohne Logbucheintrag */
				if( ErrNum > 0 ) {
					pStats->LastErr = LogErrNo;
				}
				break;
			case ERRLOG:		/* Fehlermeldung mit Logbucheintrag */
			case ERRLOGHALT:	/* Fehlermeldung mit Logbucheintrag und RPS-Halt */
				/* Fehlernummer eintragen */
				if( ErrNum > 0 ) {
					pStats->LastErr = LogErrNo;
				}
				/* Praefix in Textpuffer eintragen */
				sByteAnz = strlen( pErrPre );
				if( sByteAnz > 0 ) {	/* Praefix vorhanden! */
					if( sByteAnz >= ERRTEXT_MAXLEN ) {	/* Praefix laenger als Puffer ? */
						sByteAnz = ERRTEXT_MAXLEN - 1;
					}
					strncpy( szText, pErrPre, sByteAnz );
					pStart = szText + sByteAnz;
				}
				else {	/* Kein Praefix angegeben! */
					pStart = szText;
				}
				sTextLen = ERRTEXT_MAXLEN - ( pStart - szText );

				/* Fehlertext zeilenweise in Textpuffer eintragen und ins Logbuch schreiben */
				for( pRead = pErrText, sLine = 0, sReadyText = 0; !sReadyText && sLine < ERRTEXT_MAXLINES; sLine++ ) {
					pWrite = pStart;
					sRest = sTextLen;
					/* Zeilennummer fuer Folgezeilen einfuegen */
					if( sLine > 0 ) {
						if( sRest > 2 ) {	/* Nur, wenn mehr als 2 Zeichen fuer Nutztext zur Verfuegung! */
							*pWrite++ = '1' + sLine;
							sRest--;
						}
					}
					/* Prefic-Trennzeichen einfuegen */
					if( sRest > 2 ) {	/* Nur, wenn mehr als 2 Zeichen fuer Nutztext zur Verfuegung! */
						*pWrite++ = ':';
						sRest--;
					}

					/* Text fuer eine Meldezeile zusammenstellen */
					for( sReadyLine = 0; !sReadyLine && sRest > 0; sRest-- ) {
						/* Trennzeichen an Zeilenanfang einfuegen, wenn letzte Zeile abgeschnitten wurden */
						if( sCutFlag ) {
							sCutFlag = 0;
							if( sRest > 2 ) {	/* Nur, wenn mehr als 2 Zeichen fuer Nutztext zur Verfuegung! */
								*pWrite++ = CUT_SIGN;
								sRest--;
							}
						}
						/* Naechstes Lesezeichen auswerten */
						switch( *pRead  ) {
							case 0:		/* Ende des Fehlertextes erreicht */
								sReadyText = 1;
								sReadyLine = 1;
								break;
							case '\n':	/* Unbedingter Umbruch */
								sReadyLine = 1;
								pRead++;
								break;
							default:	/* Zu druckendes Textzeichen */
								if( sRest == 1 ) {	/* Zeilenende erreicht */
									switch( *( pRead + 1 ) ) {	/* Nachfolgendes Zeichen pruefen */
										case 0:		/* Ende des Fehlertextes erreicht */
											*pWrite++ = *pRead++;
											break;
										case '\n':	/* Unbedingter Umbruch */
										case '\t':	/* Bedingter Umbruch */
										case ' ':	/* Bedingter Umbruch */
											*pWrite++ = *pRead++;
											pRead++;	/* Trennzeichen ueberspringen */
											break;
										default:	/* Kein Trenn bzw. Endezeichen */
											/* Maximale Zeichenanzahl fuer nachfolgende Trennzeichensuche ermitteln */
											sSearchCountMax = ( SEARCH_COUNT_MAX < sTextLen ) ? SEARCH_COUNT_MAX : sTextLen;
											if( (short)strlen( pRead ) < sSearchCountMax ) {
												sSearchCountMax = strlen( pRead );
											}
											/* Letztes bedingtes Trennzeichen suchen */
											for( pReadTemp = pRead - 1, sSearchCount = 1; sSearchCount < sSearchCountMax; pReadTemp--, sSearchCount++ ) {
												if( *pReadTemp == ' ' || *pReadTemp == '\t' ) {
													break;	/* Trennzeichen gefunden => Suche abbrechen */
												}
											}
											if( sSearchCount < sSearchCountMax ) {	/* Suche vorzeitig abgebrochen, d.h. Trennzeichen gefunden */
												pRead -= sSearchCount - 1;	/* Lesezeiger auf Zeichen nach Trennzeichen setzen */
												pWrite -= sSearchCount;	/* Schreibzeiger auf Trennzeichen zuruecksetzen */
											}
											else {	/* Kein Trennzeichen gefunden => Zeile abschneiden */
												*pWrite++ = CUT_SIGN;	/* Kennzeichen fuer "harte" Trennung einfuegen */
												sCutFlag = 1;	/* Trenn-Flag fuer naechste Zeile merken */
											}
											break;
									} /* switch( *pRead ) */
								} /* if( sRest == 1 ) */
								else {	/* Noch nicht am Zeilenende ! */
									*pWrite++ = *pRead++;
								}
								break;
						} /* switch( *pRead ) */
					} /* Zeichenschleife */
					*pWrite = 0;	/* Zeichenkette abschliessen */

					/* Gesamte Zeile ins Logbuch schreiben */
					ERRxwarning( LogErrNo, bin_to_bcd_8( ErrInfo ), szText );

					/* Sonderfall "Textende gleich nach Zeilenende" pruefen */
					if( !sReadyText && *pRead == 0 ) {
						sReadyText = 1;	/*Textende-Flag setzen, um abschließende Leerzeile zu unterdruecken */
					}
				} /* Zeilenschleife */

				/* RPS anhalten ? */
				if( ErrStat == ERRLOGHALT ) {
					ERR_fatal( LogErrNo, bin_to_bcd_8( ErrInfo ) );
				}
				break;
			default:
				break;
		}
		/* Fehlerzaehler erhoehen */
		if( ErrNum > 0 && ErrNum < ERRCNTANZ ){
			if( pStats->ErrCount[ErrNum] < 65535 ) {
				pStats->ErrCount[ErrNum]++;
			}
		}
	}
}


/*-----------------------------------------------------------------------------
// ErrStatisticsX
// Erweiterte Fehlerbehandlungsroutine
// Variante 1: ErrBit entspricht Bitmuster in Fehlermaske ( 2-Byte )
// Last Edit:
//		10.05.00/RES:	pErrPre eingefuehrt
//		23.12.99/RES
//		19.05.99/RES
//
// inputs:  pErrMk		Zeiger auf Fehlermaske
//			ErrBit		Fehlerbit in Fehlermaske
// 			pSyErr  	Zeiger auf Systemfehlerzaehler
//			ErrStat1	Fehlerstatus 1. Auftreten des Fehlers: 0=Warn, 1=Err, 2=Err+Log, 3=Err+Log+HALT
//			ErrStatN	Fehlerstatus n. Auftreten des Fehlers: 0=Warn, 1=Err, 2=Err+Log, 3=Err+Log+HALT
//			ErrNum		Fehlernummer
//			ErrInfo		Fehlerinfo
//			pErrPre		Praefix zu Fehlertext (fuer jede Eintragszeile)
//			pErrText	Zeiger auf Fehlertext
//
// outputs:
//
// return:
//---------------------------------------------------------------------------*/
void ErrStatisticsX(unsigned char *pTaskName, 
					unsigned short *pErrMk, unsigned short ErrBit, unsigned short *pSyErr,
					unsigned char ErrStat1, unsigned char ErrStatN, unsigned short ErrNum,
					unsigned long ErrInfo, unsigned char *pErrPre, unsigned char *pErrText )
{
	if( !(*pErrMk & ErrBit) ) {	/* Fehlerbit noch nicht gesetzt */
		*pErrMk |= ErrBit;
		ErrStatistics( pTaskName, ErrStat1, ErrNum, ErrInfo, pErrPre, pErrText );
	}
	else {
		ErrStatistics( pTaskName, ErrStatN, ErrNum, ErrInfo, pErrPre, pErrText );
	}
	if (pSyErr != NULL)
		*pSyErr += 1;
}


/*-----------------------------------------------------------------------------
// GetGlobalsPtr
// Liefert Zeiger auf Globaldatenstruktur
// Last Edit:
//		30.06.00/RES: ErrStatistics()-Aufruf bei Fehlern entfernt
//		19.07.99/RES
//		09.06.99/RES
//
// inputs:	-
// outputs: ppGlobals	Zeiger auf Globaldatenstruktur
// return:	-
//---------------------------------------------------------------------------*/
void GetGlobalsPtr( Globals_Typ **ppGlobals )
{
	static Globals_Typ *pGlobals = NULL;
	long lPtr, lVarLen;

	if( pGlobals == NULL ) {	/* Zeiger noch nicht bekannt ? */
		/* Zeiger auf Globaldatenstruktur ermitteln */
		if( !PV_xgetadr( "SYS_pGlobals", &lPtr, &lVarLen ) ) {
			pGlobals = *(Globals_Typ **)lPtr;
		}
		else {	/* SYS_pGlobals nicht gefunden */
			pGlobals = NULL;
		}
	}

	/* Zeiger zurueckliefern */
	*ppGlobals = pGlobals;
}
/*-----------------------------------------------------------------------------
// bin_to_bcd_8
// Wandelt Unsigned-long-Wert in 8-stellige BCD-Zahl (unsigned long) um
// Last Edit:
//		20.12.99/MKrz
//
// inputs:  value		Wert im Binaer-Format
//
// return:				Wert im BCD-Format
//---------------------------------------------------------------------------*/
unsigned long bin_to_bcd_8( unsigned long value )
{
	unsigned short	i;
	unsigned long	bcd = 0;
	unsigned long	tbin = 1;
	unsigned long	tbcd = 1;

	for (i=0; i<8; i++) {
		bcd += value/tbin % 10 * tbcd;
		tbin *= 10;
		tbcd *= 16;
	}
	return bcd;
}




