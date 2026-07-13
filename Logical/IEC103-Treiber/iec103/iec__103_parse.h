/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Parserfunktionen
*  Datei:    iec__103_parse.h
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 02.05.1
*  Datum:    02-09-2008
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion: Headerdatei
*------------------------------------------------------------------------------
*  Rev.   | Datum    | Name | Kommentar
*  -------+----------+------+---------------------------------------------------
*  01.00  | 26-11-03 | KOB  | Datei-Erstellung
*         |          |      |
*  02.05.1| 02-09-08 | KOB  | Neue Modi für Doppelmeldungen:
*							| DOPPEL_EINZEL_NORMAL_NOINVALID und DOPPEL_EINZEL_INVERS_NOINVALID
*  02.07.2| 05-08-11 | WAI  | Neue grenzen für RAW_MIN,MAX wegen AUDI N60
*
******************************************************************************/
/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/
#define ERR_MSG_PRE_STRG				"iec"
#define MAX_ERROR_MSG_LOG				20				/* Anzahl Fehler, die in das Log-Buch eingetragen werden */

/* Bezeichnung der Datenmodule */
#define	DATA_OBJEKT_DPLIST				"dplist"
#define	DATA_OBJEKT_TYPLIST				"typlist"

/*-------------------------------------------------------
	Basisdatentypen
---------------------------------------------------------*/

#define BOOL_TYP						"_BOOL"
#define BOOL_IDX						0
#define USINT_TYP						"_USINT"
#define USINT_IDX						1
#define SINT_TYP						"_SINT"
#define SINT_IDX						2
#define UINT_TYP						"_UINT"
#define UINT_IDX						3
#define INT_TYP							"_INT"
#define INT_IDX							4
#define UDINT_TYP						"_UDINT"
#define UDINT_IDX						5
#define DINT_TYP						"_DINT"
#define DINT_IDX						6
#define STREAM_TYP						"_STREAM"
#define STREAM_IDX						7

/*-------------------------------------------------------
	Fehlermeldungen
---------------------------------------------------------*/
#define NO_ERROR						0


/* Fehlermeldungen: Datenmodul nicht gefunden */
#define PARS_ERR_DPLIST_NOT_FOUND		501				/* Fehler: Datenmodul dplist nicht gefunden */
#define PARS_TXT_DPLIST_NOT_FOUND		"datamodule dplist not found"

#define PARS_ERR_TYPLIST_NOT_FOUND		502				/* Fehler: Datenmodul typlist nicht gefunden */
#define PARS_TXT_TYPLIST_NOT_FOUND		"datamodule typlist not found"

#define PARS_ERR_MEM_ALLOC				503				/* Fehler: Speicher kann nicht allokiert werden */
#define PARS_TXT_MEM_ALLOC				"TMP_alloc not successfull"

#define PARS_ERR_TO_MUCH_DEVICES		504				/* Fehler: Zuviele Geräte pro Linie angemeldet */
#define PARS_TXT_TO_MUCH_DEVICES		"to much devices for one line"

