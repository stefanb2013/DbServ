#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif
/*************************************************************************************************
	Treiber für TCP/IP-Kopplung zwischen zenOn und brserver

	Stand:		11.04.03
	Verfasser:	L.Waibel, MEGASYSTEMS GmbH
    LastEdit:

	
	
        21.12.04/RES:	#include von bsrun.h bzw. dbserv.h durch Compilerschalter geregelt
        11.12.04/RES:	Fehler bei ClientCommands() - case BRT_ITM_READ beseitigt
        08.12.03/Wai:	Fehler bei ITEM_WRITE ohne vorheriges LOGON behoben
		06.07.04/WAI:	Diagnose wegen Abstürzen mit divide-error
						(ClientDiag = protokollieren aller Client-Anmeldungen, ClientsMax=max.Anzahl Clients,
						Verwendung der max.Anzahl Clients aus dplist, statt MAX_CLIENTS
    	13.08.03/RES:	Diagnose-PVs reduziert; ClientDiagnose() eingefuegt ( nur fuer Burklimat2000 )
 		11.08.03/RES:	Alle "i" ersetzt durch "clientNr"
        15.07.03/RES:	Diagnosen eingebaut DIAG_*
                        Fehler bei ClientRecv() und ClientCommands() beseitigt
		08.09.06/WAI:	ClientConnect() Client möglichst wieder bei gleicher Instanz anmelden
						wegen Archivierung
		18.02.15/BIE:	Erweiterung für ATOM-CPUs

**************************************************************************************************/

#define _REPLACE_CONST		//18.02.2015 Erweiterung für ATOM-CPUs
#define ETH_SOCK_NG 1		//18.02.2015 Erweiterung für ATOM-CPUs


#include	<bur/plc.h>
#include	<bur/plctypes.h>
#include	<ethsock.h>
#include	<AsIecCon.h>
#include	<sys_lib.h>
#include	<standard.h>
#include 	<fileio.h>

#include	"platform.h"

#include	"brtcp.h"





#ifdef BURKLIMAT2000
			#include 	"bsrun.h"
			#include	"bksys.h"
#else
			#include	"dbserv.h"
#endif

/*************************************************************************************************/
/* Diagnose wegen Abstürzen */
#define	ANZAHL_CLIENTDIAG	10

typedef struct {
	UDINT		ClientAdrs;
	RTCtime_typ	LoginTime;
	UINT		LoginCount;
} CLIENTDIAG_typ;

_LOCAL	INT	ClientsMax,MaxCfgClients;
_LOCAL	CLIENTDIAG_typ	ClientDiag[ANZAHL_CLIENTDIAG];
_LOCAL	UINT	instancestate;
_LOCAL	RTCtime_typ	ActTime;

/*************************************************************************************************/
/*	globale Variablen                                                                            */
/*************************************************************************************************/


/*************************************************************************************************/
/*	lokale Variablen                                                                             */
/*************************************************************************************************/
_LOCAL	ACCESS_SERVER	*myidBrServer;

_LOCAL	UINT	idstat,brstate,portstat,timeoutstat,timeout;

_LOCAL	UINT	typestate,typecount;
_LOCAL	USINT 	step, initok;
_LOCAL	UINT	port;
_LOCAL	unsigned long	arg, socket_descr, bacllog, client_len;
_LOCAL	signed long	new_sock;
_LOCAL	int		opt_val, stat_ioctl, stat_setsockopt, stat_bind, stat_listen, clients;
_LOCAL	UINT	StateRTCTime,StateSWTime;
_LOCAL	INT		len_opt_val;

_LOCAL	ETH_sockaddr_typ name_str, client_str;
_LOCAL	INT	error;

_LOCAL	UINT	ID,AK,Anlage,Geraet,Typ,Punkt,Item;
_LOCAL	UDINT	Len,CmdLen,SendLen;

_LOCAL	CLI_HEAD_TYP *pBRT_head;
_LOCAL	CLI_LOGON_REQUEST *pBRT_logon;
_LOCAL	CLI_LOGON_RESPONSE *pBRT_logon_resp;
_LOCAL	CLI_LOGOFF_REQUEST *pBRT_logoff;
_LOCAL	CLI_LOGOFF_RESPONSE *pBRT_logoff_resp;
_LOCAL	CLI_READ_REQUEST *pBRT_read;
_LOCAL	CLI_READ_RESPONSE *pBRT_read_resp;
_LOCAL	CLI_WRITE_REQUEST *pBRT_write;
_LOCAL	CLI_WRITE_RESPONSE *pBRT_write_resp;
_LOCAL	CLI_ADRERROR_RESPONSE *pBRT_adrerror_resp;
_LOCAL	CLI_ERROR_RESPONSE *pBRT_error_resp;
_LOCAL	CLI_EVENT_RESPONSE *pBRT_event_resp;
_LOCAL	CLI_WATCHDOG_REQUEST *pBRT_watch;
_LOCAL	CLI_WATCHDOG_RESPONSE *pBRT_watch_resp;
_LOCAL	CLI_TIMESYNC_REQUEST *pBRT_time;
_LOCAL	CLI_TIMESYNC_RESPONSE *pBRT_time_resp;
_LOCAL	CLI_DATASTREAM *pBRT_datastream;

_LOCAL	client_typ	client[MAX_CLIENTS];
_LOCAL	USINT		TempBuffIn[RECVBUFLEN];
_LOCAL	USINT		TempBuffOut[SENDBUFLEN];
_LOCAL	USINT	DataByte;
_LOCAL	UINT	DataWord;
_LOCAL	UDINT	DataLong;
_LOCAL	RTCtime_typ DataTime;

_LOCAL	EVENT_PIPE_DATA	*pMSEventHeadData;

_LOCAL	unsigned char	DIAG_Init;
_LOCAL	unsigned long	DIAG_ClientAuftraege;
_LOCAL	unsigned long	DIAG_EventAuftraege;
_LOCAL	unsigned long	DIAG_LogonCount;

/***********************************************************************************************
	htons
	host to network short
***********************************************************************************************/
UINT htons(UINT in)
{
#ifndef INTEL
	return swapUINT(in);
#else
	return in;
#endif
}
/***********************************************************************************************
	htonl
	host to network long
***********************************************************************************************/
UDINT htonl(UDINT in)
{
#ifndef INTEL
	return swapUDINT(in);
#else
	return in;
#endif
}
/***********************************************************************************************
	ntohs
	network to host short
***********************************************************************************************/
UINT ntohs(UINT in)
{
#ifndef INTEL
	return swapUINT(in);
#else
	return in;
#endif
}

