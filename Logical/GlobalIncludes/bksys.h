/* bksys.h
// Globale Include-Datei fuer BURKLIMAT2000
// LastEdit:
//		14.08.03/RES:	bksys.h aus bk.h und bkklib.h erzeugt; neu in V4.01
//		12.08.03/RES:	Pipe-Diagnosen fuer Clients eingebaut; neu in V4.01
//		08.08.03/RES:	CliPipeItemLen aus DtProps_Typ entfernt ( wird nicht mehr benötigt )
//		06.08.03/RES:	ANZ_WAIT_CYCLE_xxx ueberarbeitet
//		18.07.03/RES:	Erweiterungen fuer V4.01 eingefuegt
//		05.06.03/RES:	Funktionsprototypen fuer Client-Interface eingefuegt
//		25.04.03/RES:	Erweiterungen fuer V4.00 eingefuegt
//		24.04.03/RES:	Basis-Fehlernummer von 60000 auf 50000 (=Anwenderbereich) geaendert
//		29.04.03/RES:	DtDesc.ItemOffPab von USINT auf USHORT geaendert ( wegen pCAB gab's Ueberlauf bei BKZS )
//		23.04.03/RES:	Datentypen für Client-Abbild (CAB) erweitert
//		22.04.03/RES:	Itemanzahlen (Bk2000) korrigiert!!
// --------------------------------------------- */

#ifndef BKSYS_H_
#define BKSYS_H_

/* Burklimat2000 - Versionsnummer fuer alle Bk2-Tasks */
#define BK_VERSION_NR			600			/* Vollversion=positiv, Betaversion=negativ */
#define BK_VERSION_TEXT			"V6.00.0"	/* Vollversion="V....", Betaversion="B...." */

/* Basis-Fehlernummer für Logbuch-Einträge */
#define EC_BK2_BASE				50000

/* Definitionen fuer Systemfunktionen */
#define EC_SYS_BASE				0
#define ERR_SYS_PRE				"bks"

/* Definitionen fuer bkfbklib */
#define EC_FBK_BASE				20
/* hier kein gemeinsames Prefix; jeder FBK hat eigenes Prefix!! */

/* Definitionen fuer bkkern */
#define EC_KERN_BASE			50
#define ERR_KERN_PRE			"bkk"

/* Definitionen fuer bkvist */
#define EC_VIST1_BASE			100			/* Pei 12.04.02 */
#define ERR_VIST1_PRE			"bkv"
#define ANZ_WAIT_CYCLE_VIST1	ANZ_WAIT_CYCLE_INET + 1

/* Definitionen fuer bkvist2 */
#define EC_VIST2_BASE			150
#define ERR_VIST2_PRE			"bkw"		/* von "bkv2" -> "bkw" geaendert: 22.05.02/RES */
#define ANZ_WAIT_CYCLE_VIST2	ANZ_WAIT_CYCLE_VIST1 + 5

/* Definitionen fuer bkinet */
#define EC_INET_BASE			200
#define ERR_INET_PRE			"bki"
#define USER_ID_INET			1
#define ANZ_WAIT_CYCLE_INET		ANZ_WAIT_CYCLE_TCP + 2

/* Definitionen fuer bktcp */
#define EC_TCP_BASE				250
#define ERR_TCP_PRE				"bkt"
#define ANZ_WAIT_CYCLE_TCP		0

/* Definitionen fuer dbs */
#define EC_DBS_BASE				300
#define ERR_DBS_PRE				"dbs"
#define ANZ_WAIT_CYCLE_DBS		0

/* Anzahl Fehlerzaehler fuer ErrStatistics */
#define ERRCNTANZ				300

/* maximale Anzahl Fehlereintraege ins Logbuch beim Parsen */
#define PARS_ERR_MAXENTRIES		5