/* Fehlermeldungen des globalen Teiles */
#define PARS_ERR_UNKNOWN_GLOBAL_ENTRY	505				/* Fehler: Unbekannter Eintrag in globalen Bereich */
#define PARS_ERR_LINE_UNKNOWN			506				/* Fehler: Schnittstellendefinition kann keiner Linie zugeordnet werden */
#define PARS_ERR_TO_MUCH_LINES			507				/* Fehler: Anzahl deklarierter Linien zu gross (max. 6) */
#define PARS_ERR_NR_OF_LINES_ZERO		508				/* Fehler: Keine Linieneintraege im globalen Teil gefunden */
#define PARS_ERR_NR_OF_DP_ZERO			509				/* Fehler: Anzahl Datenpunkte einer Linie ist Null */
#define PARS_ERR_LINE_IF_NOT_DEFINED	510				/* Fehler: Einer Linie wurde keine Schnittstellenbeschreibung zugeordnet */
#define PARS_ERR_LINE_MODE_NOT_DEFINED	511				/* Fehler: Einer Linie wurde kein Schnittstellenmodus zugeordnet */
#define PARS_ERR_TIMEOUT_NOT_VALID		512				/* Fehler: Wert fuer Timeout ungueltig */
#define PARS_ERR_RETRY_ANZ_NOT_VALID	513				/* Fehler: Wert fuer Wiederholungen bei Telegrammfehlern ungueltig */
#define PARS_ERR_CONN_ANZ_NOT_VALID		514				/* Fehler: Wert fuer Anzahl Anmeldeversuche ungueltig */
#define PARS_ERR_WAIT_INTV_NOT_VALID	515				/* Fehler: Wert fuer Wartezeit für Anmeldeversuch ungueltig */
#define PARS_ERR_TIME_TS_NOT_VALID		516				/* Fehler: Wert fuer Zeitsyncronisation ungueltig */
#define PARS_ERR_IF_RESET_NOT_VALID		517				/* Fehler: Wert fuer Schnittstellen-Reset ungueltig */
#define PARS_ERR_TIME_CLASS2_NOT_VALID	518				/* Fehler: Wert fuer Abtastzeit Class 2 Telegramme ungueltig */
#define PARS_ERR_TIME_CLASS2F_NOT_VALID	519				/* Fehler: Wert fuer Hohe Abtastzeit Class 2 Telegramme ungueltig */
#define PARS_ERR_DD_ENABLE_NOT_VALID	520				/* Fehler: Wert fuer Freigabe Störschriebübertragung ungueltig */
#define PARS_ERR_DD_TIMEOUT_NOT_VALID	521				/* Fehler: Wert fuer Timeout Störschriebübertragung ungueltig */
#define PARS_ERR_DD_DELAY_NOT_VALID		522				/* Fehler: Wert fuer Verzögerung Störschriebübertragung ungueltig */
#define PARS_ERR_CMD_ENABLE_NOT_VALID	523				/* Fehler: Wert fuer Freigabe Kommandübertragung ungueltig */
#define PARS_ERR_CMD_TIMEOUT_NOT_VALID	524				/* Fehler: Wert fuer Timeout Kommandoübertragung ungueltig */
#define PARS_ERR_TIME_TIMEOUT_NOT_VALID	525				/* Fehler: Wert fuer Timeout Zeitsynchronisation */

/* Fehlermeldungen des Datenpunktbereiches */
#define PARS_ERR_UNKNOWN_DP_ENTRY		600				/* Fehler: Unbekannter Eintrag in Datenpunkt Bereich */
#define PARS_ERR_ANLNR_NOT_DEFINED		601				/* Fehler: Anlagennummer nicht definiert */
#define PARS_ERR_DPNR_NOT_DEFINED		602				/* Fehler: Datenpunktnummer nicht definiert */
#define PARS_ERR_TYP_NOT_DEFINED		603				/* Fehler: Datentyp nicht definiert */
#define PARS_ERR_LINE_NOT_DEFINED		604				/* Fehler: Liniennummer nicht definiert */
#define PARS_ERR_LINE_NOT_VALID			605				/* Fehler: Die zu einem Datenpunkt angegebene Linien Nr. wurde im globalen Teil nicht deklariert */
#define PARS_ERR_DEV_ADR_NOT_DEFINED	606				/* Fehler: Geräteadresse wurde nicht definiert */
#define PARS_ERR_DEV_ADR_NOT_VALID		607				/* Fehler: Geräteadresse ausserhalb des gültigen Bereiches */
#define PARS_ERR_TSYNCH_NOT_VALID		608				/* Fehler: Wert für TSynch ausserhalb des gültigen Bereiches */
#define PARS_ERR_DPTYP_NR_NOT_DEFINED	609				/* Fehler: Datentyp Nummer nicht definiert */
#define PARS_ERR_ITEM_NR_NOT_DEFINED	610				/* Fehler: Item Nummer nicht definiert */
#define PARS_ERR_ITEM_TYP_NOT_DEFINED	611				/* Fehler: Item Typ nicht definiert */
#define PARS_ERR_ITEM_TYP_UNKNOWN		612				/* Fehler: Item Typ unbekannt */
#define PARS_ERR_ITEM_TYP_NOT_VALID		613				/* Fehler: Item Typ nicht gueltig (Analogwert->BOOL oder Doppelmeldung->BOOL) */
#define PARS_ERR_IEC_TYPE_NOT_DEFINED	614				/* Fehler: IEC Type nicht definiert */
#define PARS_ERR_IEC_TYPE_NOT_VALID		615				/* Fehler: IEC Type ausserhalb des gültigen Bereiches */
#define PARS_ERR_IEC_INFO_NOT_DEFINED	616				/* Fehler: IEC Info nicht definiert */
#define PARS_ERR_IEC_INFO_NOT_VALID		617				/* Fehler: IEC Info ausserhalb des gültigen Bereiches */
#define PARS_ERR_IEC_ANALOGNR_NOT_VALID	618				/* Fehler: IEC Analogwertnummer ausserhalb des gültigen Bereiches */
#define PARS_ERR_RAWMIN_NOT_DEFINED		619				/* Fehler: Raw-Minwert nicht definiert */
#define PARS_ERR_RAWMIN_NOT_VALID		620				/* Fehler: Raw-Minwert nicht gültig */
#define PARS_ERR_RAWMAX_NOT_DEFINED		621				/* Fehler: Raw-Maxwert nicht definiert */
#define PARS_ERR_RAWMAX_NOT_VALID		622				/* Fehler: Raw-Maxwert nicht gültig */
#define PARS_ERR_NORMMIN_NOT_DEFINED	623				/* Fehler: Norm-Minwert nicht definiert */
#define PARS_ERR_NORMMIN_NOT_VALID		624				/* Fehler: Norm-Minwert nicht gültig */
#define PARS_ERR_NORMMAX_NOT_DEFINED	625				/* Fehler: Norm-Maxwert nicht definiert */
#define PARS_ERR_NORMMAX_NOT_VALID		626				/* Fehler: Norm-Maxwert nicht gültig */
#define PARS_ERR_MRUC_NOT_VALID			627				/* Fehler: Funktion MRuc nicht gültig */
#define PARS_ERR_MRUC_INFO_NOT_DEFINED	628				/* Fehler: MRuc.Info nicht definiert */
#define PARS_ERR_MRUC_INFO_NOT_VALID	629				/* Fehler: MRuc.Info nicht gültig */
#define PARS_ERR_MRUC_TYPE_NOT_DEFINED	630				/* Fehler: MRuc.Type nicht definiert */
#define PARS_ERR_MRUC_TYPE_NOT_VALID	631				/* Fehler: MRuc.Type nicht gültig */
#define PARS_ERR_MODE_NOT_VALID			632				/* Fehler: Mode nicht gültig */

