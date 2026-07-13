
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Burklimat 2000
*  Teil:     Parser-Funktionen
*  Datei:    bktcp.h
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    04-08-2003
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Globale Include-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 04-08-03 | MKrz | Datei-Erstellung
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/

#define FALSE	0	/* logisch falsch */
#define TRUE	1	/* logisch wahr */

/* Parser-Zustände */
enum {	cCD_PARSER_STATE_COUNT,				/* Parser-Status: Zählen der Datenpunkte */
		cCD_PARSER_STATE_DEF,				/* Parser-Status: Anmelden der Datenpunkte */
		cCD_PARSER_STATE_READY,				/* Parser-Status: Fertig */
		cCD_PARSER_STATE_ERROR,
		cCD_PARSER_STATE_ERROR_READY	};	/* Parser-Status: Fehler */

#define	cSIZE_UNIT_FILTER					32

/* Default-Werte */
#define cTCP_DEFAULT_PORT					20001	/* Default Port */
#define cTCP_DEFAULT_TIMEOUT				60		/* Default Timeout */

#define cTCP_DEFAULT_UNIT					1		/* Default Anlage */
#define cTCP_DEFAULT_DEVICE					1		/* Default Gerät */

/* Bezeichner */
#define cKW_TCP_PORT			"Client"	/* Port */
#define cKW_TCP_PORT_NUMBER		"Port"		/* Port-Nummer */

#define cKW_TCP_TIMEOUT			"Client"	/* Timeout */
#define cKW_TCP_TIMEOUT_NUMBER	"Timeout"	/* Timeout-Wert */

#define cKW_TCP_UNIT			"Anlage"	/* Anlage */
#define cKW_TCP_UNIT_NUMBER		"Nr"		/* Anlagen-Nummer */

#define cKW_TCP_POINT			"Dp"		/* Datenpunkt */
#define	cKW_TCP_POINT_NUMBER	"Nr"		/* Datenpunkt-Nummer */

#define cKW_TCP_DM_FIRST		"tcppara"	/* Datenmodul-Name 1 */
#define cKW_TCP_DM_SECOND		"dplist"	/* Datenmodul-Name 2 */

#define	cKW_TCP_STATION			"UST"		/* Schlüsselwort für Station */
#define	cKW_TCP_SECTION			"TCP"		/* Schlüsselwort für Tcp-Section */

/* Fehler-Nummern */
#define	EC_MEM_ALLOC_NEED					0	/* Fehler bei Allokieren von Speicher */
#define	ET_MEM_ALLOC_NEED					"MEM="

#define	EC_TMP_ALLOC_NEED					1	/* Fehler bei Allokieren von Speicher */
#define	ET_TMP_ALLOC_NEED					"kB, TMP="

#define	EC_DPS_FOUND						2	/* Fehler bei Allokieren von Speicher */
#define	ET_DPS_FOUND						"gef.DP="

#define	EC_DPS_NOT_FOUND					3	/* Fehler bei Allokieren von Speicher */
#define	ET_DPS_NOT_FOUND					", n.gef.DP="

#define	EC_TCP_READY						4	/* Fehler bei Allokieren von Speicher */
#define	ET_TCP_READY						" bereit"

#define	EC_TCP_NOT_READY					5	/* Fehler bei Allokieren von Speicher */
#define	ET_TCP_NOT_READY					" nicht bereit"

#define	EC_DA_NOT_FOUND						6	/* Fehler bei Allokieren von Speicher */
#define	ET_DA_NOT_FOUND						"Datenmodul nicht vorhanden"

#define	EC_TMP_ALLOC						7	/* Fehler bei Allokieren von Speicher */
#define	ET_TMP_ALLOC						"Fehler bei Allokieren von Speicher"

#define	EC_DP_FOUND							8	/* Fehler bei Allokieren von Speicher */
#define	ET_DP_FOUND							":DP gefunden: "

#define	EC_DP_NOT_FOUND						9	/* Fehler bei Allokieren von Speicher */
#define	ET_DP_NOT_FOUND						":DP nicht gefunden:"

