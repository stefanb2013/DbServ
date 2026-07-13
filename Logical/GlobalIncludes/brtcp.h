/* TCP/IP definitions > configuration data ! */
#define MAX_CLIENTS	8
#define MYADRS	ETH_INADDR_ANY
#define RECVBUFLEN		1024
#define SENDBUFLEN		2048

#define FALSE		0
#define TRUE		1
#define NULL 		0

/* AK-codes */
#define BRT_ITM_LOGON			1
#define BRT_ITM_LOGON_TS		2
#define BRT_ITM_LOGOFF			3
#define BRT_ITM_READ			4
#define BRT_ITM_READ_TS			5
#define BRT_ITM_WRITE			6
#define BRT_ITM_EVENT			7
#define BRT_ITM_EVENT_TS		8
#define BRT_DATASTREAM_READ		9
#define BRT_CLI_WATCHDOG		10
#define BRT_TIMESYNC			11
#define BRT_DATASTREAM			12

#define BRT_ITM_LOGON_OK		101
#define BRT_ITM_LOGON_TS_OK		102
#define BRT_ITM_LOGOFF_OK		103
#define BRT_ITM_READ_OK			104
#define BRT_ITM_READ_TS_OK		105
#define BRT_ITM_WRITE_OK		106
#define BRT_DATASTREAM_READ_OK	109
#define BRT_CLI_WATCHDOG_OK		110
#define BRT_TIMESYNC_OK			111

#define BRT_CLI_ERROR			200
#define BRT_ITM_LOGON_ERR		201
#define BRT_ITM_LOGOFF_ERR		203
#define BRT_ITM_READ_ERR		204
#define BRT_ITM_WRITE_ERR		206
#define BRT_DATASTREAM_READ_ERR	209
#define BRT_TIMESYNC_ERR		211

/* error codes */
#define BRT_ERR_INVALIDAK			1
#define BRT_ERR_NO_ITEM				2
#define BRT_ERR_WRONG_LEN			3
#define BRT_ERR_WRONG_HYST			4
#define BRT_ERR_IS_LOGGED			5
#define BRT_ERR_ITEM_NO_TS			6

#define BRT_ERR_ITEM_NO_LOGON		7

#define BRT_ERR_NO_STREAM			8

#define BRT_ERR_RTC_SET				9

#define BRT_ERR_ITEM_NOT_WRITEABLE	10
#define BRT_ERR_WRITE_PIPE_FULL		11
#define BRT_ERR_ITEM_NO_DATA		12
#define BRT_ERR_SWC_SET				13

#define BRT_ERR_UNKNOWN_UNIT		14
#define BRT_ERR_UNKNOWN_DEVICE		15
#define BRT_ERR_UNKNOWN_TYPE		16
#define BRT_ERR_UNKNOWN_POINT		17
#define BRT_ERR_WRONG_ITEM			18



enum {CLI_INIT, CLI_CONNECTED, CLI_DISCONNECTED, CLI_ERROR, CLI_WATCHDOG, CLI_SENDERROR, CLI_STATUSANZAHL };	/* für client-Status */
enum {STREAM_IDLE, STREAM_INIT, STREAM_BUSY, STREAM_READY};

typedef struct{
	DINT				socket;
	INT					status;
	ETH_sockaddr_typ	client_str;
	UINT				recv_cnt;
	TON_typ				TON_watchdog;
	UDINT				recv_in;
	UDINT				recv_out;
	UDINT				send_in;
	UDINT				send_out;
	USINT				recv_buf[RECVBUFLEN];
	USINT				send_buf[SENDBUFLEN];
	UINT				EventID;
	UINT				startType;
	INT					stateEvRead;
	USINT				EventBuffer[256];
	UDINT				lenEventMessage;
	UDINT				lengthTimeStamp;
	UINT				EventCnt;
	UINT				stateStream;
	USINT				*pStreamData;
	INT					stateRelease;
} client_typ;

typedef struct {
	UINT				ID;
	UINT				AK;
} CLI_HEAD_TYP;

typedef struct {
	UINT				anlage;
	UINT				geraet;
	UINT				typ;
	UINT				punkt;
	UINT				item;
} CLI_ADR_TYP;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				LenLow;
	UINT				LenHigh;
	UINT				Data;
} CLI_LOGON_REQUEST;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				LenLow;
	UINT				LenHigh;
	UINT				Data;
} CLI_LOGON_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
} CLI_LOGOFF_REQUEST;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
} CLI_LOGOFF_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
} CLI_READ_REQUEST;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				LenLow;
	UINT				LenHigh;
	UINT				Data;
} CLI_READ_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				LenLow;
	UINT				LenHigh;
	UINT				Data;
} CLI_WRITE_REQUEST;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
} CLI_WRITE_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				LenLow;
	UINT				LenHigh;
	UINT				Data;
} CLI_EVENT_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	UINT				Error;
} CLI_ERROR_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				Error;
} CLI_ADRERROR_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
} CLI_WATCHDOG_REQUEST;

typedef struct {
	CLI_HEAD_TYP		head;
} CLI_WATCHDOG_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	RTCtime_typ			Time;
} CLI_TIMESYNC_REQUEST;

typedef struct {
	CLI_HEAD_TYP		head;
} CLI_TIMESYNC_RESPONSE;

typedef struct {
	CLI_HEAD_TYP		head;
	CLI_ADR_TYP			Adr;
	UINT				LenLow;
	UINT				LenHigh;
} CLI_DATASTREAM;