enum Elementarten { EA_RK, EA_SG, EA_AP, EA_SW, EA_ZL, EA_SB, EA_BP, EA_AL, EA_BA=10, EA_ANZ };
enum Datentypen { RK, SGST, SGQS, SGIN, AP, SW, ZL, SB, SBIM, SBKL, BP, AL, BA, STAT, ZS, DT_ANZ };

enum EvTyp { IMP, EXP };						/* Event-Typ: Implizit / Explizit */
enum ErrStat { WARN, ERR, ERRLOG, ERRLOGHALT };	/* Fehlerstati */

#define USR_ANZ_MAX		8		/* maximale Anzahl der DCS2000-User */
#define CLI_ANZ_MAX		8		/* maximale Anzahl der ZLT-Clients; neu in V4.00 */

#define DTNAMELEN		4
#define DPNAMELEN		32
#define ITEMNAMELEN		5

/* Fehlertexte und -nummern */
#define	NOVALIDSTATSPTR_TEXT	"SYS_Statistic n. gefunden"
#define	NOVALIDSTATSPTR_NUM		0

#define	PCREATEERR_TEXT			"Fehler beim Erzeugen der Event-Pipes"
#define	PCREATEERR_NUM			1

#define	ECREATEERR_TEXT			"Fehler beim Erzeugen der Event-Parameterblöcke"
#define	ECREATEERR_NUM			2

#define	NOVALIDGLOBPTR_TEXT		"Ungültige Adresse in SYS_pGlobals"
#define	NOVALIDGLOBPTR_NUM		3

#define	DTERR_TEXT				"DT-Index ungültig"
#define	DTERR_NUM				4

#define	DPERR_TEXT				"DP-Index ungültig"
#define	DPERR_NUM				5

#define	EVTYPERR_TEXT			"Eventtyp ungültig"
#define	EVTYPERR_NUM			6

#define	NOEVBLKERR_TEXT			"Kein Eventblock gefunden"
#define	NOEVBLKERR_NUM			7

/* Dimensionierung der Fehlermeldungen im Logbuch */
#define ERRTEXT_MAXLEN		31
#define ERRTEXT_MAXLINES	3
#define SEARCH_COUNT_MAX	6
#define CUT_SIGN			'~'

/* Itemanzahl jedes Datentyps ( BK2000 )*/
#define RK_DT_ITEMANZ	37
#define SGST_DT_ITEMANZ	23
#define SGQS_DT_ITEMANZ	25
#define SGIN_DT_ITEMANZ	25
#define AP_DT_ITEMANZ	13
#define SW_DT_ITEMANZ	13
#define ZL_DT_ITEMANZ	16
#define SB_DT_ITEMANZ	27
#define SBIM_DT_ITEMANZ	25
#define SBKL_DT_ITEMANZ	19
#define BP_DT_ITEMANZ	8
#define AL_DT_ITEMANZ	23
#define BA_DT_ITEMANZ	17
#define STAT_DT_ITEMANZ	14
#define ZS_DT_ITEMANZ	222

#define MAX_DT_ITEMANZ	ZS_DT_ITEMANZ	/* Maximale Itemanzahl für PabDefault */

/* Itemanzahl jeder Elementart (BK alt) */
#define	RK_EA_ITEMANZ	18
#define	SGST_EA_ITEMANZ	12
#define	SGQS_EA_ITEMANZ	12
#define	SGIN_EA_ITEMANZ	12
#define AP_EA_ITEMANZ	4
#define SW_EA_ITEMANZ	4
#define ZL_EA_ITEMANZ	5
#define SB_EA_ITEMANZ	4
#define SBIM_EA_ITEMANZ	4
#define SBKL_EA_ITEMANZ	4
#define BP_EA_ITEMANZ	1
#define AL_EA_ITEMANZ	4
#define BA_EA_ITEMANZ	1
#define STAT_EA_ITEMANZ	2
#define ZS_EA_ITEMANZ	1	/* Hier nur Dummy !! */

