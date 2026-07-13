#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Burklimat 2000
*  Teil:     Parser-Funktionen
*  Datei:    parser.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 00.26
*  Datum:    26-09-2001
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Globale Include-Datei.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  00.10 | 03-11-99 | MKrz | Datei-Erstellung
*  00.25 | 26-07-00 | MKrz |
*        | 21-09-01 | Res  | burtrap ersetzt durch sys_lib
*  00.26 | 26-09-01 | Res  | #include <dvframe.h> entfernt (wird nicht benoetigt)
*  02.00 | 11-09-06 | Wai  | find_StationTypPoint: Verwendung von if/elseif statt switch
*							 wegen §-Bezeichner
******************************************************************************/


/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>		/* RPS-spezifische Typendeklarationen */
#include <sys_lib.h>		/* 21.09.01/RES: Ersatz fuer <burtrap.h> */

#include <string.h>			/* String-Library */

#include "parser.h"			/* Parser-spezifische Typendeklarationen */


/***********************************************************************/
/* Parser-Funktionen                                                   */
/***********************************************************************/

signed short find_StationTypPoint (unsigned char* pDmStart, unsigned char* pParser, unsigned char* pDmEnd,
									unsigned char** ppParser, unsigned char** ppParserLine,
									unsigned char** ppParserErr, unsigned short* pCountZeroTerms,
									unsigned char** ppDataArg1, unsigned short* pSizeArg1,
									unsigned char** ppDataArg2, unsigned short* pSizeArg2,
									unsigned short* pSectionType )
{
	unsigned char	*pDM;
	unsigned short	ParserState = 0;
	signed short	ParserError = FALSE;


	/* Pointer Arg1..Arg2 initialisieren */
	*ppDataArg1 = NULL;
	*ppDataArg2 = NULL;

	/* Textlängen der Arg1..Arg2 initialisieren */
	*pSizeArg1 = 0;
	*pSizeArg2 = 0;

	/* Line-Pointer initialisieren */
	if (pParser == pDmStart)
		*ppParserLine = pDmStart;
	else {
		*ppParserLine = pParser;
		while (*ppParserLine > pDmStart && !(**ppParserLine != 0 && *(*ppParserLine-1) == 0))
			(*ppParserLine)--;
	}

	/* Parse über Datenmodul */
	for (pDM = pParser; pDM < pDmEnd; pDM++, *ppParser = pDM, *ppParserErr = pDM)
	{
		/* Nullterminierungen des Datenmodules zählen */
		if (pCountZeroTerms != NULL) {
			if (*pDM == 0)
				(*pCountZeroTerms)++;
		}

		/* neuer Line-Pointer ? */
		if (pDM > pParser && *pDM != 0 && *(pDM-1) == 0)
			*ppParserLine = pDM;

		/* DFA */
		switch (ParserState)
		{
			/* Suche nach erstem Schlüsselwort */
			case 0:
				if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
					*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT)
				{
					switch ((char)*pDM)
					{
						case cKW_STATION:
							*pSectionType = cCD_STATION;
							break;

						case cKW_TYP:
							*pSectionType = cCD_TYP;
							break;

						case cKW_ITEM:
							*pSectionType = cCD_ITEM;
							break;

						case cKW_POINT:
							*pSectionType = cCD_POINT;
							break;
					}
					ParserState = 1;
					break;
				}
				else if (*pDM == cKW_OPTION)
				{
					ParserError = cPS_OPTION_WITHOUT_REF;
					ParserState = 5;
					break;
				}
				break;

			/* Suche nach Schlüsselwort - Bezeichner */
			case 1:
				if (*pDM == 0) {
					if (*ppDataArg1 != NULL) {
						ParserState = 2;
						break;
					}
					else {
						ParserError = cPS_NO_REF;
						ParserState = 5;
						break;
					}
					break;
				}
				else if (*pDM == ' ' || *pDM == '\t') {
					break;
				}
				else
				{
					if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
						*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
						*pDM == (unsigned char) cKW_OPTION || *pDM == (unsigned char) cKW_SECTION)
					{
						ParserError = cPS_KEYWORD_IN_REF;
						ParserState = 5;
						break;
					}
					else
					{
						/* Buchstaben eines Schlüsselwort-Bezeichners gefunden */
						if (*ppDataArg1 == NULL)
							*ppDataArg1 = pDM;

						/* Länge des Schlüsselwort-Bezeichners erhöhen */
						if (*ppDataArg1 != NULL)
							(*pSizeArg1)++;
						break;
					}
					break;
				}
				break;

			/* Suche nach optionalem Text */
			case 2:
				if (*pDM == (unsigned char) cKW_OPTION) {
					ParserState = 3;
					break;
				}
				if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
					*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
					*pDM == (unsigned char) cKW_SECTION)
				{
					ParserState = 4;
					break;
				}
				break;

			/* Suche nach optionellem Text - Bezeicher */
			case 3:
				if (*pDM == 0) {
					if (*ppDataArg2 != NULL) {
						ParserState = 4;
						break;
					}
					else {
						*ppDataArg2 = pDM;
						ParserState = 4;

					/*	ParserError = cPS_NO_OPTION;
						ParserState = 5;*/
						break;
					}
					break;
				}
				if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
					*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
					*pDM == (unsigned char) cKW_OPTION || *pDM == (unsigned char) cKW_SECTION)
				{
					ParserError = cPS_KEYWORD_IN_OPTION;
					ParserState = 5;
					break;
				}
				else
				{
					/* Buchstaben eines Schlüsselwort-Bezeichners gefunden */
					if (*ppDataArg2 == NULL)
						*ppDataArg2 = pDM;

						/* Länge des Schlüsselwort-Bezeichners erhöhen */
					if (*ppDataArg2 != NULL)
							(*pSizeArg2)++;
					break;
				}
				break;

			/* Paser korrekt beenden */
			case 4:
				break;

			/* Parser mit Fehlerzustand beenden */
			case 5:
				break;
		} /* (ParserState) */
		if (ParserState == 4 || ParserState == 5)
			break;
	} /* (pDM = pParser; pDM < pDmEnd; pDM++, *ppParser = pDM, *ppParserErr = pDM) */

	/* Nullterminierungen des Datenmodules nur einmal zählen */
	if (**ppParser == 0)
		(*ppParser)++;

	/* Pointer mindestens ein Zeichen weiter stellen */
	if (*ppParser == pParser)
		(*ppParser)++;

	/* DFA_Zustand: Suche nach optionalem Text */
	if (ParserState == 2)
		return TRUE;

	/* DFA_Endzustand ohne Fehler */
	if (ParserState == 4)
		return TRUE;

	/* DFA_Endzustand mit Fehler */
	if (ParserState == 5)
		return ParserError;

	/* DFA ohne Endzustand abgebrochen */
	return cPS_NO_DATA;
}


