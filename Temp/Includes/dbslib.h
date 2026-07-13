/* Durch Automation Studio generierte Headerdatei*/
/* Nicht bearbeiten! */
/* dbslib 6.40.0 */

#ifndef _DBSLIB_
#define _DBSLIB_
#ifdef __cplusplus
extern "C" 
{
#endif
#ifndef _dbslib_VERSION
#define _dbslib_VERSION 6.40.0
#endif

#include <bur/plctypes.h>

#ifndef _BUR_PUBLIC
#define _BUR_PUBLIC
#endif
#ifdef _SG3
		#include "sys_lib.h"
		#include "FileIO.h"
#endif
#ifdef _SG4
		#include "sys_lib.h"
		#include "FileIO.h"
#endif
#ifdef _SGC
		#include "sys_lib.h"
		#include "FileIO.h"
#endif


/* Datentypen und Datentypen von Funktionsblöcken */
typedef struct FUB_REF
{	unsigned short dbsUnit;
	unsigned short dbsDevice;
	unsigned short dbsType;
	unsigned short dbsPoint;
	unsigned short dbsItem;
	unsigned short countItem;
	unsigned short countInstAll;
	unsigned short countInstNoTs;
	unsigned short countInstTs;
	unsigned char tsFub;
	unsigned char tsItem;
	unsigned short brstate;
	unsigned long pidBrServer;
	unsigned long this;
	unsigned short ErrorMask;
} FUB_REF;

typedef struct DbsGetBool
{
	/* VAR_INPUT (analog) */
	unsigned short Unit;
	unsigned short Device;
	unsigned short Type;
	unsigned short Point;
	unsigned short Item;
	/* VAR_OUTPUT (analog) */
	unsigned short Stat;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValue;
	plcbit DbsValid;
} DbsGetBool_typ;

typedef struct DbsGetInt
{
	/* VAR_INPUT (analog) */
	unsigned short Unit;
	unsigned short Device;
	unsigned short Type;
	unsigned short Point;
	unsigned short Item;
	/* VAR_OUTPUT (analog) */
	signed short DbsValue;
	unsigned short Stat;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
} DbsGetInt_typ;

typedef struct DbsUdint
{
	/* VAR_INPUT (analog) */
	unsigned long InValue;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	unsigned long DbsValue;
	unsigned long WriteValue;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
	plcbit TrgWriteValue;
} DbsUdint_typ;

typedef struct DbsDint
{
	/* VAR_INPUT (analog) */
	signed long InValue;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	signed long DbsValue;
	signed long WriteValue;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
	plcbit TrgWriteValue;
} DbsDint_typ;

typedef struct DbsInt
{
	/* VAR_INPUT (analog) */
	signed short InValue;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	signed short DbsValue;
	signed short WriteValue;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
	plcbit TrgWriteValue;
} DbsInt_typ;

typedef struct DbsUint
{
	/* VAR_INPUT (analog) */
	unsigned short InValue;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	unsigned short DbsValue;
	unsigned short WriteValue;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
	plcbit TrgWriteValue;
} DbsUint_typ;

typedef struct DbsUsintTs
{
	/* VAR_INPUT (analog) */
	unsigned char InValue;
	struct RTCtime_typ InTimeStamp;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	unsigned char DbsValue;
	struct RTCtime_typ DbsTimeStamp;
	unsigned char WriteValue;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
	plcbit TrgWriteValue;
} DbsUsintTs_typ;

typedef struct DbsUsint
{
	/* VAR_INPUT (analog) */
	unsigned char InValue;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	unsigned char DbsValue;
	unsigned char WriteValue;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValid;
	plcbit TrgWriteValue;
} DbsUsint_typ;

typedef struct DbsBoolTs
{
	/* VAR_INPUT (analog) */
	struct RTCtime_typ InTimeStamp;
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	struct RTCtime_typ DbsTimeStamp;
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit InValue;
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValue;
	plcbit DbsValid;
	plcbit WriteValue;
	plcbit TrgWriteValue;
} DbsBoolTs_typ;

typedef struct DbsBool
{
	/* VAR_INPUT (analog) */
	unsigned short Mode;
	/* VAR_OUTPUT (analog) */
	unsigned short Stat;
	/* VAR (analog) */
	struct FUB_REF FubRef;
	/* VAR_INPUT (digital) */
	plcbit InValue;
	plcbit SetValue;
	plcbit WriteEnable;
	/* VAR_OUTPUT (digital) */
	plcbit DbsValue;
	plcbit DbsValid;
	plcbit WriteValue;
	plcbit TrgWriteValue;
} DbsBool_typ;

typedef struct DbsRtcTime
{
	/* VAR_OUTPUT (analog) */
	struct RTCtime_typ RtcTime;
	unsigned short Stat;
} DbsRtcTime_typ;



/* Prototyping of functions and function blocks */
_BUR_PUBLIC void DbsGetBool(struct DbsGetBool* inst);
_BUR_PUBLIC void DbsGetInt(struct DbsGetInt* inst);
_BUR_PUBLIC void DbsUdint(struct DbsUdint* inst);
_BUR_PUBLIC void DbsDint(struct DbsDint* inst);
_BUR_PUBLIC void DbsInt(struct DbsInt* inst);
_BUR_PUBLIC void DbsUint(struct DbsUint* inst);
_BUR_PUBLIC void DbsUsintTs(struct DbsUsintTs* inst);
_BUR_PUBLIC void DbsUsint(struct DbsUsint* inst);
_BUR_PUBLIC void DbsBoolTs(struct DbsBoolTs* inst);
_BUR_PUBLIC void DbsBool(struct DbsBool* inst);
_BUR_PUBLIC void DbsRtcTime(struct DbsRtcTime* inst);


#ifdef __cplusplus
};
#endif
#endif /* _DBSLIB_ */

