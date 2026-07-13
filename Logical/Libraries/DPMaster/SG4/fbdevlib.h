/*************************************************************************
*                                                                        *
*	Name     :	FBLib.H													 *
*	Title    :	Incliudefile for the FB-Device-Libary und user interface *
*				definition							 			 		 *
*	Version  :	0.0                                                      *
*                                           							 *
*	Date     : 	03.11.1999		                                         *
*                                           							 *
*                                           							 *
***************************** COPYRIGHT (C)*******************************
*     THIS SOFTWARE IS THE PROPERTY OF B&R AUSTRIA: ALL RIGHTS RESERVED. *
*     NO PART OF THIS SOFTWARE MAY BE USED OR COPIED IN ANY WAY WITHOUT  *
*              THE PRIOR WRITTEN PERMISSION OF B&R AUSTRIA.              *
**************************************************************************
* Description:
* ------------
* Driverfunctions for the Hilscher system-modules and Device Poll Driver
**************************************************************************/

/**************************************************************************
*                        LIBARY-FUNCTIONS
*					for more information see below !
***************************************************************************
*   INT FBDevSearch(USINT DevType, INT *pDevBoardNumber, char *DevName)
*   INT FBDevOpen(char *DevName, UDINT* ident, UDINT *taskIdent)
*   INT FBDevClose(UDINT ident)
*   INT FBDevGetBoardInfo(UDINT ident, UINT Size, void *pData)
*   INT FBDevInitBoard(UDINT ident, FBDEV_INIT_BOARD *pInitBoard)
*   INT FBDevExitBoard(UDINT ident)
*   INT FBDevPutTaskParameter(UDINT ident,  FBDEV_TASKPARAM *pTaskParameter)
*	INT FBDevGetTaskParameter(UDINT ident, UINT Number, UINT Size, void *pData)
*	INT FBDevReset(UDINT ident, FBDEV_RESET *pReset)
*	INT FBDevPutMessage(UDINT ident, FBDEV_MSG_STRUC *pMessage)
*	INT FBDevGetMessage(UDINT ident, FBDEV_MSG_STRUC *pMessage)
*	INT FBDevGetTaskState(UDINT ident, UINT Number, UINT Size, void *pData)
*	INT FBDevGetMBXState(UDINT ident, FBDEV_MBX_STATE *pMbxState)
*	INT FBDevTriggerWatchDog(UDINT ident, FBDEV_TRIGGER_WATCHDOG *pWatchDog)
*	INT FBDevGetInfo(UDINT ident, UINT InfoArea, UINT Size, void *pData)
*	INT FBDevModulInfo(char *pModulName, char *pDataPtr, UDINT *pDataLng)
*	INT FBDevSendReceiveProcessData(UDINT ident, char *pSendData, UDINT sendStartOffset, UDINT sendDataSize,
*		char *pReceiveData, UDINT receiveStartOffset, UDINT receiveDataSize);
*	INT FBDevReadFlags(UDINT ident, USINT *HostFlags, USINT *DevFlags)
*	INT FBDevWriteFlags(UDINT ident, FBDEV_WRITE_FLAGS *pWriteFlags)
****************************************************************************/

#include <Packed.h>

/*************************************************************************
***								D E F I N E S						   ***
**************************************************************************/
#define	COM_IBM		0x96
#define	COM_IBM_40MHZ	0xB3
#define	COM_IBS		0xAB
#define	COM_DPM_2k	0x9B
#define	COM_DPM_8k	0xA6
#define	COM_DPS		0x9A
#define	COM_DPS_NEW	0xB5
#define	COM_COM		0xA8
#define	COM_COS		0xAC
#define	COM_DNM		0xA9
#define COM_DNS		0xAE
#define	COM_CNS		0xAD
#define	COM_SDS		0xA7
#define	COM_ASI		0xAF

#ifndef __GNUC__
#define MAX_DEV_BOARDS				8		/* maximum numbers of boards; DO NOT CHANGE*/
#else
#define MAX_DEV_BOARDS				4		/* maximum numbers of boards; DO NOT CHANGE*/
#endif
#define MAX_OPEN_BOARD				8		/* maximum opens to one board; DO NOT CHANGE*/