signed short find_Section (unsigned char* pDmStart, unsigned char* pParser, unsigned char *pDmEnd,
							unsigned char** ppParser, unsigned char** ppParserLine,
							unsigned char** ppParserErr, unsigned short* pCountZeroTerms,
							unsigned char** ppDataArg1, unsigned short* pSizeArg1,
							unsigned char** ppDataArg2, unsigned short* pSizeArg2,
							unsigned char* pSectionName )
{
	unsigned char	*pDM;
	unsigned short	ParserState = 0;
	signed short	ParserError = FALSE;
	unsigned short	i;
	size_t			lenSectionName;


	/* Pointer Arg1..Arg2 initialisieren */
	*ppDataArg1 = NULL;
	*ppDataArg2 = NULL;

	/* Textlängen der Arg1..Arg2 initialisieren */
	*pSizeArg1 = 0;
	*pSizeArg2 = 0;

	/* Line-Pointer initialisieren */
	if (pParser == pDmStart)
		*ppParserLine = pDmStart;
	else {
		*ppParserLine = pParser;
		while (*ppParserLine > pDmStart && !(**ppParserLine != 0 && *(*ppParserLine-1) == 0))
			(*ppParserLine)--;
	}

	/* Parse über Datenmodul */
	for (pDM = pParser; pDM < pDmEnd; pDM++, *ppParser = pDM, *ppParserErr = pDM)
	{
		/* Nullterminierungen des Datenmodules zählen */
		if (pCountZeroTerms != NULL) {
			if (*pDM == 0)
				(*pCountZeroTerms)++;
		}

		/* neuer Line-Pointer ? */
		if (pDM > pParser && *pDM != 0 && *(pDM-1) == 0)
			*ppParserLine = pDM;

		/* DFA */
		switch (ParserState)
		{
			/* Suche nach erstem Schlüsselwort */
			case 0:
				if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
					*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
					*pDM == (unsigned char) cKW_OPTION)
				{
					ParserError = cPS_NO_DATA;
					ParserState = 3;
					break;
				}
				if (*pDM == cKW_SECTION)
				{
					ParserState = 1;
					break;
				}
				break;

			/* Suche nach Schlüsselwort - Bezeichner */
			case 1:
				if (*pDM == 0) {
					if (*ppDataArg1 != NULL) {
						ParserState = 2;
						break;
					}
					else {
						ParserError = cPS_NO_SECTION;
						ParserState = 3;
						break;
					}
					break;
				}
				else if (*pDM == ' ' || *pDM == '\t') {
					break;
				}
				else
				{
					if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
						*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
						*pDM == (unsigned char) cKW_OPTION || *pDM == (unsigned char) cKW_SECTION)
					{
						if (*ppDataArg1 == NULL) {
							ParserError = cPS_KEYWORD_IN_REF;
							ParserState = 3;
							break;
						}
						else {
							ParserState = 2;
							break;
						}
						break;
					}
					else
					{
						if (*ppDataArg1 == NULL) {
							*ppDataArg1 = pDM;
							lenSectionName = strlen (pSectionName);
							for (i = 0; i < lenSectionName; i++) {
								if (*(pDM + i) != *(pSectionName + i)) {
									*ppDataArg1 = NULL;
									break;
								}
							}
							if (*ppDataArg1 != NULL) {
								*pSizeArg1 = lenSectionName;
								*ppDataArg2 = pDM + lenSectionName;
								*pSizeArg2 = lenSectionName;
								break;
							}
							else {
								/* Falsche Sektion */
								ParserState = 0;
								break;
							}
							break;
						}
						if (*ppDataArg1 != NULL) {

							/* Länge des Schlüsselwort-Bezeichners erhöhen */
							if (*ppDataArg2 != NULL && *ppDataArg2 < pDM)
								*pSizeArg2 = pDM - *ppDataArg2 + 1;
							break;
						}
						break;
					}
					break;
				}
				break;

			/* Paser korrekt beenden */
			case 2:
				break;

			/* Parser mit Fehlerzustand beenden */
			case 3:
				break;
		} /* (ParserState) */
		if (ParserState == 2 || ParserState == 3)
			break;
	} /* (pDM = pParser; pDM < pDmEnd; pDM++, *ppParser = pDM, *ppParserErr = pDM) */

	/* Nullterminierungen des Datenmodules nur einmal zählen */
	if (**ppParser == 0)
		(*ppParser)++;

	/* Nullterminierungen des Datenmodules nur einmal zählen */
	if (pCountZeroTerms != NULL) {
		if (*pDM == 0)
			(*pCountZeroTerms)--;
	}

	/* DFA_Endzustand ohne Fehler */
	if (ParserState == 2)
		return TRUE;

	/* DFA_Endzustand mit Fehler */
	if (ParserState == 3)
		return ParserError;

	/* DFA ohne Endzustand abgebrochen */
	return cPS_NO_DATA;
}


