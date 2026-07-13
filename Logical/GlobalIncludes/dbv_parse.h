/******************************************************************************
*  Projekt:  DbVist Visualisierung für DbServ
*  Teil:     Parserfunktionen
*  Datei:    dbv_parse.c
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    04-0-2006
*  Autor:    WAI
*------------------------------------------------------------------------------
*  Funktion:
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 04-06-06 | WAI  | Datei-Erstellung
*        |          |      |
******************************************************************************/
/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/
#define ERR_MSG_PRE_STRG				"dbv"
#define MAX_ERROR_MSG_LOG				20				/* Anzahl Fehler, die in das Log-Buch eingetragen werden */
/* Bezeichnung der Datenmodule */
#define DATA_OBJEKT_DPLIST		"dplist"
#define DATA_OBJEKT_TYPLIST		"typlist"
#define DATA_OBJEKT_DBVTEXT 	"dbv_text"
#define DATA_OBJEKT_TEXT1 		"text_1"
#define DATA_OBJEKT_TEXT2 		"text_2"
#define DATA_OBJEKT_PAGE 		"page_1"
#define DATA_OBJEKT_OBJ1 		"obj_1"
#define DATA_OBJEKT_OBJ2 		"obj_2"
#define DATA_OBJEKT_OBJ3 		"obj_3"
#define DATA_OBJEKT_OBJ4 		"obj_4"
#define DATA_OBJEKT_OBJ5 		"obj_5"

/* Delimiter */
#define DELIM_ALLGEMEIN_ANF		"<Allgemein>"
#define DELIM_ALLGEMEIN_END		"</Allgemein>"
#define DELIM_DTYP_ANF			"<Dtyp>"
#define DELIM_DTYP_END			"</Dtyp>"
#define DELIM_OBJGRPDTYP_ANF	"<ObjGrpDtyp>"
#define DELIM_OBJGRPDTYP_END	"</ObjGrpDtyp>"
#define DELIM_OBJITEMAP_ANF		"<ObjItemAP>"
#define DELIM_OBJITEMAP_END		"</ObjItemAP>"
#define DELIM_OBJITEMBPAL_ANF	"<ObjItemBPAL>"
#define DELIM_OBJITEMBPAL_END	"</ObjItemBPAL>"
#define DELIM_OBJITEMZL_ANF		"<ObjItemZL>"
#define DELIM_OBJITEMZL_END		"</ObjItemZL>"
#define DELIM_OBJITEMALIST_ANF	"<ObjItemAList>"
#define DELIM_OBJITEMALIST_END	"</ObjItemAList>"
#define DELIM_OBJSYS_ANF		"<ObjSys>"
#define DELIM_OBJSYS_END		"</ObjSys>"
#define DELIM_PAGE_ANF 			"<Page>"
#define DELIM_PAGE_END 			"</Page>"

/* Skalierungsparameter */
#define	MAX_DT					10		/* max.Anzahl von Datentypen */
#define	MAX_GRP					100		/* max.Anzahl von Gruppen */
#define	MAX_UNIT				32		/* max.Anzahl von Einheiten */
#define	MAX_TEXTLIST			32		/* max.Anzahl von Textlisten */
#define	MAX_TEXTVALUE			16		/* max.Wert einer Textliste */

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
#define DOUBLE_TYP						"_DOUBLE"
#define DOUBLE_IDX						9

#define DTYP_AP							1
#define DTYP_BP							2
#define DTYP_AL							3
#define DTYP_ZL							4

/*-------------------------------------------------------
	Fehlermeldungen
---------------------------------------------------------*/
#define NO_ERROR						0


/* Fehlermeldungen: Datenmodul nicht gefunden */
#define PARS_ERR_DPLIST_NOT_FOUND		501				/* Fehler: Datenmodul dplist nicht gefunden */
#define PARS_TXT_DPLIST_NOT_FOUND		"datamodule dplist not found"

