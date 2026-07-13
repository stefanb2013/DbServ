/* Automation Studio generated header file */
/* Do not edit ! */

#ifndef _DPMASTER_
#define _DPMASTER_
#ifdef __cplusplus
extern "C" 
{
#endif

#include <bur/plctypes.h>

#include <runtime.h>

#ifndef _IEC_CONST
#define _IEC_CONST _WEAK const
#endif

/* Constants */
#ifdef _REPLACE_CONST
 #define WORKING 65535U
 #define INIT_FLAG 64U
 #define PDCOM_FLAG 4U
 #define RESET_FLAG 128U
 #define DEVCOM_FLAG 2U
 #define HOSTACK_FLAG 1U
 #define MAX_DP_BOARDS 8U
 #define NOTREADY_FLAG 32U
 #define STATEACK_FLAG 8U
 #define ERR_USR_NOIDENT 12182U
 #define ERR_USR_BADIDENT 12184U
 #define ERR_USR_DATA_LNG 12188U
 #define ERR_USR_INTERNAL 12181U
 #define ERR_USR_SLAVE_CONF 12189U
 #define host_reserved_flag 16U
 #define ERR_USR_NOFREEIDENT 12185U
 #define ERR_USR_WRONG_MODENR 12187U
 #define ERR_USR_DEV_MBOX_FULL 12180U
 #define ERR_USR_WRONG_MEMSIZE 12186U
 #define ERR_USR_INLEN_TOO_LONG 12192U
 #define ERR_USR_WRONG_DEVIDENT 12183U
 #define ERR_USR_OUTLEN_TOO_LONG 12193U
 #define ERR_USR_WRONG_HANDSHAKE 12191U
 #define ERR_USR_MASTER_IS_STOPPED 12190U
#else
 _IEC_CONST unsigned short WORKING = 65535U;
 _IEC_CONST unsigned char INIT_FLAG = 64U;
 _IEC_CONST unsigned char PDCOM_FLAG = 4U;
 _IEC_CONST unsigned char RESET_FLAG = 128U;
 _IEC_CONST unsigned char DEVCOM_FLAG = 2U;
 _IEC_CONST unsigned char HOSTACK_FLAG = 1U;
 _IEC_CONST unsigned char MAX_DP_BOARDS = 8U;
 _IEC_CONST unsigned char NOTREADY_FLAG = 32U;
 _IEC_CONST unsigned char STATEACK_FLAG = 8U;
 _IEC_CONST unsigned short ERR_USR_NOIDENT = 12182U;
 _IEC_CONST unsigned short ERR_USR_BADIDENT = 12184U;
 _IEC_CONST unsigned short ERR_USR_DATA_LNG = 12188U;
 _IEC_CONST unsigned short ERR_USR_INTERNAL = 12181U;
 _IEC_CONST unsigned short ERR_USR_SLAVE_CONF = 12189U;
 _IEC_CONST unsigned char host_reserved_flag = 16U;
 _IEC_CONST unsigned short ERR_USR_NOFREEIDENT = 12185U;
 _IEC_CONST unsigned short ERR_USR_WRONG_MODENR = 12187U;
 _IEC_CONST unsigned short ERR_USR_DEV_MBOX_FULL = 12180U;
 _IEC_CONST unsigned short ERR_USR_WRONG_MEMSIZE = 12186U;
 _IEC_CONST unsigned short ERR_USR_INLEN_TOO_LONG = 12192U;
 _IEC_CONST unsigned short ERR_USR_WRONG_DEVIDENT = 12183U;
 _IEC_CONST unsigned short ERR_USR_OUTLEN_TOO_LONG = 12193U;
 _IEC_CONST unsigned short ERR_USR_WRONG_HANDSHAKE = 12191U;
 _IEC_CONST unsigned short ERR_USR_MASTER_IS_STOPPED = 12190U;
#endif




/* Datatypes and datatypes of function blocks */
typedef struct DPDevice_typ
{	unsigned char DevType;
	unsigned char MemorySize;
	unsigned long DevAdress;
	unsigned long DevInfo;
	unsigned long DPIdent;
	unsigned short DevNumber;
	unsigned char ConfMsgNr;
	unsigned char CommandMsgNr;
	unsigned char StatusMsgNr;
	unsigned char OtherMsgNr;
} DPDevice_typ;

typedef struct DPMInit
{
	/* VAR_INPUT (analog) */
	unsigned long Device;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned char MemSize;
	unsigned long DevIdent;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit Run;
	plcbit Configuration;
} DPMInit_typ;

typedef struct DPMio
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned long OutBuffer;
	unsigned long OutLen;
	unsigned long InBuffer;
	unsigned long InLen;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR (analog) */
	unsigned char hmode;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMio_typ;

typedef struct DPMStop
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMStop_typ;

typedef struct DPMCommand
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned char RemoteAdress;
	unsigned char GroupSelect;
	unsigned char Command;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR (analog) */
	unsigned long istate;
	unsigned short waitCnt;
	unsigned char MessageHeader1;
	unsigned char MessageHeader2;
	unsigned char MessageHeader3;
	unsigned char MessageHeader4;
	unsigned char MessageHeader5;
	unsigned char MessageHeader6;
	unsigned char MessageHeader7;
	unsigned char MessageHeader8;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMCommand_typ;

typedef struct DPMConfig
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned long ConfModulName;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned long Remaining;
	/* VAR (analog) */
	unsigned long istate;
	unsigned short FileLength;
	unsigned short waitCnt;
	unsigned long OffsetCopy;
	unsigned long pData;
	unsigned char Ready;
	/* VAR_INPUT (digital) */
	plcbit enable;
	plcbit Mode;
	/* VAR (digital) */
	plcbit FirstMessage;
	plcbit MsgPutReady;
} DPMConfig_typ;

