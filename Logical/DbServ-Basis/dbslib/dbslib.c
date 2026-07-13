
/******************************************************************************
*    COPYRIGHT    BERNECKER + RAINER Industrie-Elektronik Ges.m.b.H           *
*******************************************************************************
*  Projekt:  DbServ
*  Teil:     Header-File
*  Datei:    bsrun.h
*  Typ:      GNU C (32 Bit)
*------------------------------------------------------------------------------
*  Revision: 01.00
*  Datum:    26-05-2003
*  Autor:    Martin Kurtz / MKrz
*------------------------------------------------------------------------------
*  Funktion: Generelle Definitionen.
*------------------------------------------------------------------------------
*  Rev.  | Datum    | Name | Kommentar
*  ------+----------+------+---------------------------------------------------
*  02.04 | 21-01-08 | MKrz | Aenderung der Zuweisung des Status-Ausgangs bei
*        |          |      | den FBKs DbsGetBool() und DbsGetInt()
*  01.00 | 14-07-03 | MKrz | Modul-Erstellung
*        |          |      |
******************************************************************************/


/***********************************************************************/
/* Headerfiles einbinden                                               */
/***********************************************************************/

#include <bur\plc.h>	/* RPS-spezifische Typendeklarationen */
#include <string.h>		/* String-Library */
#include <sys_lib.h>	/* System-Library */
#include <fileio.h>

#include "dbserv.h"
#include "bksys.h"			/* Burklimat-Library */
#include <dbslib.h>


/*=============================================================================
// Funktionsblock: DbsRtcTime
//-----------------------------------------------------------------------------
// Auslesen der Echtzeituhr.
//=============================================================================*/

void DbsRtcTime(DbsRtcTime_typ* inst) {

	RTCtime_typ 	IRtcTime;


	inst->Stat = RTC_gettime(&IRtcTime);
	inst->RtcTime = IRtcTime;

} /* DbsRtcTime */


/*=============================================================================
// Funktionsblock: DbsBool
//-----------------------------------------------------------------------------
// BOOL Zuordnung zum dbserv.
//=============================================================================*/

