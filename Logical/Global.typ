
TYPE
	XstDiagnose : 	STRUCT 
		ulFRM_rbufFailAnz : UDINT;
		ulFRM_readFailAnz : UDINT;
		ulWrongTelAnz : UDINT;
		ulDTExSetTimeFail : UDINT;
		ulTelTimeoutAnz : UDINT;
		ulSyncAnz : UDINT;
		ulRTC_gettimeFail : UDINT;
		ulSyncPosFail : UDINT;
	END_STRUCT;
	XstMeinberg : 	STRUCT 
		ucTag : USINT;
		ucMonat : USINT;
		ucJahr : USINT;
		ucStunden : USINT;
		ucMinuten : USINT;
		ucSekunden : USINT;
		ucSyncOK : USINT;
		ucPosOK : USINT;
		ucZZone : USINT;
		ucZSprung : USINT;
	END_STRUCT;
END_TYPE