/*** host flags, written by device ***/
#define HOST_FLAG_HOSTCOM	 0x01
#define HOST_FLAG_DEVACK	 0x02
#define HOST_FLAG_PDACK		 0x04
#define HOST_FLAG_ERROR		 0x08
#define HOST_FLAG_PDEXTACK	 0x10
#define HOST_FLAG_COM		 0x20
#define HOST_FLAG_RUN		 0x40
#define HOST_FLAG_READY		 0x80

#define HOST_FLAG_READYRUN	 0xC0
#define HOST_FLAG_READYRUNIO 0xE0

/*** device flags, written by host ***/
#define DEV_FLAG_HOSTACK	 0x01
#define DEV_FLAG_DEVCOM		 0x02
#define DEV_FLAG_PDCOM		 0x04
#define DEV_FLAG_RESERVED	 0x08
#define DEV_FLAG_PDEXTCOM	 0x10
#define DEV_FLAG_NOTREADY	 0x20
#define DEV_FLAG_INIT		 0x40
#define DEV_FLAG_RESET		 0x80

#define DEV_FLAG_RESETINIT	 0xC0

/***		DEVRESET		***/
#define COLDSTART			2
#define WARMSTART			3
#define BOOTSTART           4

/***		DEVMBXINFO		***/
#define DEVICE_MBX_EMPTY	0
#define DEVICE_MBX_FULL		1
#define HOST_MBX_EMPTY		0
#define HOST_MBX_FULL		1

/***     TRIGGERWATCHDOG	***/
#define WATCHDOG_STOP		0
#define WATCHDOG_START		1

/*** GETINFO InfoArea definitions ***/
#define GET_DRIVER_INFO     1
#define GET_VERSION_INFO    2
#define GET_FIRMWARE_INFO   3
#define GET_TASK_INFO       4
#define GET_RCS_INFO        5
#define GET_DEV_INFO        6
#define GET_IO_INFO         7
#define GET_IO_SEND_DATA    8

/*************************************************************************
***							   driver errors						   ***
**************************************************************************/
#define DRV_NO_ERROR				 0		/* no error										*/
#define DRV_BOARD_NOT_INITIALIZED	-1		/* DRIVER board not initialized					*/
#define DRV_INIT_STATE_ERROR		-2		/* DRIVER error in internal init state			*/
#define DRV_READ_STATE_ERROR		-3		/* DRIVER error in internal read state			*/
#define DRV_CMD_ACTIVE				-4		/* DRIVER command on this chanal is activ		*/
#define DRV_PARAMETER_UNKNOWN		-5		/* DRIVER unknown parameter in function occured	*/
#define DRV_MESSAGE_QUEUED			-6		/* DRIVER FBDevPutMessage not dent, message queued */
#define DRV_WRONG_IDENT				-7		/* DRIVER wrong task ident */
#define DRV_TOO_MUCH_OPEN			-8		/* DRIVER too much open on one device */
#define DRV_WRONG_MESSAGEID			-9		/* DRIVER message id does not fit */

#define DRV_DEV_DPM_ACCESS_ERROR	-10		/* DEVICE dual port ram not accessable			*/
#define DRV_DEV_NOT_READY			-11		/* DEVICE not ready (ready flag failed)			*/
#define DRV_DEV_NOT_RUNNING			-12		/* DEVICE not running (running flag failed)		*/
#define DRV_DEV_WATCHDOG_FAILED		-13		/* DEVICE watch dog test failed					*/
#define DRV_DEV_OS_VERSION_ERROR	-14		/* DEVICE signals wrong OS version				*/
#define DRV_DEV_SYSERR				-15		/* DEVICE error in dual port flags				*/
#define DRV_DEV_MAILBOX_FULL		-16		/* DEVICE send mailbox is full					*/
#define DRV_DEV_PUT_TIMEOUT			-17		/* DEVICE FBDevPutMessage timeout				*/
#define DRV_DEV_GET_TIMEOUT			-18		/* DEVICE FBDevGetMessage timeout				*/
#define DRV_DEV_GET_NO_MESSAGE		-19		/* DEVICE no message available					*/
#define DRV_DEV_RESET_TIMEOUT		-20		/* DEVICE reset command timeout					*/
 
