                                                                      
{REDUND_ERROR}  FUNCTION_BLOCK DPMInit                 (*searches and initializes the specified device*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		Device	:UDINT;                (*device name given as pointer, e.g., "SL2.SS1.IF1"*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
		MemSize	:USINT;                (*indicates the DPM size on the card.*)
		DevIdent	:UDINT;            (*device identifier*)
		Run	:BOOL;                     (*device in run: TRUE = in run, FALSE = not in run*)
		Configuration	:BOOL;         (*configuration: TRUE = valid, FALSE = not valid*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMio                   (*copies the input and output data areas between the B&R card and the specified buffers; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		OutBuffer	:UDINT;            (*pointer to the output data area*)
		OutLen	:UDINT;                (*length of the output data area*)
		InBuffer	:UDINT;            (*pointer to the input data area*)
		InLen	:UDINT;                (*length of the input data area*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
	END_VAR
	VAR
		hmode	:USINT;                (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMStop                 (*stops the I/O exchange process*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMCommand              (*Sends commands directly to bus stations; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		RemoteAdress	:USINT;        (*remote slave node number, 1..126 = slave with specified node number, 127 = broadcast to all slaves*)
		GroupSelect	:USINT;            (*selects the slave group, 0 = only remoteAdress applies, 1..255 = slaves in specified goup*)
		Command	:USINT;                (*spezifies the type of command, e.g., freeze*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
	END_VAR
	VAR
		istate	:UDINT;                (*internal variable*)
		waitCnt	:UINT;                 (*internal variable*)
		MessageHeader1	:USINT;        (*internal variable*)
		MessageHeader2	:USINT;        (*internal variable*)
		MessageHeader3	:USINT;        (*internal variable*)
		MessageHeader4	:USINT;        (*internal variable*)
		MessageHeader5	:USINT;        (*internal variable*)
		MessageHeader6	:USINT;        (*internal variable*)
		MessageHeader7	:USINT;        (*internal variable*)
		MessageHeader8	:USINT;        (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMConfig               (*downloads the configuration; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		ConfModulName	:UDINT;        (*configuration module name given as pointer*)
		Mode	:BOOL;                 (*download mode, TRUE = sequential, FALSE = in one step*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
		Remaining	:UDINT;            (*displays the remaining segments until the configuration is completely loaded*)
	END_VAR
	VAR
		istate	:UDINT;                (*internal variable*)
		FileLength	:UINT;             (*internal variable*)
		waitCnt	:UINT;                 (*internal variable*)
		OffsetCopy	:UDINT;            (*internal variable*)
		pData	:UDINT;                (*internal variable*)
		Ready	:USINT;                (*internal variable*)
		FirstMessage	:BOOL;         (*internal variable*)
		MsgPutReady	:BOOL;             (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMSlaveStat            (*returns the current slave status*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		SlaveReady	:UDINT;            (*pointer to a 16 byte array indicating whether a slave is in data transfer mode*)
		SlaveDiag	:UDINT;            (*pointer to a 16 byte array indicating availability of diagnose information on slaves*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
		SlaveStatus	:BOOL;             (*TRUE = a configured slave is not in data transfer mode or has diagnostic messages*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMSlaveExtStat         (*returns the exact status of a slave; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		SlaveNumber	:USINT;            (*slave number*)
		SlaveDiagnostic	:UDINT;        (*pointer to an array of 48 bytes containing extended status and diagnostic information*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
		SlaveReady	:BOOL;             (*TRUE = slave is not in data transfer mode*)
		SlaveDiag	:BOOL;             (*TRUE = slave is in diagnostics mode*)
	END_VAR
	VAR
		istate	:UDINT;               (*internal variable*)
		waitCnt	:UINT;                (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMOff                  (*stops a ProfiBus DP master layer 2 communication*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMOn                   (*starts a ProfiBus DP master layer 2 communication*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMStart                (*starts the I/O exchange process*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, 0xXXXX = see help*)
	END_VAR
END_FUNCTION_BLOCK

FUNCTION DPMSearchDevices : UINT       (*searches the target system for B&R fieldbus cards*)
	VAR_INPUT
		pInfos	:UDINT;                (*pointer to the device information structure*)
		count	:USINT;                (*number of devices to be searched*)
	END_VAR
END_FUNCTION

{REDUND_ERROR}  FUNCTION_BLOCK DPMDialog               (*send messages to the B&R card; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		Message	:UDINT;                (*pointer to the message structure*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
	END_VAR
	VAR
		istate	:UDINT;                (*internal variable*)
		tx	:USINT;                    (*internal variable*)
		waitCount	:UINT;             (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMGetMessage           (*sends a message to the B&R card and does not wait for an answer; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		Message	:UDINT;                (*pointer to the message structure*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
	END_VAR
	VAR
		waitCount	:UINT;             (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK

{REDUND_ERROR}  FUNCTION_BLOCK DPMPutMessage            (*reads out a message from the B&R card; asynchronous execution*)
	VAR_INPUT
		enable	:BOOL;                 (*enables execution*)
		DevIdent	:UDINT;            (*device identifier returned from DPMInit*)
		Message	:UDINT;                (*pointer to the message structure*)
	END_VAR
	VAR_OUTPUT
		status	:UINT;                 (*execution status: ERR_OK, ERR_FUB_ENABLE_FALSE, ERR_FUB_BUSY, 0xXXXX = see help*)
	END_VAR
	VAR
		waitCount	:UINT;             (*internal variable*)
	END_VAR
END_FUNCTION_BLOCK
