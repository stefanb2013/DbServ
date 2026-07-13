#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Hauptprogramm
*  Datei:    iec60870_5_103.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 03.00.5
*  Datum:    07-02-2019
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion:
*------------------------------------------------------------------------------
*  Rev.   | Datum    | Name | Kommentar
*  -------+----------+------+---------------------------------------------------
*  01.00  | 26-11-03 | KOB  | Datei-Erstellung
*  01.01  | 09-02-04 | KOB  | Debugkonfiguration um Liniennummer erweitert
*         |          |      | Meldung an LS, falls Schnittstelle nicht geöffnet werden
*		  |          |      | kann, dabei warten bis myidBrServer!= 0 ist.
*  01.02  | 07-05-04 | KOB  | Verbindungsstoerung bei Initialisierung auf 1 setzen, damit
*		  |          |      | beim ersten gültigen Telegramm der Datenpunkt Kopplungsstoerung
*		  |          |      | beschrieben wird.
*  01.03  | 15-12-04 | KOB  | Bei ungültigen Analogwerten (Invalid oder Overflow) wird
*         |          |      | als Ersatzwert 7FFF gesendet.
*  01.04  | 11-04-06 | WAI  | FileIO-Library includiert wegen neuer dbServ-Version mit Archivierung
*  01.05  | 24-05-06 | KOB  | Telegrammauswertung flexibler gestaltet (wegen anderem Verhalten von ABB)
*		  |          |      |
*  01.06  | 10-01-07 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Type Identification 3 : Measurands 1
*         |          |      | (check_m3, iec__103_sub.c)
*         |          |      | Type Identification 79: Privates Telegramm zur Übertragung von Zählwerten
*         |          |      | (check_m79, iec__103_sub.c)
*         |          |      | Auswertung der im privaten Telegramm empfangenen Zählwerte
*         |          |      | (save_analogwert_priv, iec__103_serv.c)
*		  |          |      |
*  02.05.1| 02-09-08 | KOB  | Neue Modi für Doppelmeldungen:
*		  |			 |		| DOPPEL_EINZEL_NORMAL_NOINVALID und DOPPEL_EINZEL_INVERS_NOINVALID
*		  |          |      |
*		  |			 |		| KE_Init: Bei der Auswertungt des Rückmeldetelegrammes
*		  |			 |		| Identification Message (check_m5) wird die Überprüfung des
*		  |			 |		| Function Type weggelassen, d.h. es wird für alle Geräte aktzeptiert.
*		  |			 |		| Bisher wurde das Telegramm nur für Function Type 160,192,240 zugelassen.
*		  |          |      |
*         |          |      |
*  02.05.2| 02-09-08 | KOB  | Funktionen wie unter 01.06 beschrieben eingefügt.
*         |          |      |
*  02.06.4| 27-09-10 | WAI  | IsANumber() mit Vorzeichen
*         |          |      |
*  02.07.2| 26-07-11 | KOB  | Erweiterung um die Telegramme:
*         |          |      | Type Identification 205: Privates Telegramm zur Übertragung von Zählwerten (Siemens)
*         |          |      | (check_m205, iec__103_sub.c)
*         |          |      | Auswertung der im privaten Telegramm empfangenen Zählwerte
*         |          |      | (save_counter_2, iec__103_serv.c)
*         |          |      |
* 		  |			 | KOB  | Ueberwachung der Zeitsynchronisation:
*         |          |      | Parameter IEC Global: Timeout fuer Rueckmeldung Zeitsyncronisation (TIME.TIMEOUT)
*         |          |      | Parameter pro Gerät : ERR.TIMESYNC
*  02.07.3| 04-12-13 | KOB  | check_m5 angepasst: Zustand von ACD Bit ignoriert, Kompatibilitätsmodus 3 akzeptieren
*         |          |      | check_m6 angepasst: nicht verwendete Bits der zurueckgemeldeten Zeit ausmaskieren
*         |          |      | neue Debug Möglichkeit (Variablen: IEC_Info_Clear,IEC_Info_Enable,IEC_InfoNumber,IEC_Info)
*         |          |      | Aufschlüsselung der IEC Telegrammsteuerelemente und Eintrag in Buffer
*  03.00.2| 28-01-14 | KOB  | check_m5 angepasst: Info Nr 2 akzeptieren
*         |          |      | Defaultwert für IEC_Config.TimeClass2PollFast von 100ms auf 200ms erhöht
*  03.00.3| 04-04-14 | KOB  | TIME_DIFF_SYNCH_MAX von 2 auf 10 geändert
*         |          |      | DeviceActivType erweitert um die Elemente
*         |          |      | TDiff und TDiffCounter;
*         |          |      | check_m6() Uhrzeitdifferenz wird in Struktur eingetragen
*         |          |      | Ist die Uhrzeitdifferenz zu groß, wird erste beim 2.Mal Felhler generiert
*  03.00.4| 11-10-16 | WAI  | Änderungen tbd (BIE)
*  03.00.5| 07-02-19 | KOB  | Senden von Kommandos überarbeitet

