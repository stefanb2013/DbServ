
typedef struct {
	unsigned short	Auftragscode;
	unsigned short	Anlage;
	unsigned short	Unterstation;
	unsigned short	Datentyp;
	unsigned short	Datenpunkt;
	unsigned short	Offset;
	unsigned short	Anzahl;
	unsigned short	res;
	unsigned char	Daten[32];
} EVENT_DOWN;

typedef struct {
	unsigned short	Auftragscode;
	unsigned short	Anlage;
	unsigned short	Unterstation;
	unsigned short	Datentyp;
	unsigned short	Datenpunkt;
	unsigned short	Offset;
	unsigned short	Anzahl;
	unsigned short	res;
} EVENT_DOWN_HEAD;

typedef struct {
	unsigned short	Auftragscode;
	unsigned short	Anlage;
	unsigned short	Unterstation;
	unsigned short	Datentyp;
	unsigned short	Datenpunkt;
	unsigned short	Offset;
	unsigned short	Anzahl;
	unsigned short	res;
	RTCtime_typ		RTCTime;
	unsigned char	Daten[32];
} EVENT_UP;

typedef struct {
	unsigned short	Auftragscode;
	unsigned short	Anlage;
	unsigned short	Unterstation;
	unsigned short	Datentyp;
	unsigned short	Datenpunkt;
	unsigned short	Offset;
	unsigned short	Anzahl;
	unsigned short	res;
	RTCtime_typ		RTCTime;
} EVENT_UP_HEAD;



