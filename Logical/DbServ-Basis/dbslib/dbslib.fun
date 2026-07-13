FUNCTION_BLOCK DbsGetBool
VAR_INPUT
		Unit	:UINT;	
		Device	:UINT;	
		Type	:UINT;	
		Point	:UINT;	
		Item	:UINT;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:BOOL;	
		DbsValid	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsGetInt
VAR_INPUT
		Unit	:UINT;	
		Device	:UINT;	
		Type	:UINT;	
		Point	:UINT;	
		Item	:UINT;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:INT;	
		DbsValid	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsUdint
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:UDINT;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:UDINT;	
		DbsValid	:BOOL;	
		WriteValue	:UDINT;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsDint
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:DINT;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:DINT;	
		DbsValid	:BOOL;	
		WriteValue	:DINT;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsInt
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:INT;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:INT;	
		DbsValid	:BOOL;	
		WriteValue	:INT;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsUint
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:UINT;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:UINT;	
		DbsValid	:BOOL;	
		WriteValue	:UINT;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsUsintTs
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:USINT;	
		InTimeStamp	:RTCtime_typ;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:USINT;	
		DbsTimeStamp	:RTCtime_typ;	
		DbsValid	:BOOL;	
		WriteValue	:USINT;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsUsint
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:USINT;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:USINT;	
		DbsValid	:BOOL;	
		WriteValue	:USINT;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsBoolTs
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:BOOL;	
		InTimeStamp	:RTCtime_typ;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:BOOL;	
		DbsTimeStamp	:RTCtime_typ;	
		DbsValid	:BOOL;	
		WriteValue	:BOOL;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsBool
VAR
		FubRef	:FUB_REF;	
	END_VAR
	VAR_INPUT
		InValue	:BOOL;	
		SetValue	:BOOL;	
		Mode	:UINT;	
		WriteEnable	:BOOL;	
	END_VAR
	VAR_OUTPUT
		DbsValue	:BOOL;	
		DbsValid	:BOOL;	
		WriteValue	:BOOL;	
		TrgWriteValue	:BOOL;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
FUNCTION_BLOCK DbsRtcTime
VAR_OUTPUT
		RtcTime	:RTCtime_typ;	
		Stat	:UINT;	
	END_VAR
END_FUNCTION_BLOCK
