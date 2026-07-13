/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IODRV (Profibus)
*  Teil:     Headerfile für Parser
*  Datei:    iodrv_parse.h
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
/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/
#define ERR_MSG_PRE_STRG				"i/o"
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
#define REAL_TYP						"_REAL"
#define REAL_IDX						8

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

/* Fehlermeldungen des Datenpunktbereiches */
#define PARS_ERR_UNKNOWN_DP_ENTRY		600				/* Fehler: Unbekannter Eintrag in Datenpunkt Bereich */
#define PARS_ERR_ANLNR_NOT_DEFINED		601				/* Fehler: Anlagennummer nicht definiert */
#define PARS_ERR_DPNR_NOT_DEFINED		602				/* Fehler: Datenpunktnummer nicht definiert */
#define PARS_ERR_TYP_NOT_DEFINED		603				/* Fehler: Datentyp nicht definiert */
#define PARS_ERR_DPTYP_NR_NOT_DEFINED	609				/* Fehler: Datentyp Nummer nicht definiert */
#define PARS_ERR_ITEM_NR_NOT_DEFINED	610				/* Fehler: Item Nummer nicht definiert */
#define PARS_ERR_ITEM_TYP_NOT_DEFINED	611				/* Fehler: Item Typ nicht definiert */
#define PARS_ERR_ITEM_TYP_UNKNOWN		612				/* Fehler: Item Typ unbekannt */
#define PARS_ERR_ITEM_TYP_NOT_VALID		613				/* Fehler: Item Typ nicht gueltig (Analogwert->BOOL oder Doppelmeldung->BOOL) */
#define PARS_ERR_ITEM_NAME_NOT_DEFINED	614				/* Fehler: Item Typ nicht definiert */
#define PARS_ERR_IODRV_INV_NOT_DEFINED	615				/* Fehler: IODRV Invers nicht definiert */
#define PARS_ERR_IODRV_QUITT_NOT_DEFINED	616			/* Fehler: IODRV ReqTyp nicht definiert */
#define PARS_ERR_PV_NOT_FOUND			617				/* Fehler: IODRV PV nicht gefunden */

/* Fehlermeldungen des Itembereiches */
#define PARS_ERR_UNKNOWN_ITEM_ENTRY		700				/* Fehler: Unbekannter Eintrag in Item Bereich (DM typlist)*/

/*-------------------------------------------------------
	Schlüsselworte
---------------------------------------------------------*/
/* Hilfskonstanten */

#define ALL_SECTION						0				/* Es wird im allgemeinen Teil geparst (Meldungsunterdrückung) */
#define IODRV_SECTION						1				/* Es wird im IODRV - Teil geparst (Meldung bei unbekannten Schlüsselworten) */
#define DP_PART							0				/* Es wird im allgemeinen Datenpunktbereich geparst */
#define ITEM_PART						1				/* Es wird im allgemeinen Itembereich geparst */

/* Schluesselworte Sectionen */
#define	KW_IODRV_SECTION				"I/O"			/* Schlüsselwort für IODRV-Section */
#define	KW_ALL_SECTION					"ALL"			/* Schlüsselwort für Allgemeine-Section */
#define	KW_BRS_SECTION					"BRS"			/* Schlüsselwort für BRS-Section */
#define KW_ITEM_SECTION					"ITEM"			/* Schlüsselwort für ITEM-Section */
#define KW_BLOCK_SECTION				"BLOCK"			/* Schlüsswlwort für Blockdefinition */

/* Schluesselworte  globaler Teil */


/* Schluesselworte  Datenpunkt Teil */
#define KW_DP_ALL_ANL					"ANLAGE"		/* Schlüsselwort für Anlagennummer */
#define KW_DP_ALL_ANL_NR				"NR"			/* Schlüsselwort für Anlagennummer (ANLAGEN.NR) */
#define KW_DP_ALL_DP					"DP"			/* Schlüsselwort für Datenpunktnummer */
#define KW_DP_ALL_DP_NR					"NR"			/* Schlüsselwort für Datenpunktnummer (DP.NR) */
#define KW_DP_ALL_TYP					"TYP"			/* Schlüsselwort für Datenpunkttyp */
#define KW_DP_ALL_ITEM					"ITEM"			/* Schlüsselwort Item Datentyp */
#define KW_DP_ALL_ITEM_TYP				"TYP"			/* Schlüsselwort Item-Datentyp ITEM.TYP */
#define KW_DP_ALL_ITEM_NR				"NR"			/* Schlüsselwort Item-Nummer (ITEM.NR) */
#define KW_DP_ALL_ITEM_NAME				"NAME"			/* Schlüsselwort Item-Nummer (ITEM.NAME) */