/* Byteanzahl jeder Elementart (BK alt ) */
#define	RK_EA_DATALEN	32
#define	SGST_EA_DATALEN	18
#define	SGQS_EA_DATALEN	18
#define	SGIN_EA_DATALEN	18
#define	AP_EA_DATALEN	7
#define	SW_EA_DATALEN	6
#define	ZL_EA_DATALEN	11
#define	SB_EA_DATALEN	4
#define	SBIM_EA_DATALEN	4
#define	SBKL_EA_DATALEN	4
#define	BP_EA_DATALEN	1
#define	AL_EA_DATALEN	4
#define	BA_EA_DATALEN	1
#define	STAT_EA_DATALEN	2
#define ZS_EA_DATALEN	1	/* Hier nur Dummy !! */

/* Anzahl DP-Events, die gleichzeitig in Eventpipe passen sollen */
#define RK_EP_ENTRIES	40
#define SGST_EP_ENTRIES	40
#define SGQS_EP_ENTRIES	40
#define SGIN_EP_ENTRIES	40
#define AP_EP_ENTRIES	100
#define SW_EP_ENTRIES	80
#define ZL_EP_ENTRIES	60
#define SB_EP_ENTRIES	40
#define SBIM_EP_ENTRIES	20
#define SBKL_EP_ENTRIES	20
#define BP_EP_ENTRIES	40
#define AL_EP_ENTRIES	80
#define BA_EP_ENTRIES	20
#define STAT_EP_ENTRIES	1	/* Hier nur Dummy !! */
#define ZS_EP_ENTRIES	1	/* Hier nur Dummy !! */

/* Berechnung der erforderlichen Puffergroesse fuer Pipes */
#define RK_EP_BUFSIZE	( RK_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + RK_EA_DATALEN   + ( RK_EA_DATALEN   & 1 ) ) + 2 )
#define SGST_EP_BUFSIZE	( SGST_EP_ENTRIES * ( sizeof( ItemAdr_Typ ) + SGST_EA_DATALEN + ( SGST_EA_DATALEN & 1 ) ) + 2 )
#define SGQS_EP_BUFSIZE	( SGQS_EP_ENTRIES * ( sizeof( ItemAdr_Typ ) + SGQS_EA_DATALEN + ( SGQS_EA_DATALEN & 1 ) ) + 2 )
#define SGIN_EP_BUFSIZE	( SGIN_EP_ENTRIES * ( sizeof( ItemAdr_Typ ) + SGIN_EA_DATALEN + ( SGIN_EA_DATALEN & 1 ) ) + 2 )
#define AP_EP_BUFSIZE	( AP_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + AP_EA_DATALEN   + ( AP_EA_DATALEN   & 1 ) ) + 2 )
#define SW_EP_BUFSIZE	( SW_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + SW_EA_DATALEN   + ( SW_EA_DATALEN   & 1 ) ) + 2 )
#define ZL_EP_BUFSIZE	( ZL_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + ZL_EA_DATALEN   + ( ZL_EA_DATALEN   & 1 ) ) + 2 )
#define SB_EP_BUFSIZE	( SB_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + SB_EA_DATALEN   + ( SB_EA_DATALEN   & 1 ) ) + 2 )
#define SBIM_EP_BUFSIZE	( SBIM_EP_ENTRIES * ( sizeof( ItemAdr_Typ ) + SBIM_EA_DATALEN + ( SBIM_EA_DATALEN & 1 ) ) + 2 )
#define SBKL_EP_BUFSIZE	( SBKL_EP_ENTRIES * ( sizeof( ItemAdr_Typ ) + SBKL_EA_DATALEN + ( SBKL_EA_DATALEN & 1 ) ) + 2 )
#define BP_EP_BUFSIZE	( BP_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + BP_EA_DATALEN   + ( BP_EA_DATALEN   & 1 ) ) + 2 )
#define AL_EP_BUFSIZE	( AL_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + AL_EA_DATALEN   + ( AL_EA_DATALEN   & 1 ) ) + 2 )
#define BA_EP_BUFSIZE	( BA_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + BA_EA_DATALEN   + ( BA_EA_DATALEN   & 1 ) ) + 2 )
#define STAT_EP_BUFSIZE	( STAT_EP_ENTRIES * ( sizeof( ItemAdr_Typ ) + STAT_EA_DATALEN + ( STAT_EA_DATALEN & 1 ) ) + 2 )
#define ZS_EP_BUFSIZE	( ZS_EP_ENTRIES   * ( sizeof( ItemAdr_Typ ) + ZS_EA_DATALEN   + ( ZS_EA_DATALEN   & 1 ) ) + 2 )