/* Fehlermeldungen des Itembereiches */
#define PARS_ERR_UNKNOWN_ITEM_ENTRY		700				/* Fehler: Unbekannter Eintrag in Item Bereich (DM typlist)*/

/*-------------------------------------------------------
	Schlüsselworte
---------------------------------------------------------*/
/* Hilfskonstanten */

#define ALL_SECTION						0				/* Es wird im allgemeinen Teil geparst (Meldungsunterdrückung) */
#define IEC_SECTION						1				/* Es wird im IEC - Teil geparst (Meldung bei unbekannten Schlüsselworten) */
#define DP_PART							0				/* Es wird im allgemeinen Datenpunktbereich geparst */
#define ITEM_PART						1				/* Es wird im allgemeinen Itembereich geparst */

/* Schluesselworte Sectionen */
#define	KW_IEC_SECTION					"IEC"			/* Schlüsselwort für IEC-Section */
#define	KW_ALL_SECTION					"ALL"			/* Schlüsselwort für Allgemeine-Section */
#define	KW_BRS_SECTION					"BRS"			/* Schlüsselwort für BRS-Section */
#define KW_ITEM_SECTION					"ITEM"			/* Schlüsselwort für ITEM-Section */
#define KW_BLOCK_SECTION				"BLOCK"			/* Schlüsswlwort für Blockdefinition */

/* Schluesselworte  globaler Teil */
#define KW_IEC_LINE						"LINE"			/* Schlüsselwort für Linie */
#define KW_IEC_LINE_IF					"IF"			/* Schlüsselwort für Interface */
#define KW_IEC_IF_RESET					"RESET"			/* Schlüsselwort für Freigabe IF-Reset (IF.RESET) */
#define KW_IEC_LINE_MODE				"MODE"			/* Schlüsselwort für Interface Mode */
#define KW_IEC_TIMEOUT					"TIMEOUT"		/* Schlüsselwort für Überwachungszeit Telegramempfang (TIMEOUT) */
#define KW_IEC_RETRY					"RETRY"			/* Schlüsselwort für Wiederholungen bei Telegrammfehlern */
#define KW_IEC_RETRY_ANZ				"ANZ"			/* Schlüsselwort für Wiederholungen bei Telegrammfehlern (RETRY.ANZ)*/
#define KW_IEC_CONN						"CONN"			/* Schlüsselwort für Anzahl Anmeldeversuche */
#define KW_IEC_CONN_ANZ					"ANZ"			/* Schlüsselwort für Anzahl Anmeldeversuche (CONN.ANZ) */
#define KW_IEC_WAIT						"WAIT"			/* Schlüsselwort für Wartezeit für Anmeldeversuch*/
#define KW_IEC_WAIT_INTV				"INTV"			/* Schlüsselwort für Wartezeit für Anmeldeversuch (WAIT.INTV) */
#define KW_IEC_TIME						"TIME"			/* Schlüsselwort für Zeitsyncronisation */
#define KW_IEC_TIME_TS					"Ts"			/* Schlüsselwort für Zeitsyncronisation (TIME.Ts) */
#define KW_IEC_TIME_TOUT				"TIMEOUT"		/* Schlüsselwort für Timeout Rueckmeldung Zeitsyncronisation (TIME.TIMEOUT) */
#define	KW_IEC_TIME_CLASS2				"CLASS2"		/* Schlüsselwort für Abtastzeit Class 2 Telegramme (TIME.CLASS2) */
#define	KW_IEC_TIME_CLASS2F				"CLASS2F"		/* Schlüsselwort für Abtastzeit Class 2 Telegramme schnell (TIME.CLASS2F) */
#define	KW_IEC_DD						"DD"			/* Schlüsselwort für Störschiebparameter (Disturbance Data) */
#define	KW_IEC_DD_ENABLE				"ENABLE"		/* Schlüsselwort für Freigabe für Störschriebübertragung (DD.ENABLE) */
#define	KW_IEC_DD_TIMEOUT				"TIMEOUT"		/* Schlüsselwort für Timeout bei Störschriebübertragung (DD.TIMEOUT) */
#define	KW_IEC_DD_DELAY					"DELAY"			/* Schlüsselwort für Verzögerung bei Störschriebübertragung des gleichen Gerätes (DD.DELAY) */
#define	KW_IEC_CMD						"CMD"			/* Schlüsselwort für Kommando-Parameter */
#define	KW_IEC_CMD_ENABLE				"ENABLE"		/* Schlüsselwort für Freigabe für Kommandoübertragung (CMD.ENABLE) */
#define KW_IEC_CMD_TIMEOUT				"TIMEOUT"		/* Schlüsswleort für Timeout bei Kommandoübertragung  (CMD.TIMEOUT) */