/* Schlüsselworte TYPLIST	ALL-Teil Datenpunkt */
#define	KW_DPTYP_ALL_TYP				"TYP"			/* Schlüsselwort Datentyp Nummer */
#define KW_DPTYP_ALL_TYP_NR				"NR"			/* Schlüsselwort Datentyp Nummer (TYP.NR) */

/* Schlüsselworte TYPLIST	ALL-Teil ITEM */

#define KW_ITEM_ALL_ITEM_TYP			"TYP"			/* Schlüsselwort Item Datentyp */
#define KW_ITEM_ALL_ITEM				"ITEM"			/* Schlüsselwort Item-Nummer */
#define KW_ITEM_ALL_ITEM_NR				"NR"			/* Schlüsselwort Item-Nummer (ITEM.NR) */

/* Schlüsselworte TYPLIST	IODRV-Teil */
#define KW_ITEM_IODRV_INV				"INV"			/* Schlüsselwort für Invers */
#define	KW_ITEM_IODRV_QUITT				"QUITT"			/* Schlüsselwort für Quittierpflichtig */
#define	KW_ITEM_IODRV_DIR				"DIR"			/* Schlüsselwort für Schribrichtung R/W/RW */

/* Schlüsselworte TYPLIST	IODRV Teil für DIR */
#define KW_ITEM_IODRV_DIR_R				"R"				/* Schlüsselwort für Read */
#define cKW_READ						0
#define KW_ITEM_IODRV_DIR_W				"W"				/* Schlüsselwort für Write */
#define cKW_WRITE						1
#define KW_ITEM_IODRV_DIR_RW			"RW"			/* Schlüsselwort für ReadWrite */
#define cKW_READWRITE					2


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
#define IODRV_TYPE_MIN					0				/* Minimaler Wert fuer IODRV-Type */
#define IODRV_TYPE_MAX					255				/* Maximaler Wert fuer IODRV-Type */
#define IODRV_INFO_MIN					0				/* Minimaler Wert fuer IODRV-Info */
#define IODRV_INFO_MAX					255				/* Maximaler Wert fuer IODRV-Info */
#define IODRV_ANALOGNR_MIN				1				/* Minimaler Wert fuer IODRV-Analogwert Nr. */
#define IODRV_ANALOGNR_MAX				MAX_ANALOGWERTE	/* Maximaler Wert fuer IODRV-Analogwert Nr. */
#define RAW_MIN							-4096			/* Minimaler Wert fuer Normierung Rohwert */
#define RAW_MAX							4095			/* Maximaler Wert fuer Normierung Rohwert */
#define NORM_MIN						-2000000000		/* Minimaler Wert fuer Normierung Normierungswert */
#define NORM_MAX						2000000000		/* Maximaler Wert fuer Normierung Normierungswert */
#define TSYNCH_MIN						0				/* Minimaler Wert fuer Zeitsynchronisation ein/aus */
#define TSYNCH_MAX						1				/* Minimaler Wert fuer Zeitsynchronisation ein/aus */
#define MRUC_MIN						0				/* Minimaler Wert fuer Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_MAX						1				/* Minimaler Wert fuer Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_TYPE_MIN			IODRV_TYPE_MIN			/* Minimaler Wert fuer Type.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_TYPE_MAX			IODRV_TYPE_MAX			/* Minimaler Wert fuer Type.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_INFO_MIN			IODRV_INFO_MIN			/* Minimaler Wert fuer Info.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MRUC_INFO_MAX			IODRV_INFO_MAX			/* Minimaler Wert fuer Info.Nr zur Aktivierung der Rücksetzung über LED-Taste */
#define MODE_MIN						0				/* Minimaler Wert fuer Mode */
#define MODE_MAX						15				/* Maximaler Wert fuer Mode */

/***********************************************************************/
/* Datentypdeklaration                                                 */
/***********************************************************************/
typedef struct
{
	plcbit					defined;
	long					value;
} IODRV_ParserParInfo_typ;

typedef struct
{
	plcbit					defined;
	REAL					value;
} IODRV_ParserParRealInfo_typ;

typedef struct
{
	plcbit					defined;
	unsigned char			*name;
	unsigned short			size;
} IODRV_ParserTypInfo_typ;


/* --------------------------*/
/* Globale Parameter         */
/* --------------------------*/

typedef struct
{
	IODRV_ParserTypInfo_typ	Interface;
	IODRV_ParserParInfo_typ	LineNr;
} IODRV_GlobalLineData_typ;

typedef struct
{
	IODRV_ParserParInfo_typ	Dummy;
} IODRV_GlobalData_typ;