#define PARS_ERR_TYPLIST_NOT_FOUND		502				/* Fehler: Datenmodul typlist nicht gefunden */
#define PARS_TXT_TYPLIST_NOT_FOUND		"datamodule typlist not found"

#define PARS_ERR_DBVTEXT_NOT_FOUND		503				/* Fehler: Datenmodul dbv_text nicht gefunden */
#define PARS_TXT_DBVTEXT_NOT_FOUND		"datamodule dbv_text not found"

#define PARS_ERR_MEM_ALLOC				504				/* Fehler: Speicher kann nicht allokiert werden */
#define PARS_TXT_MEM_ALLOC				"TMP_alloc not successfull"

#define PARS_ERR_CREATE_DM				505				/* Fehler: Datenmodul kann nicht erzeugt werden */
#define PARS_TXT_CREATE_DM				"Create DM not successfull"


/* Fehlermeldungen des globalen Teiles */
#define PARS_ERR_UNKNOWN_GLOBAL_ENTRY	505				/* Fehler: Unbekannter Eintrag in globalen Bereich */
#define PARS_ERR_GRP_UNKNOWN			506				/* Fehler: Name kann keiner Gruppe zugeordnet werden */
#define PARS_ERR_TO_MUCH_GRP			507				/* Fehler: Anzahl deklarierter Gruppen zu gross  */
#define PARS_ERR_UNIT_UNKNOWN			508				/* Fehler: Name kann keiner Einheit zugeordnet werden */
#define PARS_ERR_TO_MUCH_UNITS			509				/* Fehler: Anzahl deklarierter Einheiten zu gross  */
#define PARS_ERR_TLIST_UNKNOWN			510				/* Fehler: Name kann keiner Textliste zugeordnet werden */
#define PARS_ERR_TO_MUCH_TLISTS			511				/* Fehler: Anzahl deklarierter Textlisten zu gross  */

#define PARS_ERR_DPTYP_NR_NOT_DEFINED	609				/* Fehler: Datentyp Nummer nicht definiert */
#define PARS_ERR_ITEM_NR_NOT_DEFINED	610				/* Fehler: Item Nummer nicht definiert */
#define PARS_ERR_ITEM_TYP_NOT_DEFINED	611				/* Fehler: Item Typ nicht definiert */
#define PARS_ERR_ITEM_TYP_UNKNOWN		612				/* Fehler: Item Typ unbekannt */
#define PARS_ERR_ITEM_NAME_NOT_DEFINED	613				/* Fehler: Item Name nicht definiert */

/* Fehlermeldungen des Itembereiches */
#define PARS_ERR_UNKNOWN_ITEM_ENTRY		700				/* Fehler: Unbekannter Eintrag in Item Bereich (DM typlist)*/
#define PARS_ERR_GRP_NOTDEFINED			701				/* Fehler: Gruppe ist nicht definiert */
#define PARS_ERR_TLIST_NOTDEFINED		702				/* Fehler: Textliste ist nicht definiert */
#define PARS_ERR_ITEM_DTYP_NOT_DEFINED	703				/* Fehler: Datentyp ist nicht definiert */

#define PARS_ERR_REPLACE				710				/* Fehler beim ersetzen von Token */

#define PARS_ERR_PWI_NOT_FOUND			800				/* Fehler: Interface zu PWI nicht gefunden */
#define PARS_ERR_PWI_ERROR				801				/* Fehler: PWI meldet Fehler */
/*-------------------------------------------------------
	Schlüsselworte
---------------------------------------------------------*/
/* Hilfskonstanten */

#define ALL_SECTION						0				/* Es wird im allgemeinen Teil geparst (Meldungsunterdrückung) */
#define DBV_SECTION						1				/* Es wird im VIS - Teil geparst (Meldung bei unbekannten Schlüsselworten) */
#define DP_PART							0				/* Es wird im allgemeinen Datenpunktbereich geparst */
#define ITEM_PART						1				/* Es wird im allgemeinen Itembereich geparst */