/* Schluesselworte  Datenpunkt Teil */
#define KW_DP_ALL_ANL					"ANLAGE"		/* Schlüsselwort für Anlagennummer */
#define KW_DP_ALL_ANL_NR				"NR"			/* Schlüsselwort für Anlagennummer (ANLAGEN.NR) */
#define KW_DP_ALL_DP					"DP"			/* Schlüsselwort für Datenpunktnummer */
#define KW_DP_ALL_DP_NR					"NR"			/* Schlüsselwort für Datenpunktnummer (DP.NR) */
#define KW_DP_ALL_TYP					"TYP"			/* Schlüsselwort für Datenpunkttyp */
#define KW_DP_ALL_ITEM					"ITEM"			/* Schlüsselwort Item Datentyp */
#define KW_DP_ALL_ITEM_TYP				"TYP"			/* Schlüsselwort Item-Datentyp ITEM.TYP */
#define KW_DP_ALL_ITEM_NR				"NR"			/* Schlüsselwort Item-Nummer (ITEM.NR) */

#define	KW_DP_IEC_DEV					"DEV"			/* Schlüsselwort für Device Adresse */
#define KW_DP_IEC_DEV_ADR				"ADR"			/* Schlüsselwort für Device Adresse (DEV.ADR) */
#define KW_DP_IEC_TSYNCH				"TSYNC"			/* Schlüsselwort für Zeitsynchronisation ein/aus */


/* Schlüsselworte TYPLIST	ALL-Teil Datenpunkt */
#define	KW_DPTYP_ALL_TYP				"TYP"			/* Schlüsselwort Datentyp Nummer */
#define KW_DPTYP_ALL_TYP_NR				"NR"			/* Schlüsselwort Datentyp Nummer (TYP.NR) */

/* Schlüsselworte TYPLIST	ALL-Teil ITEM */
#define KW_ITEM_ALL_ITEM_TYP			"TYP"			/* Schlüsselwort Item Datentyp */
#define KW_ITEM_ALL_ITEM				"ITEM"			/* Schlüsselwort Item-Nummer */
#define KW_ITEM_ALL_ITEM_NR				"NR"			/* Schlüsselwort Item-Nummer (ITEM.NR) */