/* Berechnung der erforderlichen Blockgroesse fuer einen Event-Pipeblock */
#define EP_BUFSIZE ( RK_EP_BUFSIZE + SGST_EP_BUFSIZE + SGQS_EP_BUFSIZE + SGIN_EP_BUFSIZE + AP_EP_BUFSIZE + SW_EP_BUFSIZE + ZL_EP_BUFSIZE + SB_EP_BUFSIZE + SBIM_EP_BUFSIZE + SBKL_EP_BUFSIZE + BP_EP_BUFSIZE + AL_EP_BUFSIZE + BA_EP_BUFSIZE + STAT_EP_BUFSIZE + ZS_EP_BUFSIZE )
#define EP_BLOCKSIZE ( sizeof(EvPbHead_Typ) + ( DT_ANZ * sizeof(PipeHead_Typ) ) + EP_BUFSIZE )


/* Union fuer verschiedene Datentypen */
typedef union {
	long Long;
	unsigned long Ulong;
	short Short;
	unsigned short Ushort;
	char Char[4];
	unsigned char Uchar[4];
} VarData_Typ;

/* Item-Adresse */
typedef struct {
	unsigned short DpIdx;
	unsigned char ItemOff;
	unsigned char ItemLen;
} ItemAdr_Typ;

/* Item-Adresse fuer CAB */
typedef struct {
	unsigned short DpIdx;
	unsigned char ItemIdx;
	unsigned char ItemLen;
} CabItemAdr_Typ;

/* Pipe-Header */
typedef struct {
	unsigned char *pStart;
	unsigned char *pEnd;
	unsigned char *pWrite;
	unsigned char *pRead;
	unsigned char WriteFlag;
	unsigned char ReadFlag;
} PipeHead_Typ;

/* Pipeblock-Header fuer bkinet */
typedef struct EvPbHead {
	unsigned char	UserID;
	PipeHead_Typ	*pDtPipe[DT_ANZ];
	struct EvPbHead	*pNext;
} EvPbHead_Typ;

/* Pipe-Eintrag für ZLT-Clients (fuer alle Items gleich !) ; neu in V4.01 */
typedef struct CliPipeEntry {
	CabItemAdr_Typ	ItemAdr;
	unsigned long	ItemVal;
} CliPipeEntry_Typ;

/* Pipe-Header für ZLT-Clients ; neu in V4.01 */
typedef struct {
	CliPipeEntry_Typ	*pFirst;
	CliPipeEntry_Typ	*pLast;
	CliPipeEntry_Typ	*pWrite;
	CliPipeEntry_Typ	*pRead;
	unsigned char 		WriteFlag;
	unsigned char 		ReadFlag;
} CliPipeHead_Typ;

/* Pipeblock-Header für ZLT-Clients ; neu in V4.00 */
typedef struct CliPbHead {
	CliPipeHead_Typ		*pDtPipe[DT_ANZ];
} CliPbHead_Typ;

/* Alloc-Struktur fuer EvParaBlock-Speicherverwaltung */
typedef struct {
	unsigned char *pEvParBase;
	unsigned long lEvParLen;
	unsigned char *pEvParAct;
	unsigned long lEvParFree;
} EvAlloc_Typ;

/* Pab-Header */
typedef struct {
	unsigned char	*pDtBase[DT_ANZ];
	unsigned short 	DpAnz[DT_ANZ];
} PabHeader_Typ ;