/***********************************************************************************************
	ntohl
	network to host long
***********************************************************************************************/
UDINT ntohl(UDINT in)
{
#ifndef INTEL
	return swapUDINT(in);
#else
	return in;
#endif
}
/***********************************************************************************************
	GetData
	kopiert plattformabhägig die Daten und stellt die korrekte Byte-Order her
	Rückgabe:	Zeiger auf die korrekten Daten
***********************************************************************************************/
void GetData( USINT *pDataSrc, USINT **pDataDest, UDINT Len)
{
	switch(Len)	{
		default:
		case 1:
			DataByte = *(USINT*)pDataSrc;
			*pDataDest = &DataByte;
			break;
		case 2:
			DataWord = ntohs(*(UINT*)pDataSrc);
			*pDataDest = (USINT*)&DataWord;
			break;
		case 4:
			DataLong = ntohl(*(UDINT*)pDataSrc);
			*pDataDest = (USINT*)&DataLong;
			break;
		case sizeof(RTCtime_typ):
			memcpy((USINT*)&DataTime,pDataSrc,Len);
			DataTime.year = ntohs(DataTime.year);
			*pDataDest = (USINT*)&DataTime;
			break;
	}
}

/***********************************************************************************************
	TcpError
	kodiert die Meldeserver-Fehlernummern auf TCP-Fehlernummern um
***********************************************************************************************/
UINT TcpError(UINT inerror)
{
	UINT	outerror;

	switch (inerror)	{
		case cEC_INVALID_UNIT:
			outerror = BRT_ERR_UNKNOWN_UNIT;
			break;
		case cEC_INVALID_DEVICE:
			outerror = BRT_ERR_UNKNOWN_DEVICE;
			break;
		case cEC_INVALID_TYPE:
			outerror = BRT_ERR_UNKNOWN_TYPE;
			break;
		case cEC_INVALID_POINT:
			outerror = BRT_ERR_UNKNOWN_POINT;
			break;
		case cEC_INVALID_ITEM:
			outerror = BRT_ERR_WRONG_ITEM;
			break;
		case cEC_INVALID_LENGTH:
			outerror = BRT_ERR_WRONG_LEN;
			break;
		case cEC_ITEM_ALREADY_LINKED:
			outerror = BRT_ERR_IS_LOGGED;
			break;
		case cEC_ITEM_ALREADY_UNLINKED:
			outerror = BRT_ERR_ITEM_NO_LOGON;
			break;
		case cEC_ITEM_NOT_WRITEABLE:
			outerror = BRT_ERR_ITEM_NOT_WRITEABLE;
			break;
		case cEC_WRITE_PIPE_FULL:
			outerror = BRT_ERR_WRITE_PIPE_FULL;
			break;
		case cEC_ITEM_NO_TIMESTAMP:
			outerror = BRT_ERR_ITEM_NO_TS;
			break;
		case cEC_INVALID_VALUE:
			outerror = BRT_ERR_ITEM_NO_DATA;
			break;
		default:
			outerror = inerror;
			break;
	}
	return outerror;
}
/***********************************************************************************************
	GetAdrs
	liest die aktuelle Adresse aus dem Telegramm in Einzelvariablen
***********************************************************************************************/
void GetAdrs(CLI_ADR_TYP *pAdrs)
{
	Anlage = ntohs(pAdrs->anlage);
	Geraet = ntohs(pAdrs->geraet);
	Typ = ntohs(pAdrs->typ);
	Punkt = ntohs(pAdrs->punkt);
	Item = ntohs(pAdrs->item);
}
/***********************************************************************************************
	PutAdrs
	schreibt die aktuelle Adresse in das Telegramm aus Einzelvariablen
***********************************************************************************************/
void PutAdrs(CLI_ADR_TYP *pAdrs)
{
	pAdrs->anlage = htons(Anlage);
	pAdrs->geraet = htons(Geraet);
	pAdrs->typ = htons(Typ);
	pAdrs->punkt = htons(Punkt);
	pAdrs->item = htons(Item);
}
/***********************************************************************************************
	NextCmd
	schaltet Empfangspuffer auf nächsten Befehl und setzt ggf. Puffer zurück
***********************************************************************************************/
void NextCmd(INT clientNr, UDINT len)
{
	client[clientNr].recv_out += len;
	if(client[clientNr].recv_out == client[clientNr].recv_in)	{
		client[clientNr].recv_out = client[clientNr].recv_in = 0;		/* reset buffer */
	}
}
/***********************************************************************************************
	SpaceSendBuf
	prüft, ob die angeforderte Datenlänge noch in den Sendepuffer passt
***********************************************************************************************/
int SpaceSendBuf(INT clientNr, UDINT len)
{
	return( (sizeof(client[clientNr].send_buf) - client[clientNr].send_in) > len ? 1:0);
}


