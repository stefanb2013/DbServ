/****************************************************************************
*                                                                           *
*  $Workfile:: Packed.h                                                    $*
*  Titel    :  Textersatz fuer C-Compiler "packed" Anweisungen              *
*  Version  :  V1.00                                                        *
*  $Revision:: 11                                                          $*
*  $Date:: 6.05.10 9:24                                                    $*
*                                                                           *
*  BESCHREIBUNG:                                                            *
*  -------------                                                            *
*  Die unterschiedlichen Compiler benoetigen unterschiedliche Anweisungen   *
*  um Strukturen gepackt anzulegen.                                         *
*                                                                           *
******************************COPYRIGHT (C)**********************************
*     THIS SOFTWARE IS THE PROPERTY OF B&R AUSTRIA: ALL RIGHTS RESERVED.    *
*     NO PART OF THIS SOFTWARE MAY BE USED OR COPIED IN ANY WAY WITHOUT     *
*              THE PRIOR WRITTEN PERMISSION OF B&R AUSTRIA.                 *
*****************************************************************************
*                                                                           *
*  Projekt  : Automation Runtime                                            *
*  Datum    : 15.11.1999                                                    *
*  Autor    : Thomas Wuernschimmel                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
*  AENDERUNGEN:                                                             *
*  ------------                                                             *
*                                                                           *
*  Version  Datum       Aenderungsbeschreibung und Markierung               *
*           18.11.08 TW PACKED_TYPE fuer Microtec-C-Compiler                *
*           21.08.09 DH empty PACKED fuer GCC >= 4                          *
*                                                                           *
****************************************************************************/

#ifndef _PACKED_H_
#define _PACKED_H_

/* PACKED-definition for structure-members with GNU-C-Compiler (except mc68000 targets) */
#if (((!defined(__mc68000__)) || defined(_FORCE_68K_PACKED_)) && defined(__GNUC__))
	#if (__GNUC__ < 4)
		#define PACKED __attribute__((packed))
	#else
		/* empty definition for other Compilers (including GCC >= 4) */
		#define PACKED 
	#endif
#else
    /* empty definition for other Compilers */
    #define PACKED 
#endif

#endif  /* ! _PACKED_H_ */