/* Cab-Header ; neu in V4.00 */
typedef struct {
	unsigned short 	ClientAnz;					/* Anzahl Clients */
	unsigned char 	CliLogon[CLI_ANZ_MAX]; 		/* Logon-Flag fuer jeden Client */
	unsigned char	*pDtBase[DT_ANZ];			/* Zeiger auf Daten in CAB (je DT) */
	unsigned short 	DpAnz[DT_ANZ];				/* Anzahl DP-Eintraege in CAB (je DT) */
	CliPbHead_Typ	*pClientPb[CLI_ANZ_MAX];	/* Array mit Zeigern auf Client-Pipebloecke in CAB */
} CabHeader_Typ ;

/* CAB-Itemeintrag ; neu in V4.00 */
typedef struct {
	unsigned char 	Link; 		/* Link-Flag fuer dieses Item */
	unsigned char 	Force; 		/* Kennung fuer ueberfaelligen Event */
	unsigned long 	LastVal;	/* Zuletzt an Client gemeldeter Wert */
	unsigned long	Hyst;		/* Hysteresewert fuer Eventueberwachung */
} CabItemEntry_Typ ;



/* Datentyp-Eigenschaften */
typedef struct {
	unsigned char  DtName[DTNAMELEN + 1];
	unsigned char  DtMaxItemIdx;
	unsigned char  DtDataLen;
	unsigned char  Ea;
	unsigned char  EaMaxItemIdx;
	unsigned char  EaDataLen;
	unsigned char  ItemConvMaxIdx;
	unsigned short DtPabLen;
	unsigned char  DpNameOff;
	unsigned char  DtIdxOff;
	unsigned char  DpIdxOff;
	unsigned char  AliasPtrOff;
	unsigned char  PabPtrOffAlias;
	unsigned char  InitOffAlias;
	unsigned char  AlMkOffAlias;
	unsigned char  QuStOffAlias;
	unsigned char  Quit2OffAlias;
	unsigned char  HandOffAlias;
	unsigned char  HaufOffAlias;
	unsigned char  HzuOffAlias;
	unsigned char  IntOffAlias;
	unsigned char  IntUserLevel;
	unsigned short EvCtrlOff;
	unsigned char  EvUserOff;
	unsigned char  EvNextOff;
	unsigned char  EvParLen;
	unsigned short EpBufsize;
	unsigned char  CabItemAnz;		/* Anzahl Itemeintraege in CAB ; neu in V4.00 */
	unsigned char  CabItemEntryLen;	/* Laenge eines Itemeintrages in CAB ; neu in V4.00 */
	unsigned char  CabPtrOff;		/* Offset des CAB-Zeigers im PAB ; neu in V4.00 */
} DtProps_Typ;

/* Elementart-Deskriptor mit Infos zu jedem Ea-Item */
typedef struct {
	unsigned char	Offset;
	unsigned char	Byteanz;
} EaDesc_Typ;

/* Datentyp-Deskriptor mit Infos zu jedem Dt-Item */
typedef struct {
	unsigned char	ItemName[ITEMNAMELEN + 1];
	unsigned char	ItemOff;
	char 			ItemLen;
	unsigned char	EvParOff;
	unsigned short	ItemOffPab;		/* Item-Offset im PAB ( 0 = kein PAB-Item */
	short			ItemDefault;
	unsigned char 	ItemNrCab;	/* Item-Nummer in CAB ( 0 = kein CAB-Item ) ; neu in V4.00 */
} DtDesc_Typ;

/* Item-Konvertierungstabelle ( Offset -> ItemIndex ) */
typedef struct {
	char ItemBk;
	char ItemBk2;
} ItemConv_Typ;