******************************************************************************/

/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>			/* RPS-spezifische Typendeklarationen */
#include <string.h>
#include <dvframe.h>
#include <asbrstr.h>
#include <sys_lib.h>
#include <brsystem.h>
#include <fileio.h>
#include "iec__103_date.h"		/* Funktionen zur Datumsberechnung */
#include "iec60870_5_103.h"		/* Definition der globalen Variablen */
#include "iec__103_debug.h"		/* Funktionen fuer Ausgabe von Debug-Meldungen */
#include "iec__103_parse.h"		/* Parser Funktionen */
#include "dbserv.h"				/* Meldeserver */

extern DebugConfig_typ	DebugConfig;


/***********************************************************************/
/* 					Initialisierungs-Unterprogramm                     */
/***********************************************************************/

void _INIT init_iec(void)
{
	signed short i,n;

	/*--------------------------------*/
	/* Variablen initialisieren       */
	/*--------------------------------*/
	for(i=0;i<ITEM_DIAGNOSE_SIZE;i++) {
		IEC_ItemDiagnose[i].showData	= 0;
	}
	IEC_NormDiagnose.showData			= 0;

	dbg_buf_iec_clear();
	IEC_Info_Clear	= 0;
	IEC_Info_Enable = 0;


	memset(&SYS_Statistic,0,sizeof(SYS_Statistic));
	memset(&DistDataServer,0,sizeof(DistDataServer));						/* Variable fuer die Uebergabe von Stoerschrieben an das Leitsystem */
	memset(&stANL,0,sizeof(stANL));											/* Variable für Linien und Geräteverwaltung */
	memset(&stComIni,0,sizeof(stComIni));									/* Variable zur Verwaltung der Schnittstellen */

	myidBrServer			= 0;											/* ID BR-Server */
	DisturbanceSimulation	= 0;											/* Merker zur Simulation eines Stoerschriebes */

	rt_info.enable = 1; 													/* Taskklassenzykluszeit ermitteln */
	RTInfo(&rt_info);
	if(rt_info.status==0)	CycleTime	= (rt_info.cycle_time / 1000);		/* Zykluszeit in ms */
	else 					CycleTime	= 10;								/* Zykluszeit in ms */

	DebugConfig.OpenDevice			= 0;									/* Debug Funktion deaktivieren */
	DebugConfig.DebugEnable			= 0;
	DebugConfig.DebugDelayAdresse	= 0;
	DebugConfig.DebugDelayTime		= 0;
	DebugConfig.DebugDelayCounter	= 0;

	Error	= 0;															/* Generelle Variable fuer Initialisierungsfehler */

	/*----------------------------------------------------------------------*/
	/* initialize debugging 												*/
	/*----------------------------------------------------------------------*/

	#ifdef DEBUG
		debug_initialize();
	#endif

	/*--------------------------------------------------*/
	/* Globale Konfigurationsparameter initialisieren	*/
	/*--------------------------------------------------*/

	IEC_Config.TimeOut				= 100;				/* Timeout bei Telegrammempfang (in ms) */
	IEC_Config.RetryAnz				= 6;				/* Anzahl Telegrammwiederholungen bei bestehender Verbindung */
	IEC_Config.ConnAnz				= 3;				/* Anzahl Telegrammwiederholung bei neu aufzubauender Verbindung */
	IEC_Config.WaitIntv				= 100000;			/* Wartezeit für erneuten Verbindungsaufbau (in ms) */
	IEC_Config.TimeTS				= 60000;			/* Intervallzeit fuer Uhrzeitsynchronisation (in ms) */
	IEC_Config.TimeOutTimeSynch		= 10000;			/* Timeout fuer die Rückantwort bei Zeitsynchronisation in (ms) */
	IEC_Config.EnableComReset		= 1;				/* Freigabe fuer das Schliessen und Wieder-Oeffnen der Schnittstelle bei Schreibfehlern */
	IEC_Config.TimeClass2Poll		= 1000;				/* Abtastzeit Class 2 Telegramme (Analogwerte) (in ms) */
	IEC_Config.TimeClass2PollFast	= 200;				/* Abtastzeit Class 2 Telegramme (Analogwerte) (in ms) während Übertragung von Störschrieben in ms */
	IEC_Config.DistDataTimeoutMax	= 20000;			/* Timeout fuer den Telegrammempfang bei Stoerschriebuebertragung in (ms) */
	IEC_Config.DistDataEnable		= 1;				/* Freigabe fuer Stoerschriebuebertragung */
	IEC_Config.DisturbanceDelay		= 10000;			/* Zeit die ein Lesen von Stoerschrieben der gleichen Station verzoegert wird in (ms) */
	IEC_Config.CommandEnable		= 0;				/* Freigabe für die Übertragung von Kommandos */
	IEC_Config.CommandTimeoutMax	= 40000;			/* Timeout fuer die Uebertragung von Kommandos in (ms) */

	/*****************************************************************************/
	/* Initialisierung nach Konfigurationsdatenmodul dplist und typlist (DbServ) */
	/*****************************************************************************/
#ifdef INIT_MODE_DM_DPLIST

	/*--------------------------------------------------*/
	/* Konfiguration aus Konfigurationsmodulen auslesen */
	/*--------------------------------------------------*/

	for(aktANL= 0; aktANL < MAX_LINES; aktANL++) {
		for(n = 0; n < DEVICES_PRO_LINE; n++) {
			stANL[aktANL].DeviceActiv[n].FaultFunctionType		= 160; /* Vorbelegung, wird später vom Telegramm des Geräts übernommen */
		}
	}

	iec_parser();

	Error = IEC_ParserInfo.error;

	/* Kein Schwerwiegender Fehler beim Parsen */
	if(Error==0) {
		/* Maximale Anzahl Linien (Endevariable für Schleifen) */
		if(IEC_ParserInfo.NrOfLines>0) {
			maxANL = IEC_ParserInfo.NrOfLines - 1;
		}
		else {
			maxANL = 0;
		}

		for(aktANL= 0; aktANL <= maxANL; aktANL++) {
			stANL[aktANL].LinienNummer			= IEC_GlobalData.Line[aktANL].LineNr.value;
			stANL[aktANL].Error					= IEC_ParserInfo.Line[aktANL].error;
			stANL[aktANL].dm.AdrDM_norm			= IEC_ParserInfo.memAdr;
			stANL[aktANL].dm.MaxWerteDM_norm	= IEC_ParserInfo.NrOfNormValues;
			stANL[aktANL].dm.AdrDM_wert			= IEC_ParserInfo.Line[aktANL].memAdr;
			stANL[aktANL].dm.MaxWerteDM_wert	= IEC_ParserInfo.Line[aktANL].NrOfItems;

			stANL[aktANL].DeviceAtWork 			= 0;
			for(n = 0; n < DEVICES_PRO_LINE; n++) {
				stANL[aktANL].DeviceActiv[n].Enabled = stANL[aktANL].DeviceTAB[n].Aktiv;
				if(stANL[aktANL].DeviceActiv[n].Enabled) {
					stANL[aktANL].DeviceActiv[n].Adresse 				= stANL[aktANL].DeviceTAB[n].Adresse;
					stANL[aktANL].DeviceActiv[n].AdresseLeitsystem 		= 1;
					stANL[aktANL].DeviceActiv[n].AnlagenNr				= stANL[aktANL].DeviceTAB[n].AnlagenNr;
					stANL[aktANL].DeviceActiv[n].Verbindungsversuche	= 0;
					stANL[aktANL].DeviceActiv[n].VerbindungsStoerung	= 1;	/* Sorgt fuer Eintrag in Datenbank bei bestehender Verbindung */
					stANL[aktANL].DeviceActiv[n].ZeitLetzteVerbindung	= 0;
					stANL[aktANL].DeviceActiv[n].ZeitsynchAktiv			= 0;
					stANL[aktANL].DeviceActiv[n].StateTimeSynchError	= STATE_ERROR_UNDEFINED;
					stANL[aktANL].DeviceActiv[n].TimeoutTimeSynch		= 0;
					stANL[aktANL].DeviceActiv[n].FCB_Toggle				= 0;
					stANL[aktANL].DeviceActiv[n].KE_InitOk				= 0;
					stANL[aktANL].DeviceActiv[n].ZeitLetzteZeitsync		= IEC_Config.TimeTS;
					stANL[aktANL].DeviceActiv[n].Class1Request			= 0;
					stANL[aktANL].DeviceActiv[n].TimoutNext				= 0;
					stANL[aktANL].DeviceActiv[n].GI_Ok					= 0;
					stANL[aktANL].DeviceActiv[n].LetzterACD				= 0;
					stANL[aktANL].DeviceActiv[n].Class2Verz				= 0;
					stANL[aktANL].DeviceActiv[n].Class1Verz				= 0;
				}
				if(stANL[aktANL].DeviceActiv[n].Enabled && !stANL[aktANL].DeviceAtWork) {
					stANL[aktANL].DeviceAtWork = n + 1;
				}
			}
		}
	}
#endif

	/*****************************************************************************/
	/* Initialisierung mit Datenmoduln lines,devax,wertx,mruc,norm               */
	/*****************************************************************************/
#ifdef INIT_MODE_DM_WERT

	/*----------------------------------------------------------------------*/
	/* read data-modul lines 												*/
	/*----------------------------------------------------------------------*/

	DOI_lines.enable	= 1;
	DOI_lines.pName	= (UDINT) "lines";

	DatObjInfo(&DOI_lines);


	if(DOI_lines.status==0) {
		DOR_lines.enable 		= 1;
		DOR_lines.ident 		= DOI_lines.ident;
		DOR_lines.Offset 		= 0;
		DOR_lines.len			= sizeof(ActiveLines);
		DOR_lines.pDestination	= (UDINT) &ActiveLines;
		DatObjRead(&DOR_lines);


		if(ActiveLines>MAX_LINES) ActiveLines = MAX_LINES;

		if(ActiveLines>0)		maxANL = ActiveLines - 1;
		else					maxANL = 0;

		DOR_lines.Offset 		= 2;
		DOR_lines.enable 		= 1;
		DOR_lines.ident 		= DOI_lines.ident;
		for(aktANL = 0; aktANL <= maxANL; aktANL++) {
			DOR_lines.len			= sizeof(stANL[aktANL].LinienNummer);
			DOR_lines.pDestination	= (UDINT) &stANL[aktANL].LinienNummer;
			DatObjRead(&DOR_lines);
			DOR_lines.Offset 		+= 2;
		}


		DOR_lines.Offset 		= 14;
		DOR_lines.enable 		= 1;
		DOR_lines.ident 		= DOI_lines.ident;
		DOR_lines.len			= sizeof(DistDataType);
		DOR_lines.pDestination	= (UDINT) &DistDataType;
		DatObjRead(&DOR_lines);

		DOR_lines.Offset 		+= 2;
		DOR_lines.enable 		= 1;
		DOR_lines.ident 		= DOI_lines.ident;
		DOR_lines.len			= sizeof(DistDataPoint);
		DOR_lines.pDestination	= (UDINT) &DistDataPoint;
		DatObjRead(&DOR_lines);


		DOR_lines.Offset 		= 20;
		for(aktANL = 0; aktANL <= maxANL; aktANL++) {
			DOR_lines.len			= 23;
			DOR_lines.pDestination	= (UDINT) &stComIni[aktANL].StringDevice;
			DatObjRead(&DOR_lines);
			DOR_lines.Offset 		+= 24;

			DOR_lines.len			= 79;
			DOR_lines.pDestination	= (UDINT) &stComIni[aktANL].StringMode;
			DatObjRead(&DOR_lines);
			DOR_lines.Offset 		+= 80;

			for(i=sizeof(stComIni[aktANL].StringDevice)-1; i>=0;i--) {
				if((stComIni[aktANL].StringDevice[i]!=' ') && (stComIni[aktANL].StringDevice[i]!=0)) break;
				if(stComIni[aktANL].StringDevice[i]==' ') stComIni[aktANL].StringDevice[i] = 0;
			}
			for(i=sizeof(stComIni[aktANL].StringMode)-1; i>=0;i--) {
				if((stComIni[aktANL].StringMode[i]!=' ') && (stComIni[aktANL].StringMode[i]!=0)) break;
				if(stComIni[aktANL].StringMode[i]==' ') stComIni[aktANL].StringMode[i] = 0;
			}
		}
	}
	else {
		Error	= INI_ERR_DO_LINES;
		maxANL	= 0;
	}

	/*----------------------------------------------------------------------*/
	/* read data-modul mruk 												*/
	/*----------------------------------------------------------------------*/

	DOI_mruk.enable	= 1;
	DOI_mruk.pName	= (UDINT) "mruk";

	DatObjInfo(&DOI_mruk);

	if(DOI_mruk.status==0) {
		for(aktANL=0; aktANL<=maxANL; aktANL++) {
			stANL[aktANL].dm.AdrDM_mruk			= DOI_mruk.pDatObjMem;
			stANL[aktANL].dm.MaxWerteDM_mruk	= DOI_mruk.len / 2;
		}
	}
	else {
		for(aktANL=0; aktANL<=maxANL; aktANL++) {
			stANL[aktANL].dm.AdrDM_mruk			= 0;
			stANL[aktANL].dm.MaxWerteDM_mruk	= 0;
		}
		Error = INI_ERR_DO_MRUK;
	}


	/*----------------------------------------------------------------------*/
	/* read data-modul norm 												*/
	/*----------------------------------------------------------------------*/

	DOI_norm.enable	= 1;
	DOI_norm.pName	= (UDINT) "norm";

	DatObjInfo(&DOI_norm);

	if(DOI_norm.status==0) {
		for(aktANL=0; aktANL<=maxANL; aktANL++) {
			stANL[aktANL].dm.AdrDM_norm			= DOI_norm.pDatObjMem;
			stANL[aktANL].dm.MaxWerteDM_norm	= DOI_norm.len / 8;
		}
	}
	else {
		for(aktANL=0; aktANL<=maxANL; aktANL++) {
			stANL[aktANL].dm.AdrDM_norm			= 0;
			stANL[aktANL].dm.MaxWerteDM_norm	= 0;
		}
		Error = INI_ERR_DO_NORM;
	}


	/*----------------------------------------------------------------------*/
	/* read data-modul deva and wert for line 1 to n											*/
	/*----------------------------------------------------------------------*/

	for(aktANL=0; aktANL<=maxANL; aktANL++) {
		if(!stANL[aktANL].Error) {
			DOI_deva[aktANL].enable	= 1;
			switch(aktANL) {
				case 0:	DOI_deva[aktANL].pName	= (UDINT) "deva1"; break;
				case 1:	DOI_deva[aktANL].pName	= (UDINT) "deva2"; break;
				case 2:	DOI_deva[aktANL].pName	= (UDINT) "deva3"; break;
				case 3:	DOI_deva[aktANL].pName	= (UDINT) "deva4"; break;
				case 4:	DOI_deva[aktANL].pName	= (UDINT) "deva5"; break;
				case 5:	DOI_deva[aktANL].pName	= (UDINT) "deva6"; break;
			}

			DatObjInfo(&DOI_deva[aktANL]);

			if(DOI_deva[aktANL].status!=0) {
				stANL[aktANL].Error = INI_ERR_DO_DEVA;
			}
			else {
				if(DOI_deva[aktANL].len>sizeof(stANL[aktANL].DeviceTAB)) {
					DOR_deva[aktANL].len = sizeof(stANL[aktANL].DeviceTAB);
				}
				else {
					DOR_deva[aktANL].len = DOI_deva[aktANL].len;
				}

				DOR_deva[aktANL].enable 		= 1;
				DOR_deva[aktANL].ident 			= DOI_deva[aktANL].ident;
				DOR_deva[aktANL].Offset 		= 0;
				DOR_deva[aktANL].pDestination	= (UDINT) &stANL[aktANL].DeviceTAB;

				DatObjRead(&DOR_deva[aktANL]);

				if(DOR_deva[aktANL].status != 0) {
					stANL[aktANL].Error = INI_ERR_DO_DEVA_READ;
				}
				else {
					stANL[aktANL].DeviceAtWork = 0;
					for(n = 0; n < DEVICES_PRO_LINE; n++) {
						stANL[aktANL].DeviceActiv[n].Enabled = stANL[aktANL].DeviceTAB[n].Aktiv;
						if(stANL[aktANL].DeviceActiv[n].Enabled) {
							stANL[aktANL].DeviceActiv[n].Adresse 				= stANL[aktANL].DeviceTAB[n].Adresse;
							stANL[aktANL].DeviceActiv[n].AdresseLeitsystem 		= stANL[aktANL].LinienNummer + stANL[aktANL].DeviceTAB[n].Adresse;
							stANL[aktANL].DeviceActiv[n].AnlagenNr				= stANL[aktANL].DeviceTAB[n].AnlagenNr;
							stANL[aktANL].DeviceActiv[n].Verbindungsversuche	= 0;
							stANL[aktANL].DeviceActiv[n].VerbindungsStoerung	= 1;
							stANL[aktANL].DeviceActiv[n].ZeitLetzteVerbindung	= 0;
							stANL[aktANL].DeviceActiv[n].ZeitsynchAktiv			= 0;
							stANL[aktANL].DeviceActiv[n].FCB_Toggle				= 0;
							stANL[aktANL].DeviceActiv[n].KE_InitOk				= 0;
							stANL[aktANL].DeviceActiv[n].ZeitLetzteZeitsync		= IEC_Config.TimeTS;
							stANL[aktANL].DeviceActiv[n].Class1Request			= 0;
							stANL[aktANL].DeviceActiv[n].TimoutNext				= 0;
							stANL[aktANL].DeviceActiv[n].GI_Ok					= 0;
							stANL[aktANL].DeviceActiv[n].LetzterACD				= 0;
							stANL[aktANL].DeviceActiv[n].Class2Verz				= 0;
							stANL[aktANL].DeviceActiv[n].Class1Verz				= 0;
							stANL[aktANL].DeviceActiv[n].FaultFunctionType		= 160; /* Vorbelegung, wird später vom Telegramm des Geräts übernommen */
						}
						if(stANL[aktANL].DeviceActiv[n].Enabled && !stANL[aktANL].DeviceAtWork)
							stANL[aktANL].DeviceAtWork = n + 1;
					}
				} /* if(DOR_deva[aktANL].status != 0)  */
			} /* if(DOI_deva[aktANL].status!=0)  */
		} /* if(!stANL[aktANL].Error) */

		if(!stANL[aktANL].Error) {
			DOI_wert[aktANL].enable	= 1;
			switch(aktANL) {
				case 0:	DOI_wert[aktANL].pName	= (UDINT) "wert1"; break;
				case 1:	DOI_wert[aktANL].pName	= (UDINT) "wert2"; break;
				case 2:	DOI_wert[aktANL].pName	= (UDINT) "wert3"; break;
				case 3:	DOI_wert[aktANL].pName	= (UDINT) "wert4"; break;
				case 4:	DOI_wert[aktANL].pName	= (UDINT) "wert5"; break;
				case 5:	DOI_wert[aktANL].pName	= (UDINT) "wert6"; break;
			}

			DatObjInfo(&DOI_wert[aktANL]);

			if(DOI_wert[aktANL].status==0) {
				stANL[aktANL].dm.AdrDM_wert			= DOI_wert[aktANL].pDatObjMem;
				stANL[aktANL].dm.MaxWerteDM_wert	= DOI_wert[aktANL].len / 8;
			}
			else {
				stANL[aktANL].Error = INI_ERR_DO_WERT;
			}
		}

	} /* for(aktANL=0; aktANL<=maxANL; aktANL++) */

#endif


	/*-----------------------------------------------------------------------------------*/
	/* Struktur zur Verwaltung der Linien und Geräte mit Konfigurationsdaten beschreiben */
	/*-----------------------------------------------------------------------------------*/

	for(aktANL = 0; aktANL <= MAX_LINES-1; aktANL++) {
		stANL[aktANL].com.receive_timeout	= IEC_Config.TimeOut;	/* Timeout für Empfangstelegramm in ms */
		stANL[aktANL].com.Ident 			= 0;
		stANL[aktANL].com.OpenError			= 0;
		stANL[aktANL].com.WriteError		= 0;
		stANL[aktANL].com.WriteErrorCount	= 0;
		stANL[aktANL].com.FirstWriteError	= 0;
		stANL[aktANL].com.ReOpenCount		= 0;
		stANL[aktANL].ComStart				= 1;					/* Freigabe für Kommunikation */
		stANL[aktANL].DistData.Function		= NO_FUNCTION;
		stANL[aktANL].DistData.DataSend		= 0;
		stANL[aktANL].Command.Function		= NO_FUNCTION;
		stANL[aktANL].SNr					= 0;
	}


	/*----------------------------------------------------------------------*/
	/* initialize and open interface for line 1 to n						*/
	/*----------------------------------------------------------------------*/

	/* initialize config structure */

	for(aktANL=0; aktANL<=maxANL; aktANL++) {
		if(!stANL[aktANL].Error) {
			stComIni[aktANL].XOpenConfigStruct.idle		= 4;
			stComIni[aktANL].XOpenConfigStruct.delimc	= 0;
			stComIni[aktANL].XOpenConfigStruct.delim[0]	= 0;
			stComIni[aktANL].XOpenConfigStruct.delim[1]	= 0;
			stComIni[aktANL].XOpenConfigStruct.tx_cnt	= 2;
			stComIni[aktANL].XOpenConfigStruct.rx_cnt	= 2;
			stComIni[aktANL].XOpenConfigStruct.tx_len	= 256;
			stComIni[aktANL].XOpenConfigStruct.rx_len	= 256;
			stComIni[aktANL].XOpenConfigStruct.argc		= 0;
			stComIni[aktANL].XOpenConfigStruct.argv		= 0;

			/* initialize open structure */

			stComIni[aktANL].FrameXOpenStruct.device	= (UDINT) stComIni[aktANL].StringDevice;
			stComIni[aktANL].FrameXOpenStruct.mode		= (UDINT) stComIni[aktANL].StringMode;
			stComIni[aktANL].FrameXOpenStruct.config	= (UDINT) &stComIni[aktANL].XOpenConfigStruct;
			stComIni[aktANL].FrameXOpenStruct.enable	= 1;

			/* open interface */

			FRM_xopen(&stComIni[aktANL].FrameXOpenStruct); 						/* open an interface */
			if (stComIni[aktANL].FrameXOpenStruct.status != 0) {				/* check status */
				stANL[aktANL].com.OpenError = 1;
				stANL[aktANL].Error			= INI_ERR_DEVICE_OPEN;
				strcpy(MsgTxt,"Fehler beim Oeffnen von: ");
				strcat(MsgTxt,stComIni[aktANL].StringDevice);
				ErrStatistics("iec103", ERRLOG, stANL[aktANL].Error, stComIni[aktANL].FrameXOpenStruct.status, ERR_MSG_PRE_STRG, MsgTxt);
			}
			else {
				stANL[aktANL].com.Ident = stComIni[aktANL].FrameXOpenStruct.ident; 	/* get ident */
			}
		}
	}

	/*-------------------------------*/
	/* Buffer fuer Stoerschriebdaten */
	/*-------------------------------*/

	memset(&DataBuffer,0,sizeof(DataBuffer));

	if(IEC_Config.DistDataEnable) {								/* Freigabe fuer Stoerschriebuebertragung */

		/* Speicher fuer die Buffer allokieren */

		for(i=0;i<NR_OF_BUFFERS;i++) {
			DataBuffer.Info[i].Size = BUFFER_SIZE;
			DataBuffer.Info[i].status = TMP_alloc(DataBuffer.Info[i].Size, (void**)&DataBuffer.Info[i].DataAdr); 	/* allocate new memory from RAM*/
			if(DataBuffer.Info[i].status==0) {
				DataBuffer.NrOfBuffers++;
			}
			else {
				strcpy(MsgTxt,"Fehler TMP_alloc Stoerschrieb");
				ErrStatistics("iec103", ERRLOG, 0, DataBuffer.Info[i].status, ERR_MSG_PRE_STRG, MsgTxt);
			}
		}
	}

	/*-------------------------------*/
	/* Startschritt bestimmen        */
	/*-------------------------------*/

	for(aktANL=0; aktANL<=maxANL; aktANL++) {
		if(!stANL[aktANL].ComStart) {
			stANL[aktANL].SNr = 4;
		}
		else {
			if(stANL[aktANL].Error) {
				stANL[aktANL].SNr = 3;
			}
			else {
				stANL[aktANL].SNr = 0;
			}
		}
	}
}