/* Schluesselworte Sectionen */
#define	KW_DBV_SECTION					"VIS"			/* Schlüsselwort für VIS-Section */
#define	KW_ALL_SECTION					"ALL"			/* Schlüsselwort für Allgemeine-Section */
#define KW_ITEM_SECTION					"ITEM"			/* Schlüsselwort für ITEM-Section */
#define KW_BLOCK_SECTION				"BLOCK"			/* Schlüsswlwort für Blockdefinition */

/* Schluesselworte  globaler Teil */
#define KW_UST							"UST"			/* Schlüsselwort für UST */
#define KW_VIS_USTNR					"NR"			/* Schlüsselwort für UST-Nummer */
#define KW_VIS_USTNAME					"NAME"			/* Schlüsselwort für UST-Name */
#define KW_VIS_USTIF					"IF"			/* Schlüsselwort für Interface */
#define KW_VIS_GRP						"GRP"			/* Schlüsselwort für Gruppe */
#define KW_VIS_GRPNAME					"NAME"			/* Schlüsselwort für Gruppennamen */
#define KW_VIS_UNIT						"EINH"			/* Schlüsselwort für Einheit */
#define KW_VIS_TLIST					"TLIST"			/* Schlüsselwort für Textliste */
#define KW_VIS_TLISTT0					"T0"			/* Schlüsselwort für Textliste Zusatnd 0*/
#define KW_VIS_TLISTT1					"T1"			/* Schlüsselwort für Textliste Zusatnd 1*/
#define KW_VIS_TLISTT2					"T2"			/* Schlüsselwort für Textliste Zusatnd 2*/
#define KW_VIS_TLISTT3					"T3"			/* Schlüsselwort für Textliste Zusatnd 3*/
#define KW_VIS_TLISTT4					"T4"			/* Schlüsselwort für Textliste Zusatnd 4*/
#define KW_VIS_TLISTT5					"T5"			/* Schlüsselwort für Textliste Zusatnd 5*/
#define KW_VIS_TLISTT6					"T6"			/* Schlüsselwort für Textliste Zusatnd 6*/
#define KW_VIS_TLISTT7					"T7"			/* Schlüsselwort für Textliste Zusatnd 7*/
#define KW_VIS_TLISTT8					"T8"			/* Schlüsselwort für Textliste Zusatnd 8*/
#define KW_VIS_TLISTT9					"T9"			/* Schlüsselwort für Textliste Zusatnd 9*/
#define KW_VIS_TLISTT10					"T10"			/* Schlüsselwort für Textliste Zusatnd 10*/
#define KW_VIS_TLISTT11					"T11"			/* Schlüsselwort für Textliste Zusatnd 11*/
#define KW_VIS_TLISTT12					"T12"			/* Schlüsselwort für Textliste Zusatnd 12*/
#define KW_VIS_TLISTT13					"T13"			/* Schlüsselwort für Textliste Zusatnd 13*/
#define KW_VIS_TLISTT14					"T14"			/* Schlüsselwort für Textliste Zusatnd 14*/
#define KW_VIS_TLISTT15					"T15"			/* Schlüsselwort für Textliste Zusatnd 15*/

/* Schluesselworte  Datenpunkt Teil */
#define KW_DP_ALL_ANL					"ANLAGE"		/* Schlüsselwort für Anlagennummer */
#define KW_DP_ALL_ANL_NR				"NR"			/* Schlüsselwort für Anlagennummer (ANLAGEN.NR) */
#define KW_DP_ALL_DP					"DP"			/* Schlüsselwort für Datenpunktnummer */
#define KW_DP_ALL_DP_NR					"NR"			/* Schlüsselwort für Datenpunktnummer (DP.NR) */
#define KW_DP_ALL_TYP					"TYP"			/* Schlüsselwort für Datenpunkttyp */
#define KW_DP_ALL_ITEM					"ITEM"			/* Schlüsselwort Item Datentyp */
#define KW_DP_ALL_ITEM_TYP				"TYP"			/* Schlüsselwort Item-Datentyp ITEM.TYP */
#define KW_DP_ALL_ITEM_NR				"NR"			/* Schlüsselwort Item-Nummer (ITEM.NR) */

