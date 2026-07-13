
(* 
-----------------------------------------------
FUNCTION Block IEC103Ut_WrCmd

	Write commands to a device, using the command structure OF IEC103 Task

	Inputs:
		Enable		 		Enable function block
		DataSend			Send command do device (edge sensitive)
		LineVarStr			Option: Adress OF name OF local PV structure element in Task IEC103 (example: "iec103:stANL[0].Command")
		LineNumber			Option:	Linenumber (1..12)
							Either LineVarStr or LineNumber is necessary
		DeviceAdr			Adress of device
		FunctionType		Function Type
		InformationNumber	Information Number
		Val					Value

	Outputs:
		Status				0 		Function ready, without error

							52000	Invalid input parameter
							52008	Timeout response telegram
							52020	command NOT accepted
							52023	response telegram: value not equal
							52024	response telegram: return information not equal
							52025	command function disabled
							52026	connection to device not ready
							52027	wrong device adress
							52028	command function still active

							3300 - 14714	error state of function PV_xgetadr or PV_ninfo (see B&R Help)

							65533	functionblock idle
							65534	functionblock disabled
							65535	functionblock busy

	Version:
	07.02.2019	Version 1.0	
				first edition

-----------------------------------------------
*)
FUNCTION_BLOCK IEC103Ut_WrCmd

	IF(Enable = TRUE) THEN
		CASE WrCmdStep OF
			0:
				// ---------------------
				// check input parameter 
				IF((LineVarStr = 0) AND (LineNumber = 0)) THEN
					Status		:= IED103Ut_ERROR_INPUT_INVALID;
					WrCmdStep	:= 210;
				ELSE
					
					Status := IEC103Ut_FUB_BUSY;
					// ---------------------------------------------
					// get name and adress of main structure element
					IF(LineVarStr = 0) THEN
						// Adresse der Anlagenstruktur ermitteln: PV Name erzeugen und PV Adresse ermitteln
						brsstrcpy(ADR(PVStr),ADR('iec103:stANL['));
						brsitoa(USINT_TO_DINT(LineNumber-1),ADR(ValStr));		
						brsstrcat(ADR(PVStr),ADR(ValStr));
						brsstrcat(ADR(PVStr),ADR('].Command'));
					ELSE
						// Adresse der Anlagenstruktur ermitteln: Uebergebenen PV Namen verwenden und PV Adresse ermitteln
						brsstrcpy(ADR(PVStr),LineVarStr);
					END_IF
					
					PV_xgetadr_status := PV_xgetadr(ADR(PVStr),ADR(PVInfo_AnlStruct.PVAdr),ADR(PVInfo_AnlStruct.PVLen));
	
					IF(PV_xgetadr_status = 0) THEN
						LineNumberOld	:= LineNumber;
						LineVarStrOld	:= LineVarStr;
						WrCmdStep		:= 10;
					ELSE
						Status			:= PV_xgetadr_status;
						WrCmdStep		:= 210;
					END_IF
				END_IF
				
			10:	// get adress and info about structure element: DataSend
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.DataSend'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_DataSend.PVAdr),ADR(PVInfo_DataSend.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 11;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			11:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_DataSend.PVDataType),ADR(PVInfo_DataSend.PVLen),ADR(PVInfo_DataSend.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 15;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			15:	// get adress and info about structure element: PVInfo_DeviceAdress
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.DeviceAdress'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_DeviceAdress.PVAdr),ADR(PVInfo_DeviceAdress.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 16;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			16:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_DeviceAdress.PVDataType),ADR(PVInfo_DeviceAdress.PVLen),ADR(PVInfo_DeviceAdress.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 20;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			20:	// get adress and info about structure element: FunctionType
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.FunctionType'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_FunctionType.PVAdr),ADR(PVInfo_FunctionType.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 21;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			21:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_FunctionType.PVDataType),ADR(PVInfo_FunctionType.PVLen),ADR(PVInfo_FunctionType.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 25;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			25:	// get adress and info about structure element: InformationNumber
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.InformationNumber'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_InformationNumber.PVAdr),ADR(PVInfo_InformationNumber.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 26;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			26:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_InformationNumber.PVDataType),ADR(PVInfo_InformationNumber.PVLen),ADR(PVInfo_InformationNumber.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 30;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			30:	// get adress and info about structure element: Val
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.Val'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_Val.PVAdr),ADR(PVInfo_Val.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 31;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			31:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_Val.PVDataType),ADR(PVInfo_Val.PVLen),ADR(PVInfo_Val.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 35;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			35:	// get adress and info about structure element: Ready
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.Ready'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_Ready.PVAdr),ADR(PVInfo_Ready.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 36;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			36:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_Ready.PVDataType),ADR(PVInfo_Ready.PVLen),ADR(PVInfo_Ready.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 40;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			40:	// get adress and info about structure element: Error
				brsstrcpy(ADR(ItemStr),ADR(PVStr));
				brsstrcat(ADR(ItemStr),ADR('.Error'));
				PV_xgetadr_status := PV_xgetadr(ADR(ItemStr),ADR(PVInfo_Error.PVAdr),ADR(PVInfo_Error.PVLen));
				IF(PV_xgetadr_status = 0) THEN
					WrCmdStep	:= 41;
				ELSE
					Status		:= PV_xgetadr_status;
					WrCmdStep	:= 210;
				END_IF

			41:
				PV_ninfo_status := PV_ninfo(ADR(ItemStr),ADR(PVInfo_Error.PVDataType),ADR(PVInfo_Error.PVLen),ADR(PVInfo_Error.PVDimension));
				IF(PV_ninfo_status = 0) THEN
					WrCmdStep	:= 100;
				ELSE
					Status		:= PV_ninfo_status;
					WrCmdStep	:= 210;
				END_IF

			100:
				// Idle step, wait for command to send
				IF(DataSend = TRUE) THEN
					IF((LineNumberOld <> LineNumber) OR (LineVarStrOld <> LineVarStr)) THEN
						Status		:= IEC103Ut_FUB_BUSY;
						WrCmdStep	:= 0;
					ELSE
						IF(DeviceAdr = 0) THEN
							Status		:= IED103Ut_ERROR_INPUT_INVALID;
							WrCmdStep	:= 120;
						ELSE
							// Send command to device
							Status := IEC103Ut_FUB_BUSY;

							// Write parameter and command to local structure of iec task
							IECIEC103Ut_WrSimplePV_0(PVAdr := PVInfo_Ready.PVAdr, 				DataType := PVInfo_Ready.PVDataType,				PVValue := 0);
							IECIEC103Ut_WrSimplePV_0(PVAdr := PVInfo_FunctionType.PVAdr,		DataType := PVInfo_FunctionType.PVDataType,			PVValue := FunctionType);
							IECIEC103Ut_WrSimplePV_0(PVAdr := PVInfo_InformationNumber.PVAdr,	DataType := PVInfo_InformationNumber.PVDataType,	PVValue := InformationNumber);
							IECIEC103Ut_WrSimplePV_0(PVAdr := PVInfo_Val.PVAdr,					DataType := PVInfo_Val.PVDataType,					PVValue := Val);
							IECIEC103Ut_WrSimplePV_0(PVAdr := PVInfo_DeviceAdress.PVAdr,		DataType := PVInfo_DeviceAdress.PVDataType,			PVValue := DeviceAdr);
							IECIEC103Ut_WrSimplePV_0(PVAdr := PVInfo_DataSend.PVAdr,			DataType := PVInfo_DataSend.PVDataType,				PVValue := 1);
							WrCmdStep		:= 110;
						END_IF
					END_IF
				ELSE
					Status	:= IEC103Ut_FUB_IDLE;
				END_IF
				
			110:
				// Read result information from local structure of iec task
				IEC103Ut_RdSimplePV_0(PVAdr := PVInfo_Ready.PVAdr, DataType := PVInfo_Ready.PVDataType);

				// Ready
				IF(IEC103Ut_RdSimplePV_0.PVValue > 0) THEN
					// Error Information
					IEC103Ut_RdSimplePV_0(PVAdr := PVInfo_Error.PVAdr, DataType := PVInfo_Error.PVDataType);
					IF(IEC103Ut_RdSimplePV_0.PVValue = 0) THEN
						Status	:= 0;
					ELSE
						Status	:= IED103Ut_ERROR_START + DINT_TO_UINT(IEC103Ut_RdSimplePV_0.PVValue);
					END_IF
					WrCmdStep	:= 120;
				END_IF

				IF(DataSend = FALSE) THEN
					Status		:= IEC103Ut_FUB_IDLE;
					WrCmdStep	:= 100;
				END_IF

			120:
				// Wait until command is cleared
				IF(DataSend = FALSE) THEN
					Status		:= IEC103Ut_FUB_IDLE;
					WrCmdStep	:= 100;
				END_IF

			210:
				// Error Step, function block has to be disabled
		END_CASE
	ELSE
		WrCmdStep	:= 0;
		Status		:= IEC103Ut_FUB_DISABLED;
	END_IF
	
END_FUNCTION_BLOCK