/* ----------------------------*/
/* Parameter Datenpunkt / Item */
/* ----------------------------*/

typedef struct
{
	IODRV_ParserParInfo_typ	AnlagenNr;
	IODRV_ParserParInfo_typ	DPNr;
	IODRV_ParserTypInfo_typ	Typ;
	IODRV_ParserParInfo_typ	DPTypNr;
	IODRV_ParserTypInfo_typ	DPName;
	IODRV_ParserParInfo_typ	ItemNr;
	IODRV_ParserTypInfo_typ	ItemTyp;
	IODRV_ParserParInfo_typ	ItemTypNr;
	IODRV_ParserTypInfo_typ	ItemName;
	IODRV_ParserParInfo_typ	Invers;
	IODRV_ParserParInfo_typ	Quitt;
	IODRV_ParserParInfo_typ	Dir;
} IODRV_ItemData_typ;


/* -----------------------------*/
/* Infos des Parsers			*/
/* -----------------------------*/

/* Infos des Parsers */
typedef struct
{
	unsigned short			NrOfDataPoints;
	unsigned short			NrOfItems;
	unsigned short			ActItemIdx;
	long					memAdr;
	long					memSize;
	unsigned short			allocStatus;
	plcbit					ErrorMsg;
	unsigned short			NrOfLogEntrys;
	unsigned short			error;
	unsigned short			warning;
} IODRV_ParserInfo_typ;

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


typedef struct {
	plcbit					showData;
	plcbit					DataValid;
	unsigned short			AnlagenNr;
	unsigned short			DPTypNr;
	unsigned short			DPNr;
	unsigned short			ItemNr;
	IODRV_DP_Info_typ 		*pItemData;
	unsigned short			ItemIdx;
} IODRV_ItemDiagnose_typ;


/***********************************************************************/
/* Prototypen		                                                   */
/***********************************************************************/
void			iodrv_parser();
void			iodrv_parser_dplist_clear();
void			iodrv_parser_typlist_clear();
void			iodrv_parser_global();
void			iodrv_parser_dpinfo(plcbit *pBlockFound, plcbit *pDPFound);
unsigned short	get_line_idx(IODRV_ItemData_typ *pDPInfo, IODRV_GlobalData_typ *pGlobalData, unsigned short *LineIdx);
unsigned short	get_device_idx(unsigned short LineIdx, long DeviceAdr, unsigned short *DeviceIdx, plcbit *NewDevice);
plcbit			normierung_defined(IODRV_ItemData_typ *pItemData);
plcbit			iodrv_parser_dpinfo_ok(IODRV_ItemData_typ *pData);
plcbit			iodrv_parser_dptype(unsigned char *pName, unsigned short size);
plcbit			iodrv_parser_iteminfo();
plcbit			iodrv_parser_iteminfo_ok(IODRV_ItemData_typ *pData);
void			iodrv_parser_data(unsigned char *pSrcStart,unsigned char *pSrcEnd, Parser_typ *pPa, IODRV_ItemData_typ *pData, unsigned char SektorInfo, unsigned char PartInfo);
void			copyItemData(IODRV_ItemData_typ *pItemDest, IODRV_ItemData_typ *pItemSrc);
unsigned char	IsANumber(unsigned char *pData);
void			iodrv_parser_warn(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3);
void			iodrv_parser_err(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3);
void			iodrv_mem_diagnose();

/**********************************************************
*				Lokale Variablen Declaration
**********************************************************/

_LOCAL	Parser_typ				PaDPList,PaDPListSave;
_LOCAL	Parser_typ				PaTypList,PaTypListSave;
_LOCAL	DatObjInfo_typ			DOI_dplist,DOI_typlist;
_LOCAL	IODRV_ParserInfo_typ		IODRV_ParserInfo;
_LOCAL	IODRV_GlobalData_typ		IODRV_GlobalData;
_LOCAL	IODRV_ItemData_typ		IODRV_DPData,IODRV_ItemData,IODRV_BlockData;
_LOCAL_RETAIN	signed short			State_find_StationTypPoint, State_find_Section, State_find_Data	;
_LOCAL	unsigned char			MsgTxt[256];
_LOCAL	unsigned char			*pPointName;
_LOCAL	unsigned short			lenPointName;


#define ITEM_DIAGNOSE_SIZE		2
_LOCAL	IODRV_ItemDiagnose_typ	IODRV_ItemDiagnose[ITEM_DIAGNOSE_SIZE];
_LOCAL	IODRV_DP_Info_typ 		*pItemData0Diagnose;
_LOCAL	IODRV_DP_Info_typ 		*pItemData1Diagnose;

/*** eof *****************************************************************************/