/* Schlüsselworte TYPLIST	IEC-Teil */
#define KW_ITEM_IEC_TYPE				"TYPE"			/* Schlüsselwort Type (IEC-Type) */
#define KW_ITEM_IEC_INFO				"INFO"			/* Schlüsselwort Info (IEC-Info) */
#define KW_ITEM_IEC_INFO_NR				"NR"			/* Schlüsselwort Info (IEC-Info) (INFO.NR) */
#define KW_ITEM_IEC_ANALOG				"ANALOG"		/* Schlüsselwort Analogwert-Nr.  */
#define KW_ITEM_IEC_ANALOG_NR			"NR"			/* Schlüsselwort Analogwert-Nr. (ANALOG.NR) */
#define KW_ITEM_IEC_RAW					"RAW"			/* Schlüsselwort Rohwert*/
#define KW_ITEM_IEC_RAW_MIN				"MIN"			/* Schlüsselwort Rohwert Min (RAW.MIN) */
#define KW_ITEM_IEC_RAW_MAX				"MAX"			/* Schlüsselwort Rohwert Max (RAW.MAX) */
#define KW_ITEM_IEC_NORM				"NORM"			/* Schlüsselwort Normierter Wert */
#define KW_ITEM_IEC_NORM_MIN			"MIN"			/* Schlüsselwort Normierter Wert Min (NORM.MIN) */
#define KW_ITEM_IEC_NORM_MAX			"MAX"			/* Schlüsselwort Normierter Wert Max (NORM.MAX) */
#define KW_ITEM_IEC_SEC					"SEC"			/* Schlüsselwort Zweiter Datenpunkt */
#define KW_ITEM_IEC_SEC_DP				"DP"			/* Schlüsselwort Zweiter Datenpunkt (SEC.DP)*/
#define KW_ITEM_IEC_SEC_NR				"NR"			/* Schlüsselwort Nummer des zweiten Datenpunktes (SEC.NR) */
#define KW_ITEM_IEC_MRUC				"MRUC"			/* Schlüsselwort Meldung über LED-RST löschen (nein/ja) */
#define KW_ITEM_IEC_MRUC_INFO			"INFO"			/* Schlüsselwort Info für LED RST */
#define KW_ITEM_IEC_MRUC_TYPE			"TYPE"			/* Schlüsselwort Type für LED RST */
#define KW_ITEM_IEC_MODE				"MODE"			/* Schlüsselwort Modus Digitalmeldungen */
#define KW_ITEM_IEC_ERROR				"ERR"			/* Schlüsswlwort Fehlermeldung */
#define KW_ITEM_IEC_KOPPLUNG			"KOPPLUNG"		/* Schlüsselwort Auswertung Kopplungsstoerung (ERR.KOPPLUNG) */
#define KW_ITEM_IEC_INVALID				"INVALID"		/* Schlüsselwort Auswertung Invalid-Bit Analogwerte (ERR.INVALID) */
#define KW_ITEM_IEC_OVERFLOW			"OVERFLOW"		/* Schlüsselwort Auswertung Overflow-Bit Analogwerte (ERR.OVERFLOW) */
#define KW_ITEM_IEC_TIMESYNCH			"TIMESYNC"		/* Schlüsselwort Auswertung Fehler Zeitsynchronisation (ERR.TIMESYNC) */


/*-------------------------------------------------------
	Grenzwerte
---------------------------------------------------------*/

/* Grenzwerte der globalen Parameter */