/***********************************************************************************************
	ClientConnect
	akzeptiert neue Client-Verbindungen
***********************************************************************************************/
void ClientConnect()
{
	INT	clientNr;
	BOOL new_client;
	BOOL old_entry;

	new_client = FALSE;
	client_len = sizeof(client_str);
	new_sock = accept(socket_descr, (unsigned long)&client_str, (unsigned long)&client_len);
	if (new_sock != ERR_SOCKET)	{
		/* new connection */
		for(clientNr=0;clientNr<MaxCfgClients;clientNr++)	{		/* 06.07.04: statt MAX_CLIENTS */
			/* 08.09.06: gleichen Client über IP-Adrs möglichst an gleicher Instanz anmelden wegen Archivierung */
			if(client[clientNr].socket==ERR_SOCKET && client[clientNr].client_str.sin_addr == client_str.sin_addr)	{
				/* new client */
				client[clientNr].socket = new_sock;
				client[clientNr].recv_in = 0;
				client[clientNr].recv_out = 0;
				client[clientNr].send_in = 0;
				client[clientNr].send_out = 0;
				client[clientNr].status = CLI_CONNECTED;
				client[clientNr].TON_watchdog.IN = FALSE;
				client[clientNr].stateEvRead = cEC_NO_DATA_IN_EVENTBUFFER;
				clients++;
				/* 06.07.04: save ClientsMax */
				if(clients > ClientsMax)	ClientsMax = clients;

				new_client = TRUE;
				memcpy(&client[clientNr].client_str,&client_str,sizeof(client_str));
				break;
			}
		}
		if(!new_client)	{
			/* 08.09.06: falls Client noch nie angemeldet war: nächsten freien Client nehmen */
			for(clientNr=0;clientNr<MaxCfgClients;clientNr++)	{		/* 06.07.04: statt MAX_CLIENTS */
				if(client[clientNr].socket==ERR_SOCKET)	{
					/* new client */
					client[clientNr].socket = new_sock;
					client[clientNr].recv_in = 0;
					client[clientNr].recv_out = 0;
					client[clientNr].send_in = 0;
					client[clientNr].send_out = 0;
					client[clientNr].status = CLI_CONNECTED;
					client[clientNr].TON_watchdog.IN = FALSE;
					client[clientNr].stateEvRead = cEC_NO_DATA_IN_EVENTBUFFER;
					clients++;
					/* ArchiveFiles löschen bei neuem Client */

					/* 06.07.04: save ClientsMax */
					if(clients > ClientsMax)	ClientsMax = clients;

					new_client = TRUE;
					memcpy(&client[clientNr].client_str,&client_str,sizeof(client_str));
					break;
				}
			}
		}
		/*---------------------------------------------------------------------------*/
		/* 06.07.04: diagnostics: log every connecting client */
		old_entry = 0;
		for(clientNr=0;clientNr< ANZAHL_CLIENTDIAG;clientNr++)	{
			if(ClientDiag[clientNr].ClientAdrs == client_str.sin_addr)	{
				RTC_gettime(&ActTime);
				memcpy(&ClientDiag[clientNr].LoginTime,&ActTime,sizeof(ActTime));
				ClientDiag[clientNr].LoginCount++;
				old_entry = TRUE;
				break;
			}
		}
		if(!old_entry)	{
			/* look for new entry in list */
			for(clientNr=0;clientNr< ANZAHL_CLIENTDIAG;clientNr++)	{
				if(ClientDiag[clientNr].ClientAdrs == 0 || clientNr == ANZAHL_CLIENTDIAG-1)	{	/* auf jeden in letztem Platz eintragen */
					RTC_gettime(&ActTime);
					ClientDiag[clientNr].ClientAdrs = client_str.sin_addr;
					memcpy(&ClientDiag[clientNr].LoginTime,&ActTime,sizeof(ActTime));
					ClientDiag[clientNr].LoginCount++;
					break;
				}
			}
		}
		/* 06.07.04: end diagnostics: log every connecting client */
		/*---------------------------------------------------------------------------*/

		if(!new_client)	{
			close(new_sock);		/* immediately terminate new connection */
		}
	}
}
/***********************************************************************************************
	ClientClose
	beendet Verbindung zu einem Client
***********************************************************************************************/
void ClientClose(INT clientNr, INT reason)
{
	close(client[clientNr].socket);
	client[clientNr].socket = ERR_SOCKET;
	client[clientNr].status = reason;
	client[clientNr].TON_watchdog.IN = FALSE;
/*	UnlinkEventInstance(myidBrServer,clientNr);*/
	UnlinkEventInstanceWithoutFileBuffer(myidBrServer,clientNr);		/* WAI 09.04.06 */
	if(clients)	clients--;
}
/***********************************************************************************************
	ClientWatch
	prüft auf Verbindungstimeout zu einem Client
***********************************************************************************************/
void ClientWatch(INT clientNr)
{
	TON(&client[clientNr].TON_watchdog);
	client[clientNr].TON_watchdog.IN = TRUE;
	if(client[clientNr].TON_watchdog.Q == TRUE)	{
		/* Timeout !	close connection */
		ClientClose(clientNr,CLI_WATCHDOG);
	}
}

