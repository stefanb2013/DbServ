
{REDUND_ERROR} FUNCTION_BLOCK IEC103Ut_WrCmd (*TODO: Hier einen Kommentar eingeben*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Enable : BOOL;
		DataSend : BOOL;
		LineVarStr : UDINT;
		LineNumber : USINT;
		DeviceAdr : USINT;
		FunctionType : USINT;
		InformationNumber : USINT;
		Val : USINT;
	END_VAR
	VAR_OUTPUT
		Status : DINT;
	END_VAR
	VAR
		WrCmdStep : UINT;
		PVStr : STRING[100];
		ValStr : STRING[10];
		ItemStr : STRING[100];
		PV_xgetadr_status : UINT;
		PV_ninfo_status : UINT;
		PVInfo_AnlStruct : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_AnlStructOld : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_DataSend : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_DeviceAdress : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_FunctionType : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_InformationNumber : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_Val : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_Ready : IEC103Ut_WrCmd_PVInfo_Type;
		PVInfo_Error : IEC103Ut_WrCmd_PVInfo_Type;
		IECIEC103Ut_WrSimplePV_0 : IEC103Ut_WrSimplePV;
		IEC103Ut_RdSimplePV_0 : IEC103Ut_RdSimplePV;
		LineVarStrOld : UDINT;
		LineNumberOld : USINT;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK IEC103Ut_WrSimplePV (*TODO: Hier einen Kommentar eingeben*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		PVAdr : {REDUND_UNREPLICABLE} UDINT;
		DataType : UDINT;
		PVValue : DINT;
	END_VAR
	VAR_OUTPUT
		valid : BOOL;
	END_VAR
	VAR
		pPV_BOOL : REFERENCE TO BOOL;
		pPV_USINT : REFERENCE TO USINT;
		pPV_UINT : REFERENCE TO UINT;
		pPV_UDINT : REFERENCE TO UDINT;
		pPV_SINT : REFERENCE TO SINT;
		pPV_INT : REFERENCE TO INT;
		pPV_DINT : REFERENCE TO DINT;
		pPV_REAL : REFERENCE TO REAL;
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR} FUNCTION_BLOCK IEC103Ut_RdSimplePV (*TODO: Hier einen Kommentar eingeben*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		PVAdr : {REDUND_UNREPLICABLE} UDINT;
		DataType : UDINT;
	END_VAR
	VAR_OUTPUT
		PVValue : DINT;
		valid : BOOL;
	END_VAR
	VAR
		pPV_BOOL : REFERENCE TO BOOL;
		pPV_USINT : REFERENCE TO USINT;
		pPV_UINT : REFERENCE TO UINT;
		pPV_UDINT : REFERENCE TO UDINT;
		pPV_SINT : REFERENCE TO SINT;
		pPV_INT : REFERENCE TO INT;
		pPV_DINT : REFERENCE TO DINT;
		pPV_REAL : REFERENCE TO REAL;
	END_VAR
END_FUNCTION_BLOCK
