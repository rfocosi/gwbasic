10 REM GW-BASIC Comprehensive Test Suite
20 CLS
30 COLOR 7, 0
40 LOCATE 1, 1
50 PRINT "=== GW-BASIC Comprehensive Test Suite ==="
60 LET A = 100
70 LET B = 25
80 SUM = A + B
90 SUB = A - B
100 MUL = A * B
110 DIV = A / B
120 MODVAL = A MOD 7
130 LET TITLE$ = "Math Results: "
140 PRINT TITLE$; "SUM="; SUM; " SUB="; SUB; " MUL="; MUL; " DIV="; DIV; " MOD="; MODVAL
150 REM Verify comment processing
160 PRINT "All Basic commands executed successfully."
170 END
180 PRINT "ERROR: Line after END executed!"
