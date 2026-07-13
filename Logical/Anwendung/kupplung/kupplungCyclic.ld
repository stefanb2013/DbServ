<?xml version="1.0" encoding="utf-8"?>
<?AutomationStudio FileVersion="6.0"?>
<Project xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xhtml="http://www.w3.org/1999/xhtml" schemaVersion="1.0" xmlns="www.iec.ch/public/TC65SC65BWG7TF10">
  <FileHeader companyName="B&amp;R Industrial Automation GmbH" productName="Automation Studio" productVersion="6.0" />
  <ContentHeader name="" creationDateTime="0001-01-01T00:00:00">
    <CoordinateInfo />
    <AddData>
      <Data name="BR_EditorMode" handleUnknown="implementation">
        <EditorMode value="Legacy" />
      </Data>
    </AddData>
  </ContentHeader>
  <Types>
    <GlobalNamespace>
      <Program name="">
        <AddData>
          <Data name="BR_EntryPoint" handleUnknown="implementation">
            <EntryPoint entryPointType="_CYCLIC" />
          </Data>
        </AddData>
        <MainBody>
          <AddData>
            <Data name="BR_CodeInformation" handleUnknown="implementation">
              <CodeInformation><![CDATA[CODE_INFORMATION_754EF481_1721_4FA5_BF7F_DE1B91DFA69B_BEGIN
PROGRAM _CYCLIC

VAR
END_VAR


{NetworkNumber := 1}
LD	1
ST	DbsGet_MSN1215KBM03.Unit
LD	1
ST	DbsGet_MSN1215KBM03.Device
LD	50
ST	DbsGet_MSN1215KBM03.Type
LD	1
ST	DbsGet_MSN1215KBM03.Point
LD	1
ST	DbsGet_MSN1215KBM03.Item
CAL	DbsGet_MSN1215KBM03
LD	DbsGet_MSN1215KBM03.DbsValue
ST	ABB_025101_MSN01215KBM03 {CheckDataType := BOOL}
LD	DbsGet_MSN1215KBM03.DbsValid
ST	ABB_025101KBM03Valid {CheckDataType := BOOL}

{NetworkNumber := 2}
LD	1
ST	DbsGet_MSN1215KBM04.Unit
LD	1
ST	DbsGet_MSN1215KBM04.Device
LD	50
ST	DbsGet_MSN1215KBM04.Type
LD	2
ST	DbsGet_MSN1215KBM04.Point
LD	11
ST	DbsGet_MSN1215KBM04.Item
CAL	DbsGet_MSN1215KBM04
LD	DbsGet_MSN1215KBM04.DbsValue
ST	ABB_025101_MSN01215KBM04 {CheckDataType := BOOL}
LD	DbsGet_MSN1215KBM04.DbsValid
ST	ABB_025101KBM04Valid {CheckDataType := BOOL}

{NetworkNumber := 3}
LD	1
ST	DbsGet_MSN1215KBM05.Unit
LD	1
ST	DbsGet_MSN1215KBM05.Device
LD	50
ST	DbsGet_MSN1215KBM05.Type
LD	2
ST	DbsGet_MSN1215KBM05.Point
LD	12
ST	DbsGet_MSN1215KBM05.Item
CAL	DbsGet_MSN1215KBM05
LD	DbsGet_MSN1215KBM05.DbsValue
ST	ABB_025101_MSN01215KBM05 {CheckDataType := BOOL}
LD	DbsGet_MSN1215KBM05.DbsValid
ST	ABB_025101KBM05Valid {CheckDataType := BOOL}

{NetworkNumber := 4}
LD	1
ST	DbsGet_MSN1215KBM06.Unit
LD	1
ST	DbsGet_MSN1215KBM06.Device
LD	50
ST	DbsGet_MSN1215KBM06.Type
LD	2
ST	DbsGet_MSN1215KBM06.Point
LD	13
ST	DbsGet_MSN1215KBM06.Item
CAL	DbsGet_MSN1215KBM06
LD	DbsGet_MSN1215KBM06.DbsValue
ST	ABB_025101_MSN01215KBM06 {CheckDataType := BOOL}
LD	DbsGet_MSN1215KBM06.DbsValid
ST	ABB_025101KBM06Valid {CheckDataType := BOOL}

END_PROGRAM
CODE_INFORMATION_754EF481_1721_4FA5_BF7F_DE1B91DFA69B_END]]></CodeInformation>
            </Data>
            <Data name="BR_LineMapInformation" handleUnknown="implementation">
              <LineMapInfos guid="083CC528_CBC9_4705_A12D_02E20D3F2FC6">
                <LineMapInfo line="1" column="1" position="[POU start]" />
                <LineMapInfo line="7" column="1" position="Net:1" />
                <LineMapInfo line="8" column="4" position="Net:1, Ln:2, Col:1" />
                <LineMapInfo line="9" column="4" position="Net:1, Ln:1, Col:4 [Block input Unit]" />
                <LineMapInfo line="10" column="4" position="Net:1, Ln:3, Col:1" />
                <LineMapInfo line="11" column="4" position="Net:1, Ln:1, Col:4 [Block input Device]" />
                <LineMapInfo line="12" column="4" position="Net:1, Ln:4, Col:1" />
                <LineMapInfo line="13" column="4" position="Net:1, Ln:1, Col:4 [Block input Type]" />
                <LineMapInfo line="14" column="4" position="Net:1, Ln:5, Col:1" />
                <LineMapInfo line="15" column="4" position="Net:1, Ln:1, Col:4 [Block input Point]" />
                <LineMapInfo line="16" column="4" position="Net:1, Ln:6, Col:1" />
                <LineMapInfo line="17" column="4" position="Net:1, Ln:1, Col:4 [Block input Item]" />
                <LineMapInfo line="18" column="5" position="Net:1, Ln:1, Col:4" />
                <LineMapInfo line="19" column="4" position="Net:1, Ln:1, Col:4 [Block output DbsValue]" />
                <LineMapInfo line="20" column="4" position="Net:1, Ln:2, Col:10" />
                <LineMapInfo line="21" column="4" position="Net:1, Ln:1, Col:4 [Block output DbsValid]" />
                <LineMapInfo line="22" column="4" position="Net:1, Ln:3, Col:10" />
                <LineMapInfo line="24" column="1" position="Net:2" />
                <LineMapInfo line="25" column="4" position="Net:2, Ln:2, Col:1" />
                <LineMapInfo line="26" column="4" position="Net:2, Ln:1, Col:4 [Block input Unit]" />
                <LineMapInfo line="27" column="4" position="Net:2, Ln:3, Col:1" />
                <LineMapInfo line="28" column="4" position="Net:2, Ln:1, Col:4 [Block input Device]" />
                <LineMapInfo line="29" column="4" position="Net:2, Ln:4, Col:1" />
                <LineMapInfo line="30" column="4" position="Net:2, Ln:1, Col:4 [Block input Type]" />
                <LineMapInfo line="31" column="4" position="Net:2, Ln:5, Col:1" />
                <LineMapInfo line="32" column="4" position="Net:2, Ln:1, Col:4 [Block input Point]" />
                <LineMapInfo line="33" column="4" position="Net:2, Ln:6, Col:1" />
                <LineMapInfo line="34" column="4" position="Net:2, Ln:1, Col:4 [Block input Item]" />
                <LineMapInfo line="35" column="5" position="Net:2, Ln:1, Col:4" />
                <LineMapInfo line="36" column="4" position="Net:2, Ln:1, Col:4 [Block output DbsValue]" />
                <LineMapInfo line="37" column="4" position="Net:2, Ln:2, Col:10" />
                <LineMapInfo line="38" column="4" position="Net:2, Ln:1, Col:4 [Block output DbsValid]" />
                <LineMapInfo line="39" column="4" position="Net:2, Ln:3, Col:10" />
                <LineMapInfo line="41" column="1" position="Net:3" />
                <LineMapInfo line="42" column="4" position="Net:3, Ln:2, Col:1" />
                <LineMapInfo line="43" column="4" position="Net:3, Ln:1, Col:4 [Block input Unit]" />
                <LineMapInfo line="44" column="4" position="Net:3, Ln:3, Col:1" />
                <LineMapInfo line="45" column="4" position="Net:3, Ln:1, Col:4 [Block input Device]" />
                <LineMapInfo line="46" column="4" position="Net:3, Ln:4, Col:1" />
                <LineMapInfo line="47" column="4" position="Net:3, Ln:1, Col:4 [Block input Type]" />
                <LineMapInfo line="48" column="4" position="Net:3, Ln:5, Col:1" />
                <LineMapInfo line="49" column="4" position="Net:3, Ln:1, Col:4 [Block input Point]" />
                <LineMapInfo line="50" column="4" position="Net:3, Ln:6, Col:1" />
                <LineMapInfo line="51" column="4" position="Net:3, Ln:1, Col:4 [Block input Item]" />
                <LineMapInfo line="52" column="5" position="Net:3, Ln:1, Col:4" />
                <LineMapInfo line="53" column="4" position="Net:3, Ln:1, Col:4 [Block output DbsValue]" />
                <LineMapInfo line="54" column="4" position="Net:3, Ln:2, Col:10" />
                <LineMapInfo line="55" column="4" position="Net:3, Ln:1, Col:4 [Block output DbsValid]" />
                <LineMapInfo line="56" column="4" position="Net:3, Ln:3, Col:10" />
                <LineMapInfo line="58" column="1" position="Net:4" />
                <LineMapInfo line="59" column="4" position="Net:4, Ln:2, Col:1" />
                <LineMapInfo line="60" column="4" position="Net:4, Ln:1, Col:4 [Block input Unit]" />
                <LineMapInfo line="61" column="4" position="Net:4, Ln:3, Col:1" />
                <LineMapInfo line="62" column="4" position="Net:4, Ln:1, Col:4 [Block input Device]" />
                <LineMapInfo line="63" column="4" position="Net:4, Ln:4, Col:1" />
                <LineMapInfo line="64" column="4" position="Net:4, Ln:1, Col:4 [Block input Type]" />
                <LineMapInfo line="65" column="4" position="Net:4, Ln:5, Col:1" />
                <LineMapInfo line="66" column="4" position="Net:4, Ln:1, Col:4 [Block input Point]" />
                <LineMapInfo line="67" column="4" position="Net:4, Ln:6, Col:1" />
                <LineMapInfo line="68" column="4" position="Net:4, Ln:1, Col:4 [Block input Item]" />
                <LineMapInfo line="69" column="5" position="Net:4, Ln:1, Col:4" />
                <LineMapInfo line="70" column="4" position="Net:4, Ln:1, Col:4 [Block output DbsValue]" />
                <LineMapInfo line="71" column="4" position="Net:4, Ln:2, Col:10" />
                <LineMapInfo line="72" column="4" position="Net:4, Ln:1, Col:4 [Block output DbsValid]" />
                <LineMapInfo line="73" column="4" position="Net:4, Ln:3, Col:10" />
                <LineMapInfo line="75" column="1" position="[POU end]" />
              </LineMapInfos>
            </Data>
            <Data name="BR_ChecksumInformation" handleUnknown="implementation">
              <ChecksumInformation guid="A3044021_E131_45AC_AB9B_59041547F16C" checksum="E049059B" />
            </Data>
          </AddData>
          <BodyContent xsi:type="LD">
            <Rung evaluationOrder="1">
              <Documentation xsi:type="SimpleText"><![CDATA[Wert aus SS2 umrangieren]]></Documentation>
              <LdObject xsi:type="LeftPowerRail">
                <RelPosition x="0" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="28" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="66" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="104" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="142" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="180" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="218" />
                </ConnectionPointOut>
              </LdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="1">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="2">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="50">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="132" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="3">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="170" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="4">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="208" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="5">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="Block" typeName="DbsGetBool" instanceName="DbsGet_MSN1215KBM03">
                <RelPosition x="93" y="38" />
                <Size x="180" y="190" />
                <InputVariables>
                  <InputVariable parameterName="Unit">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="28" />
                      <Connection refConnectionPointOutId="1" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Device">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="66" />
                      <Connection refConnectionPointOutId="2" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Type">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="104" />
                      <Connection refConnectionPointOutId="3" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Point">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="142" />
                      <Connection refConnectionPointOutId="4" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Item">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="180" />
                      <Connection refConnectionPointOutId="5" />
                    </ConnectionPointIn>
                  </InputVariable>
                </InputVariables>
                <OutputVariables>
                  <OutputVariable parameterName="DbsValue">
                    <ConnectionPointOut connectionPointOutId="6">
                      <RelPosition x="180" y="28" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="DbsValid">
                    <ConnectionPointOut connectionPointOutId="7">
                      <RelPosition x="180" y="66" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="Stat">
                    <ConnectionPointOut connectionPointOutId="0">
                      <RelPosition x="180" y="104" />
                    </ConnectionPointOut>
                  </OutputVariable>
                </OutputVariables>
              </FbdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101_MSN01215KBM03" edge="none" latch="none">
                <RelPosition x="273" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="6" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="8">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101KBM03Valid" edge="none" latch="none">
                <RelPosition x="273" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="7" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="9">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="RightPowerRail">
                <RelPosition x="363" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="28" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="66" />
                  <Connection refConnectionPointOutId="8" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="104" />
                  <Connection refConnectionPointOutId="9" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="142" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="180" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="218" />
                </ConnectionPointIn>
              </LdObject>
            </Rung>
            <Rung evaluationOrder="2">
              <Documentation xsi:type="SimpleText"><![CDATA[Wert aus SS2 umrangieren]]></Documentation>
              <LdObject xsi:type="LeftPowerRail">
                <RelPosition x="0" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="28" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="66" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="104" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="142" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="180" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="218" />
                </ConnectionPointOut>
              </LdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="1">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="2">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="50">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="132" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="3">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="2">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="170" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="4">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="11">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="208" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="5">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="Block" typeName="DbsGetBool" instanceName="DbsGet_MSN1215KBM04">
                <RelPosition x="93" y="38" />
                <Size x="180" y="190" />
                <InputVariables>
                  <InputVariable parameterName="Unit">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="28" />
                      <Connection refConnectionPointOutId="1" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Device">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="66" />
                      <Connection refConnectionPointOutId="2" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Type">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="104" />
                      <Connection refConnectionPointOutId="3" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Point">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="142" />
                      <Connection refConnectionPointOutId="4" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Item">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="180" />
                      <Connection refConnectionPointOutId="5" />
                    </ConnectionPointIn>
                  </InputVariable>
                </InputVariables>
                <OutputVariables>
                  <OutputVariable parameterName="DbsValue">
                    <ConnectionPointOut connectionPointOutId="6">
                      <RelPosition x="180" y="28" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="DbsValid">
                    <ConnectionPointOut connectionPointOutId="7">
                      <RelPosition x="180" y="66" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="Stat">
                    <ConnectionPointOut connectionPointOutId="0">
                      <RelPosition x="180" y="104" />
                    </ConnectionPointOut>
                  </OutputVariable>
                </OutputVariables>
              </FbdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101_MSN01215KBM04" edge="none" latch="none">
                <RelPosition x="273" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="6" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="8">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101KBM04Valid" edge="none" latch="none">
                <RelPosition x="273" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="7" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="9">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="RightPowerRail">
                <RelPosition x="363" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="28" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="66" />
                  <Connection refConnectionPointOutId="8" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="104" />
                  <Connection refConnectionPointOutId="9" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="142" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="180" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="218" />
                </ConnectionPointIn>
              </LdObject>
            </Rung>
            <Rung evaluationOrder="3">
              <Documentation xsi:type="SimpleText"><![CDATA[Wert aus SS2 umrangieren]]></Documentation>
              <LdObject xsi:type="LeftPowerRail">
                <RelPosition x="0" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="28" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="66" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="104" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="142" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="180" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="218" />
                </ConnectionPointOut>
              </LdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="1">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="2">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="50">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="132" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="3">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="2">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="170" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="4">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="12">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="208" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="5">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="Block" typeName="DbsGetBool" instanceName="DbsGet_MSN1215KBM05">
                <RelPosition x="93" y="38" />
                <Size x="180" y="190" />
                <InputVariables>
                  <InputVariable parameterName="Unit">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="28" />
                      <Connection refConnectionPointOutId="1" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Device">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="66" />
                      <Connection refConnectionPointOutId="2" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Type">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="104" />
                      <Connection refConnectionPointOutId="3" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Point">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="142" />
                      <Connection refConnectionPointOutId="4" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Item">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="180" />
                      <Connection refConnectionPointOutId="5" />
                    </ConnectionPointIn>
                  </InputVariable>
                </InputVariables>
                <OutputVariables>
                  <OutputVariable parameterName="DbsValue">
                    <ConnectionPointOut connectionPointOutId="6">
                      <RelPosition x="180" y="28" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="DbsValid">
                    <ConnectionPointOut connectionPointOutId="7">
                      <RelPosition x="180" y="66" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="Stat">
                    <ConnectionPointOut connectionPointOutId="0">
                      <RelPosition x="180" y="104" />
                    </ConnectionPointOut>
                  </OutputVariable>
                </OutputVariables>
              </FbdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101_MSN01215KBM05" edge="none" latch="none">
                <RelPosition x="273" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="6" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="8">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101KBM05Valid" edge="none" latch="none">
                <RelPosition x="273" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="7" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="9">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="RightPowerRail">
                <RelPosition x="363" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="28" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="66" />
                  <Connection refConnectionPointOutId="8" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="104" />
                  <Connection refConnectionPointOutId="9" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="142" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="180" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="218" />
                </ConnectionPointIn>
              </LdObject>
            </Rung>
            <Rung evaluationOrder="4">
              <Documentation xsi:type="SimpleText"><![CDATA[Wert aus SS2 umrangieren]]></Documentation>
              <LdObject xsi:type="LeftPowerRail">
                <RelPosition x="0" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="28" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="66" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="104" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="142" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="180" />
                </ConnectionPointOut>
                <ConnectionPointOut connectionPointOutId="0">
                  <RelPosition x="3" y="218" />
                </ConnectionPointOut>
              </LdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="1">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="1">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="2">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="50">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="132" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="3">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="2">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="170" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="4">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="DataSource" identifier="13">
                <AddData>
                  <Data name="BR_AnalogValue" handleUnknown="implementation">
                    <AnalogValue value="true" />
                  </Data>
                </AddData>
                <RelPosition x="3" y="208" />
                <Size x="90" y="20" />
                <ConnectionPointOut connectionPointOutId="5">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </FbdObject>
              <FbdObject xsi:type="Block" typeName="DbsGetBool" instanceName="DbsGet_MSN1215KBM06">
                <RelPosition x="93" y="38" />
                <Size x="180" y="190" />
                <InputVariables>
                  <InputVariable parameterName="Unit">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="28" />
                      <Connection refConnectionPointOutId="1" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Device">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="66" />
                      <Connection refConnectionPointOutId="2" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Type">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="104" />
                      <Connection refConnectionPointOutId="3" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Point">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="142" />
                      <Connection refConnectionPointOutId="4" />
                    </ConnectionPointIn>
                  </InputVariable>
                  <InputVariable parameterName="Item">
                    <ConnectionPointIn>
                      <RelPosition x="0" y="180" />
                      <Connection refConnectionPointOutId="5" />
                    </ConnectionPointIn>
                  </InputVariable>
                </InputVariables>
                <OutputVariables>
                  <OutputVariable parameterName="DbsValue">
                    <ConnectionPointOut connectionPointOutId="6">
                      <RelPosition x="180" y="28" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="DbsValid">
                    <ConnectionPointOut connectionPointOutId="7">
                      <RelPosition x="180" y="66" />
                    </ConnectionPointOut>
                  </OutputVariable>
                  <OutputVariable parameterName="Stat">
                    <ConnectionPointOut connectionPointOutId="0">
                      <RelPosition x="180" y="104" />
                    </ConnectionPointOut>
                  </OutputVariable>
                </OutputVariables>
              </FbdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101_MSN01215KBM06" edge="none" latch="none">
                <RelPosition x="273" y="56" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="6" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="8">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="Coil" operand="ABB_025101KBM06Valid" edge="none" latch="none">
                <RelPosition x="273" y="94" />
                <Size x="90" y="20" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="10" />
                  <Connection refConnectionPointOutId="7" />
                </ConnectionPointIn>
                <ConnectionPointOut connectionPointOutId="9">
                  <RelPosition x="90" y="10" />
                </ConnectionPointOut>
              </LdObject>
              <LdObject xsi:type="RightPowerRail">
                <RelPosition x="363" y="0" />
                <Size x="3" y="228" />
                <ConnectionPointIn>
                  <RelPosition x="0" y="28" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="66" />
                  <Connection refConnectionPointOutId="8" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="104" />
                  <Connection refConnectionPointOutId="9" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="142" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="180" />
                </ConnectionPointIn>
                <ConnectionPointIn>
                  <RelPosition x="0" y="218" />
                </ConnectionPointIn>
              </LdObject>
            </Rung>
          </BodyContent>
        </MainBody>
      </Program>
    </GlobalNamespace>
  </Types>
  <Instances />
</Project>