/*******		  Error from Interface functions		 ********/
#define DRV_USR_OPEN_ERROR			-30		/* USER driver not oppend						*/
#define DRV_USR_INIT_DRV_ERROR		-31		/* USER can't connect with DEV board			*/
#define DRV_USR_NOT_INITIALIZED		-32		/* USER board not initialized					*/
#define DRV_USR_COMM_ERR			-33		/* USER IOCTRL function faild					*/
#define DRV_USR_DEV_NUMBER_INVALID	-34		/* USER parameter for DEV number invalid		*/
#define DRV_USR_INFO_AREA_INVALID	-35		/* USER parameter InfoArea unknown				*/
#define DRV_USR_NUMBER_INVALID		-36		/* USER parameter Number invalid				*/
#define DRV_USR_MODE_INVALID		-37		/* USER parameter Mode invalid					*/
#define DRV_USR_MSG_BUF_NULL_PTR	-38		/* USER NULL pointer assignment					*/
#define DRV_USR_MSG_BUF_TOO_SHORT	-39		/* USER message buffer too short				*/
#define DRV_USR_SIZE_INVALID		-40		/* USER size parameter invalid					*/
#define DRV_USR_SIZE_ZERO			-42		/* USER size parameter with zero length			*/
#define DRV_USR_SIZE_TOO_LONG		-43		/* USER size parameter too long					*/
#define DRV_USR_DEV_PTR_NULL		-44		/* USER device address null pointer				*/
#define DRV_USR_BUF_PTR_NULL		-45		/* USER pointer to buffer is a null pointer		*/

#define DRV_USR_DEV_TYP_INVALID		-100	/* USER device-typ is not a hilscher board		*/
#define DRV_USR_DEV_ADRESS_INVALID	-101	/* USER device-adress is invalid				*/ 
#define DRV_USR_ERR_SS_NOTFOUND		-102	/* USER entry not found							*/
#define DRV_USR_ERR_SS_NOENTRY		-103	/* USER entrynumber is invalid					*/
#define DRV_USR_ERR_SS_NOTAB		-104	/* USER table number out of range				*/
#define DRV_USR_OPEN_DRV_ERROR		-105	/* USER the device-name was not correct			*/

/*************************************************************************
***								message definition					   ***
**************************************************************************/
#pragma pack(1)


/***		max. length is 288 Bytes		***/
typedef struct tagMSG_STRUC
{
	USINT		rx                  PACKED ;
	USINT		tx                  PACKED ;
	USINT		ln                  PACKED ;
	USINT		nr                  PACKED ;
	USINT		a                   PACKED ;
	USINT		f                   PACKED ;
	USINT		b                   PACKED ;
	USINT		e                   PACKED ;
	USINT		DeviceAdr           PACKED ;
	USINT		DataArea            PACKED ;
	UINT		DataAdr             PACKED ;
	USINT		DataIdx             PACKED ;
	USINT		DataCnt             PACKED ;
	USINT		DataType            PACKED ;
	USINT		Fnc                 PACKED ;
	USINT		data[ 0x120 - 16 ]  PACKED ;
} MSG_STRUC;

typedef struct tagFBDEV_MSG_STRUC
{
	UDINT taskIdent             PACKED ;
	USINT messageIdent          PACKED ;
	USINT reserve	            PACKED ;
	UINT size                   PACKED ;
	UDINT timeout               PACKED ;
	MSG_STRUC *pMessage         PACKED ;
} FBDEV_MSG_STRUC;

typedef struct tagFBDEV_RESET
{
	UDINT taskIdent             PACKED ;
	UINT mode                   PACKED ;
	UDINT timeout               PACKED ;
} FBDEV_RESET;

typedef struct tagFBDEV_TASKPARAM
{
	UDINT taskIdent             PACKED ;
	UINT taskNumber             PACKED ;
	UINT size                   PACKED ;
	void *pData                 PACKED ;
} FBDEV_TASKPARAM;

