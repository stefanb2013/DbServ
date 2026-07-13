<?xml version="1.0" encoding="utf-8"?>
<?AutomationStudio FileVersion="4.9"?>
<SwConfiguration CpuAddress="SL1" xmlns="http://br-automation.co.at/AS/SwConfiguration">
  <TaskClass Name="Cyclic#1" />
  <TaskClass Name="Cyclic#2" />
  <TaskClass Name="Cyclic#3">
    <Task Name="io" Source="IO-Treiber.io.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="kupplung" Source="Anwendung.kupplung.prg" Memory="UserROM" Language="IEC" Debugging="true" />
    <Task Name="anlage" Source="Anwendung.anlage.prg" Memory="UserROM" Language="IEC" Debugging="true" />
    <Task Name="iec103" Source="IEC103-Treiber.iec103.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="iodrv" Source="IO-Treiber.iodrv.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="dbsevent" Source="DbServ-Basis.dbsevent.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="iec103Cmd" Source="IEC103-Treiber.iec103Cmd.prg" Memory="UserROM" Language="IEC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#4">
    <Task Name="def" Source="def.prg" Memory="UserROM" Language="IEC" Debugging="true" />
    <Task Name="Lebensbit" Source="Lebensbit.prg" Memory="UserROM" Language="IEC" Debugging="true" />
    <Task Name="zaehler" Source="zaehler.prg" Memory="UserROM" Language="IEC" Debugging="true" />
  </TaskClass>
  <TaskClass Name="Cyclic#5" />
  <TaskClass Name="Cyclic#6" />
  <TaskClass Name="Cyclic#7" />
  <TaskClass Name="Cyclic#8">
    <Task Name="dbstcp" Source="DbServ-Basis.dbstcp.prg" Memory="UserROM" Language="ANSIC" Debugging="true" />
    <Task Name="dbserv" Source="DbServ-Basis.dbserv.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="dbsmon" Source="DbServ-Basis.dbsmon.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <Task Name="dbstest" Source="DbServ-Basis.dbstest.prg" Memory="UserROM" Language="ANSIC" Debugging="false" />
  </TaskClass>
  <DataObjects>
    <DataObject Name="dplist" Source="Anwendung.dplist.dob" Memory="UserROM" Language="Simple" />
    <DataObject Name="typlist" Source="Anwendung.typlist.dob" Memory="UserROM" Language="Simple" />
  </DataObjects>
  <Binaries>
    <BinaryObject Name="TCData" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="udbdef" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="sysconf" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="iomap" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="asfw" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="arconfig" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="ashwd" Source="" Memory="SystemROM" Language="Binary" />
    <BinaryObject Name="ashwac" Source="" Memory="UserROM" Language="Binary" />
    <BinaryObject Name="mvLoader" Source="" Memory="UserROM" Language="Binary" />
  </Binaries>
  <Libraries>
    <LibraryObject Name="dbslib" Source="DbServ-Basis.dbslib.lby" Memory="UserROM" Language="ANSIC" Debugging="false" />
    <LibraryObject Name="sys_lib" Source="Libraries.sys_lib.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="standard" Source="Libraries.standard.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="runtime" Source="Libraries.runtime.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="OPERATOR" Source="Libraries.OPERATOR.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="FileIO" Source="Libraries.FileIO.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="dvframe" Source="Libraries.dvframe.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="DPMaster" Source="Libraries.DPMaster.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="DataObj" Source="Libraries.DataObj.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="CAN_Lib" Source="Libraries.CAN_Lib.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="brsystem" Source="Libraries.brsystem.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="astime" Source="Libraries.astime.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsHW" Source="Libraries.AsHW.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsBrStr" Source="Libraries.AsBrStr.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="IEC103Ut" Source="Libraries.IEC103Ut.lby" Memory="UserROM" Language="IEC" Debugging="true" />
    <LibraryObject Name="ArEventLog" Source="Libraries.ArEventLog.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="AsIecCon" Source="Libraries.AsIecCon.lby" Memory="UserROM" Language="Binary" Debugging="true" />
    <LibraryObject Name="ethsock" Source="" Memory="UserROM" Language="Binary" Debugging="true" />
  </Libraries>
</SwConfiguration>