void DbsBool(DbsBool_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	BOOL			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = FALSE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_BOOL_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_BOOL_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_BOOL_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						case 0:
							/* Einzelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 1:
							/* Einzelmeldung invertitert */
							InValue = !inst->InValue;
							break;
						case 2:
							/* Einzelmeldung als Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 1;
									break;
								case 1:
									InValue = 2;
									break;
								default:
									break;
							}
							break;
						case 3:
							/* Einzelmeldung als invertierte Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 2;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									break;
							}
							break;
						case 10:
							/* Doppelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 11:
							/* Doppelmeldung invertier auf Doppelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 2;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 12:
							/* Doppelmeldung auf Einzelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 0;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 13:
							/* Doppelmeldung invertiert auf Einzelmeldung */
							switch (inst->InValue) {
								case 2:
									InValue = 0;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(BOOL)) {
							inst->DbsValue = *(BOOL*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(BOOL)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(BOOL*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(BOOL),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(BOOL)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsBool */


/*=============================================================================
// Funktionsblock: DbsBoolTs
//-----------------------------------------------------------------------------
// BOOL Zuordnung mit Zeitstempel zum dbserv.
//=============================================================================*/

void DbsBoolTs(DbsBoolTs_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	BOOL			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = TRUE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_BOOL_TS_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_BOOL_TS_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_BOOL_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						case 0:
							/* Einzelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 1:
							/* Einzelmeldung invertitert */
							InValue = !inst->InValue;
							break;
						case 2:
							/* Einzelmeldung als Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 1;
									break;
								case 1:
									InValue = 2;
									break;
								default:
									break;
							}
							break;
						case 3:
							/* Einzelmeldung als invertierte Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 2;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									break;
							}
							break;
						case 10:
							/* Doppelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 11:
							/* Doppelmeldung invertier auf Doppelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 2;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 12:
							/* Doppelmeldung auf Einzelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 0;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 13:
							/* Doppelmeldung invertiert auf Einzelmeldung */
							switch (inst->InValue) {
								case 2:
									InValue = 0;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											inst->InTimeStamp, TRUE, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(BOOL)) {
							inst->DbsValue = *(BOOL*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
							inst->DbsTimeStamp = *pDataTime;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(BOOL)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(BOOL*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(BOOL),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(BOOL)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsTsBool */


/*=============================================================================
// Funktionsblock: DbsUsint
//-----------------------------------------------------------------------------
// USINT Zuordnung zum dbserv.
//=============================================================================*/

void DbsUsint(DbsUsint_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	USINT			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = FALSE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_USINT_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_USINT_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_USINT_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						case 0:
							/* Einzelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 1:
							/* Einzelmeldung invertitert */
							InValue = !inst->InValue;
							break;
						case 2:
							/* Einzelmeldung als Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 1;
									break;
								case 1:
									InValue = 2;
									break;
								default:
									break;
							}
							break;
						case 3:
							/* Einzelmeldung als invertierte Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 2;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									break;
							}
							break;
						case 10:
							/* Doppelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 11:
							/* Doppelmeldung invertier auf Doppelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 2;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 12:
							/* Doppelmeldung auf Einzelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 0;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 13:
							/* Doppelmeldung invertiert auf Einzelmeldung */
							switch (inst->InValue) {
								case 2:
									InValue = 0;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(USINT)) {
							inst->DbsValue = *(USINT*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(USINT)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(USINT*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(USINT),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(USINT)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsUsint */


/*=============================================================================
// Funktionsblock: DbsUsintTs
//-----------------------------------------------------------------------------
// USINT Zuordnung mit Zeitstempel zum dbserv.
//=============================================================================*/

void DbsUsintTs(DbsUsintTs_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	USINT			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = TRUE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_USINT_TS_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_USINT_TS_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_USINT_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						case 0:
							/* Einzelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 1:
							/* Einzelmeldung invertitert */
							InValue = !inst->InValue;
							break;
						case 2:
							/* Einzelmeldung als Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 1;
									break;
								case 1:
									InValue = 2;
									break;
								default:
									break;
							}
							break;
						case 3:
							/* Einzelmeldung als invertierte Doppelmeldung */
							switch (inst->InValue) {
								case 0:
									InValue = 2;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									break;
							}
							break;
						case 10:
							/* Doppelmeldung unverändert */
							InValue = inst->InValue;
							break;
						case 11:
							/* Doppelmeldung invertier auf Doppelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 2;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 12:
							/* Doppelmeldung auf Einzelmeldung */
							switch (inst->InValue) {
								case 1:
									InValue = 0;
									break;
								case 2:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						case 13:
							/* Doppelmeldung invertiert auf Einzelmeldung */
							switch (inst->InValue) {
								case 2:
									InValue = 0;
									break;
								case 1:
									InValue = 1;
									break;
								default:
									InValue = inst->InValue;
									break;
							}
							break;
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											inst->InTimeStamp, TRUE, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(USINT)) {
							inst->DbsValue = *(USINT*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
							inst->DbsTimeStamp = *pDataTime;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(USINT)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(USINT*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(USINT),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(USINT)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsTsUsint */


/*=============================================================================
// Funktionsblock: DbsUint
//-----------------------------------------------------------------------------
// UINT Zuordnung zum dbserv.
//=============================================================================*/

void DbsUint(DbsUint_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	UINT			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = FALSE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_UINT_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_UINT_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_UINT_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(UINT)) {
							inst->DbsValue = *(UINT*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(UINT)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(UINT*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(UINT),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(UINT)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsUint */


/*=============================================================================
// Funktionsblock: DbsInt
//-----------------------------------------------------------------------------
// INT Zuordnung zum dbserv.
//=============================================================================*/

void DbsInt(DbsInt_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	INT				InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = FALSE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_INT_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_INT_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_INT_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(INT)) {
							inst->DbsValue = *(INT*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(INT)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(INT*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(INT),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(INT)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsInt */


/*=============================================================================
// Funktionsblock: DbsUdint
//-----------------------------------------------------------------------------
// UDINT Zuordnung zum dbserv.
//=============================================================================*/

void DbsUdint(DbsUdint_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	UDINT			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = FALSE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_UDINT_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_UDINT_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_UDINT_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(UDINT)) {
							inst->DbsValue = *(UDINT*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(UDINT)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(UDINT*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(UDINT),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(UDINT)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsUdint */


/*=============================================================================
// Funktionsblock: DbsDint
//-----------------------------------------------------------------------------
// DINT Zuordnung zum dbserv.
//=============================================================================*/

void DbsDint(DbsDint_typ* inst) {

	#define cNO_LINK_FUB_DBSERV_MASK	1024
	#define cNO_LINK_DBSERV_FUB_MASK	2048
	#define cNO_TIMESTAMP_FUB_MASK		4096

	void			*idBrServer;
	UINT			brstate;
	USINT			*pFubInstance;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	RTCTime, *pDataTime;
	unsigned long	lenDataExtern, lenDataTime;
	DINT			InValue;

	USINT	WriteBuffer[32];
	UDINT	len, lenTimeStamp;
	USINT	*pByteValue;


	/* ts */
	inst->FubRef.tsFub = FALSE;

	/* default */
	inst->DbsValid = FALSE;
	inst->TrgWriteValue = FALSE;
	inst->Stat = cEC_OK;

	/* ------------------------- */
	/* Verkettung FUB <-> DBSERV */
	/* ------------------------- */
	if (inst->FubRef.pidBrServer == 0 || inst->FubRef.this == 0 || inst->FubRef.this != (UDINT)inst) {
		intStat = GetIdBrServer(&idBrServer, &brstate);
		if (intStat == cEC_OK) {
			inst->Stat = cEC_FUB_NOT_LINKED_DBSERV;
			ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_FUB_DBSERV_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_FUB_DBSERV, 0, ET_DINT_PREFIX, ET_NO_LINK_FUB_DBSERV);
		}
	}
	else {

		/* ---------- */
		/* idBrServer */
		/* ---------- */
		intStat = GetIdBrServer(&idBrServer, &brstate);
		inst->FubRef.brstate = brstate;
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		/* ------------------------- */
		/* Verkettung DBSERV <-> FUB */
		/* ------------------------- */
		intStat = GetFubInstance(idBrServer,
									inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
									&pFubInstance);
		if (intStat != cEC_OK) {
			inst->Stat = intStat;
		}

		if (intStat == cEC_OK) {
			if (pFubInstance != (USINT*)inst->FubRef.this) {
				inst->Stat = cEC_DBSERV_NOT_LINKED_FUB;
				ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_LINK_DBSERV_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_LINK_DBSERV_FUB, 0, ET_DINT_PREFIX, ET_NO_LINK_DBSERV_FUB);
			}
			else {

				/* ---------- */
				/* Linkstatus */
				/* ---------- */
				intStat = GetEventLinkInfo(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											&inst->FubRef.countInstAll, &inst->FubRef.countInstNoTs, &inst->FubRef.countInstTs, &inst->FubRef.tsItem);
				if (intStat != cEC_OK) {
					inst->Stat = intStat;
				}
				if (inst->FubRef.tsFub == FALSE && inst->FubRef.tsItem == TRUE) {
					inst->Stat = cEC_FUB_WITHOUT_TIMESTAMP;
					ErrStatisticsX("", &inst->FubRef.ErrorMask, cNO_TIMESTAMP_FUB_MASK, NULL, ERRLOG, ERR, EC_DBS_BASE + EC_NO_TIMESTAMP_FUB, 0, ET_DINT_PREFIX, ET_NO_TIMESTAMP_FUB);
				}

				if (inst->SetValue == TRUE) {

					/* ----- */
					/* Modus */
					/* ----- */
					switch (inst->Mode) {
						default:
							/* sonst */
							InValue = inst->InValue;
							break;
					}
					/* ------------- */
					/* Wert nach PAB */
					/* ------------- */
					intStat = SetValueItem(idBrServer,
											inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
											(USINT*)&InValue, sizeof(InValue),
											RTCTime, 0, TRUE);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					/* ----------------- */
					/* Wert nach Ausgang */
					/* ----------------- */
					inst->DbsValue = inst->InValue;
					inst->DbsValid = TRUE;
				}
				else if (inst->SetValue == FALSE) {

					/* ------------ */
					/* Wert aus PAB */
					/* ------------ */
					intStat = GetValueItemNoUpdate(idBrServer,
													inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
													&pDataExtern, &lenDataExtern,
													(USINT**)&pDataTime, &lenDataTime);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
						if (lenDataExtern == sizeof(DINT)) {
							inst->DbsValue = *(DINT*)pDataExtern;
							inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
						}
						else {
							intStat = cEC_INVALID_LENGTH;
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}
						}
					}
				}

				/* --------- */
				/* Schreiben */
				/* --------- */
				if (inst->WriteEnable == FALSE) {
					intStat = UnlinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_UNLINKED) {
						inst->Stat = intStat;
					}
				}
				else if (inst->WriteEnable == TRUE) {
					intStat = LinkWriteItem(idBrServer, cINSTANCE_DBSLIB,
												inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
												inst->FubRef.this);
					if (intStat != cEC_OK && intStat != cEC_ITEM_ALREADY_LINKED) {
						inst->Stat = intStat;
					}

					/* Schreibauftrag vorhanden ? */
					intStat = ScanWriteItem(idBrServer, cINSTANCE_DBSLIB,
												WriteBuffer, sizeof(WriteBuffer),
												&len, &lenTimeStamp);
					if (intStat != cEC_OK) {
						inst->Stat = intStat;
					}

					if (intStat == cEC_OK) {
						/* für mich ? */
						if (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) {
							intStat = GetWriteItem(idBrServer, cINSTANCE_DBSLIB,
													WriteBuffer, sizeof(WriteBuffer),
													&len, &lenTimeStamp);
							if (intStat != cEC_OK) {
								inst->Stat = intStat;
							}

							if (intStat == cEC_OK) {
								if (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(DINT)) {
									pByteValue = WriteBuffer + sizeof(WRITE_PIPE_DATA);
									inst->WriteValue = *(DINT*)pByteValue;
									inst->TrgWriteValue = TRUE;

									/* Wert optional nach PAB zurückschreiben */
									if (inst->SetValue == FALSE) {
										intStat = SetValueItem(idBrServer,
																inst->FubRef.dbsUnit, inst->FubRef.dbsDevice, inst->FubRef.dbsType, inst->FubRef.dbsPoint, inst->FubRef.dbsItem,
																pByteValue, sizeof(DINT),
																RTCTime, 0, TRUE);
										if (intStat != cEC_OK) {
											inst->Stat = intStat;
										}
									}
								} /* (((WRITE_PIPE_DATA*)WriteBuffer)->length == sizeof(DINT)) */
								else {
									intStat = cEC_INVALID_LENGTH;
									if (intStat != cEC_OK) {
										inst->Stat = intStat;
									}
								}
							} /* (intStat == cEC_OK) */
						} /* (((WRITE_PIPE_DATA*)WriteBuffer)->pData == (unsigned char*)inst->FubRef.this) */
					} /* (intStat == cEC_OK) */
				}
			}
		} /* (intStat == cEC_OK) */
	}
} /* IfDbsDint */


/*=============================================================================
// Funktionsblock: DbsGetBool
//-----------------------------------------------------------------------------
// BOOL Abfrage an dbserv.
//=============================================================================*/

void DbsGetBool(DbsGetBool_typ* inst) {

	void			*idBrServer;
	UINT			brstate;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	*pDataTime;
	unsigned long	lenDataExtern, lenDataTime;


	/* ---------- */
	/* idBrServer */
	/* ---------- */
	intStat = GetIdBrServer(&idBrServer, &brstate);

	/* ------------ */
	/* Wert aus PAB */
	/* ------------ */
	intStat = GetValueItemNoUpdate(idBrServer,
									inst->Unit, inst->Device, inst->Type, inst->Point, inst->Item,
									&pDataExtern, &lenDataExtern,
									(USINT**)&pDataTime, &lenDataTime);
	if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
		if (lenDataExtern == sizeof(BOOL)) {
			inst->DbsValue = *(BOOL*)pDataExtern;
			inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
		}
		else {
			intStat = cEC_INVALID_LENGTH;
		}
	}
	inst->Stat = intStat;

} /* DbsGetBool */


/*=============================================================================
// Funktionsblock: DbsGetInt
//-----------------------------------------------------------------------------
// INT Abfrage an dbserv.
//=============================================================================*/

void DbsGetInt(DbsGetInt_typ* inst) {

	void			*idBrServer;
	UINT			brstate;
	unsigned short	intStat;
	unsigned char	*pDataExtern;
	RTCtime_typ 	*pDataTime;
	unsigned long	lenDataExtern, lenDataTime;


	/* ---------- */
	/* idBrServer */
	/* ---------- */
	intStat = GetIdBrServer(&idBrServer, &brstate);

	/* ------------ */
	/* Wert aus PAB */
	/* ------------ */
	intStat = GetValueItemNoUpdate(idBrServer,
									inst->Unit, inst->Device, inst->Type, inst->Point, inst->Item,
									&pDataExtern, &lenDataExtern,
									(USINT**)&pDataTime, &lenDataTime);
	if (intStat == cEC_OK || intStat == cEC_INVALID_VALUE) {
		if (lenDataExtern == sizeof(INT)) {
			inst->DbsValue = *(INT*)pDataExtern;
			inst->DbsValid = (intStat == cEC_OK) ? TRUE : FALSE;
		}
		else {
			intStat = cEC_INVALID_LENGTH;
		}
	}
	inst->Stat = intStat;

} /* DbsGetInt */

/*============================= Ende der Datei ==============================*/


