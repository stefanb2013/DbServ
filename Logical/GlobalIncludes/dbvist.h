/************************************************************************/
/*	Headerfile für DbVist												*/
/*																		*/
/************************************************************************/
#include "pwi_lib.h"

/* Tasknamen für Versionsanzeige */
#define	TASK_DBSERV		"dbserv"
#define	TASK_DBSTCP		"dbstcp"
#define	TASK_DBVIST		"dbvist"
#define	TASK_IEC103		"iec103"
#define	TASK_L2DP		"l2dp"
#define	TASK_IODRV		"iodrv"
#define	TASK_GPS		"gps"

#define	MAX_ITEM			4000	/* max.Anzahl von Items */
#define ANZ_SCAN_AL			64		/* gescannte Alarme pro Zyklus */

#define	MENU_MAIN	1
#define	MENU_ITEM	2
#define	MENU_ALISTE	3
#define	MENU_NODP	5
#define	MENU_SYS	6
#define	MENU_CLOCK	7
#define	MENU_VERSION	8
#define MENU_MAX	10

#define KEY_OPT		33
#define KEY_F		34
#define KEY_STERN	35
#define KEY_ALISTE	39
#define KEY_AP		40
#define KEY_BP		41
#define KEY_AL		42
#define KEY_ZL		43
#define KEY_Q		48
#define KEY_SYS		51
#define KEY_PAGEUP	53
#define KEY_ONEUP	54
#define KEY_PLUS	55
#define KEY_PAGEDWN	56
#define KEY_ONEDWN	57
#define KEY_MINUS	58
#define KEY_UP		59
#define KEY_RIGHT	60
#define KEY_DOWN	61
#define KEY_LEFT	62
#define KEY_HOME	63
#define KEY_ENTER	65

typedef struct {
	STRING	time[32];
} ALTIME_typ;

/*
typedef struct {
	UINT	LifeCheck;
	UINT	TimeOut;
	INT		DefcurNo;
	INT		DefcmdNo;
	INT		NewcurNo;
	INT		NewcmdNo;
	USINT	DefownPage;
	USINT	NewownPage;
	SINT	DefcurPage;
	SINT	NewcurPage;
	USINT	triggerCur;
	USINT	disCursor;
	USINT	disCurLFRG;
	SINT	nextInpENo;
	SINT	nextInpESt;
	USINT	loopInput;
	USINT	disInput;
	USINT	disKey;
	USINT	signKey;
	USINT	dpChar;
	USINT	dpFix;
	USINT	hiddenChar;
	USINT	strInsert;
	USINT	srcKeyLim;
	USINT	curChar;
	USINT	curInput;
	SINT	curPosObj;
	USINT	signFormat;
	USINT	optNewPage;
	USINT	nNCnewPage;
	USINT	nNXnewPage;
	USINT	notValidChar;
} PWI_env;
*/
typedef struct {
	UINT	PwiStep;
	BOOL	okPWI;
	UDINT	idPWI;
	UINT	codePWI;
	UINT	infoPWINo;
	UINT	infoPWILn;
	UINT	waitCycles;
	UINT	stateOut;
	USINT	szTrends[256];
	USINT	SpeechEnab[256];
	UINT	SpeechAct;
	UINT	PageAct;
	BOOL	PageAgain;
	BOOL	PageReady;
	UINT	PageNew;
	UINT	SpeechNew;
	INT		ActcurNo;
	INT		ActcmdNo;
	INT		ActownPage;
	USINT	KeyFieldT[16];
	USINT	KeyField[16];
	USINT	KeyFieldP[16];
	USINT	KeyFieldO[16];
	BOOL	TableauOK;
	BOOL	keypress;
	BOOL	keypuls;
	USINT	keyold;
	USINT	keycode;
	USINT	keycodeTab;
	USINT	messageTab;
	BOOL	enterpuls;
	UINT	stateCanTX;
	UINT	stateCanRX;
	UDINT	idCAN;
	UINT	nodeCAN;
	UINT	errCAN;
	USINT	Device[32];
	UINT	stateRsTX;
	UINT	stateRsRX;
	UDINT	idRS;
	UINT	argc;
	UDINT	argv[32];
	USINT	keyuser;
	USINT	ASCIIuser;
	BOOL	changePuls;
	BOOL	changepuls;
	BOOL	inputopen;
	BOOL	funcInp;
	USINT	funcEnter;
	BOOL	funcInpRun;
	UINT	stateInp;

	UINT	codePwi;
	UINT	infoPwi1;
	UINT	infoPwi2;

	UINT	codeText;
	UINT	infoTextNo;
	UINT	infoTextLn;

	UINT	codeMask;
	UINT	infoMaskNo;
	UINT	infoMaskLn;

	UINT	codePage;
	UINT	infoPageNo;
	UINT	infoPageLn;

	UINT	codeObj;
	UINT	infoObjNo;
	UINT	infoObjLn;

	UINT	codeGrph;
	UINT	infoGrphNo;
	UINT	infoGrphLn;

	UINT	codeTrend;
	UINT	infoTrendNo;
	UINT	infoTrendLn;

	UINT	codeCan;
	UDINT	infoCan1;
	UDINT	infoCan2;

	UINT	codeRs;
	UDINT	infoRs1;
	UDINT	infoRs2;

	PWI_env	envPWI;
} PWI_INTERFACE;