/* Schlüsselworte TYPLIST	ALL-Teil Datenpunkt */
#define	KW_DPTYP_ALL_TYP				"TYP"			/* Schlüsselwort Datentyp Nummer */
#define KW_DPTYP_ALL_TYP_NR				"NR"			/* Schlüsselwort Datentyp Nummer (TYP.NR) */

/* Schlüsselworte TYPLIST	ALL-Teil ITEM */
#define KW_ITEM_ALL_ITEM_TYP			"TYP"			/* Schlüsselwort Item Datentyp */
#define KW_ITEM_ALL_ITEM				"ITEM"			/* Schlüsselwort Item-Nummer */
#define KW_ITEM_ALL_ITEM_NR				"NR"			/* Schlüsselwort Item-Nummer (ITEM.NR) */
#define KW_ITEM_ALL_ITEM_NAME			"NAME"			/* Schlüsselwort Item-Nummer (ITEM.NAME) */
#define KW_ITEM_ALL_ITEM_TEXT			"TEXT"			/* Schlüsselwort Item-Nummer (ITEM.TEXT) */

/* Schlüsselworte TYPLIST	VIS-Teil */
#define KW_ITEM_VIS_GRP					"GRP"			/* Schlüsselwort Gruppe */
#define KW_ITEM_VIS_DTYP				"DTYP"			/* Schlüsselwort Datentyp */
#define KW_ITEM_VIS_EINH				"EINH"			/* Schlüsselwort Einheit */
#define KW_ITEM_VIS_KPOS				"KPOS"			/* Schlüsselwort Kommaposition */
#define KW_ITEM_VIS_TLIST				"TLIST"			/* Schlüsselwort Textliste */
#define KW_ITEM_VIS_QUITT				"QUITT"			/* Schlüsselwort Quittierung */


/* Infos des Parsers */
typedef struct
{
	long					memAdr;
	long					memSize;
	unsigned short			allocStatus;
	plcbit					ErrorMsg;
	unsigned short			NrOfLogEntrys;
	unsigned short			error;
	unsigned short			warning;
	UDINT					OffsPage;
	UDINT					LenPage;
	UDINT					OffsObjGrpDtyp;
	UDINT					LenObjGrpDtyp;
	UDINT					OffsObjItemAP;
	UDINT					LenObjItemAP;
	UDINT					OffsObjItemBPAL;
	UDINT					LenObjItemBPAL;
	UDINT					OffsObjItemZL;
	UDINT					LenObjItemZL;
	UDINT					OffsObjAlist;
	UDINT					LenObjAlist;
	UDINT					OffsObjSys;
	UDINT					LenObjSys;
	UDINT					OffsAllgemein;
	UDINT					LenAllgemein;
	UINT					AnzahlAllgemein;
	UDINT					OffsDtyp;
	UDINT					LenDtyp;
	UINT					AnzahlDtyp;
	unsigned short			NrOfGrp;
	unsigned short			GrpIdx;
	long					GrpNrAct;
	plcbit					GrpNrValid;
	unsigned short			NrOfUnits;
	unsigned short			NrOfTlist;
	unsigned short			TlistIdx;
	long					TlistNrAct;
	plcbit					TlistNrValid;
	unsigned short			NrOfDataPoints;
	unsigned short			NrOfItems;
	unsigned short			NrOfAL;
	unsigned char			*DPName;
	unsigned short			LenDPName;
	unsigned char			*OptName;
	unsigned short			LenOptName;
	unsigned short			LenItemName;
	unsigned short			LenItemText;
	unsigned long			LenAllItemName;
	unsigned long			LenAllItemText;
	unsigned long			LenText1,LenText2;
	long					memAdrText1,memAdrText2,memAdrPage,memAdrObj1,memAdrObj2,memAdrObj3,memAdrObj4,memAdrObj5;
	long					memSizeText1,memSizeText2;
	unsigned short			allocStatusText1,allocStatusText2,allocStatusPage,allocStatusObj1,allocStatusObj2,allocStatusObj3,allocStatusObj4,allocStatusObj5;
	unsigned short			OffsetAllgemein;
	unsigned short			OffsetDtyp;
	unsigned short			OffsetGrp;
	unsigned short			OffsetTextlist;
	unsigned short			OffsetUnit;
	unsigned short			OffsetItems;
	unsigned short			ActItemIdx;
} DBV_ParserInfo_typ;

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

