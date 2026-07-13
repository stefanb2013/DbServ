
(* 
-----------------------------------------------
FUNCTION Block IEC103Ut_WrSimplePV

	Write value of a PV

	Inputs:
		PVAdr				Adress of PV
		DataType			Datatype of PV
		PVValue				Value witch is written

	Outputs:
		valid				TRUE	Datatype valid
							FALSE	Adress of PV or Datatype not valid

	Version:
	07.02.2019	Version 1.0	
				first edition

-----------------------------------------------
*)
FUNCTION_BLOCK IEC103Ut_WrSimplePV

	valid := TRUE;
	
	IF(PVAdr <> 0) THEN
		CASE DataType OF

			IEC103Ut_DATA_TYPE_BOOL:
				pPV_BOOL ACCESS PVAdr;
				pPV_BOOL := DINT_TO_BOOL(PVValue);

			IEC103Ut_DATA_TYPE_USINT:
				pPV_USINT ACCESS PVAdr;
				pPV_USINT := DINT_TO_USINT(PVValue);

			IEC103Ut_DATA_TYPE_UINT:
				pPV_UINT ACCESS PVAdr;
				pPV_UINT := DINT_TO_UINT(PVValue);

			IEC103Ut_DATA_TYPE_UDINT:
				pPV_UDINT ACCESS PVAdr;
				pPV_UDINT := DINT_TO_UDINT(PVValue);

			IEC103Ut_DATA_TYPE_SINT:
				pPV_SINT ACCESS PVAdr;
				pPV_SINT := DINT_TO_SINT(PVValue);

			IEC103Ut_DATA_TYPE_INT:
				pPV_INT ACCESS PVAdr;
				pPV_INT := DINT_TO_INT(PVValue);
				
			IEC103Ut_DATA_TYPE_DINT:
				pPV_DINT ACCESS PVAdr;
				pPV_DINT := PVValue;
			ELSE
				valid:= FALSE;
		END_CASE
	ELSE
		valid:= FALSE;
	END_IF
	
END_FUNCTION_BLOCK