typedef struct {
	STRING	vers_dbserv[40];
	STRING	vers_dbvist[40];
	STRING	vers_dbstcp[40];
	STRING	vers_iec103[40];
	STRING	vers_l2dp[40];
	STRING	vers_iodrv[40];
	STRING	vers_gps[40];
} VERSION_typ;

/*_GLOBAL	UINT	PageAct,PageNew,,SpeechAct,SpeechNew;
_GLOBAL	BOOL	keypuls,;
_GLOBAL	USINT	keycode;
*/
_LOCAL	PWI_INTERFACE	*pPWI_INTERFACE;
_LOCAL	UDINT	lenPWI_INTERFACE;
_LOCAL	UINT	PageLast;
_LOCAL	BOOL	keypuls_old;

_LOCAL	STRING	UstName[32],Filter[32],FilterUnten[41];
_LOCAL	USINT	enableItem[MAX_ITEM];
_LOCAL	INT		UstNr;

_LOCAL	UINT	testUINT;
_LOCAL	UINT	GrpIdx,DTypIdx,DPIdx;
_LOCAL	USINT	enableGrp,enableDT,enableDP,enableOGrp,enableODT,enableODP;
_LOCAL	USINT	enableGrp_old,enableDT_old,indexGrp_old,indexDT_old;
_LOCAL	BOOL	activGrp,activDT,activDP;
_LOCAL	BOOL	activGrp_old,activDT_old;
_LOCAL	BOOL	setClock,enableSet,activSet,setClockStarted;
_LOCAL	RTCtime_typ	RTCtime,RTCtimeSet;
_LOCAL	INT		indexSYS, indexGrp,indexDT,indexDP,indexItem,indexAP,indexBP,indexAL,indexZL,indexALL;
_LOCAL	USINT	minute_old;
_LOCAL	UINT	AnzDPfiltered,LastFiltered;

_LOCAL void (*pInitPic[MENU_MAX])();
_LOCAL void (*pCyclPic[MENU_MAX])();
_LOCAL void (*pKeysPic[MENU_MAX])();

_LOCAL	UDINT	myidBrServer;
_LOCAL	UINT	statePV_xgetadr,CheckPWIerrors	_VAR_RETAIN;
_LOCAL 	UINT	idstat,brstate;
_LOCAL	UINT	scanAL,countAL,aktiveAlarme,countALGRP[MAX_GRP],aktiveAlarmeGrp[MAX_GRP];
_LOCAL	BOOL	SammStoer,GrpStoer[MAX_GRP];
_LOCAL	USINT	LED_Aliste;
_LOCAL	USINT	*pData_extern, *pData_iextern, *pData_intern, *pData_valid, *pData_time, *pData_itime, *pData_writelink, *pData_ieventlink, *pData_ihyst, *pData_ievent;
_LOCAL	UDINT 	length_extern, length_intern, length_iextern, length_valid, length_time, length_itime, length_writelink, length_ieventlink, length_ihyst, length_ievent;
_LOCAL	UINT	infoerror,LatchStatusSetValueItem;
_LOCAL	BOOL	Quitt;
_LOCAL	UINT	quitIndex;
_LOCAL	VERSION_typ	Version;
_LOCAL MoVerStruc_typ MO_ver_struct;
_LOCAL	STRING	TestText1[256],TestText2[256];
_LOCAL	UINT	TestTextNr;