/* Pipe-Diagnose fuer einen Client; neu in V4.01 */
typedef struct {
	unsigned long 		AnyPipeEntry;			/* Summenzaehler fuer alle erfolgreichen ClientPipe-Eintraege */
	unsigned long 		AnyPipeEntryPerSec;		/* Anzahl erfolgreicher ClientPipe-Eintraege pro Sekunde */
	unsigned long 		AnyPipeFull; 			/* Summenzaehler fuer alle ClientPipe-Vollmeldungen */
	unsigned long 		DtPipeFull[DT_ANZ]; 	/* Zaehler fuer ClientPipe-Vollmeldungen je Datentyp */
	unsigned char 		DtPipeUse[DT_ANZ];	 	/* Fuellstandsanzeige der Pipe in Prozent */
} CliPipeDiagOne_Typ;

/* Gemeinsame Pipe-Diagnose fuer alle Clients; neu in V4.01 */
typedef struct {
	unsigned short		Init;					/* Initialisierungs-Flag */
	unsigned long		EntryCount;				/* Summenzaehler fuer alle erfolgreichen ClientPipe-Eintraege aller Clients */
	unsigned long		EntryCountPerSec;		/* Anzahl erfolgreicher ClientPipe-Eintraege aller Clients pro Sekunde */
	unsigned short		FullClient;				/* Client, dem die zuletzt als voll erkannte Pipe gehoert */
	unsigned short		FullDt;					/* Datentyp, dessen Pipe zuletzt als voll erkannt wurde */
	unsigned long		FullCount;				/* Summenzaehler fuer alle ClientPipe-Vollmeldungen aller CLients */
	CliPipeDiagOne_Typ	Client[CLI_ANZ_MAX];
} CliPipeDiagAll_Typ;

/* Struktur fuer globale Informationen */
typedef struct {
	unsigned char			PabValid;					/* Kennung PAB gültig */
	PabHeader_Typ			*pPab;						/* Zeiger auf PAB */
	unsigned long			PabLen;						/* Länge PAB */
	CabHeader_Typ			*pCab;						/* Zeiger auf CAB ; neu in V4.00 */
	unsigned long			CabLen;						/* Länge des gesamten CAB ; neu in V4.00 */
	EvPbHead_Typ			*pEvPipes;					/* Zeiger auf Event-Pipes */
	EvAlloc_Typ				*pEvAlloc;					/* Zeiger auf Arrays mit Event-Alloc-Infos */
	DtProps_Typ				*pDtProps;					/* Zeiger auf Array mit Datentyp-Eigenschaften */
	EaDesc_Typ				*pEaDesc[DT_ANZ];			/* Array mit Zeigern auf Elementart-Deskriptoren */
	DtDesc_Typ				*pDtDesc[DT_ANZ];			/* Array mit Zeigern auf Datentyp-Deskriptoren */
	ItemConv_Typ			*pItemConv[DT_ANZ];			/* Array mit Zeigern auf Item-Konvertierungstabellen */
	char					*pCabItemConv[DT_ANZ];		/* Array mit Zeigern auf CAB-Item-Konvertierungstabellen */
	unsigned short			Count10ms;					/* 10ms-Takt-Zaehler */
	unsigned short			Count100ms;					/* 100ms-Takt-Zaehler */
	unsigned char			UstNr;						/* Unterstations-Nummer */
	unsigned char			Oertl;						/* Oertlich */
	unsigned char			SammH;						/* Sammelhand */
	unsigned char			KplSt;						/* Kopplungsstoerung */
	unsigned char			ALnLe;						/* Alarmliste nicht leer */
	unsigned char			AlUnq;						/* Mindestens 1 unquittierter Alarm */
	unsigned char			OertlBkvist;				/* Oertlich von bkvist */
	unsigned char			OertlFremd;					/* Oertlich von Fremd-Visu */
	CliPipeDiagAll_Typ		*pCliPipeDiag;				/* Zeiger auf Pipe-Diagnose fuer alle Clients */
} Globals_Typ;

