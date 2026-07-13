SHELL := cmd.exe
CYGWIN=nontsec
export PATH := C:\Program Files\Python314\Scripts\;C:\Program Files\Python314\;C:\Program Files\Python311\Scripts\;C:\Program Files\Python311\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Pulse Secure\VC142.CRT\X64\;C:\Program Files (x86)\Pulse Secure\VC142.CRT\X86\;C:\Program Files (x86)\Common Files\Pulse Secure\TNC Client Plugin\;C:\Program Files\GitExtensions\;C:\Program Files\dotnet\;C:\Program Files\Microsoft VS Code\bin;C:\Program Files\TortoiseGit\bin;C:\Program Files\Git\cmd;C:\Users\DESTBIE1\AppData\Local\Microsoft\WindowsApps;C:\Users\DESTBIE1\AppData\Local\Programs\Ollama;C:\Users\DESTBIE1\AppData\Local\Microsoft\WindowsApps;C:\Users\DESTBIE1\AppData\Local\Programs\Ollama;C:\Program Files (x86)\BRAutomation\AS6\bin-de
export AS_BUILD_MODE := Rebuild
export AS_VERSION := 6.5.3.11 SP
export AS_WORKINGVERSION := 6.5
export AS_COMPANY_NAME := ABB
export AS_USER_NAME := DESTBIE1
export AS_PATH := C:/Program Files (x86)/BRAutomation/AS6
export AS_BIN_PATH := C:/Program Files (x86)/BRAutomation/AS6/bin-de
export AS_PROJECT_PATH := C:/BIE/03_AS_Projekte/DBServ/07.00/dbserv_0700
export AS_PROJECT_NAME := dbserv_0640
export AS_SYSTEM_PATH := C:/Program\ Files\ (x86)/BRAutomation/AS6/AS/System
export AS_VC_PATH := C:/Program\ Files\ (x86)/BRAutomation/AS6/AS/VC
export AS_TEMP_PATH := C:/BIE/03_AS_Projekte/DBServ/07.00/dbserv_0700/Temp
export AS_CONFIGURATION := CP3586
export AS_BINARIES_PATH := C:/BIE/03_AS_Projekte/DBServ/07.00/dbserv_0700/Binaries
export AS_GNU_INST_PATH := C:/Program\ Files\ \(x86\)/BRAutomation/AS6/AS/GnuInst/V11.3.0
export AS_GNU_BIN_PATH := C:/Program\ Files\ \(x86\)/BRAutomation/AS6/AS/GnuInst/V11.3.0/6.3/bin
export AS_GNU_INST_PATH_SUB_MAKE := C:/Program Files (x86)/BRAutomation/AS6/AS/GnuInst/V11.3.0
export AS_GNU_BIN_PATH_SUB_MAKE := C:/Program Files (x86)/BRAutomation/AS6/AS/GnuInst/V11.3.0/6.3/bin
export AS_INSTALL_PATH := C:/Program\ Files\ \(x86\)/BRAutomation/AS6
export WIN32_AS_PATH := "C:\Program Files (x86)\BRAutomation\AS6"
export WIN32_AS_BIN_PATH := "C:\Program Files (x86)\BRAutomation\AS6\bin-de"
export WIN32_AS_PROJECT_PATH := "C:\BIE\03_AS_Projekte\DBServ\07.00\dbserv_0700"
export WIN32_AS_SYSTEM_PATH := "C:\Program Files (x86)\BRAutomation\AS6\AS\System"
export WIN32_AS_VC_PATH := "C:\Program Files (x86)\BRAutomation\AS6\AS\VC"
export WIN32_AS_TEMP_PATH := "C:\BIE\03_AS_Projekte\DBServ\07.00\dbserv_0700\Temp"
export WIN32_AS_BINARIES_PATH := "C:\BIE\03_AS_Projekte\DBServ\07.00\dbserv_0700\Binaries"
export WIN32_AS_GNU_INST_PATH := "C:\Program Files (x86)\BRAutomation\AS6\AS\GnuInst\V11.3.0"
export WIN32_AS_GNU_BIN_PATH := "C:\Program Files (x86)\BRAutomation\AS6\AS\GnuInst\V11.3.0\bin"
export WIN32_AS_INSTALL_PATH := "C:\Program Files (x86)\BRAutomation\AS6"

.suffixes:

ProjectMakeFile:

	@'$(AS_BIN_PATH)/BR.AS.AnalyseProject.exe' '$(AS_PROJECT_PATH)/dbserv_0640.apj' -t '$(AS_TEMP_PATH)' -c '$(AS_CONFIGURATION)' -o '$(AS_BINARIES_PATH)'   -sfas -buildMode 'Rebuild'   

