
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Burklimat 2000
*  Teil:     Parser-Funktionen
*  Datei:    parser.h
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 00.25
*  Datum:    26-07-2000
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Globale Include-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  00.00 | 23-06-99 | MKrz | Datei-Erstellung
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Konstantendefinition                                                */
/***********************************************************************/

#define FALSE	0	/* logisch falsch */
#define TRUE	1	/* logisch wahr */

#define cKW_STATION		'&'		/* "&" Stations-Schlüssel */
#define cKW_TYP			'$'		/* "$" Typ-Schlüssel */
#define cKW_ITEM		'§'		/* "/" Item-Schlüssel */
#define cKW_POINT		'@'		/* "@" Punkt-Schlüssel */
#define cKW_OPTION		'~'		/* "~" Options-Schlüssel */
#define cKW_SECTION		'#'		/* "#" Sektions-Schlüssel */

#define cKW_ASSIGNMENT	'='		/* "=" Zuweisungs-Schlüssel */
#define cKW_SEPARATOR	'.'		/* "." Separations-Schlüssel */
#define cKW_STRING		'\''	/* "'" String-Schlüssel */

enum {	cCD_STATION,			/* '&' Stations-Schlüssel */
		cCD_TYP,				/* '$' Typ-Schlüssel */
		cCD_ITEM,				/* '%' Item-Schlüssel */
		cCD_POINT,				/* '@' Punkt-Schlüssel */
		cCD_OPTION,				/* '~' Otions-Schlüssel */
		cCD_SECTION	};			/* '#' Sektions-Schlüssel */

#define cPS_NO_DATA					-1		/* Keine gültigen Daten gefunden */
#define cPS_OPTION_WITHOUT_REF		-2		/* Optioneller Text ohne Station/Typ/Punkt */
#define cPS_NO_REF					-3		/* Station/Typ/Punkt - Bezeichner nicht vorhanden */
#define cPS_KEYWORD_IN_REF			-4		/* Schlüsselwort in Station/Typ/Punkt - Bezeichner */
#define cPS_NO_OPTION				-5		/* Optioneller Text - Bezeichner nicht vorhanden */
#define cPS_KEYWORD_IN_OPTION		-6		/* Schlüsselwort in optionellem Text */
#define cPS_NO_SECTION				-7		/* Sektions - Bezeichner nicht vorhanden */
#define cPS_KEYWORD_IN_SECTION		-8		/* Schlüsselwort in Sektion */
#define cPS_UNEXPECTED_POINT		-9		/* Unerwarteter Punkt */
#define cPS_UNEXPECTED_ASSIGNMENT	-10		/* Unerwartete Zuweisung */


/***********************************************************************/
/* Parser-Funktionen                                                   */
/***********************************************************************/

signed short find_StationTypPoint (unsigned char* pDmStart, unsigned char* pParser, unsigned char* pDmEnd,
									unsigned char** ppParser, unsigned char** ppParserLine,
									unsigned char** ppParserErr, unsigned short* pCountZeroTerms,
									unsigned char** ppDataArg1, unsigned short* pSizeArg1,
									unsigned char** ppDataArg2, unsigned short* pSizeArg2,
									unsigned short* pSectionType );

signed short find_Section (unsigned char* pDmStart, unsigned char* pParser, unsigned char *pDmEnd,
							unsigned char** ppParser, unsigned char** ppParserLine,
							unsigned char** ppParserErr, unsigned short* pCountZeroTerms,
							unsigned char** ppDataArg1, unsigned short* pSizeArg1,
							unsigned char** ppDataArg2, unsigned short* pSizeArg2,
							unsigned char* pSectionName );

signed short find_Data (unsigned char* pDmStart, unsigned char* pParser, unsigned char *pDmEnd,
						 unsigned char** ppParser, unsigned char** ppParserLine,
						 unsigned char** ppParserErr, unsigned short* pCountZeroTerms,
						 unsigned char** ppDataArg1, unsigned short* pSizeArg1,
						 unsigned char** ppDataArg2, unsigned short* pSizeArg2,
						 unsigned char** ppDataArg3, unsigned short* pSizeArg3 );