signed short find_Data (unsigned char* pDmStart, unsigned char* pParser, unsigned char *pDmEnd,
						 unsigned char** ppParser, unsigned char** ppParserLine,
						 unsigned char** ppParserErr, unsigned short* pCountZeroTerms,
						 unsigned char** ppDataArg1, unsigned short* pSizeArg1,
						 unsigned char** ppDataArg2, unsigned short* pSizeArg2,
						 unsigned char** ppDataArg3, unsigned short* pSizeArg3 )
{
	unsigned char	*pDM;
	unsigned short	ParserState = 0;
	signed short	ParserError = FALSE;


	/* Pointer Arg1..Arg3 initialisieren */
	*ppDataArg1 = NULL;
	*ppDataArg2 = NULL;
	*ppDataArg3 = NULL;

	/* Textlängen der Arg1..Arg3 initialisieren */
	*pSizeArg1 = 0;
	*pSizeArg2 = 0;
	*pSizeArg3 = 0;

	/* Line-Pointer initialisieren */
	if (pParser == pDmStart)
		*ppParserLine = pDmStart;
	else {
		*ppParserLine = pParser;
		while (*ppParserLine > pDmStart && !(**ppParserLine != 0 && *(*ppParserLine-1) == 0))
			(*ppParserLine)--;
	}

	/* Parse über Datenmodul */
	for (pDM = pParser; pDM < pDmEnd; pDM++, *ppParser = pDM, *ppParserErr = pDM)
	{
		/* Nullterminierungen des Datenmodules zählen */
		if (pCountZeroTerms != NULL) {
			if (*pDM == 0)
				(*pCountZeroTerms)++;
		}

		/* neuer Line-Pointer ? */
		if (pDM > pParser && *pDM != 0 && *(pDM-1) == 0)
			*ppParserLine = pDM;

		/* DFA */
		switch (ParserState)
		{
			/* Suche nach erstem Zeichen */
			case 0:
				/* Schlüsselwort gefunden */
				if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
					*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
					*pDM == (unsigned char) cKW_OPTION || *pDM == (unsigned char) cKW_SECTION)
				{
					ParserError = cPS_KEYWORD_IN_SECTION;
					ParserState = 5;
					break;
				}
				else if (*pDM == 0) {
					ParserError = cPS_NO_DATA;
					ParserState = 5;
					break;
				}
				else if (*pDM == ' ' || *pDM == '\t') {
					break;
				}
				else {
					/* Buchstaben eines Argument1 - Bezeichners gefunden */
					if (*ppDataArg1 == NULL)
						*ppDataArg1 = pDM;

					/* Länge des Argument1 - Bezeichners erhöhen */
					if (*ppDataArg1 != NULL)
						(*pSizeArg1)++;

					ParserState = 1;
					break;
				}
				break;

			/* Suche nach pre '.' - Bezeichner */
			case 1:
				if (*pDM == 0) {
					if (*ppDataArg1 != NULL) {
						ParserState = 4;
						break;
					}
					else {
						ParserError = cPS_NO_DATA;
						ParserState = 5;
						break;
					}
					break;
				}
				else if (*pDM == ' ' || *pDM == '\t') {
					if (*ppDataArg1 != NULL) {
						ParserState = 2;
					}
					break;
				}
				else if (*pDM == cKW_SEPARATOR) {
					if (*ppDataArg1 != NULL) {
						ParserState = 2;
						break;
					}
					else {
						ParserError = cPS_NO_DATA;
						ParserState = 5;
						break;
					}
					break;
				}
				else if (*pDM == cKW_ASSIGNMENT) {
					if (*ppDataArg1 != NULL) {
						ParserState = 3;
						break;
					}
					else {
						ParserError = cPS_NO_DATA;
						ParserState = 5;
						break;
					}
					break;
				}
				else
				{
					if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
						*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
						*pDM == (unsigned char) cKW_SECTION || *pDM == (unsigned char) cKW_OPTION)
					{
						ParserError = cPS_KEYWORD_IN_SECTION;
						ParserState = 5;
						break;
					}
					else
					{
						/* Buchstaben eines Argument1 - Bezeichners gefunden */
						if (*ppDataArg1 == NULL)
							*ppDataArg1 = pDM;

						/* Länge des Argument1 - Bezeichners erhöhen */
						if (*ppDataArg1 != NULL)
							(*pSizeArg1)++;
						break;
					}
					break;
				}
				break;

			/* Suche nach post '.' - Bezeichner */
			case 2:
				if (*pDM == 0)
				{
					if (*ppDataArg1 != NULL) {
						ParserState = 4;
						break;
					}
					else {
						ParserError = cPS_NO_DATA;
						ParserState = 5;
						break;
					}
					break;
				}
				else if (*pDM == ' ' || *pDM == '\t') {
					if (*ppDataArg3 != NULL) {
						ParserState = 3;
						break;
					}
				}
				else if (*pDM == cKW_SEPARATOR) {
					ParserError = cPS_UNEXPECTED_POINT;
					ParserState = 5;
					break;
				}
				else if (*pDM == cKW_ASSIGNMENT) {
					ParserState = 3;
					break;
				}
				else
				{
					if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
						*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
						*pDM == (unsigned char) cKW_SECTION || *pDM == (unsigned char) cKW_OPTION)
					{
						ParserError = cPS_KEYWORD_IN_SECTION;
						ParserState = 5;
						break;
					}
					else
					{
						/* Buchstaben eines Argument2 - Bezeichners gefunden */
						if (*ppDataArg2 == NULL)
							*ppDataArg2 = pDM;

						/* Länge des Argument2 - Bezeichners erhöhen */
						if (*ppDataArg2 != NULL)
							(*pSizeArg2)++;
						break;
					}
					break;
				}
				break;

			/* Suche nach post '=' - Bezeichner */
			case 3:
				if (*pDM == 0)
				{
					if (*ppDataArg3 != NULL) {
						ParserState = 4;
						break;
					}
					else {
						ParserError = cPS_NO_DATA;
						ParserState = 5;
						break;
					}
					break;
				}
				else if (*pDM == cKW_STRING)
				{
					ParserState = 6;
					break;
				}
				else if (*pDM == ' ' || *pDM == '\t')
				{
					if (*ppDataArg3 != NULL) {
						ParserState = 4;
					}
					break;
				}
				/*else if (*pDM == cKW_SEPARATOR) {
					ParserError = cPS_UNEXPECTED_POINT;
					ParserState = 5;
					break;
				}*/
				else if (*pDM == cKW_ASSIGNMENT) {
					ParserError = cPS_UNEXPECTED_ASSIGNMENT;
					ParserState = 5;
					break;
				}
				else
				{
					if (*pDM == (unsigned char) cKW_STATION || *pDM == (unsigned char) cKW_TYP ||
						*pDM == (unsigned char) cKW_ITEM || *pDM == (unsigned char) cKW_POINT ||
						*pDM == (unsigned char) cKW_SECTION || *pDM == (unsigned char) cKW_OPTION)
					{
						ParserError = cPS_KEYWORD_IN_SECTION;
						ParserState = 5;
						break;
					}
					else
					{
						/* Buchstaben eines Argument3 - Bezeichners gefunden */
						if (*ppDataArg3 == NULL)
							*ppDataArg3 = pDM;

						/* Länge des Argument3 - Bezeichners erhöhen */
						if (*ppDataArg3 != NULL)
							(*pSizeArg3)++;
						break;
					}
					break;
				}
				break;

			/* Suche nach post ''' - Bezeichner */
			case 6:
				if (*pDM == 0)
				{
					ParserError = cPS_NO_DATA;
					ParserState = 5;
					break;
				}
				else if (*pDM == cKW_STRING)
				{
					if (*ppDataArg3 != NULL) {
						(*ppParser)++;
						ParserState = 4;
						break;
					}
					else {
						ParserError = cPS_NO_DATA;
						ParserState = 5;
						break;
					}
					break;
				}
				else
				{
					/* Buchstaben eines Argument3 - Bezeichners gefunden */
					if (*ppDataArg3 == NULL)
						*ppDataArg3 = pDM;

					/* Länge des Argument3 - Bezeichners erhöhen */
					if (*ppDataArg3 != NULL)
						(*pSizeArg3)++;
					break;
				}
				break;

			/* Paser korrekt beenden */
			case 4:
				break;

			/* Parser mit Fehlerzustand beenden */
			case 5:
				break;
		} /* (ParserState) */
		if (ParserState == 4 || ParserState == 5)
			break;
	} /* (pDM = pParser; pDM < pDmEnd; pDM++, *ppParser = pDM, *ppParserErr = pDM) */

	/* Nullterminierungen des Datenmodules nur einmal zählen */
	if (pCountZeroTerms != NULL) {
		if (*pDM == 0)
			(*pCountZeroTerms)--;
	}

	/* DFA_Endzustand ohne Fehler */
	if (ParserState == 4)
		return TRUE;

	/* DFA_Endzustand mit Fehler */
	if (ParserState == 5)
		return ParserError;

	/* DFA ohne Endzustand abgebrochen */
	return cPS_NO_DATA;
}