#define IF_RESET_MIN					0				/* Minimaler Wert für Freigabe IF Reset (IF.RESET) */
#define IF_RESET_MAX					1				/* Maximaler Wert für Freigabe IF Reset (IF.RESET) */
#define TIMEOUT_MIN						10				/* Minimaler Wert fuer Timeout (TIMEOUT) (in ms) */
#define TIMEOUT_MAX						10000			/* Maximaler Wert fuer Timeout (TIMEOUT) (in ms) */
#define RETRY_ANZ_MIN					0				/* Minimaler Wert fuer Wiederholungen bei Telegrammfehlern (RETRY.ANZ) */
#define RETRY_ANZ_MAX					1000			/* Maximaler Wert fuer Wiederholungen bei Telegrammfehlern (RETRY.ANZ) */
#define CONN_ANZ_MIN					0				/* Minimaler Wert fuer Anzahl Anmeldeversuche (CONN.ANZ) */
#define CONN_ANZ_MAX					1000			/* Maximaler Wert fuer Anzahl Anmeldeversuche (CONN.ANZ) */
#define WAIT_INTV_MIN					0				/* Minimaler Wert fuer Wartezeit für Anmeldeversuch (WAIT.INTV) (in ms) */
#define WAIT_INTV_MAX					1000000			/* Maximaler Wert fuer Wartezeit für Anmeldeversuch (WAIT.INTV) (in ms) */
#define TIME_TS_MIN						0				/* Minimaler Wert fuer Zeitsyncronisation (TIME.TS) (in ms) */
#define TIME_TS_MAX						10000000		/* Maximaler Wert fuer Zeitsyncronisation (TIME.TS) (in ms) */
#define TIME_TIMEOUT_MIN				1000			/* Minimaler Wert für Timeout Zeitsynchronisation (TIME.TIMEOUT) */
#define TIME_TIMEOUT_MAX				500000			/* Maximaler Wert für Timeout Zeitsynchronisation (TIME.TIMEOUT) */
#define TIME_CLASS2_MIN					10				/* Minimaler Wert für Abtastzeit Class2 Telegramme (TIME.CLASS2) */
#define TIME_CLASS2_MAX					10000			/* Maximaler Wert für Abtastzeit Class2 Telegramme (TIME.CLASS2) */
#define TIME_CLASS2F_MIN				10				/* Minimaler Wert für hohe Abtastzeit Class2 Telegramme (TIME.CLASS2F) */
#define TIME_CLASS2F_MAX				10000			/* Maximaler Wert für hohe Abtastzeit Class2 Telegramme (TIME.CLASS2F) */
#define DD_ENABLE_MIN					0				/* Minimaler Wert für Freigabe der Störschriebübertragung (DD.ENABLE) */
#define DD_ENABLE_MAX					1				/* Maximaler Wert für Freigabe der Störschriebübertragung (DD.ENABLE) */
#define DD_TIMEOUT_MIN					1000			/* Minimaler Wert für Timeout bei Störschriebübertragung (DD.TIMEOUT) */
#define DD_TIMEOUT_MAX					500000			/* Maximaler Wert für hohe Abtastzeit Class2 Telegramme (DD.TIMEOUT) */
#define DD_DELAY_MIN					1000			/* Minimaler Wert für Verzögerung der Störschriebübertragung des gleichen Gerätes (DD.DELAY) */
#define DD_DELAY_MAX					500000			/* Maximaler Wert für Verzögerung der Störschriebübertragung des gleichen Gerätes (DD.DELAY) */
#define CMD_ENABLE_MIN					0				/* Minimaler Wert für Freigabe Kommandoübertragung (CMD.ENABLE) */
#define CMD_ENABLE_MAX					1				/* Maximaler Wert für Freigabe Kommandoübertragung (CMD.ENABLE) */
#define CMD_TIMEOUT_MIN					1000			/* Minimaler Wert für Timeout bei Kommandoübertragung (CMD.TIMEOUT) */
#define CMD_TIMEOUT_MAX					500000			/* Maximaler Wert für Timeout bei Kommandoübertragung (CMD.TIMEOUT) */

/* Grenzwerte der Datenpunkt Parameter */

#define DEV_ADR_MIN						1				/* Minimaler Wert fuer Device Adresse */
#define DEV_ADR_MAX						254				/* Maximaler Wert fuer Device Adresse */
#define IEC_TYPE_MIN					0				/* Minimaler Wert fuer IEC-Type */
#define IEC_TYPE_MAX					255				/* Maximaler Wert fuer IEC-Type */
#define IEC_INFO_MIN					0				/* Minimaler Wert fuer IEC-Info */
#define IEC_INFO_MAX					255				/* Maximaler Wert fuer IEC-Info */
#define IEC_ANALOGNR_MIN				1				/* Minimaler Wert fuer IEC-Analogwert Nr. */
#define IEC_ANALOGNR_MAX				MAX_ANALOGWERTE	/* Maximaler Wert fuer IEC-Analogwert Nr. */
/*#define RAW_MIN							-4096*/			/* Minimaler Wert fuer Normierung Rohwert */
/*#define RAW_MAX							4095*/			/* Maximaler Wert fuer Normierung Rohwert */
/* 05.08.11 Wai. Grenzen RAW angepasst wegen Normierung Zählwerte */
#define RAW_MIN							-2000000			/* Minimaler Wert fuer Normierung Rohwert */
#define RAW_MAX							2000000			/* Maximaler Wert fuer Normierung Rohwert */

#define NORM_MIN						-2000000000		/* Minimaler Wert fuer Normierung Normierungswert */
#define NORM_MAX						2000000000		/* Maximaler Wert fuer Normierung Normierungswert */
#define TSYNCH_MIN						0				/* Minimaler Wert fuer Zeitsynchronisation ein/aus */
#define TSYNCH_MAX						1				/* Minimaler Wert fuer Zeitsynchronisation ein/aus */
#define MRUC_MIN						0				/* Minimaler Wert fuer Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_MAX						1				/* Minimaler Wert fuer Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_TYPE_MIN			IEC_TYPE_MIN			/* Minimaler Wert fuer Type.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_TYPE_MAX			IEC_TYPE_MAX			/* Minimaler Wert fuer Type.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_INFO_MIN			IEC_INFO_MIN			/* Minimaler Wert fuer Info.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_INFO_MAX			IEC_INFO_MAX			/* Minimaler Wert fuer Info.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MODE_MIN						0				/* Minimaler Wert fuer Mode */
#define MODE_MAX						15				/* Maximaler Wert fuer Mode */