/***********************************************************************************************
	ClientRecv
	empfängt Daten von den aktiven Clients
***********************************************************************************************/
UDINT ClientRecv(INT clientNr)
{
	DINT	bytes = 0;		/* 15.07.03/RES: bytes = 0 */
	UDINT	recv_len;

	/* read data from client */
	recv_len = sizeof(client[clientNr].recv_buf) - client[clientNr].recv_in;
	if( recv_len > 0 ) {		/* 15.07.03/RES */
		bytes = recv(client[clientNr].socket,(UDINT)&client[clientNr].recv_buf[client[clientNr].recv_in],recv_len,0);
		if(bytes==0)	{
			/* connection closed */
			ClientClose(clientNr,CLI_DISCONNECTED);
		}
		else if(bytes == ERR_SOCKET){
			/* error */
			error = EthGetError();
			if(error != ERR_ETH_WOULDBLOCK )	{
				/* all errors except: waiting for data */
				ClientClose(clientNr,CLI_ERROR);
			}
		}
		else {
			/* data received */
			client[clientNr].recv_cnt++;
			client[clientNr].TON_watchdog.IN = FALSE;
			client[clientNr].recv_in +=bytes;
		}
	}
	return bytes;
}
/***********************************************************************************************
	ClientCommands
	wertet die Kommandos eines Clients aus
***********************************************************************************************/
void ClientCommands(INT clientNr)
{
	BOOL	wait_data = FALSE;
	UDINT	shiftlen,TempLen;
	UINT	mserror = 0;
	USINT	*pData,*pTime,*pDest;
	UDINT	lenData,lenTime;

	if(client[clientNr].recv_in  < (client[clientNr].recv_out+sizeof(CLI_HEAD_TYP))) {
		wait_data = TRUE;		/* no data */
	}
	else {
		do	{

			memcpy(TempBuffIn,&client[clientNr].recv_buf[client[clientNr].recv_out],client[clientNr].recv_in - client[clientNr].recv_out);
			pBRT_head = (CLI_HEAD_TYP *)TempBuffIn;
			ID  = ntohs(pBRT_head->ID);
			AK	= ntohs(pBRT_head->AK);

			DIAG_ClientAuftraege++;

			switch(AK)	{
				/*...........................................................................................*/
				/* Item mit/ohne Zeitstempel anmelden */
				case BRT_ITM_LOGON:
				case BRT_ITM_LOGON_TS:
					if(client[clientNr].recv_in  >= (client[clientNr].recv_out+sizeof(CLI_LOGON_REQUEST)-2))	{
						pBRT_logon = (CLI_LOGON_REQUEST *)pBRT_head;
						Len = (UDINT)ntohs(pBRT_logon->LenHigh)<<16 | ntohs(pBRT_logon->LenLow) ;
						CmdLen = sizeof(CLI_LOGON_REQUEST)+ Len - 2;
						if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
							GetAdrs(&pBRT_logon->Adr);		/* Adresse aus Telegramm lesen */


							if(AK == BRT_ITM_LOGON)
								SendLen = CmdLen;						/* LOGON_RESPONSE max. gleich lang wie REQUEST ! */
							else
								SendLen = CmdLen +sizeof(RTCtime_typ);	/* LOGON_RESPONSE max. gleich lang wie REQUEST ! */

							if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/

								DIAG_LogonCount++;

								/* Hysterese kopieren */
								GetData((USINT*)&pBRT_logon->Data,(USINT**)&pData,Len);	/* Daten ggf. swappen */
								if((mserror = SetHystItem (myidBrServer,clientNr,Anlage,Geraet,Typ,Punkt,Item,pData,Len))==0)	{
									/* Datenpunkt anmelden */
									mserror=LinkEventItem(myidBrServer,clientNr,Anlage,Geraet,Typ,Punkt,Item,(AK == BRT_ITM_LOGON ? 0:1));
									if(mserror == 0 || mserror == cEC_ITEM_ALREADY_LINKED_FILE_BUFFER)	{	/* wegen Langzeitarchivierung */
										/* AktWert besorgen */
										mserror = GetValueItem (myidBrServer,clientNr, Anlage, Geraet, Typ, Punkt, Item, &pData, &lenData, &pTime, &lenTime);
									}
									if(mserror == 0)	{
										if( Len != lenData || (AK==BRT_ITM_LOGON_TS && lenTime==0))	{
											/* falsche Länge oder kein Zeitstempel vorhanden */
											if(Len != lenData)	mserror = BRT_ERR_WRONG_LEN;
											else				mserror = BRT_ERR_ITEM_NO_TS;

											pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
											pBRT_adrerror_resp->head.ID = htons(ID);
											pBRT_adrerror_resp->head.AK = htons(BRT_ITM_LOGON_ERR);
											PutAdrs(&pBRT_adrerror_resp->Adr);
											pBRT_adrerror_resp->Error = htons(TcpError(mserror));
											SendLen = sizeof(CLI_ADRERROR_RESPONSE);
										}
										else {
											/* Sendedaten bereitstellen */
											pBRT_logon_resp = (CLI_LOGON_RESPONSE *)&TempBuffOut;
											pBRT_logon_resp->head.ID = htons(ID);
											PutAdrs(&pBRT_logon_resp->Adr);

											if(AK == BRT_ITM_LOGON)	{
												pBRT_logon_resp->head.AK = htons(BRT_ITM_LOGON_OK);
											}
											else	{
												pBRT_logon_resp->head.AK = htons(BRT_ITM_LOGON_TS_OK);
											}
											TempLen = htonl(lenData);
											memcpy(&pBRT_logon_resp->LenLow,&TempLen,sizeof(TempLen));
											/* Daten kopieren */
											GetData(pData,(USINT**)&pDest,lenData);
											memcpy(&pBRT_logon_resp->Data,pDest,lenData);
											/* ggf. Zeitstempel kopieren */
											if(AK == BRT_ITM_LOGON_TS)	{
												GetData(pTime,(USINT**)&pDest,lenTime);
												memcpy((USINT*)&pBRT_logon_resp->Data+lenData,pDest,lenTime);
											}
										}
									}
									else {	/* Fehler beim Lesen des Datenpunkts */
										/* Sendedaten bereitstellen */
										pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
										pBRT_adrerror_resp->head.ID = htons(ID);
										pBRT_adrerror_resp->head.AK = htons(BRT_ITM_LOGON_ERR);
										PutAdrs(&pBRT_adrerror_resp->Adr);
										pBRT_adrerror_resp->Error = htons(TcpError(mserror));
										SendLen = sizeof(CLI_ADRERROR_RESPONSE);
									}
								}
								else {	/* logon-error */
									/* Sendedaten bereitstellen */
									pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
									pBRT_adrerror_resp->head.ID = htons(ID);
									pBRT_adrerror_resp->head.AK = htons(BRT_ITM_LOGON_ERR);
									PutAdrs(&pBRT_adrerror_resp->Adr);
									pBRT_adrerror_resp->Error = htons(TcpError(mserror));

									SendLen = sizeof(CLI_ADRERROR_RESPONSE);
								}
								memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
								client[clientNr].send_in += SendLen;
								/* goto next command */
								NextCmd(clientNr, CmdLen);
							}
							else	wait_data = TRUE;	/* wait for sendbuffer free */
						}
						else wait_data = TRUE;	/* incomplete data */
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* Item abmelden */
				case BRT_ITM_LOGOFF:
					CmdLen = sizeof(CLI_LOGOFF_REQUEST);
					if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
						pBRT_logoff = (CLI_LOGOFF_REQUEST *)pBRT_head;
						GetAdrs(&pBRT_logoff->Adr);		/* Adresse aus Telegramm lesen */

						SendLen = sizeof(CLI_ADRERROR_RESPONSE);	/* LOGOFF_RESPONSE max. gleich lang wie ADRERROR_RESPONSE ! */

						if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
							if((error=UnlinkEventItem(myidBrServer,clientNr,Anlage,Geraet,Typ,Punkt,Item))==0)	{
								/* Sendedaten bereitstellen */
								pBRT_logoff_resp = (CLI_LOGOFF_RESPONSE *)TempBuffOut;
								pBRT_logoff_resp->head.ID = htons(ID);
								pBRT_logoff_resp->head.AK = htons(BRT_ITM_LOGOFF_OK);
								PutAdrs(&pBRT_logoff_resp->Adr);
								SendLen = CmdLen;
							}
							else {	/* logoff-error */
								/* Sendedaten bereitstellen */
								pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
								pBRT_adrerror_resp->head.ID = htons(ID);
								pBRT_adrerror_resp->head.AK = htons(BRT_ITM_LOGOFF_ERR);
								PutAdrs(&pBRT_adrerror_resp->Adr);
								pBRT_adrerror_resp->Error = htons(TcpError(error));
								SendLen = sizeof(CLI_ADRERROR_RESPONSE);
							}
							memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
							client[clientNr].send_in += SendLen;
							/* goto next command */
							NextCmd(clientNr, CmdLen);
						}
						else	wait_data = TRUE;	/* wait for sendbuffer free */
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* Item mit/ohne Zeitstempel lesen */
				case BRT_ITM_READ:
				case BRT_ITM_READ_TS:
					CmdLen = sizeof(CLI_READ_REQUEST);
					if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
						pBRT_read = (CLI_READ_REQUEST *)pBRT_head;
						GetAdrs(&pBRT_read->Adr);		/* Adresse aus Telegramm lesen */

						/* AktWert besorgen */
						mserror = GetValueItem (myidBrServer,clientNr, Anlage, Geraet, Typ, Punkt, Item, &pData, &lenData, &pTime, &lenTime);
						if(mserror == 0)	{
							if(AK==BRT_ITM_READ)
								SendLen = sizeof(CLI_READ_RESPONSE)+lenData-2;
							else
								SendLen = sizeof(CLI_READ_RESPONSE)+lenData-2+sizeof(RTCtime_typ);

							if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
								if(AK==BRT_ITM_READ_TS && lenTime==0)	{
								/* kein Zeitstempel vorhanden */
									mserror = BRT_ERR_ITEM_NO_TS;

									pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
									pBRT_adrerror_resp->head.ID = htons(ID);
									pBRT_adrerror_resp->head.AK = htons(BRT_ITM_READ_ERR);
									PutAdrs(&pBRT_adrerror_resp->Adr);
									pBRT_adrerror_resp->Error = htons(TcpError(mserror));
									SendLen = sizeof(CLI_ADRERROR_RESPONSE);
								}
								else {
									/* Sendedaten bereitstellen */
									pBRT_read_resp = (CLI_READ_RESPONSE *)&TempBuffOut;
									pBRT_read_resp->head.ID = htons(ID);
									PutAdrs(&pBRT_read_resp->Adr);

									if(AK == BRT_ITM_READ)	{
										pBRT_read_resp->head.AK = htons(BRT_ITM_READ_OK);
									}
									else	{
										pBRT_read_resp->head.AK = htons(BRT_ITM_READ_TS_OK);
										Len += sizeof(RTCtime_typ);
									}
									TempLen = htonl(lenData);	/* wegen alignment UDINT ! */
									memcpy(&pBRT_read_resp->LenLow,&TempLen,sizeof(TempLen));
									/* Daten kopieren */
									GetData(pData,(USINT**)&pDest,lenData);
									memcpy(&pBRT_read_resp->Data,pDest,lenData);
									/* ggf. Zeitstempel kopieren */
									if(AK == BRT_ITM_READ_TS) {						/* 11.12.04/RES: hier fehlende Klammern eingefuegt !! */
										GetData(pTime,(USINT**)&pDest,lenTime);
										memcpy((USINT*)&pBRT_read_resp->Data+lenData,pDest,lenTime);
									}
								}
								memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
								client[clientNr].send_in += SendLen;
								/* goto next command */
								NextCmd(clientNr, CmdLen);
							}
							else wait_data = TRUE;	/* wait for sendbuffer free */
						}
						else	{	/* Fehler beim Lesen des Datenpunkts */
							SendLen = sizeof(CLI_ADRERROR_RESPONSE);
							if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
								pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
								pBRT_adrerror_resp->head.ID = htons(ID);
								pBRT_adrerror_resp->head.AK = htons(BRT_ITM_LOGON_ERR);
								PutAdrs(&pBRT_adrerror_resp->Adr);
								pBRT_adrerror_resp->Error = htons(TcpError(mserror));

								memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
								client[clientNr].send_in += SendLen;
								/* goto next command */
								NextCmd(clientNr, CmdLen);
							}
							else wait_data = TRUE;	/* wait for sendbuffer free */
						}
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* Item schreiben */
				case BRT_ITM_WRITE:
					if(client[clientNr].recv_in  >= (client[clientNr].recv_out+sizeof(CLI_WRITE_REQUEST)-2))	{
						pBRT_write = (CLI_WRITE_REQUEST *)pBRT_head;
						Len = (UDINT)ntohs(pBRT_write->LenHigh)<<16 | ntohs(pBRT_write->LenLow) ;
						CmdLen = sizeof(CLI_WRITE_REQUEST)+ Len - 2;
						if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
							GetAdrs(&pBRT_write->Adr);		/* Adresse aus Telegramm lesen */

							SendLen = sizeof(CLI_ADRERROR_RESPONSE);	/* WRITE_RESPONSE max. gleich lang wie ADRERROR_RESPONSE ! */

							if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
								GetData((USINT*)&pBRT_write->Data,(USINT**)&pData,Len);	/* Daten ggf. swappen */
								mserror = SetWriteItem(myidBrServer,Anlage,Geraet,Typ,Punkt,Item,pData,Len);

								if(mserror==0)	{
									/* Sendedaten bereitstellen */
									pBRT_write_resp = (CLI_WRITE_RESPONSE *)TempBuffOut;
									pBRT_write_resp->head.ID = htons(ID);
									PutAdrs(&pBRT_write_resp->Adr);
									pBRT_write_resp->head.AK = htons(BRT_ITM_WRITE_OK);
									SendLen = sizeof(CLI_WRITE_RESPONSE);
								}
								else {	/* write-error */
									/* Sendedaten bereitstellen */
									pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
									pBRT_adrerror_resp->head.ID = htons(ID);
									pBRT_adrerror_resp->head.AK = htons(BRT_ITM_WRITE_ERR);
									PutAdrs(&pBRT_adrerror_resp->Adr);
									pBRT_adrerror_resp->Error = htons(TcpError(mserror));
									SendLen = sizeof(CLI_ADRERROR_RESPONSE);
								}
								memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
								client[clientNr].send_in += SendLen;
								/* goto next command */
								NextCmd(clientNr, CmdLen);
							}
							else	wait_data = TRUE;	/* wait for sendbuffer free */
						}
						else wait_data = TRUE;	/* incomplete data */
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* Datenstrom lesen */
				case BRT_DATASTREAM_READ:
					CmdLen = sizeof(CLI_READ_REQUEST);
					if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
						pBRT_read = (CLI_READ_REQUEST *)pBRT_head;
						GetAdrs(&pBRT_read->Adr);		/* Adresse aus Telegramm lesen */

						/* Sonderbehandlung fehlt noch . eigener Buffer, eigenes Flag .. */
						/* AktWert besorgen */
						/*mserror = get_station_type_point (idMessageServer, Geraet, Typ, Punkt, &pData, &lenData, 0, 0, &pTime, &lenTime, 0, 0);*/
						if(mserror == 0)	{
							SendLen = sizeof(CLI_READ_RESPONSE)+lenData-2;

							if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
								/* Sendedaten bereitstellen */
								pBRT_read_resp = (CLI_READ_RESPONSE *)&TempBuffOut;
								pBRT_read_resp->head.ID = htons(ID);
								PutAdrs(&pBRT_read_resp->Adr);

								pBRT_read_resp->head.AK = htons(BRT_DATASTREAM_READ_OK);

								TempLen = htonl(lenData);	/* wegen alignment UDINT ! */
								memcpy(&pBRT_read_resp->LenLow,&TempLen,sizeof(TempLen));
								/* Daten kopieren */
								memcpy(&pBRT_read_resp->Data,pData,lenData);

								memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
								client[clientNr].send_in += SendLen;
								/* goto next command */
								NextCmd(clientNr, CmdLen);
							}
							else wait_data = TRUE;	/* wait for sendbuffer free */
						}
						else	{	/* Fehler beim Lesen des Datenpunkts */
							SendLen = sizeof(CLI_ADRERROR_RESPONSE);
							if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
								pBRT_adrerror_resp = (CLI_ADRERROR_RESPONSE *)TempBuffOut;
								pBRT_adrerror_resp->head.ID = htons(ID);
								pBRT_adrerror_resp->head.AK = htons(BRT_DATASTREAM_READ_ERR);
								PutAdrs(&pBRT_adrerror_resp->Adr);
								pBRT_adrerror_resp->Error = htons(TcpError(mserror));
							}
							else wait_data = TRUE;	/* wait for sendbuffer free */
						}
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* Watchdog */
				case BRT_CLI_WATCHDOG:
					CmdLen = sizeof(CLI_WATCHDOG_REQUEST);
					if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
						pBRT_watch = (CLI_WATCHDOG_REQUEST *)pBRT_head;
						SendLen = sizeof(CLI_WATCHDOG_RESPONSE);

						if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
							/* Sendedaten bereitstellen */
							pBRT_watch_resp = (CLI_WATCHDOG_RESPONSE *)TempBuffOut;
							pBRT_watch_resp->head.ID = htons(ID);
							pBRT_watch_resp->head.AK = htons(BRT_CLI_WATCHDOG_OK);
							memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
							client[clientNr].send_in += SendLen;
							/* goto next command */
							NextCmd(clientNr, CmdLen);
						}
						else	wait_data = TRUE;	/* wait for sendbuffer free */
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* Zeitsynchronisation */
				case BRT_TIMESYNC:
					CmdLen = sizeof(CLI_TIMESYNC_REQUEST);
					if(client[clientNr].recv_in  >= client[clientNr].recv_out + CmdLen)	{
						pBRT_time = (CLI_TIMESYNC_REQUEST *)pBRT_head;
						SendLen = sizeof(CLI_ERROR_RESPONSE);		/* max.Antwortlänge */

						if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
							/* Echtzeituhr stellen */
							pBRT_time->Time.year = ntohs(pBRT_time->Time.year );
							pBRT_time->Time.reserve = 0;
/*							pBRT_time->Time.millisec = 0;
							pBRT_time->Time.microsec = 0;*/
							StateRTCTime = RTC_settime(&pBRT_time->Time);
	#ifndef INTEL
							StateSWTime = SW_settime(&pBRT_time->Time);
	#else
							StateSWTime = 0;
	#endif

							if(StateRTCTime==0 && StateSWTime==0)	{
								/* Sendedaten bereitstellen */
								pBRT_time_resp = (CLI_TIMESYNC_RESPONSE *)TempBuffOut;
								pBRT_time_resp->head.ID = htons(ID);
								pBRT_time_resp->head.AK = htons(BRT_TIMESYNC_OK);
								SendLen = sizeof(CLI_TIMESYNC_RESPONSE );
							}
							else {	/* Fehler bei Uhrzeit stellen */
								pBRT_error_resp = (CLI_ERROR_RESPONSE *)TempBuffOut;
								pBRT_error_resp->head.ID = htons(ID);
								pBRT_error_resp->head.AK = htons(BRT_TIMESYNC_ERR);
								if(StateRTCTime)	pBRT_error_resp->Error = htons(BRT_ERR_RTC_SET);
								else				pBRT_error_resp->Error = htons(BRT_ERR_SWC_SET);
							}
							memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
							client[clientNr].send_in += SendLen;
							/* goto next command */
							NextCmd(clientNr, CmdLen);
						}
						else	wait_data = TRUE;	/* wait for sendbuffer free */
					}
					else wait_data = TRUE;	/* incomplete data */
					break;
				/*...........................................................................................*/
				/* unbekannte Auftragskennung */
				default:
					SendLen = sizeof(CLI_ERROR_RESPONSE);

					if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
						/* Sendedaten bereitstellen */
						pBRT_error_resp = (CLI_ERROR_RESPONSE *)TempBuffOut;
						pBRT_error_resp->head.ID = htons(ID);
						pBRT_error_resp->head.AK = htons(BRT_CLI_ERROR);
						error = BRT_ERR_INVALIDAK;
						pBRT_error_resp->Error = htons(error);
						SendLen = sizeof(CLI_ERROR_RESPONSE);
						memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
						client[clientNr].send_in += SendLen;

						/* discard receivebuffer */
						client[clientNr].recv_in  = client[clientNr].recv_out =0;
					}
					else	wait_data = TRUE;	/* wait for sendbuffer free */
					break;
			}
		} while((client[clientNr].recv_in  > (client[clientNr].recv_out+sizeof(CLI_HEAD_TYP))) && wait_data == FALSE);
	}
	/* "verbrauchte" Daten aus Empfangspuffer entfernen d.h.: Rest an den Pufferanfang verschieben */
	if(wait_data && (client[clientNr].recv_out !=0))	{
		if(client[clientNr].recv_out > RECVBUFLEN / 4)	{
			/* nur schieben, wenn mehr als 1/4 Puffer zu schieben ist */
			shiftlen = client[clientNr].recv_in - client[clientNr].recv_out;
			memcpy(&client[clientNr].recv_buf[0],&client[clientNr].recv_buf[client[clientNr].recv_out],shiftlen);
			client[clientNr].recv_in -= client[clientNr].recv_out;
			client[clientNr].recv_out = 0;
		}
	}
}
/***********************************************************************************************
	ClientEvents
	prüft die EventPipes auf Daten
***********************************************************************************************/
void ClientEvents(INT clientNr)
{
	BOOL	wait_data = FALSE;
	INT		typ;
	UDINT	TempLen;
	EVENT_PIPE_DATA	*pEvent;
	USINT	*pData,*pTime;

	/* Events über alle Typen abfragen */
	for(typ=0;typ<typecount;typ++)	{
		do	{
			if(client[clientNr].stateEvRead !=0)		/* letztes Event abgearbeitet */
				client[clientNr].stateEvRead = GetEventItem(myidBrServer,clientNr,(client[clientNr].startType+typ)%typecount,
										client[clientNr].EventBuffer,sizeof(client[clientNr].EventBuffer),
										&client[clientNr].lenEventMessage,&client[clientNr].lengthTimeStamp);
			if(client[clientNr].stateEvRead ==0)	{
				/* Diag: Events zählen */
				client[clientNr].EventCnt++;
				/* Event vorhanden */
				pMSEventHeadData = (EVENT_PIPE_DATA*) client[clientNr].EventBuffer;
				Anlage = pMSEventHeadData->unit;
				Geraet = pMSEventHeadData->device;
				Typ = pMSEventHeadData->type;
				Punkt = pMSEventHeadData->point;
				Item = pMSEventHeadData->item;
				Len = client[clientNr].lenEventMessage;

				if( pMSEventHeadData->pData != NULL)	{
					client[clientNr].stateEvRead = cEC_NO_DATA_IN_EVENTBUFFER;
					/* Daten von Stream */
					client[clientNr].stateStream = STREAM_INIT;
					/* Abbruch wegen Start Datastream */
					wait_data = TRUE;
					break;
				}
				else {
					/* Daten aus Prozessabbild */
					SendLen = sizeof(CLI_EVENT_RESPONSE) - 2 + client[clientNr].lenEventMessage + client[clientNr].lengthTimeStamp;

					if(SpaceSendBuf(clientNr, SendLen))	{	/* Platz in Sendepuffer vorhanden ?*/
						client[clientNr].stateEvRead = cEC_NO_DATA_IN_EVENTBUFFER;
						/* Sendedaten bereitstellen */
						pBRT_event_resp = (CLI_EVENT_RESPONSE *)TempBuffOut;
						pBRT_event_resp->head.ID = htons(client[clientNr].EventID++);
						PutAdrs(&pBRT_event_resp->Adr);
						if(!client[clientNr].lengthTimeStamp)
							pBRT_event_resp->head.AK = htons(BRT_ITM_EVENT);
						else
							pBRT_event_resp->head.AK = htons(BRT_ITM_EVENT_TS);

						TempLen = htonl(Len);
						memcpy(&pBRT_event_resp->LenLow,&TempLen,sizeof(TempLen));

						/* Daten und ggf. Zeitstempel kopieren */
						pEvent = (EVENT_PIPE_DATA*)&client[clientNr].EventBuffer;
						GetData((USINT *)&client[clientNr].EventBuffer+sizeof(EVENT_PIPE_DATA),(USINT**)&pData,
								client[clientNr].lenEventMessage);
						memcpy(&pBRT_event_resp->Data, pData, client[clientNr].lenEventMessage);

						if(client[clientNr].lengthTimeStamp)	{
							GetData((USINT *)&client[clientNr].EventBuffer+sizeof(EVENT_PIPE_DATA)+client[clientNr].lenEventMessage,(USINT**)&pTime,
									client[clientNr].lengthTimeStamp);
							memcpy((USINT*)&pBRT_event_resp->Data + client[clientNr].lenEventMessage, pTime, client[clientNr].lengthTimeStamp);
						}
						/* Daten an Sendepuffer anhängen */

						DIAG_EventAuftraege++;

						memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
						client[clientNr].send_in += SendLen;
					}
					else {
						/* Abbruch wegen Überschreitung der maximalen Telegrammlänge */
						wait_data = TRUE;
					}
					break;
				}
			} /* if(client[clientNr].stateEvRead ==0) */
			else	break;
		} while (wait_data == FALSE);
		if(wait_data == TRUE)	break;
	} /*for(ev=0;ev<typecount;ev++) */

	/* Event-Abfrage um einen Typ weiterschalten */
	client[clientNr].startType  = (client[clientNr].startType+typ+1)%typecount;

}
/***********************************************************************************************
	ClientStream
	sendet Streamdaten blockweise an einen Client
***********************************************************************************************/
void ClientStream(INT clientNr)
{
	UDINT	TempLen,SendRest;

	if(client[clientNr].send_in != 0)	return;		/* wait until buffer is sent */

	switch(client[clientNr].stateStream)	{
		case STREAM_INIT:
			pMSEventHeadData = (EVENT_PIPE_DATA*) client[clientNr].EventBuffer;
			Anlage = pMSEventHeadData->unit;
			Geraet = pMSEventHeadData->device;
			Typ = pMSEventHeadData->type;
			Punkt = pMSEventHeadData->point;
			Item = pMSEventHeadData->item;
			Len = client[clientNr].lenEventMessage;

			pBRT_datastream = (CLI_DATASTREAM *)TempBuffOut;
			pBRT_datastream->head.ID = htons(client[clientNr].EventID++);
			pBRT_datastream->head.AK = htons(BRT_DATASTREAM);
			PutAdrs(&pBRT_datastream->Adr);
			TempLen = htonl(Len);
			memcpy(&pBRT_datastream->LenLow,&TempLen,sizeof(TempLen));
			/* Header in Sendebuffer eintragen */
			SendLen = sizeof(CLI_DATASTREAM);
			memcpy(&client[clientNr].send_buf[client[clientNr].send_in],TempBuffOut,SendLen);
			client[clientNr].send_in += SendLen;
			client[clientNr].stateStream = STREAM_BUSY;
			client[clientNr].pStreamData = pMSEventHeadData->pData;

		case STREAM_BUSY:
			/* Daten in Sendebuffer eintragen, bis alles gesendet ist */
			SendRest = pMSEventHeadData->pData + client[clientNr].lenEventMessage - client[clientNr].pStreamData;
			if( SendRest > (sizeof(client[clientNr].send_buf) - client[clientNr].send_in))	{
				SendLen = sizeof(client[clientNr].send_buf) - client[clientNr].send_in;
			}
			else	{
				SendLen = SendRest;
				client[clientNr].stateStream = STREAM_READY;
			}
			memcpy(&client[clientNr].send_buf[client[clientNr].send_in],client[clientNr].pStreamData,SendLen);
			client[clientNr].send_in += SendLen;
			client[clientNr].pStreamData +=SendLen;
			break;
		case STREAM_READY:
			/* Stream freigeben */
			pMSEventHeadData = (EVENT_PIPE_DATA*) client[clientNr].EventBuffer;
			Anlage = pMSEventHeadData->unit;
			Geraet = pMSEventHeadData->device;
			Typ = pMSEventHeadData->type;
			Punkt = pMSEventHeadData->point;
			Item = pMSEventHeadData->item;
/*			client[clientNr].stateRelease = ReleaseEventItemStream(myidBrServer,clientNr,Anlage,Geraet,Typ,Punkt,Item);*/

			client[clientNr].stateStream = STREAM_IDLE;
			break;
		default:
			client[clientNr].stateStream = STREAM_IDLE;
			break;
	}
}
/***********************************************************************************************
	ClientSend
	sendet Daten an einen Client
***********************************************************************************************/
void ClientSend(INT clientNr)
{
	DINT	send_len,send_ist;

	if(client[clientNr].send_in > client[clientNr].send_out)	{
		send_len = client[clientNr].send_in - client[clientNr].send_out;
		send_ist = send(client[clientNr].socket,(UDINT)&client[clientNr].send_buf[client[clientNr].send_out],send_len,0);
		if(send_ist == ERR_SOCKET)	{
			/* error */
			error = EthGetError();
			if(error != ERR_ETH_WOULDBLOCK )	{
				/* all errors except: waiting for sendbuffer free */
				ClientClose(clientNr,CLI_SENDERROR);
			}
		}
		else if (send_ist >= 0) {
			client[clientNr].send_out += send_ist;
			if(client[clientNr].send_out >= client[clientNr].send_in)	{
				client[clientNr].send_out = client[clientNr].send_in = 0;		/* total buffer was sent */
			}
		}
	}
}

