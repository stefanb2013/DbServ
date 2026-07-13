/* Durch Automation Studio generierte Headerdatei*/
/* Nicht bearbeiten! */
/* IEC103Ut 6.40.0 */

#ifndef _IEC103UT_
#define _IEC103UT_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _IEC103Ut_VERSION
#define _IEC103Ut_VERSION 6.40.0
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
/* Datentypen und Datentypen von Funktionsblöcken */
typedef struct IEC103Ut_WrCmd_PVInfo_Type
{	unsigned long PVAdr;
	unsigned long PVLen;
	unsigned long PVDataType;
	unsigned short PVDimension;
} IEC103Ut_WrCmd_PVInfo_Type;

typedef struct IEC103Ut_WrSimplePV
{
	/* VAR_INPUT (analog) */
	unsigned long PVAdr;
	unsigned long DataType;
	signed long PVValue;
	/* VAR (analog) */
	plcbit* pPV_BOOL;
	unsigned char* pPV_USINT;
	unsigned short* pPV_UINT;
	unsigned long* pPV_UDINT;
	signed char* pPV_SINT;
	signed short* pPV_INT;
	signed long* pPV_DINT;
	float* pPV_REAL;
	/* VAR_OUTPUT (digital) */
	plcbit valid;
} IEC103Ut_WrSimplePV_typ;

typedef struct IEC103Ut_RdSimplePV
{
	/* VAR_INPUT (analog) */
	unsigned long PVAdr;
	unsigned long DataType;
	/* VAR_OUTPUT (analog) */
	signed long PVValue;
	/* VAR (analog) */
	plcbit* pPV_BOOL;
	unsigned char* pPV_USINT;
	unsigned short* pPV_UINT;
	unsigned long* pPV_UDINT;
	signed char* pPV_SINT;
	signed short* pPV_INT;
	signed long* pPV_DINT;
	float* pPV_REAL;
	/* VAR_OUTPUT (digital) */
	plcbit valid;
} IEC103Ut_RdSimplePV_typ;

typedef struct IEC103Ut_WrCmd
{
	/* VAR_INPUT (analog) */
	unsigned long LineVarStr;
	unsigned char LineNumber;
	unsigned char DeviceAdr;
	unsigned char FunctionType;
	unsigned char InformationNumber;
	unsigned char Val;
	/* VAR_OUTPUT (analog) */
	signed long Status;
	/* VAR (analog) */
	unsigned short WrCmdStep;
	plcstring PVStr[101];
	plcstring ValStr[11];
	plcstring ItemStr[101];
	unsigned short PV_xgetadr_status;
	unsigned short PV_ninfo_status;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_AnlStruct;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_AnlStructOld;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_DataSend;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_DeviceAdress;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_FunctionType;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_InformationNumber;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_Val;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_Ready;
	struct IEC103Ut_WrCmd_PVInfo_Type PVInfo_Error;
	struct IEC103Ut_WrSimplePV IECIEC103Ut_WrSimplePV_0;
	struct IEC103Ut_RdSimplePV IEC103Ut_RdSimplePV_0;
	unsigned long LineVarStrOld;
	unsigned char LineNumberOld;
	/* VAR_INPUT (digital) */
	plcbit Enable;
	plcbit DataSend;
} IEC103Ut_WrCmd_typ;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC void IEC103Ut_WrCmd(struct IEC103Ut_WrCmd* inst);
_BUR_PUBLIC void IEC103Ut_WrSimplePV(struct IEC103Ut_WrSimplePV* inst);
_BUR_PUBLIC void IEC103Ut_RdSimplePV(struct IEC103Ut_RdSimplePV* inst);


/* Constants */
#ifdef _REPLACE_CONST
 #define IEC103Ut_DATA_TYPE_STRUCT 0U
 #define IEC103Ut_DATA_TYPE_STRUCT_2 15U
 #define IEC103Ut_DATA_TYPE_BOOL 1U
 #define IEC103Ut_DATA_TYPE_SINT 2U
 #define IEC103Ut_DATA_TYPE_INT 3U
 #define IEC103Ut_DATA_TYPE_DINT 4U
 #define IEC103Ut_DATA_TYPE_USINT 5U
 #define IEC103Ut_DATA_TYPE_UINT 6U
 #define IEC103Ut_DATA_TYPE_UDINT 7U
 #define IEC103Ut_DATA_TYPE_REAL 8U
 #define IEC103Ut_DATA_TYPE_STRING 9U
 #define IEC103Ut_DATA_TYPE_ULINT 10U
 #define IEC103Ut_DATA_TYPE_DATE_AND_TIME 11U
 #define IEC103Ut_DATA_TYPE_TIME 12U
 #define IEC103Ut_DATA_TYPE_DATE 13U
 #define IEC103Ut_DATA_TYPE_LREAL 14U
 #define IEC103Ut_FUB_IDLE 65533
 #define IEC103Ut_FUB_DISABLED 65534
 #define IEC103Ut_FUB_BUSY 65535
 #define IED103Ut_ERROR_START 50000
 #define IED103Ut_ERROR_INPUT_INVALID 52000
#else
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_STRUCT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_STRUCT_2;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_BOOL;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_SINT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_INT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_DINT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_USINT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_UINT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_UDINT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_REAL;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_STRING;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_ULINT;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_DATE_AND_TIME;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_TIME;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_DATE;
 _GLOBAL_CONST unsigned char IEC103Ut_DATA_TYPE_LREAL;
 _GLOBAL_CONST signed long IEC103Ut_FUB_IDLE;
 _GLOBAL_CONST signed long IEC103Ut_FUB_DISABLED;
 _GLOBAL_CONST signed long IEC103Ut_FUB_BUSY;
 _GLOBAL_CONST signed long IED103Ut_ERROR_START;
 _GLOBAL_CONST signed long IED103Ut_ERROR_INPUT_INVALID;
#endif




#ifdef __cplusplus
};
#endif
#endif /* _IEC103UT_ */