/***********************************************************************/
/* Modus fuer Digitalmeldungen                                         */
/***********************************************************************/
#define	EINZEL_NORMAL					0
#define EINZEL_INVERS					1
#define EINZEL_DOPPEL_NORMAL			2
#define EINZEL_DOPPEL_INVERS			3
#define DOPPEL_NORMAL					10
#define DOPPEL_INVERS					11
#define DOPPEL_EINZEL_NORMAL			12
#define DOPPEL_EINZEL_INVERS			13

/* Beginn 02.05.1| 02-09-08 | KOB */
#define DOPPEL_EINZEL_NORMAL_NOINVALID  14				/* 0>0, 1>0, 2>1, 3>0, default>0 */
#define DOPPEL_EINZEL_INVERS_NOINVALID  15				/* 0>0, 1>1, 2>0, 3>0, default>0 */
/* Ende   02.05.1| 02-09-08 | KOB */

/***********************************************************************/
/* Defaultwerte                                                        */
/***********************************************************************/


#define	DEFAULT_MODE					DOPPEL_EINZEL_NORMAL	/* Defaultwert fuer Modus digitaler Signale: 12 (Doppelmeldung auf Einzelmeldung abbilden) */

/***********************************************************************/
/* Datentypdeklaration                                                 */
/***********************************************************************/
typedef struct
{
	plcbit					defined;
	long					value;
} IEC_ParserParInfo_typ;

typedef struct
{
	plcbit					defined;
	unsigned char			*name;
	unsigned short			size;
} IEC_ParserTypInfo_typ;


/* --------------------------*/
/* Globale Parameter         */
/* --------------------------*/

typedef struct
{
	IEC_ParserTypInfo_typ	Interface;
	IEC_ParserTypInfo_typ	InterfaceMode;
	IEC_ParserParInfo_typ	LineNr;
} IEC_GlobalLineData_typ;

typedef struct
{
	IEC_GlobalLineData_typ	Line[MAX_LINES];
	IEC_ParserParInfo_typ	TimeOut;
	IEC_ParserParInfo_typ	RetryAnz;
	IEC_ParserParInfo_typ	ConnAnz;
	IEC_ParserParInfo_typ	WaitIntv;
	IEC_ParserParInfo_typ	TimeTS;
	IEC_ParserParInfo_typ	EnableComReset;
	IEC_ParserParInfo_typ	TimeClass2Poll;
	IEC_ParserParInfo_typ	TimeClass2PollFast;
	IEC_ParserParInfo_typ	DistDataEnable;
	IEC_ParserParInfo_typ	DistDataTimeoutMax;
	IEC_ParserParInfo_typ	DisturbanceDelay;
	IEC_ParserParInfo_typ	CommandEnable;
	IEC_ParserParInfo_typ	CommandTimeoutMax;
	IEC_ParserParInfo_typ	TimeOutTimeSynch;
} IEC_GlobalData_typ;

/* ----------------------------*/
/* Parameter Datenpunkt / Item */
/* ----------------------------*/

typedef struct
{
	IEC_ParserParInfo_typ	AnlagenNr;
	IEC_ParserParInfo_typ	DPNr;
	IEC_ParserTypInfo_typ	Typ;
	IEC_ParserParInfo_typ	DeviceAdr;
	IEC_ParserParInfo_typ	Line;
	IEC_ParserParInfo_typ	TSynch;
	IEC_ParserParInfo_typ	DPTypNr;
	IEC_ParserParInfo_typ	ItemNr;
	IEC_ParserTypInfo_typ	ItemTyp;
	IEC_ParserParInfo_typ	ItemTypNr;
	IEC_ParserParInfo_typ	IECType;
	IEC_ParserParInfo_typ	IECInfoNr;
	IEC_ParserParInfo_typ	IECAnalogNr;
	IEC_ParserParInfo_typ	RawMin;
	IEC_ParserParInfo_typ	RawMax;
	IEC_ParserParInfo_typ	NormMin;
	IEC_ParserParInfo_typ	NormMax;
	IEC_ParserParInfo_typ	MRuc;
	IEC_ParserParInfo_typ	MRucType;
	IEC_ParserParInfo_typ	MRucInfo;
	IEC_ParserParInfo_typ	Mode;
	IEC_ParserParInfo_typ	Invalid;
	IEC_ParserParInfo_typ	Overflow;
	IEC_ParserParInfo_typ	KopplungsStoerung;
	IEC_ParserParInfo_typ	TimeSynchError;
} IEC_ItemData_typ;


/* -----------------------------*/
/* Infos des Parsers			*/
/* -----------------------------*/

/* Info einer Linie */
typedef struct
{
	unsigned short			NrOfDataPoints;
	unsigned short			NrOfItems;
	unsigned short			ActItemIdx;
	unsigned short			error;
	unsigned long			memAdr;
	unsigned long			memSize;
} IEC_ParserLineInfo_typ;

