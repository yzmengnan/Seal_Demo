TYPE Axis_output :
STRUCT
Control_Word AT%Q*:WORD;
Target_Position AT%Q*:DINT;
Profile_Velocity AT%Q*:UDINT;
Max_Velocity AT%Q*:UDINT;
Mode_of_Operation AT%Q* :BYTE:=1;
Target_Torque AT%Q* :INT:=0;
Max_Torque AT%Q*: UINT :=0;
END_STRUCT
END_TYPE


TYPE Axis_STATUS :
STRUCT
Status : WORD;
Mode_of_Operation_disp:BYTE;
Actual_Position:DINT;
Actual_Velocity :DINT;
Following_error:DINT;
Actual_Torque:INT;
END_STRUCT
END_TYPE


	Axis0 AT%QD0:Axis_output;
	Axis1 AT%QD6:Axis_output;
	Axis2 AT%QD12:Axis_output;
	Axis0_status AT%ID0:Axis_STATUS;
	Axis1_status AT%ID5:Axis_STATUS;
	Axis2_status AT%ID10:Axis_STATUS;
	BREAK0 AT%QD100 :BOOL:=0;
	BREAK1 AT%QD101 :BOOL:=0;
	BREAK2 AT%QD102 :BOOL:=0;