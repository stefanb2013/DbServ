/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  Protokoll Treiber IEC 60870-5-103
*  Teil:     Datum-/Uhrzeitfunktionen
*  Datei:    iec__103_sub.c
*  Typ:      GNU-C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    26-11-2003
*  Autor:    KOB
*------------------------------------------------------------------------------
*  Funktion: Headerdatei
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  01.00 | 26-11-03 | KOB  | Datei-Erstellung
*        |          |      |
******************************************************************************/

#define VALID 	 1
#define NOTVALID 0

short int the_next_day(RTCtime_typ *in_date, RTCtime_typ *out_date);
short int the_day_before(RTCtime_typ *in_date, RTCtime_typ *out_date);
short int date_is_correct(RTCtime_typ *in_date);
short int is_leap_year(short int year);

/*** eof *****************************************************************************/