/* Infos des Parsers */
typedef struct
{
	unsigned short			NrOfLines;
	unsigned short			LineIdx;
	long					LineNrAct;
	plcbit					LineNrValid;
	IEC_ParserLineInfo_typ	Line[MAX_LINES];
	unsigned short			NrOfNormValues;
	unsigned short			ActNormIdx;
	long					memAdr;
	long					memSize;
	unsigned short			allocStatus;
	plcbit					ErrorMsg;
	unsigned short			NrOfLogEntrys;
	unsigned short			error;
	unsigned short			warning;
} IEC_ParserInfo_typ;

/* Hilfsvariable des Parsers */
typedef struct {
	unsigned char			*pParser;
	unsigned char			*pParserLine;
	unsigned char			*pParserErr;
	unsigned char			*pDataArg1;
	unsigned char			*pDataArg2;
	unsigned char			*pDataArg3;
	unsigned short			SizeArg1;
	unsigned short			SizeArg2;
	unsigned short			SizeArg3;
	unsigned short			SectionType;
	unsigned short			CountLines;
} Parser_typ;


/* Variable fuer Diagnosezwecke */
typedef struct {
	plcbit					showData;
	plcbit					DataValid;
	unsigned short			NormierungIdx;
	IEC_Normierung_typ 		*pNormData;
} IEC_NormDiagnose_typ;

typedef struct {
	plcbit					showData;
	plcbit					DataValid;
	unsigned short			LineNr;
	unsigned short			DeviceAdr;
	unsigned short			ItemNr;
	IEC_DP_Info_typ 		*pItemData;
	unsigned short			LineIdx;
	unsigned short			ItemIdx;
} IEC_ItemDiagnose_typ;


/***********************************************************************/
/* Prototypen		                                                   */
/***********************************************************************/
void			iec_parser();
void			iec_parser_dplist_clear();
void			iec_parser_typlist_clear();
void			iec_parser_global();
void			iec_parser_dpinfo(plcbit *pBlockFound, plcbit *pDPFound);
unsigned short	get_line_idx(IEC_ItemData_typ *pDPInfo, IEC_GlobalData_typ *pGlobalData, unsigned short *LineIdx);
unsigned short	get_device_idx(unsigned short LineIdx, long DeviceAdr, unsigned short *DeviceIdx, plcbit *NewDevice);
plcbit			normierung_defined(IEC_ItemData_typ *pItemData);
plcbit			iec_parser_dpinfo_ok(IEC_ItemData_typ *pData);
plcbit			iec_parser_dptype(unsigned char *pName, unsigned short size);
plcbit			iec_parser_iteminfo();
plcbit			iec_parser_iteminfo_ok(IEC_ItemData_typ *pData);
void			iec_parser_data(unsigned char *pSrcStart,unsigned char *pSrcEnd, Parser_typ *pPa, IEC_ItemData_typ *pData, unsigned char SektorInfo, unsigned char PartInfo);
void			copyItemData(IEC_ItemData_typ *pItemDest, IEC_ItemData_typ *pItemSrc);
unsigned char	IsANumber(unsigned char *pData);
void			iec_parser_warn(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3);
void			iec_parser_err(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3);
void			iec_mem_diagnose();

/**********************************************************
*				Lokale Variablen Declaration
**********************************************************/

_LOCAL	Parser_typ				PaDPList,PaDPListSave;
_LOCAL	Parser_typ				PaTypList,PaTypListSave;
_LOCAL	DatObjInfo_typ			DOI_dplist,DOI_typlist;
_LOCAL	IEC_ParserInfo_typ		IEC_ParserInfo;
_LOCAL	IEC_GlobalData_typ		IEC_GlobalData;
_LOCAL	IEC_ItemData_typ		IEC_DPData,IEC_ItemData,IEC_BlockData;
_LOCAL_RETAIN	signed short			State_find_StationTypPoint, State_find_Section, State_find_Data	;
_LOCAL	unsigned char			MsgTxt[256];

#define ITEM_DIAGNOSE_SIZE		2
_LOCAL	IEC_NormDiagnose_typ	IEC_NormDiagnose;
_LOCAL	IEC_ItemDiagnose_typ	IEC_ItemDiagnose[ITEM_DIAGNOSE_SIZE];
_LOCAL	IEC_DP_Info_typ 		*pItemData0Diagnose;
_LOCAL	IEC_DP_Info_typ 		*pItemData1Diagnose;
_LOCAL	IEC_Normierung_typ 		*pNormDataDiagnose;

/*** eof *****************************************************************************/





