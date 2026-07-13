/* Durch Automation Studio generierte Headerdatei*/
/* Nicht bearbeiten! */

#ifndef _BUR_1783687830_1_
#define _BUR_1783687830_1_

#include <bur/plctypes.h>

/* Datentypen und Datentypen von Funktionsblöcken */
typedef struct XstDiagnose
{	unsigned long ulFRM_rbufFailAnz;
	unsigned long ulFRM_readFailAnz;
	unsigned long ulWrongTelAnz;
	unsigned long ulDTExSetTimeFail;
	unsigned long ulTelTimeoutAnz;
	unsigned long ulSyncAnz;
	unsigned long ulRTC_gettimeFail;
	unsigned long ulSyncPosFail;
} XstDiagnose;

typedef struct XstMeinberg
{	unsigned char ucTag;
	unsigned char ucMonat;
	unsigned char ucJahr;
	unsigned char ucStunden;
	unsigned char ucMinuten;
	unsigned char ucSekunden;
	unsigned char ucSyncOK;
	unsigned char ucPosOK;
	unsigned char ucZZone;
	unsigned char ucZSprung;
} XstMeinberg;






__asm__(".section \".plc\"");

/* Verwendete IEC Dateien */
__asm__(".ascii \"iecfile \\\"Logical/Global.typ\\\" scope \\\"global\\\"\\n\"");

/* Exportierte Bibliotheksfunktionen und Funktionsbausteine */

__asm__(".previous");


#endif /* _BUR_1783687830_1_ */