typedef struct tagFBDEV_MBX_STATE
{
	UINT deviceMBXState         PACKED ;
	UINT hostMBXState           PACKED ;
} FBDEV_MBX_STATE;

typedef struct tagFBDEV_WRITE_FLAGS
{
	UDINT taskIdent             PACKED ;
	USINT deviceFlags           PACKED ;
} FBDEV_WRITE_FLAGS;

#pragma pack()


/*************************************************************************
***					   INFO structure definitions					   ***
**************************************************************************/
#pragma pack(1)


/* Device exchange IO information */
typedef struct tagIOINFO 
{
  USINT   bComBit            PACKED ;        /* Actual state of the COM bit                */
  USINT   bIOExchangeMode    PACKED ;        /* Actual data exchange mode (0..5)           */
  UDINT   ulIOExchangeCnt    PACKED ;        /* Exchange IO counter                        */
} IOINFO;

typedef struct tagVERSIONINFO		/* DEV serial number and OS versions */
{
	UDINT	ulDate                PACKED ;
	UDINT	ulDeviceNo            PACKED ;
	UDINT	ulSerialNo            PACKED ;
	UDINT	ulReserved            PACKED ;
	USINT	abPcOsName0[4]        PACKED ;
	USINT	abPcOsName1[4]        PACKED ;
	USINT	abPcOsName2[4]        PACKED ;
	USINT	abOemIdentifier[4]    PACKED ;
} VERSIONINFO;

typedef struct tagFIRMWAREINFO
{
	USINT	abFirmwareName[16]    PACKED ;
	USINT	abFirmwareVersion[16] PACKED ;
} FIRMWAREINFO;

typedef struct tagTASKSTATE
{
	USINT	abTaskState[64]       PACKED ;
} TASKSTATE;

typedef struct tagTASKPARAM
{
	USINT	abTaskParameter[64]   PACKED ;
} TASKPARAM;

typedef	struct tagTaskInfo
	{
		USINT	abTaskName[8]       PACKED ;		/* Task name						 */
		UINT	usTaskVersion       PACKED ;		/* Task version						 */
		USINT	bTaskCondition      PACKED ;
		USINT	abreserved[5]       PACKED ;		/* n.c.								 */
  } STASKINFO;

typedef struct tagTASKINFO
{
	STASKINFO  tTaskInfo [7]    PACKED ;
} TASKINFO;

typedef struct tagRCSINFO
{
	UINT	usRcsVersion          PACKED ;			/* Operationsystem Version			 */
	USINT	bRcsError             PACKED ;
	USINT	bHostWatchDog         PACKED ;
	USINT	bDevWatchDog          PACKED ;
	USINT	bSegmentCount         PACKED ;
	USINT	bDeviceAdress         PACKED ;
	USINT	bDriverType           PACKED ;
} RCSINFO;

typedef struct tagDEVINFO
{
	USINT	bDpmSize              PACKED ;
	USINT	bDevType              PACKED ;
	USINT	bDevModel             PACKED ;
	USINT	abDevIdentifier[3]    PACKED ;
} DEVINFO;

#pragma pack()

/*************************************************************************
***							DEV DPM structure						   ***
**************************************************************************/
#pragma pack(1)


typedef struct tagDPB_MEMORY_2K
{
	USINT			SendProcessData[512];		/* Send process data image				*/
	USINT			ReceiveProcessData[512];	/* Receive process data image			*/
} DPM_MEMORY_2K;

typedef struct tagDPB_MEMORY_8K
{
	USINT			SendProcessData[3584];		/* Send process data image				*/
	USINT			ReceiveProcessData[3584];	/* Receive process data image			*/
} DPM_MEMORY_8K;

#pragma pack()