typedef struct {
	unsigned char Jahr;
	unsigned char Mon;
	unsigned char Tag;
	unsigned char Std;
	unsigned char Min;
	unsigned char Sek;
	unsigned short Milli;
} Timestamp_Typ;

/* Struktur fuer Fehler-Statistik */
typedef struct {
	unsigned short	LastWarn;
	unsigned short	LastErr;
	unsigned short	Init;
	Timestamp_Typ	LastInit;
	unsigned short	ErrCount[ERRCNTANZ];
} Stats_Typ;

/* Input-Struktur fuer InitTabItem() */
typedef struct {
	unsigned char EaItemAnz;
	EaDesc_Typ *pEaDesc;
	unsigned char DtItemAnz;
	DtDesc_Typ *pDtDesc;
	void *pAlias;
	void *pPab;
	void *pEvPb;
	ItemConv_Typ *pItemConv;
	char *pCabItemConv;
} InitTabInfo_Typ;


/* prototypes */
/* fuer bksysall --------------- */
void ErrStatistics( unsigned char *pTaskName, unsigned char ErrStat, unsigned short ErrNum, unsigned long ErrInfo, unsigned char *pErrPre, unsigned char *pErrText );
void GetGlobalsPtr( Globals_Typ **ppGlobals );
unsigned long bin_to_bcd_8 (unsigned long value);

/* fuer bksystools ------------- */
short GetDpInfoDt( char *pDpName, unsigned char DtIdx, unsigned short *pDpIdx, unsigned char **ppAlias );

/* fuer bksysfbk --------------- */
void ErrStatisticsX( unsigned char *pTaskName, unsigned short *pErrMk, unsigned short ErrBit, unsigned short *pSyErr, unsigned char ErrStat1, unsigned char ErrStatN, unsigned short ErrNum, unsigned long ErrInfo, unsigned char *pErrPre, unsigned char *pErrText );
short GetPabValidPtr( unsigned char **ppPabValid );
short GetSysClockPtr( unsigned short **ppC10, unsigned short **ppC100 );
void UserEvents( unsigned char DtIdx, ItemAdr_Typ ItemAdr, unsigned char EvUserId,
				 unsigned char *pPab, unsigned char *pFlags, unsigned char *pEvPara,
				 unsigned char *pOldBuf, unsigned char *pNewBuf, unsigned short DpMsgLen,
				 unsigned short sItemIdxMax, DtDesc_Typ *pDtDesc, EaDesc_Typ *pEaDesc,
				 unsigned char dT, unsigned char *pErrPre, unsigned short *pErrMask, unsigned short *pSysErr );
short CheckItemEvent( char *pVal, char *pValOld, void *pEvPara, unsigned short sEvItemOff, short sDataFormat, unsigned char dT100ms );
short MinMaxLong( long *pVal, long lMin, long lMax );
short MinMaxShort( short *pVal, short sMin, short sMax );
void SetAllEvParaData( unsigned short sItemIdxMax, unsigned char *pPab, unsigned char *pEvPara, DtDesc_Typ *pDtDesc );
void SetEvParaData( unsigned short sItem, unsigned char *pPab, unsigned char *pEvPara, DtDesc_Typ *pDtDesc );
short CopyPabToElBuf( unsigned char *pBuf, ItemAdr_Typ *pAdr, unsigned char *pPab, unsigned short sItemIdxMax, EaDesc_Typ *pEaDesc, DtDesc_Typ *pDtDesc );
short Kennlinie( short X, short Xmin, short Xmax, short Ymin, short Ymax );
void CliEvents( unsigned char DtIdx, unsigned short DpIdx, unsigned char *pPab, CabItemEntry_Typ *pCab, DtProps_Typ *pDtProps, DtDesc_Typ *pDtDesc );
short PipeWrite( PipeHead_Typ* pPipe, unsigned char* pMsg, unsigned short sLen );
short EvWrite( unsigned char UserID, unsigned char DtIdx, unsigned char* pMsg, unsigned short sLen );
short CliEvWrite( unsigned char Client, unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx, unsigned char ItemLen, unsigned long Value );