/***********************************************************************************************/
_INIT void init(void)
{
	clients = 0;
	step = 0;
	initok = 0;
	DIAG_Init = 1;
}

/***********************************************************************************************/
_CYCLIC void cyclic(void)
{
	int clientNr;


	if(!initok)	{
		idstat = GetIdBrServer((void*)&myidBrServer,&brstate);
		if(!idstat)	{
			/* Portnummer holen */
			portstat = GetPortNumber(myidBrServer,&port);
			if(portstat)	{
				ERRxwarning(portstat,0,"brtcp: Portnummer ungültig");
				port = 20001;				/* Defaultwert im Fehlerfall */
			}

			/* Timeout holen */
			timeoutstat = GetTimeoutValue(myidBrServer,&timeout);
			if(timeoutstat)	{
				ERRxwarning(timeoutstat,0,"brtcp: Timeout ungültig");
				timeout = 60;				/* Defaultwert im Fehlerfall */
			}

			/* Clients initialisieren */
			for(clientNr=0;clientNr<MAX_CLIENTS;clientNr++)	{
				client[clientNr].socket = ERR_SOCKET;
				client[clientNr].status = CLI_INIT;
				client[clientNr].recv_cnt = 0;
				client[clientNr].EventCnt = 0;
				client[clientNr].TON_watchdog.PT = (UDINT)timeout * 1000;		/* [s] > [ms] */
				client[clientNr].TON_watchdog.IN = FALSE;
				TON(&client[clientNr].TON_watchdog);
				client[clientNr].stateStream = STREAM_IDLE;
			}
			initok = 1;
		}	/* idstat */
	} /* !initok */

	if(!initok)		return;

	if( DIAG_Init ) {
		DIAG_Init = 0;
		DIAG_ClientAuftraege = 0;
		DIAG_EventAuftraege = 0;
		DIAG_LogonCount = 0;
	}

	switch(step)
	{
		/*..............................................................................................*/
		/* init-states */
		case 0:
			socket_descr = socket(ETH_AF_INET, ETH_SOCK_STREAM, ETH_IPPROTO_TCP);
			if (stat_ioctl != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: socket()");
				step = 255;
			}
			break;
		case 1:
			arg = 1;
			stat_ioctl = ioctl(socket_descr, ETH_FIONBIO, (unsigned long)&arg);
			if (stat_ioctl != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: ioctl(ETH_FIONBIO)");
				step = 255;
			}
			break;
		case 2:
			opt_val = 1;
			len_opt_val = sizeof(opt_val);
			stat_setsockopt = setsockopt(socket_descr, ETH_SOL_SOCKET, ETH_SO_KEEPALIVE, (unsigned long)&opt_val, sizeof(opt_val));
			if (stat_setsockopt != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: setsockopt(KEEPALIVE)");
				step = 255;
			}
			break;
		case 3:
			opt_val = 1;
			stat_setsockopt = setsockopt(socket_descr, ETH_SOL_SOCKET, ETH_SO_REUSEADDR, (unsigned long)&opt_val, sizeof(opt_val));
			if (stat_setsockopt != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: setsockopt(REUSEADDR)");
				step = 255;
			}
			break;
		case 4:
			opt_val = sizeof(client[0].recv_buf);
			stat_setsockopt = setsockopt(socket_descr, ETH_SOL_SOCKET, ETH_SO_RCVBUF, (unsigned long)&opt_val, sizeof(opt_val));
			if (stat_setsockopt != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: setsockopt(SO_RCVBUF)");
				step = 255;
			}
			break;
		case 5:
			opt_val = sizeof(client[0].send_buf);
			stat_setsockopt = setsockopt(socket_descr, ETH_SOL_SOCKET, ETH_SO_SNDBUF, (unsigned long)&opt_val, sizeof(opt_val));
			if (stat_setsockopt != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: setsockopt(SO_SNDBUF)");
				step = 255;
			}
			break;
		case 6:
			name_str.sin_family = ETH_AF_INET;
#ifdef INTEL
			name_str.sin_port = swapUINT(port);
#else
			name_str.sin_port = port;
#endif
			name_str.sin_addr = MYADRS; /*(unsigned long)inet_addr((UDINT)MYADRS);*/
			memset(name_str.sin_zero,0,sizeof(name_str.sin_zero));

			stat_bind = bind(socket_descr, (unsigned long)&name_str, sizeof(name_str));
			if (stat_bind != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: bind()");
				step = 255;
			}
			break;
		case 7:
			bacllog = MAX_CLIENTS;
			stat_listen = listen(socket_descr, (unsigned long)bacllog); //18.02.2015 Erweiterung für ATOM-CPUs: der Übergabeparameter bacllog muss als Variable und nicht als Zeiger übergeben werden!
			if (stat_listen != ERR_SOCKET)	step++;
			else	{
				error = EthGetError();
				ERRxwarning(error,0,"brtcp: listen()");
				step = 255;
			}
			break;
		/*..............................................................................................*/
		/* init server */
		case 8:
			if(myidBrServer!=NULL)	{
				typestate = GetTypeInfo(myidBrServer,&typecount);
				instancestate = GetEventInstanceInfo(myidBrServer,&MaxCfgClients);		/* 06.07.04 */
				if(MaxCfgClients > MAX_CLIENTS)	MaxCfgClients = MAX_CLIENTS;
				if(typestate==cEC_OK && instancestate==cEC_OK )	step++;					/* ok goto next state */
			}
			break;
		/*..............................................................................................*/
		/* working state */
		case 9:
			if(myidBrServer==NULL)	{
				/* myidBrServer invalid: goto previous state */
				step--;
				break;
			}

			ClientConnect();					/* call accept()-Funktion to connect to clients */

#ifdef BURKLIMAT2000
			ClientDiagnose();					/* Client-Diagnose ( nur Burklimat2000 ); 13.08.03/RES */
#endif

			for(clientNr=0;clientNr<MaxCfgClients;clientNr++)	{		/* 06.07.04: statt MAX_CLIENTS */
				if(client[clientNr].socket != ERR_SOCKET)	{
					ClientWatch(clientNr);		/* check for receive timeout, terminate connection */
				}
				if(client[clientNr].socket != ERR_SOCKET)	{
					if(client[clientNr].stateStream == STREAM_IDLE)	{
						ClientRecv(clientNr);		/* read frames from client */
						ClientCommands(clientNr);	/* examine client commands */
						ClientEvents(clientNr);	/* check for events */
					}
					else {
						ClientStream(clientNr);	/* send stream data */
					}
					ClientSend(clientNr);		/* send data to client (also handling of fractional sends) */
				}
			}
			break;
		/*..............................................................................................*/
		/* error state */
		case 255:
			break;
	}
}


