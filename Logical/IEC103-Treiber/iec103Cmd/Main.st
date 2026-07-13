
PROGRAM _INIT

	IEC103Ut_WrCmd_0.Enable				:= TRUE;
	IEC103Ut_WrCmd_0.DataSend			:= FALSE;
	IEC103Ut_WrCmd_0.LineNumber			:= 1;
//	IEC103Ut_WrCmd_0.LineVarStr			:= ADR('iec103:stANL[0].Command');
	IEC103Ut_WrCmd_0.LineVarStr			:= 0;
	IEC103Ut_WrCmd_0.DeviceAdr			:= 104;
	IEC103Ut_WrCmd_0.FunctionType		:= 100;
	IEC103Ut_WrCmd_0.InformationNumber	:= 120;
	IEC103Ut_WrCmd_0.Val				:= 0;

END_PROGRAM

PROGRAM _CYCLIC

	IEC103Ut_WrCmd_0.DataSend	:= Send;
	IEC103Ut_WrCmd_0();

	IF(IEC103Ut_WrCmd_0.DataSend = TRUE) THEN
		IF(IEC103Ut_WrCmd_0.Status <> IEC103Ut_FUB_BUSY) THEN
			status	:= IEC103Ut_WrCmd_0.Status;
			Ready	:= TRUE;
			Send	:= FALSE;
		END_IF
	END_IF
	
END_PROGRAM

PROGRAM _EXIT
	(* Insert code here *)
	 
END_PROGRAM