/* fuer bksysgetinfo ----------- */
short GetDpInfo( char *pDpName, unsigned char *pDtIdx, unsigned short *pDpIdx, unsigned char **ppAlias );
short GetDtDescInfos( unsigned char DtIdx, DtProps_Typ **ppDtProps,
					  EaDesc_Typ **ppEaDesc, unsigned char *pEaDescMaxIdx,
					  DtDesc_Typ **ppDtDesc, unsigned char *pDtDescMaxIdx,
					  ItemConv_Typ **ppItemConv, unsigned char *pItemConvMaxIdx );

/* fuer bksysgetindex ---------- */
short GetItemIndex(DtDesc_Typ *pDtDesc, char *ItemName, short MaxItemIdx);
short GetDtIndex(DtProps_Typ *pProps, char *DtName, short MaxDtIdx);

/* fuer bksysgetpipe ----------- */
short EvGetPipePtr( unsigned char UserID, unsigned char DtIdx, PipeHead_Typ** ppPipe );
short CliGetPipePtr( unsigned char Client, unsigned char DtIdx, CliPipeHead_Typ** ppPipe );

/* fuer bksysinet -------------- */
void ErrStatisticsY( unsigned char *pErrMk, unsigned short ErrBit, unsigned short *pSyErr, unsigned char ErrStat1, unsigned char ErrStatN, unsigned short ErrNum, unsigned long ErrInfo, unsigned char *pErrPre, unsigned char *pErrText );
short EvRead( EvPbHead_Typ* pEvBlock, unsigned char DtIdx, unsigned char* pMsg, unsigned short sSize, unsigned short* pLen );
short EvInit( unsigned char UserID, unsigned short AnzEvPar[DT_ANZ], EvPbHead_Typ** ppEvBlockHead );
short EvCreate( unsigned char UserID, unsigned char DtIdx, unsigned short DpIdx, unsigned char EvTyp, unsigned char cLink, unsigned char cFlag );
short EvParaInit( unsigned char UserID, unsigned char DtIdx, unsigned short DpIdx, unsigned char *pItemName, short Hyst, short Intvl );
short EvLink( unsigned char UserID, unsigned char EvTyp, unsigned char DtIdx, unsigned short DpIdx);
short EvCheckLink( unsigned char UserID, unsigned char EvTyp, unsigned char DtIdx, unsigned short DpIdx);
short EvSetFlag( unsigned char UserID, unsigned char EvTyp, unsigned char DtIdx, unsigned short DpIdx);
short EvPbCreate( unsigned char UserID, EvPbHead_Typ** ppEvBlock, DtProps_Typ *pProps, EvPbHead_Typ** ppEvBlockHead );
char GetUserIndex( unsigned char UserID);
void EvPipeInit( PipeHead_Typ* pPipe, short sBufSize );

/* fuer bksysbrtcp ------------- */
void ClientDiagnose( void );

/* fuer BkTcp ---------- */
short CliGetClientAnzMax( unsigned char *pCliAnzMax );
short CliGetDtAnz( unsigned char *pDtAnz );
short CliItemLink( unsigned char Client, unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx, unsigned char Link );
short CliItemLinkAll( unsigned char Client, unsigned char Link );
short CliSetItemHyst( unsigned char Client, unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx, unsigned char Len, unsigned char *pHyst );
short CliForceWriteEvent( unsigned char DtIdx, unsigned short DpIdx, unsigned char ItemIdx );
short CliEvRead( unsigned char Client, unsigned char DtIdx, unsigned short *pDpIdx, unsigned char *pItemIdx, unsigned char *pItemLen, unsigned long *pValue );
short CliAktValToCabVal( DtProps_Typ *pDtProps, DtDesc_Typ *pDtDesc, unsigned char *pPab, CabItemEntry_Typ *pCab, unsigned char ItemIdx );

#endif /* BKSYS_H_ */


