#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Datum-/Uhrzeitfunktionen
*  Datei:    iec__103_date.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    26-11-2003
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion:
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 26-11-03 | KOB  | Datei-Erstellung
*        |          |      |
******************************************************************************/
#include <bur/plctypes.h>
#include <sys_lib.h>		/* SYS_LIB */
#include "iec__103_date.h"


char daytab[2][13] = {							/* Anzahl Tage pro Monat */
	{0,31,28,31,30,31,30,31,31,30,31,30,31},	/* Kein Schaltjahr */
	{0,31,29,31,30,31,30,31,31,30,31,30,31},	/* Schaltjahr */
};


/*-------------------------------------------------
  Pruefen, ob es sich um ein Schaltjahr handelt
  -----------------------------------------------*/

short int is_leap_year(short int year)
{
	return( ((year%4==0) && (year%100!=0)) || (year%400==0) );
}

/*-------------------------------------------------
  Pruefen, ob es sich um ein korrektes Datum handelt
  -----------------------------------------------*/

short int date_is_correct(RTCtime_typ *in_date)
{
	if((in_date->day<1)   || (in_date->day>31))   return(NOTVALID);
	if((in_date->month<1) || (in_date->month>12)) return(NOTVALID);
	return(VALID);
}

/*-------------------------------------------------
  Naechsten Tag ermitteln
  -----------------------------------------------*/

short int the_next_day(RTCtime_typ *in_date,RTCtime_typ *out_date)
{
	short int leap;

	if(date_is_correct(in_date)==VALID) {
		leap = is_leap_year(in_date->year);
		if(in_date->day >= daytab[leap][(short int) in_date->month]) {
			out_date->day = 1;
			if(in_date->month==12) {
				out_date->month	= 1;
				out_date->year	= in_date->year + 1;
			}
			else {
				out_date->month	= in_date->month +  1;
				out_date->year	= in_date->year;
			}
		}
		else {
			out_date->day	= in_date->day + 1;
			out_date->month	= in_date->month;
			out_date->year	= in_date->year;
		}

		return(VALID);
	}
	else {
		return(NOTVALID);
	}
}

/*-------------------------------------------------
  Vorhergehenden Tag ermitteln
  -----------------------------------------------*/

short int the_day_before(RTCtime_typ *in_date,RTCtime_typ *out_date)
{
	short int leap;

	if(date_is_correct(in_date)==VALID) {
		if(in_date->day==1) {
			if(in_date->month==1) {
				out_date->year	= in_date->year - 1;
				out_date->month = 12;
			}
			else {
				out_date->year	= in_date->year;
				out_date->month = in_date->month - 1;
			}
			leap = is_leap_year(out_date->year);
 			out_date->day = daytab[leap][(short int) out_date->month];
		}
		else {
			out_date->year	= in_date->year;
			out_date->month	= in_date->month;
			out_date->day	= in_date->day - 1;
		}
		return(VALID);
	}
	else {
		return(NOTVALID);
	}
}


/*** eof *****************************************************************************/