typedef struct tagDPB_LASTMEMORY
{
	MSG_STRUC		tDevMbx;		/* Mailbox PC  --> DEV					(288 Bytes) */
	VERSIONINFO		tDevVersion;	/* DEV verion information				( 32 Bytes) */
	MSG_STRUC		tPcMbx;			/* Mailbox DEV --> PC					(288 Bytes) */
	FIRMWAREINFO	tFiwInfo;		/* Firmware info						( 32 Bytes) */
	TASKPARAM		tKpt1Param;		/* Task 1 interface parameter			( 64 Bytes) */
	TASKPARAM		tKpt2Param;		/* Task 2 interface parameter			( 64 Bytes) */
	TASKSTATE		tKpt1State;		/* Task 1 state							( 64 Bytes) */
	TASKSTATE		tKpt2State;		/* Task 2 state							( 64 Bytes) */
	TASKINFO		tTaskInfo;		/* Task 1 to 7 info field				(112 Bytes) */
	RCSINFO			tRcsInfo;		/* RCS information						(  8 Bytes) */
	DEVINFO			tDevInfo;		/* DEV information						(  6 Bytes) */
	USINT			HostFlags;		/* DPM communication DEV->PC(PcFlags)	(  1 Byte ) */
	USINT			DevFlags;		/* DPM communication PC->DEV(CifFlags)	(  1 Byte ) */
} DPM_LASTMEMORY;


/*************************************************************************
***					driver info structure definitions				   ***
**************************************************************************/
#pragma pack(1)


typedef struct tagBOARD_INFO
{
	USINT abDriverVersion[16];		/* DRV driver information string	 */
	struct tagBOARD
	{
		UINT	usBoardNumber        PACKED ;		/* DRV board number					 */
		UINT	usAvailable          PACKED ;		/* DRV board is available			 */
		UDINT	ulPhysicalAddress    PACKED ;		/* DRV physical DPM address			 */
		UINT	usIrqNumber          PACKED ;		/* DRV irq number					 */
	} tBoard [MAX_DEV_BOARDS];
} BOARD_INFO;

typedef struct tagDRIVERINFO
{
	UDINT	ulOpenCnt              PACKED ;		/* number of driver open			 */
	UDINT	ulCloseCnt             PACKED ;		/* number of driver close			 */
	UDINT	ulReadCnt              PACKED ;		/* number of DevGetMessage commands	 */
	UDINT	ulWriteCnt             PACKED ;		/* number of DevPutMessage commands	 */
	UDINT	ulIRQCnt               PACKED ;		/* number of IRQs					 */
	USINT	bInitMsgFlag           PACKED ;		/* DPM state init					 */
	USINT	bReadMsgFlag           PACKED ;		/* DPM state read  message			 */
	USINT	bWriteMsgFlag          PACKED ;		/* DPM state write message			 */
	USINT	bLastFunction          PACKED ;		/* DRV last function in driver		 */
	USINT	bWriteState            PACKED ;		/* DRV actual write state			 */
	USINT	bReadState             PACKED ;		/* DRV actual read state			 */
	USINT	bHostFlags             PACKED ;		/* DPM HostFlags					 */
	USINT	bMyDevFlags            PACKED ;		/* DPM (internal) DevFlags			 */
	USINT	bExIOFlag			   PACKED ;		/* Actual IO flags */
	UDINT	ulExIOCnt              PACKED ;     /* DevExchangeIO() counter */     /* NEU !!!!! */
} DRIVERINFO;

typedef struct tagDEVICEINFOS
{
	USINT			bDeviceType			PACKED ;
	USINT			abDeviceString[32]	PACKED ;
	VERSIONINFO		tVersionInfo;
	FIRMWAREINFO	tFwInfo;
	DEVINFO			tDeviceInfo;
	RCSINFO			tRcsInfo;
	DRIVERINFO		tDriverInfo;
	BOARD_INFO		tBoardInfo;
} DEVICEINFOS;

#pragma pack()


/*************************************************************************
***							function prototypes						   ***
**************************************************************************/