typedef struct DPMSlaveStat
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned long SlaveReady;
	unsigned long SlaveDiag;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit SlaveStatus;
} DPMSlaveStat_typ;

typedef struct DPMSlaveExtStat
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned char SlaveNumber;
	unsigned long SlaveDiagnostic;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR (analog) */
	unsigned long istate;
	unsigned short waitCnt;
	/* VAR_INPUT (digital) */
	plcbit enable;
	/* VAR_OUTPUT (digital) */
	plcbit SlaveReady;
	plcbit SlaveDiag;
} DPMSlaveExtStat_typ;

typedef struct DPMOff
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMOff_typ;

typedef struct DPMOn
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMOn_typ;

typedef struct DPMStart
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMStart_typ;

typedef struct DPMDialog
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned long Message;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR (analog) */
	unsigned long istate;
	unsigned char tx;
	unsigned short waitCount;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMDialog_typ;

typedef struct DPMGetMessage
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned long Message;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR (analog) */
	unsigned short waitCount;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMGetMessage_typ;

typedef struct DPMPutMessage
{
	/* VAR_INPUT (analog) */
	unsigned long DevIdent;
	unsigned long Message;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	/* VAR (analog) */
	unsigned short waitCount;
	/* VAR_INPUT (digital) */
	plcbit enable;
} DPMPutMessage_typ;



/* Prototyping of functions and function blocks */
void DPMInit(struct DPMInit* inst);
void DPMio(struct DPMio* inst);
void DPMStop(struct DPMStop* inst);
void DPMCommand(struct DPMCommand* inst);
void DPMConfig(struct DPMConfig* inst);
void DPMSlaveStat(struct DPMSlaveStat* inst);
void DPMSlaveExtStat(struct DPMSlaveExtStat* inst);
void DPMOff(struct DPMOff* inst);
void DPMOn(struct DPMOn* inst);
void DPMStart(struct DPMStart* inst);
void DPMDialog(struct DPMDialog* inst);
void DPMGetMessage(struct DPMGetMessage* inst);
void DPMPutMessage(struct DPMPutMessage* inst);
unsigned short DPMSearchDevices(unsigned long pInfos, unsigned char count);


#ifdef __cplusplus
};
#endif
#endif /* _DPMASTER_ */

                                                           
