(*-----------------------------------------------
FUNCTION Block IEC103Ut_RdSimplePV

	Read value of a PV

	Inputs:
		PVAdr				Adress OF PV
		DataType			Datatype OF PV

	Outputs:
		PVValue				Value of the PV
		valid				TRUE	Datatype valid
							FALSE	Adress OF PV OR Datatype NOT valid

	Version:
		07.02.2019	Version 1.0	
		first edition

-----------------------------------------------
*)

FUNCTION_BLOCK IEC103Ut_RdSimplePV
	valid := TRUE;
	
	IF(PVAdr <> 0) THEN
		CASE DataType OF

			IEC103Ut_DATA_TYPE_BOOL:
				pPV_BOOL ACCESS PVAdr;
				PVValue := BOOL_TO_DINT(pPV_BOOL);

			IEC103Ut_DATA_TYPE_USINT:
				pPV_USINT ACCESS PVAdr;
				PVValue := USINT_TO_DINT(pPV_USINT);

			IEC103Ut_DATA_TYPE_UINT:
				pPV_UINT ACCESS PVAdr;
				PVValue := UINT_TO_DINT(pPV_UINT);

			IEC103Ut_DATA_TYPE_UDINT:
				pPV_UDINT ACCESS PVAdr;
				PVValue := UDINT_TO_DINT(pPV_UDINT);

			IEC103Ut_DATA_TYPE_SINT:
				pPV_SINT ACCESS PVAdr;
				PVValue := SINT_TO_DINT(pPV_SINT);

			IEC103Ut_DATA_TYPE_INT:
				pPV_INT ACCESS PVAdr;
				PVValue := INT_TO_DINT(pPV_INT);

			IEC103Ut_DATA_TYPE_DINT:
				pPV_DINT ACCESS PVAdr;
				PVValue := pPV_DINT;

			ELSE
				valid:= FALSE;
		END_CASE
	ELSE
		valid:= FALSE;
	END_IF
END_FUNCTION_BLOCK