/*************************************************************************
* Description:
*     search for Hilscher-modules
* Parameters:
*     DevType          - hardwaretype                 - inputparameter
*     pDevBoardNumber - number of the device board   - in/out parameter
*     DevName         - declarationstring for search - outputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function searchs for the different kinds of Hilscher-Boards 
*     on the systembus. The variable pDevBoardNumber is a input-/
*     outputparameter. With this parameter you can search more then	 
*     one Hilscher-modules. On first call this parameter is 0. For a	 
*     new search you must enter the return value.						 
**************************************************************************/
INT FBDevSearch(USINT DevType, INT *pDevBoardNumber, char *DevName);
INT FBDevSearchDevices(DEVICEINFOS *pDeviceInfos, USINT count);
/*************************************************************************
* Description:
*     open communication link with a Hilscher board
* Parameters:
*     DevName    - declarationstring for search - inputparameter
*     ident      - ident for other funktions    - outputparameter		
*     taskID     - taks ident for this instance - outputparameter		
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     If an application wants to communicate with a board, it must
*     call this function first. This function checks if the device
*     is available and opens a link to it. Once an link is opened,
*     all other functions can be used. Call FBDevClose() to close	
*     the link.													
**************************************************************************/
INT FBDevOpen(char *DevName, UDINT* ident, UDINT *taskIdent);
/*************************************************************************
* Description:
*     closes the connection to the DEV driver
* Parameters:
*     ident            - ident from FBDevOpen   - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     Close an open link to the device driver. An application has to
*     call this function before it ends.
**************************************************************************/
INT FBDevClose(UDINT ident, UDINT taskID);
/*************************************************************************
* Description:
*     reads special board informations from the driver
* Parameters:
*     ident            - ident from FBDevOpen          - inputparameter
*     Size             - data size                     - inputparameter
*     pData            - data structure form the board - outputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     With FBDevGetBoardInfo(), the user can read global information
*     of all com-munication boards the device driver knows.
*     The users interface offers the user a data structure which
*     describes the board in-formation data. The function copies the
*     number of data, given in the parameter Size.
*     This function can be used after FBDevOpen() and before
*     opening a specific DEVICE with the FBDevInitBoard() function.
**************************************************************************/
INT FBDevGetBoardInfo(UDINT ident, UINT Size, void *pData);
/*************************************************************************
* Description:
*     initializes a device-board
* Parameters:
*     ident            - ident from FBDevOpen   - inputparameter
*     pInitBoard       - data structue for init - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     After an application has opened a link to the device driver, it
*     must call FBDevInitBoard() before it can start with the
*     communication.
*     FBDevInitBoard() tells the device driver that an application
*     wants to work with a defined board. The device driver checks if
*     the board is physical available, if the board works properly
*     and setup up all the internal state flags for the ad-dressed
*     board.
**************************************************************************/
INT FBDevInitBoard(UDINT ident, UDINT taskIdent);
/*************************************************************************
* Description:
*     closes the connection to a DEV board
* Parameters:
*     ident            - ident from FBDevOpen   - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     If an application wants to end communication it has to call
*     FBDevExitBoard(). for each board which has been opened by a
*     previous call to FBDevInitBoard(). These function frees all
*     internal driver structures and unlink itself from the
*     communication board.
**************************************************************************/
INT FBDevExitBoard(UDINT ident);
/*************************************************************************
* Description:
*     write communication parameters
* Parameters:
*     ident            - ident from FBDevOpen        - inputparameter
*     pTaskParameter   - data structue for parameter - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function hands over parameter to a task. This is only
*     possible, if the protocol picks up the parameters of the DPM.
*     The parameters in the DPM will only be taken over from the
*     tasks with the next WARMRESET.
**************************************************************************/
INT FBDevPutTaskParameter(UDINT ident,  FBDEV_TASKPARAM *pTaskParameter);
/*************************************************************************
* Description:
*     read parameter area from a task
* Parameters:
*     ident     - ident from FBDevOpen                          - inputparameter
*     Number    - taskno. of the parameter area (1,2)           - inputparameter
*     Size      - size of the parameter area ( >0, <= max.Size) - inputparameter
*     pData     - pointer to task parameter buffer              - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
**************************************************************************/
INT FBDevGetTaskParameter(UDINT ident, UINT Number, UINT Size, void *pData);
/*************************************************************************
* Description:
*     reset a DEV board
* Parameters:
*     ident            - ident from FBDevOpen    - inputparameter
*     pReset           - data structue for reset - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function provokes a reset on a communication board. The
*     passed parameter Mode switches a coldstart or a warmstart.
*     The amount of the timeout Timeout depends on the used protocol
*     and reset mode. A coldstart needs a longer time then a warmstart
*     because there will be made a complete hardware check by the
*     device operating system. Usually the time for a coldstart will
*     be between 3 and 10 seconds, a warmstart needs between 2 and 8
*     seconds.
**************************************************************************/
INT FBDevReset(UDINT ident, FBDEV_RESET *pReset);
/*************************************************************************
* Description:
*     send a message to a DEV board
* Parameters:
*     ident            - ident from FBDevOpen      - inputparameter
*     pMessage         - data structue for message - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function sends (transfers) a message to the communication
*     board. The function copies the number of data, given in the
*     length entry (msg.ln) of the message structure and the message
*     header.
*     If no timeout (Timeout = 0) is used, the function returns
*     immediately. The return code shows if the function was able to
*     write the message to the device or not.
*     If a timeout (Timeout != 0) is used and the send mailbox of
*     the device is empty, the message is written to the mailbox and
*     the function returns also immediately. If the mailbox is full,
*     the function will wait until the mailbox is free. If this does
*     not happen during the timeout duration, the function returns
*     with an error code.
*     How the timeout is realized depends on the mode the DEVICE is
*     configured. Polling mode will run a loop in the driver while
*     waiting the timeout duration. In interrupt mode the calling
*     application will blocked to free the CPU for other work.
**************************************************************************/
INT FBDevPutMessage(UDINT ident, FBDEV_MSG_STRUC *pMessage);
/*************************************************************************
* Description:
*     read a message from a DEV board
* Parameters:
*     ident            - ident from FBDevOpen      - inputparameter
*     pMessage         - data structue for message - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function reads a message out of the dual port memory (DPM)
*     of a communication board and puts it into the data buffer that
*     is given by the user. The function checks if the message fits
*     in the users data buffer. This is done by comparing the
*     parameter usSize with the length which is given in the message
*     structure. If the message doesn't fit, the function will fail
*     and returns an error.
*     If no timeout (Timeout = 0) is used, the function returns
*     immediately. The return code shows if the function was able to
*     read a message from the device or not.
*     If a timeout (Timeout != 0) is used and a message is
*     available, the function reads the message and returns also
*     immediately. If no message is available, the function will wait
*     until a message is available. If this does not happen during the
*     timeout duration, the function returns with an error code. How
*     the timeout is realized depends on the mode	the	DEVICE is
*     configured. Polling mode will run a loop in the driver while
*     waiting the timeout duration In interrupt mode the calling
*     application will blocked to free the CPU for other work.
**************************************************************************/
INT FBDevGetMessage(UDINT ident, FBDEV_MSG_STRUC *pMessage);
/*************************************************************************
* Description:
*     read a task state field form a DEV board
* Parameters:
*     ident        - ident from FBDevOpen             - inputparameter
*     Number       - number of the status field (1,2) - inputparameter
*     Size         - size of the users data buffer    - inputparameter
*     pData	       - pointer to users data buffer     - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function reads one of the task state areas of a DEVICE. The
*     data will be transferred into the user data buffer. The function
*     copies the number of data, given in the parameter usSize.
**************************************************************************/
INT FBDevGetTaskState(UDINT ident, UINT Number, UINT Size, void *pData);
/*************************************************************************
* Description:
*     test send and receive MBX
* Parameters:
*     ident            - ident from FBDevOpen        - inputparameter
*     pMbxState        - data structue for MBX state - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function reads the actual state of the host and device
*     mailbox of a communication board.
*     You can use this function for writing applications to poll the
*     device without waiting for device events.
**************************************************************************/
INT FBDevGetMBXState(UDINT ident, FBDEV_MBX_STATE *pMbxState);
/*************************************************************************
* Description:
*     triggers the PC watchdog an delivers the actual DEV watchdog
* Parameters:
*     ident            - ident from FBDevOpen        - inputparameter
*     pWatchDog        - data structue for watchdog  - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     The FBDevTriggerWatchdog() command can be used to check the
*     device operating system for normal operation.
*     The parameter function determines what action on the boards
*     watchdog should be done (WATCHDOG_START, WATCHDOG_STOP).
*     The function reads the PcWatchDog/RPSWatchDog- cell and write
*     it to the FBDevWatchDog cell of the DPM.
*     With writing a number unequal to zero in the FBDevWatchDog cell
*     of the DPM, the watchdog function of the board is activated.
*     Since the watchdog is activated, the application must trigger
*     the watchdog within the time which is defined in the protocols
*     database.
**************************************************************************/
INT FBDevTriggerWatchDog(UDINT ident, UINT mode);
/*************************************************************************
* Description:
*     read the differnt information areas from a DEV board
* Parameters:
*     ident            - ident from FBDevOpen            - inputparameter
*     InfoArea         - 1..n, inputparameter            - inputparameter
*     Size             - size of the users data buffer   - inputparameter
*     pData	           - pointer to users data buffer    - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function reads the various information out of the DPM of a
*     communication board and the driver internal state information
*     for a board. The information that can be read are as followed:
*
*     Driver state information            GET_DRIVER_INFO
*     Board version information           GET_VERSION_INFO
*     Board firmware information          GET_FIRMWARE_INFO
*     Task information area	              GET_TASK_INFO
*     Board operation system information  GET_RCS_INFO
*     Device information area             GET_DEV_INFO
*     Device IO information               GET_IO_INFO
*     Device IO send data                 GET_IO_SEND_DATA
*
*     The function copies the number of data, given in the parameter
*     Size. The information areas which are located in DPM of a
*     board are defined in the device documentation. For each area you
*     can find a structure definition in the user interface header
*     file.
**************************************************************************/
INT FBDevGetInfo(UDINT ident, UINT InfoArea, UINT Size, void *pData);
/*************************************************************************
* Description:
*     get the module information of a config/fw-file
* Parameters:
*     pModulName       - name of the config-/firmwaremodule - inputparameter
*     pDataPtr         - begin of data section              - outputparameter
*     pDataLng         - length of the date section         - outputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
*     This function finds the begin and the length of the data-section
*     in module.
**************************************************************************/
INT FBDevModulInfo(char *pModulName, char *pDataPtr, UDINT *pDataLng);
/*************************************************************************
* Description:
*     reads the host- and devflags
* Parameters:
*     ident            - ident from FBDevOpen            - inputparameter
*     HostFlags        - the hostflags of the DPM        - outputparameter
*     DevFlags         - the devflags of the DPM         - outputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
* Remarks:
**************************************************************************/
INT FBDevReadFlags(UDINT ident, USINT *HostFlags, USINT *DevFlags);
/*************************************************************************
* Description:
*     overwrites the devflags
* Parameters:
*     ident            - ident from FBDevOpen          - inputparameter
*     pWatchDog        - data structue for write flags - inputparameter
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
**************************************************************************/
INT FBDevWriteFlags(UDINT ident, FBDEV_WRITE_FLAGS *pWriteFlags);
/*************************************************************************
* Description:
*     sends/receives processdata
* Parameters:
*     ident                - ident from FBDevOpen          - inputparameter
*     pSendData            - data for send			       - inputparameter
*     sendStartOffset      - offset in send area of device - inputparameter 
*     sendDataSize         - datasize for send             - inputparameter 
*     pReceiveData         - pointer to users data buffer  - inputparameter 
*     receiveStartOffset   - offset in the receive area of device - input-/outputparameter
*     receiveDataSize      - datasize for receive          - inputparameter
*
* Return Value:
*     DRV_NO_ERROR     - no error
*     !DRV_NO_ERROR    - funktion was not successful
**************************************************************************/
INT FBDevSendReceiveProcessData(UDINT ident, char *pSendData, UDINT sendStartOffset, UDINT sendDataSize,
	char *pReceiveData, UDINT receiveStartOffset, UDINT receiveDataSize);
/*************************************************************************
* Description:
*     test if device is ready
* Parameters:
*     ident            - ident from FBDevOpen   - inputparameter
* Return Value:
*     FALSE            - device is not ready
*     TRUE             - device is ready
**************************************************************************/
UINT FBDevTestReady(UDINT ident); 
INT FBRemoveMessageLock(UDINT ident, UDINT taskID);
void FBDevMemcpy(void *destination, const void *source, unsigned int size);
void FBDevUsecLoop(unsigned long time);