/***********************************************************************/
/* 						zyklisches Hauptprogramm                       */
/***********************************************************************/

void _CYCLIC cyclic_iec(void)
{

	plcbit	found;
	UINT error,i,n;
	UINT LineIdx=0;
	UINT DeviceIdx=0;
	RTCtime_typ SysTime;
	DeviceActivType *pDeviceActiv;

	pDeviceActiv = &(stANL[0].DeviceActiv[0]);		/* nur um Compiler-Warnung zu verhindern */

	/* Einlesen der Echtzeituhr */

    error = RTC_gettime(&SysTime);
    if(error==0) {
      memcpy(&RPSTime,&SysTime,sizeof(RPSTime));
      RPSTimeOK = 1;
	}

	/* ID Meldeserver besorgen */

	get_idBrServer();

	/* Pruefen, ob Stoerschriebe angefordert bzw. geloeschet werden muessen */

	check_dist_data_transmission();

	/* Testfunktion für Onlinemonitor */

	if(DigTest.enable==1)
	{
		DigTest.enable	= 0;
		DigTest.ready	= 0;

		/* Liniennummer vorhanden ? */
		found = 0;
		for(n = 0; n <= maxANL; n++) {
			if(stANL[n].LinienNummer == DigTest.LinienNummer) {
				found	= 1;
				LineIdx	= n;
			}
		}

		/* Geraeteadresse vorhanden ? */
		if(found) {
			found = 0;
			for(n = 0; n < DEVICES_PRO_LINE; n++) {
				if(stANL[LineIdx].DeviceActiv[n].Adresse == DigTest.DeviceAdresse) {
					found		= 1;
					DeviceIdx	= n;
				}
			}
		}

		if(found) {
			save_digitalwert(	(DMInfoType *) &stANL[LineIdx].dm,
								stANL[LineIdx].LinienNummer,
								(DeviceActivType *) &stANL[LineIdx].DeviceActiv[DeviceIdx],
								DigTest.FunctionType,
								DigTest.InformationNumber,
								DigTest.Messwert,
								&RPSTime,
								stANL[LineIdx].DeviceActiv[0].set_event);						/* Event für Leitsystem */
			DigTest.ready	= 1;
		}
	}

	if(AnaTest.enable==1)
	{
		AnaTest.enable	= 0;
		AnaTest.ready	= 0;

		/* Liniennummer vorhanden ? */
		found = 0;
		for(n = 0; n <= maxANL; n++) {
			if(stANL[n].LinienNummer == AnaTest.LinienNummer) {
				found	= 1;
				LineIdx	= n;
			}
		}

		/* Geraeteadresse vorhanden ? */
		if(found) {
			found = 0;
			for(n = 0; n < DEVICES_PRO_LINE; n++) {
				if(stANL[LineIdx].DeviceActiv[n].Adresse == AnaTest.DeviceAdresse) {
					found		= 1;
					DeviceIdx	= n;
				}
			}
		}

		if(found) {
			save_analogwert	(	(DMInfoType *) &stANL[LineIdx].dm,
								stANL[LineIdx].LinienNummer,
								(DeviceActivType *) &stANL[LineIdx].DeviceActiv[DeviceIdx],
								AnaTest.FunctionType,
								AnaTest.InformationNumber,
								AnaTest.AnzahlMesswerte,
								(USINT *) &AnaTest.Messwerte,
     							stANL[LineIdx].DeviceActiv[0].set_event);						/* Event für Leitsystem */
			AnaTest.ready	= 1;
		}
	}

	if(GlobalTest.enable==1)
	{
		GlobalTest.enable = 0;
/*		set_global_event_station (idMessageServer, RPSTime, (UINT) GlobalTest.Adresse);*/
	}

	/* Uebergabe von Stoerschriebdaten aus einem Datenmodul an den Meldeserver */
	disturbance_simulation();


	iec_mem_diagnose();

	// Hilfsbuffer fuer IEC Diagnose
	if(IEC_Info_Clear) {
		dbg_buf_iec_clear();
		IEC_Info_Clear = 0;
	}

	/* Bis hierher nur Test */


	#ifdef DEBUG
		debug_open_device(DebugConfig.OpenDevice);
	#endif

	if ((Error == 0) && (myidBrServer!=0)) {

		for(aktANL=0; aktANL<=maxANL; aktANL++) {

				switch(stANL[aktANL].SNr) {
					case 0:		/* Normaler Schreib- Lesebetrieb */
						Zeitmessung(&stANL[aktANL]);
						Device_Reaktivieren(&stANL[aktANL]);
						disturbance_data_timeout(&stANL[aktANL]);
						command_timeout(&stANL[aktANL]);
						timesynch_timeout(&stANL[aktANL]);

						if(stANL[aktANL].com.write_read_activ == WRITE) {
							Data_write(&stANL[aktANL]);
						}
						else {
							Data_read(&stANL[aktANL]);
						}


						FrameReadStruct.enable = 1;
						FrameReadStruct.ident = stANL[aktANL].com.Ident;

						FRM_read(&FrameReadStruct); 								/* read data form interface */

						ReadBuffer 			= (UDINT*) FrameReadStruct.buffer;		/* get adress of read buffer */
						ReadBufferLength	= FrameReadStruct.buflng; 				/* get length of read buffer */
						StatusRead			= FrameReadStruct.status; 				/* get status */

						if (StatusRead == 0) {										/* check status */
							if((ReadBufferLength + stANL[aktANL].com.RBufferAdr)<=sizeof(stANL[aktANL].com.ReadData)) {
								memcpy(&stANL[aktANL].com.ReadData[stANL[aktANL].com.RBufferAdr], ReadBuffer, ReadBufferLength);/* copy read data into array */
						 		stANL[aktANL].com.RBufferAdr += ReadBufferLength;
							}

							if(stANL[aktANL].DeviceAtWork>0)
								pDeviceActiv = &(stANL[aktANL].DeviceActiv[stANL[aktANL].DeviceAtWork-1]);

							#ifdef DEBUG
								dbg_msg(READBUFFER,
										stANL[aktANL].com.RBufferAdr,
										0,
										stANL[aktANL].LinienNummer,
										pDeviceActiv->Adresse,
										0,
										0,
										0,
										stANL[aktANL].com.ReadData);
							#endif
																					/* initialize release buffer structure */
							FrameReleaseBufferStruct.enable = 1;
							FrameReleaseBufferStruct.ident = stANL[aktANL].com.Ident;
							FrameReleaseBufferStruct.buffer = (UDINT) ReadBuffer;
							FrameReleaseBufferStruct.buflng = ReadBufferLength;

							FRM_rbuf(&FrameReleaseBufferStruct); 						/* release read buffer */

							StatusReleaseBuffer = FrameReleaseBufferStruct.status; 		/* get status */

							if (StatusReleaseBuffer != 0)  								/* check status */
								ReadError = 1; 											/* set error level for FRM_rbuf */
						}
						if((stANL[aktANL].com.WriteErrorCount>3) && (IEC_Config.EnableComReset)) stANL[aktANL].SNr++;
						break;

					case 1:		/* Schnittstelle schliessen */
						/* initialize close structure */
						FrameCloseStruct.ident	= stANL[aktANL].com.Ident;
						FrameCloseStruct.enable = 1;
						FRM_close(&FrameCloseStruct); 					/* close interface */
						stANL[aktANL].SNr++;
						break;

					case 2:		/* Schnittstelle erneut oeffnen */
						FRM_xopen(&stComIni[aktANL].FrameXOpenStruct); 						/* open an interface */
						if (stComIni[aktANL].FrameXOpenStruct.status != 0) {				/* check status */
							stANL[aktANL].com.OpenError = 1;
							stANL[aktANL].Error			= INI_ERR_DEVICE_OPEN;
							stANL[aktANL].SNr			= 3;
						}
						else {
							stANL[aktANL].com.Ident				= stComIni[aktANL].FrameXOpenStruct.ident; 	/* get ident */
							stANL[aktANL].SNr 					= 0;
							stANL[aktANL].com.LastWriteError	= stANL[aktANL].com.WriteError;
							stANL[aktANL].com.WriteErrorCount	= 0;
							stANL[aktANL].com.WriteError		= 0;
						}

						stANL[aktANL].com.ReOpenCount++;
						break;

					/* Fehlermeldung aller Geräte: Verbindungsstoerung */
					case 3:
						if(myidBrServer!=0) {
							for(i=0;i<DEVICES_PRO_LINE;i++) {
								if(stANL[aktANL].DeviceTAB[i].Aktiv==0) continue;

								save_kopplungsstoerung( &(stANL[aktANL].dm),								/* Adresse Datenmodul-Infostruktur */
														stANL[aktANL].LinienNummer,							/* Liniennummer */
														(DeviceActivType *) &stANL[aktANL].DeviceActiv[i],
											 			2,													/* Messwert	(Doppelmeldung 1=aus 2=ein) */
							 							&RPSTime,											/* Zeitstempel 		 */
											 			TRUE);												/* Event für Leitsystem */
							}
							stANL[aktANL].SNr = 4;
						}
						break;

					/* Schritt 4: Keine Funktion */
					case 4:
						break;

				} /* switch */
		} /* for () */
	} /* if(Error==0) */
}


/*** eof *****************************************************************************/


