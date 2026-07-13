(********************************************************************
 * COPYRIGHT --  Sascha Geiﬂer  --  Kropf Solutions
 ********************************************************************
 * Programm: zaehler
 * Datei: zaehler.st
 * Autor: Sascha Geiﬂer
 * Erstellt: 30. M‰rz 2015
 ********************************************************************
 * Berechnung Messwerte
 ********************************************************************)

PROGRAM _INIT

END_PROGRAM

PROGRAM _CYCLIC
	
	clock_impuls(IN:=NOT clock_pause.Q AND NOT clock_impuls.Q,PT:=t#0s);
	clock_pause(IN:=NOT clock_pause.Q AND NOT clock_impuls.Q,PT:=t#60s);
	clock_rtrig(CLK:=clock_impuls.Q);
	clock_trigger:=clock_rtrig.Q;
	
	IF clock_trigger THEN

		Impuls_MS1_K1_delta:=UDINT_TO_REAL(Impuls_MS1_K1-Impuls_MS1_K1_lastMinute)*0.4/2;
		MW_7010480MSA01K01AZP01:=MW_7010480MSA01K01AZP01+Impuls_MS1_K1_delta;	
		MW_7010480MSA01K01AMP01:=calcMP(Impuls_MS1_K1_delta);
		Impuls_MS1_K1_lastMinute:=Impuls_MS1_K1;
		
		Impuls_MS1_K2_delta:=UDINT_TO_REAL(Impuls_MS1_K2-Impuls_MS1_K2_lastMinute)*0.4/2;
		MW_7010480MSA01K02AZP01:=MW_7010480MSA01K02AZP01+Impuls_MS1_K2_delta;	
		MW_7010480MSA01K02AMP01:=calcMP(Impuls_MS1_K2_delta);
		Impuls_MS1_K2_lastMinute:=Impuls_MS1_K2;
		
		Impuls_MS2_K1_delta:=UDINT_TO_REAL(Impuls_MS2_K1-Impuls_MS2_K1_lastMinute)*5.0/2;
		MW_7020480MSA02K01AZP01:=MW_7020480MSA02K01AZP01+Impuls_MS2_K1_delta;	
		MW_7020480MSA02K01AMP01:=calcMP(Impuls_MS2_K1_delta);
		Impuls_MS2_K1_lastMinute:=Impuls_MS2_K1;
		
		Impuls_MS2_K2_delta:=UDINT_TO_REAL(Impuls_MS2_K2-Impuls_MS2_K2_lastMinute)*5.0/2;
		MW_7020480MSA02K02AZP01:=MW_7020480MSA02K02AZP01+Impuls_MS2_K2_delta;	
		MW_7020480MSA02K02AMP01:=calcMP(Impuls_MS2_K2_delta);
		Impuls_MS2_K2_lastMinute:=Impuls_MS2_K2;
		
		Impuls_MS3_K1_delta:=UDINT_TO_REAL(Impuls_MS3_K1-Impuls_MS3_K1_lastMinute)*5.0/2;
		MW_7030480MSA03K01AZP01:=MW_7030480MSA03K01AZP01+Impuls_MS3_K1_delta;	
		MW_7030480MSA03K01AMP01:=calcMP(Impuls_MS3_K1_delta);
		Impuls_MS3_K1_lastMinute:=Impuls_MS3_K1;
		
		Impuls_MS3_K2_delta:=UDINT_TO_REAL(Impuls_MS3_K2-Impuls_MS3_K2_lastMinute)*5.0/2;
		MW_7030480MSA03K02AZP01:=MW_7030480MSA03K02AZP01+Impuls_MS3_K2_delta;	
		MW_7030480MSA03K02AMP01:=calcMP(Impuls_MS3_K2_delta);
		Impuls_MS3_K2_lastMinute:=Impuls_MS3_K2;
		
		Impuls_MS4_K1_delta:=UDINT_TO_REAL(Impuls_MS4_K1-Impuls_MS4_K1_lastMinute)*0.6/2;
		MW_7040480MSA04K01AZP01:=MW_7040480MSA04K01AZP01+Impuls_MS4_K1_delta;	
		MW_7040480MSA04K01AMP01:=calcMP(Impuls_MS4_K1_delta);
		Impuls_MS4_K1_lastMinute:=Impuls_MS4_K1;
		
		Impuls_MS4_K2_delta:=UDINT_TO_REAL(Impuls_MS4_K2-Impuls_MS4_K2_lastMinute)*0.6/2;
		MW_7040480MSA04K02AZP01:=MW_7040480MSA04K02AZP01+Impuls_MS4_K2_delta;	
		MW_7040480MSA04K02AMP01:=calcMP(Impuls_MS4_K2_delta);
		Impuls_MS4_K2_lastMinute:=Impuls_MS4_K2;
		
		Impuls_MS5_K1_delta:=UDINT_TO_REAL(Impuls_MS5_K1-Impuls_MS5_K1_lastMinute)*0.6/2;
		MW_7050480MSA05K01AZP01:=MW_7050480MSA05K01AZP01+Impuls_MS5_K1_delta;	
		MW_7050480MSA05K01AMP01:=calcMP(Impuls_MS5_K1_delta);
		Impuls_MS5_K1_lastMinute:=Impuls_MS5_K1;
		
		Impuls_MS5_K2_delta:=UDINT_TO_REAL(Impuls_MS5_K2-Impuls_MS5_K2_lastMinute)*0.6/2;
		MW_7050480MSA05K02AZP01:=MW_7050480MSA05K02AZP01+Impuls_MS5_K2_delta;	
		MW_7050480MSA05K02AMP01:=calcMP(Impuls_MS5_K2_delta);
		Impuls_MS5_K2_lastMinute:=Impuls_MS5_K2;
		
		Impuls_MS6_K1_delta:=UDINT_TO_REAL(Impuls_MS6_K1-Impuls_MS6_K1_lastMinute)*0.4/2;
		MW_7060480MSA06K01AZP01:=MW_7060480MSA06K01AZP01+Impuls_MS6_K1_delta;	
		MW_7060480MSA06K01AMP01:=calcMP(Impuls_MS6_K1_delta);
		Impuls_MS6_K1_lastMinute:=Impuls_MS6_K1;
		
		Impuls_MS6_K2_delta:=UDINT_TO_REAL(Impuls_MS6_K2-Impuls_MS6_K2_lastMinute)*0.4/2;
		MW_7060480MSA06K02AZP01:=MW_7060480MSA06K02AZP01+Impuls_MS6_K2_delta;	
		MW_7060480MSA06K02AMP01:=calcMP(Impuls_MS6_K2_delta);
		Impuls_MS6_K2_lastMinute:=Impuls_MS6_K2;
		
		Impuls_MS7_K1_delta:=UDINT_TO_REAL(Impuls_MS7_K1-Impuls_MS7_K1_lastMinute)*0.4/2;
		MW_7070480MSA07K01AZP01:=MW_7070480MSA07K01AZP01+Impuls_MS7_K1_delta;	
		MW_7070480MSA07K01AMP01:=calcMP(Impuls_MS7_K1_delta);
		Impuls_MS7_K1_lastMinute:=Impuls_MS7_K1;
		
		Impuls_MS7_K2_delta:=UDINT_TO_REAL(Impuls_MS7_K2-Impuls_MS7_K2_lastMinute)*0.4/2;
		MW_7070480MSA07K02AZP01:=MW_7070480MSA07K02AZP01+Impuls_MS7_K2_delta;	
		MW_7070480MSA07K02AMP01:=calcMP(Impuls_MS7_K2_delta);
		Impuls_MS7_K2_lastMinute:=Impuls_MS7_K2;
		
		Impuls_MS8_K1_delta:=UDINT_TO_REAL(Impuls_MS8_K1-Impuls_MS8_K1_lastMinute)*0.6/2;
		MW_7080480MSA08K01AZP01:=MW_7080480MSA08K01AZP01+Impuls_MS8_K1_delta;	
		MW_7080480MSA08K01AMP01:=calcMP(Impuls_MS8_K1_delta);
		Impuls_MS8_K1_lastMinute:=Impuls_MS8_K1;
		
		Impuls_MS8_K2_delta:=UDINT_TO_REAL(Impuls_MS8_K2-Impuls_MS8_K2_lastMinute)*0.6/2;
		MW_7080480MSA08K02AZP01:=MW_7080480MSA08K02AZP01+Impuls_MS8_K2_delta;	
		MW_7080480MSA08K02AMP01:=calcMP(Impuls_MS8_K2_delta);
		Impuls_MS8_K2_lastMinute:=Impuls_MS8_K2;
		
		Impuls_MS10_K1_delta:=UDINT_TO_REAL(Impuls_MS10_K1-Impuls_MS10_K1_lastMinute)*0.6/2;
		MW_7100480MSA10K01AZP01:=MW_7100480MSA10K01AZP01+Impuls_MS10_K1_delta;	
		MW_7100480MSA10K01AMP01:=calcMP(Impuls_MS10_K1_delta);
		Impuls_MS10_K1_lastMinute:=Impuls_MS10_K1;
		
		Impuls_MS10_K2_delta:=UDINT_TO_REAL(Impuls_MS10_K2-Impuls_MS10_K2_lastMinute)*0.6/2;
		MW_7100480MSA10K02AZP01:=MW_7100480MSA10K02AZP01+Impuls_MS10_K2_delta;	
		MW_7100480MSA10K02AMP01:=calcMP(Impuls_MS10_K2_delta);
		Impuls_MS10_K2_lastMinute:=Impuls_MS10_K2;

		Impuls_MS12_K1_delta:=UDINT_TO_REAL(Impuls_MS12_K1-Impuls_MS12_K1_lastMinute)*0.4/2;
		MW_7120480MSA12K01AZP01:=MW_7120480MSA12K01AZP01+Impuls_MS12_K1_delta;	
		MW_7120480MSA12K01AMP01:=calcMP(Impuls_MS12_K1_delta);
		Impuls_MS12_K1_lastMinute:=Impuls_MS12_K1;
		
		Impuls_MS12_K2_delta:=UDINT_TO_REAL(Impuls_MS12_K2-Impuls_MS12_K2_lastMinute)*0.4/2;
		MW_7120480MSA12K02AZP01:=MW_7120480MSA12K02AZP01+Impuls_MS12_K2_delta;	
		MW_7120480MSA12K02AMP01:=calcMP(Impuls_MS12_K2_delta);
		Impuls_MS12_K2_lastMinute:=Impuls_MS12_K2;
		
		Impuls_MS14_K1_delta:=UDINT_TO_REAL(Impuls_MS14_K1-Impuls_MS14_K1_lastMinute)*0.6/2;
		MW_7140480MSA14K01AZP01:=MW_7140480MSA14K01AZP01+Impuls_MS14_K1_delta;	
		MW_7140480MSA14K01AMP01:=calcMP(Impuls_MS14_K1_delta);
		Impuls_MS14_K1_lastMinute:=Impuls_MS14_K1;
		
		Impuls_MS14_K2_delta:=UDINT_TO_REAL(Impuls_MS14_K2-Impuls_MS14_K2_lastMinute)*0.6/2;
		MW_7140480MSA14K02AZP01:=MW_7140480MSA14K02AZP01+Impuls_MS14_K2_delta;	
		MW_7140480MSA14K02AMP01:=calcMP(Impuls_MS14_K2_delta);
		Impuls_MS14_K2_lastMinute:=Impuls_MS14_K2;
		
		Impuls_MS16_K1_delta:=UDINT_TO_REAL(Impuls_MS16_K1-Impuls_MS16_K1_lastMinute)*0.4/2;
		MW_7160480MSA16K01AZP01:=MW_7160480MSA16K01AZP01+Impuls_MS16_K1_delta;	
		MW_7160480MSA16K01AMP01:=calcMP(Impuls_MS16_K1_delta);
		Impuls_MS16_K1_lastMinute:=Impuls_MS16_K1;
		
		Impuls_MS16_K2_delta:=UDINT_TO_REAL(Impuls_MS16_K2-Impuls_MS16_K2_lastMinute)*0.4/2;
		MW_7160480MSA16K02AZP01:=MW_7160480MSA16K02AZP01+Impuls_MS16_K2_delta;	
		MW_7160480MSA16K02AMP01:=calcMP(Impuls_MS16_K2_delta);
		Impuls_MS16_K2_lastMinute:=Impuls_MS16_K2;
		
		Impuls_MS18_K1_delta:=UDINT_TO_REAL(Impuls_MS18_K1-Impuls_MS18_K1_lastMinute)*0.4/2;
		MW_7180480MSA18K01AZP01:=MW_7180480MSA18K01AZP01+Impuls_MS18_K1_delta;	
		MW_7180480MSA18K01AMP01:=calcMP(Impuls_MS18_K1_delta);
		Impuls_MS18_K1_lastMinute:=Impuls_MS18_K1;
		
		Impuls_MS18_K2_delta:=UDINT_TO_REAL(Impuls_MS18_K2-Impuls_MS18_K2_lastMinute)*0.4/2;
		MW_7180480MSA18K02AZP01:=MW_7180480MSA18K02AZP01+Impuls_MS18_K2_delta;	
		MW_7180480MSA18K02AMP01:=calcMP(Impuls_MS18_K2_delta);
		Impuls_MS18_K2_lastMinute:=Impuls_MS18_K2;
		
		Impuls_MS19_K1_delta:=UDINT_TO_REAL(Impuls_MS19_K1-Impuls_MS19_K1_lastMinute)*0.3/2;
		MW_7190480MSA19K01AZP01:=MW_7190480MSA19K01AZP01+Impuls_MS19_K1_delta;	
		MW_7190480MSA19K01AMP01:=calcMP(Impuls_MS19_K1_delta);
		Impuls_MS19_K1_lastMinute:=Impuls_MS19_K1;
		
		Impuls_MS19_K2_delta:=UDINT_TO_REAL(Impuls_MS19_K2-Impuls_MS19_K2_lastMinute)*0.3/2;
		MW_7190480MSA19K02AZP01:=MW_7190480MSA19K02AZP01+Impuls_MS19_K2_delta;	
		MW_7190480MSA19K02AMP01:=calcMP(Impuls_MS19_K2_delta);
		Impuls_MS19_K2_lastMinute:=Impuls_MS19_K2;
			
		Impuls_Not_Sued_delta:=UDINT_TO_REAL(Impuls_Not_Sued-Impuls_Not_Sued_lastMinute)*0.3/2;
		MW_7500480MSA02K01AZP01:=MW_7500480MSA02K01AZP01+Impuls_Not_Sued_delta;	
		MW_7500480MSA02K01AMP01:=calcMP(Impuls_Not_Sued_delta);
		Impuls_Not_Sued_lastMinute:=Impuls_Not_Sued;
		
		Impuls_Not_Nord_delta:=UDINT_TO_REAL(Impuls_Not_Nord-Impuls_Not_Nord_lastMinute)*0.3/2;
		MW_7600480MSA01K01AZP01:=MW_7600480MSA01K01AZP01+Impuls_Not_Nord_delta;	
		MW_7600480MSA01K01AMP01:=calcMP(Impuls_Not_Nord_delta);
		Impuls_Not_Nord_lastMinute:=Impuls_Not_Nord;
		
		Impuls_Not_Station_delta:=UDINT_TO_REAL(Impuls_Not_Station-Impuls_Not_Station_lastMinute)*0.3/2;
		MW_7xx0480MSAxxNOTAZP01:=MW_7xx0480MSAxxNOTAZP01+Impuls_Not_Station_delta;	
		MW_7xx0480MSAxxNOTAMP01:=calcMP(Impuls_Not_Station_delta);
		Impuls_Not_Station_lastMinute:=Impuls_Not_Station;
		
		Impuls_Trafo_1_delta:=UDINT_TO_REAL(Impuls_Trafo_1-Impuls_Trafo_1_lastMinute)*3.3/2;
		MW_7xx0480MSAxxT01AZP01:=MW_7xx0480MSAxxT01AZP01+Impuls_Trafo_1_delta;	
		MW_7xx0480MSAxxT01AMP01:=calcMP(Impuls_Trafo_1_delta);
		Impuls_Trafo_1_lastMinute:=Impuls_Trafo_1;
		
		Impuls_Trafo_2_delta:=UDINT_TO_REAL(Impuls_Trafo_2-Impuls_Trafo_2_lastMinute)*3.3/2;
		MW_7xx0480MSAxxT02AZP01:=MW_7xx0480MSAxxT02AZP01+Impuls_Trafo_2_delta;	
		MW_7xx0480MSAxxT02AMP01:=calcMP(Impuls_Trafo_2_delta);
		Impuls_Trafo_2_lastMinute:=Impuls_Trafo_2;
		
		Impuls_Trafo_3_delta:=UDINT_TO_REAL(Impuls_Trafo_3-Impuls_Trafo_3_lastMinute)*3.3/2;
		MW_7xx0480MSAxxT03AZP01:=MW_7xx0480MSAxxT03AZP01+Impuls_Trafo_3_delta;	
		MW_7xx0480MSAxxT03AMP01:=calcMP(Impuls_Trafo_3_delta);
		Impuls_Trafo_3_lastMinute:=Impuls_Trafo_3;
		
		Impuls_Trafo_4_delta:=UDINT_TO_REAL(Impuls_Trafo_4-Impuls_Trafo_4_lastMinute)*3.3/2;
		MW_7xx0480MSAxxT04AZP01:=MW_7xx0480MSAxxT04AZP01+Impuls_Trafo_4_delta;	
		MW_7xx0480MSAxxT04AMP01:=calcMP(Impuls_Trafo_4_delta);
		Impuls_Trafo_4_lastMinute:=Impuls_Trafo_4;
		
		Impuls_EB_Trafo_delta:=UDINT_TO_REAL(Impuls_EB_Trafo-Impuls_EB_Trafo_lastMinute)*0.1/2;
		MW_7xx0480MSAxxTEBAZP01:=MW_7xx0480MSAxxTEBAZP01+Impuls_EB_Trafo_delta;	
		MW_7xx0480MSAxxTEBAMP01:=calcMP(Impuls_EB_Trafo_delta);
		Impuls_EB_Trafo_lastMinute:=Impuls_EB_Trafo;
		
		Impuls_Kabel_1_delta:=UDINT_TO_REAL(Impuls_Kabel_1-Impuls_Kabel_1_lastMinute)*10/2;
		MW_7xx0480MSAxxK01AZP01:=MW_7xx0480MSAxxK01AZP01+Impuls_Kabel_1_delta;	
		MW_7xx0480MSAxxK01AMP01:=calcMP(Impuls_Kabel_1_delta);
		Impuls_Kabel_1_lastMinute:=Impuls_Kabel_1;
		
		Impuls_Kabel_2_delta:=UDINT_TO_REAL(Impuls_Kabel_2-Impuls_Kabel_2_lastMinute)*10/2;
		MW_7xx0480MSAxxK02AZP01:=MW_7xx0480MSAxxK02AZP01+Impuls_Kabel_2_delta;	
		MW_7xx0480MSAxxK02AMP01:=calcMP(Impuls_Kabel_2_delta);
		Impuls_Kabel_2_lastMinute:=Impuls_Kabel_2;
		
	END_IF;

END_PROGRAM