typedef struct
{
	plcbit					defined;
	unsigned char			*name;
	unsigned short			size;
} DBV_ParserTypInfo_typ;

typedef struct
{
	plcbit					defined;
	long					value;
} DBV_ParserParInfo_typ;

typedef struct
{
	plcbit					defined;
	long					value;
	unsigned char			*name[MAX_TEXTVALUE];
	unsigned short			size[MAX_TEXTVALUE];
} DBV_ParserTlistInfo_typ;

typedef struct
{
	DBV_ParserParInfo_typ	GrpNr;
	DBV_ParserTypInfo_typ	GrpName;
} DBV_ParserGrpInfo_typ;

/* --------------------------*/
/* Globale Parameter         */
/* --------------------------*/

typedef struct
{
	DBV_ParserParInfo_typ	UstNr;
	DBV_ParserTypInfo_typ	UstName;
	DBV_ParserTypInfo_typ	Interface;
	DBV_ParserTypInfo_typ	Dtyp[MAX_DT];
	DBV_ParserGrpInfo_typ	Grp[MAX_GRP];
	DBV_ParserTypInfo_typ	Unit[MAX_UNIT];
	DBV_ParserTlistInfo_typ	Textlist[MAX_TEXTLIST];
} DBV_GlobalData_typ;
/* ----------------------------*/
/* Parameter Datenpunkt / Item */
/* ----------------------------*/

typedef struct
{
	DBV_ParserParInfo_typ	AnlagenNr;
	DBV_ParserParInfo_typ	DPNr;
	DBV_ParserTypInfo_typ	Typ;
	DBV_ParserParInfo_typ	DPTypNr;
	DBV_ParserParInfo_typ	ItemNr;
	DBV_ParserTypInfo_typ	ItemTyp;
	DBV_ParserParInfo_typ	ItemTypNr;
	DBV_ParserTypInfo_typ	ItemName;
	DBV_ParserTypInfo_typ	ItemText;
	DBV_ParserParInfo_typ	GrpNr;
	DBV_ParserParInfo_typ	Dtyp;
	DBV_ParserParInfo_typ	Unit;
	DBV_ParserParInfo_typ	Tlist;
	DBV_ParserParInfo_typ	Kpos;
	DBV_ParserParInfo_typ	Quitt;
	USINT					noVIS;
} DBV_ItemData_typ;

/* Struktur eines Datenpunkt Items im Speicher */
typedef struct {
	UINT	AnlagenNr;						/* Anlagennummer */
	UINT	Device;							/* Devicenummer (immer 1)*/
	UINT	DPTypNr;						/* Datentypnummer */
	UINT	DPNr;							/* Datenpunktnummer */
	UINT	ItemNr;							/* Item Nummer */
	UINT	TextNr;							/* Textnummer im Textpool */
	USINT	GrpNr;							/* Gruppennummer */
	USINT	Dtyp;							/* Datentyp */
	UINT	Unit;							/* Textnummer Einheit im Textpool */
	USINT	Kpos;							/* Komaposition */
	USINT	Tlist;							/* textlist-Nummer */
	USINT	Quitt;							/* Quittier-Datenpunkt */
} DBV_DP_Info_typ;