#define	EC_TCP_VERS							10	/* Fehler bei Allokieren von Speicher */
#define	ET_TCP_VERS							"V"

#define	EC_TCP_NOT_VERS						11	/* Fehler bei Allokieren von Speicher */
#define	ET_TCP_NOT_VERS						"B"

#define	EC_PARSER_READY						12	/* Fehler bei Allokieren von Speicher */
#define	ET_PARSER_READY						":Parser bereit"

#define	EC_PARSER_STATE						13	/* Fehler im Parser-Zustand */
#define	ET_PARSER_STATE						":Syntax-Fehler -"

#define	EC_PARSER_STATIONTYPPOINT			14	/* Fehler im Parser-Zustand: Station/Typ/Punkt */
#define	ET_PARSER_STATIONTYPPOINT			":DP-Attr unbekannt"

#define	EC_PARSER_SECTION					15	/* Fehler im Parser-Zustand: Section */
#define	ET_PARSER_SECTION					":DP-Attr unbekannt"

#define	EC_PARSER_DATA						16	/* Fehler im Parser-Zustand: Data */
#define	ET_PARSER_DATA						":DP-Attr unbekannt"

#define	EC_NO_PGLOBALS						17	/* Fehler, da kein gültiger pGlobals-Pointer */
#define	ET_NO_PGLOBALS						":kein pGlobals"

#define	EC_PORT_ALREADY_EXISTS				18	/* Fehler, da Port bereits vergeben */
#define	ET_PORT_ALREADY_EXISTS				":Port bereits vorh."

#define	EC_TIMEOUT_ALREADY_EXISTS			19	/* Fehler, da Tiemout bereits vergeben */
#define	ET_TIMEOUT_ALREADY_EXISTS			":Timeout bereits vorh"

#define	EC_POINT_ALREADY_EXISTS				20	/* Fehler, da Datenpunkt bereits existiert */
#define	ET_POINT_ALREADY_EXISTS				":Dp bereits vorhanden"

#define	EC_MAX								21	/* Anzahl Fehlereinträge */

/***********************************************************************/
/* Datentypdeklaration                                                 */
/***********************************************************************/

typedef plcbit BIT;			/* Datentyp BIT */

/* ---------------------------------- */
/* Datentypen für Zugriff von Tcp aus */
/* ---------------------------------- */
typedef struct {
	long					TcpPoint;
	unsigned short			Bk2Point;
	unsigned char			*pAlias;
} TCP_ACCESS_BK2_POINT;

typedef struct {
	unsigned short			countTcpPoints;
	TCP_ACCESS_BK2_POINT	*pTcpAccessBk2Point;
} TCP_ACCESS_BK2_TYPE;

typedef struct {
	long					TcpUnit;
	unsigned long			countTcpUnitPoints;
	TCP_ACCESS_BK2_TYPE		*pTcpAccessBk2Type;
} TCP_ACCESS_BK2_UNIT;

typedef struct {
	unsigned short			countTcpUnits;
	TCP_ACCESS_BK2_UNIT		*pTcpAccessBk2Unit;
} TCP_ACCESS_BK2_SYSTEM;

/* ---------------------------------- */
/* Datentypen für Zugriff von Bk2 aus */
/* ---------------------------------- */
typedef struct {
	long					TcpUnit;
	long					TcpPoint;
	unsigned char			*pAlias;
} BK2_ACCESS_TCP_POINT;

typedef struct {
	unsigned short			countBk2Points;
	BK2_ACCESS_TCP_POINT	*pBk2AccessTcpPoint;
} BK2_ACCESS_TCP_TYPE;

typedef struct {
	unsigned short			countBk2Types;
	BK2_ACCESS_TCP_TYPE		*pBk2AccessTcpType;
} BK2_ACCESS_TCP_SYSTEM;

/***********************************************************************/
/* inetbk2-Funktionen                                                  */
/***********************************************************************/
unsigned char IsANumber(unsigned char *pData);
int ltoa(long value, char *buffer);
int ultoa(unsigned long value, char *buffer);

/*============================= Ende der Datei ==============================*/


