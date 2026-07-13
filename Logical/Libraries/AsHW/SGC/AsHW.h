/* Automation Studio generated header file */
/* Do not edit ! */

#ifndef _ASHW_
#define _ASHW_
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
 #define ASHW_BATTERY_OK 1U
 #define ASHW_BATTERY_LOW 0U
 #define ASHW_USER_LED_ON 1U
 #define ASHW_BATTERY_NOTEST 2U
 #define ASHW_BATTERY_MISSING 3U
 #define ERR_ASHW_INVALID_DEVICE 28850U
 #define ERR_ASHW_INVALID_ORDINAL 28851U
 #define ERR_ASHW_INVALID_BEEP_PARAMETER 28852U
#else
 _IEC_CONST unsigned char ASHW_BATTERY_OK = 1U;
 _IEC_CONST unsigned char ASHW_BATTERY_LOW = 0U;
 _IEC_CONST unsigned char ASHW_USER_LED_ON = 1U;
 _IEC_CONST unsigned char ASHW_BATTERY_NOTEST = 2U;
 _IEC_CONST unsigned char ASHW_BATTERY_MISSING = 3U;
 _IEC_CONST unsigned short ERR_ASHW_INVALID_DEVICE = 28850U;
 _IEC_CONST unsigned short ERR_ASHW_INVALID_ORDINAL = 28851U;
 _IEC_CONST unsigned short ERR_ASHW_INVALID_BEEP_PARAMETER = 28852U;
#endif




/* Datatypes and datatypes of function blocks */
typedef struct HwGetTemperature
{
	/* VAR_INPUT (analog) */
	unsigned long pDevice;
	unsigned char ordinal;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned long temperature;
	/* VAR (analog) */
	unsigned short i_state;
	unsigned short i_result;
	/* VAR_INPUT (digital) */
	plcbit enable;
} HwGetTemperature_typ;

typedef struct HwGetMode
{
	/* VAR_INPUT (analog) */
	unsigned long pDevice;
	unsigned char ordinal;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned char mode;
	/* VAR (analog) */
	unsigned short i_state;
	unsigned short i_result;
	/* VAR_INPUT (digital) */
	plcbit enable;
} HwGetMode_typ;

typedef struct HwGetNode
{
	/* VAR_INPUT (analog) */
	unsigned long pDevice;
	unsigned char ordinal;
	/* VAR_OUTPUT (analog) */
	unsigned short status;
	unsigned char node;
	/* VAR (analog) */
	unsigned short i_state;
	unsigned short i_result;
	unsigned long i_reserve;
	/* VAR_INPUT (digital) */
	plcbit enable;
} HwGetNode_typ;



/* Prototyping of functions and function blocks */
void HwGetTemperature(struct HwGetTemperature* inst);
void HwGetMode(struct HwGetMode* inst);
void HwGetNode(struct HwGetNode* inst);


#ifdef __cplusplus
};
#endif
#endif /* _ASHW_ */

                                                           