typedef struct {
	plcbit					showData;
	plcbit					DataValid;
	unsigned short			AnlagenNr;
	unsigned short			DPTypNr;
	unsigned short			DPNr;
	unsigned short			ItemNr;
	DBV_DP_Info_typ 		*pItemData;
	unsigned short			ItemIdx;
} DBV_ItemDiagnose_typ;


/***********************************************************************/
/* Prototypen		                                                   */
/***********************************************************************/
void dbv_parser();
void dbv_parser_err(unsigned short errorNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3);
void dbv_parser_warn(unsigned short warnNr, unsigned char *pTxtInfo, unsigned char *pTxt0, unsigned char *pTxt1, long info1, unsigned char *pTxt2, long info2, unsigned char *pTxt3, long info3);
int  dbv_parser_sect(USINT *pData, UDINT len, USINT *delim_anf, USINT *delim_end, UDINT *pOffset,UDINT *pLen, UINT *pAnzahl);
void dbv_parser_global();
unsigned char IsANumber(unsigned char *pData);
void dbv_parser_dplist_clear();
void dbv_parser_typlist_clear();
void copyItemData(DBV_ItemData_typ *pItemDest, DBV_ItemData_typ *pItemSrc);
plcbit dbv_parser_iteminfo();
void dbv_parser_dpinfo(plcbit *pBlockFound, plcbit *pDPFound);
plcbit dbv_parser_dptype(unsigned char *pName, unsigned short size);
void dbv_parser_data(unsigned char *pSrcStart,unsigned char *pSrcEnd, Parser_typ *pPa, DBV_ItemData_typ *pData, unsigned char SectionInfo, unsigned char PartInfo);
plcbit dbv_parser_iteminfo_ok(DBV_ItemData_typ *pData);
unsigned char *dbv_findToken(USINT *pSrc,UDINT len,USINT *token);
void dbv_replace_refpab(USINT *pFound,DBV_DP_Info_typ *pDBV_DP_Info);
void dbv_replace_index(USINT *pFound,INT i);
void make_page();
void make_obj1();
void make_obj2();
void make_obj3();
void make_obj4();
void make_obj5();
void get_idBrServer();
void dbv_mem_diagnose();

/**********************************************************
*				Lokale Variablen Declaration
**********************************************************/
_LOCAL	Parser_typ				PaDPList,PaDPListSave;
_LOCAL	Parser_typ				PaTypList,PaTypListSave;

_LOCAL	DatObjInfo_typ			DOI_dplist,DOI_typlist,DOI_dbvtext,DOI_text1,DOI_text2,DOI_page,DOI_obj1,DOI_obj2,DOI_obj3,DOI_obj4,DOI_obj5;
_LOCAL	DatObjDelete_typ		DOD_text1,DOD_text2,DOD_page,DOD_obj1,DOD_obj2,DOD_obj3,DOD_obj4,DOD_obj5;
_LOCAL	DatObjCreate_typ		DOC_text1,DOC_text2,DOC_page,DOC_obj1,DOC_obj2,DOC_obj3,DOC_obj4,DOC_obj5;
_LOCAL	DBV_ParserInfo_typ		DBV_ParserInfo;
_LOCAL	DBV_GlobalData_typ		DBV_GlobalData;
_LOCAL	DBV_ItemData_typ		DBV_DPData,DBV_ItemData,DBV_BlockData;
_LOCAL	signed short			State_find_StationTypPoint, State_find_Section, State_find_Data	_VAR_RETAIN;
_LOCAL	unsigned char			MsgTxt[256];
_LOCAL	UDINT					check_len1,check_len2;

#define ITEM_DIAGNOSE_SIZE		2
_LOCAL	DBV_ItemDiagnose_typ	DBV_ItemDiagnose[ITEM_DIAGNOSE_SIZE];
_LOCAL	DBV_DP_Info_typ 		*pItemData0Diagnose;
_LOCAL	DBV_DP_Info_typ 		*pItemData1Diagnose;

/* Test */
_LOCAL	BOOL					go	_VAR_RETAIN;



