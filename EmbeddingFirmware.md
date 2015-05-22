# Introduction #

There are plenty of definitions to control firmware features. This article is valid from 3bfb5aeb3f revision. From this revision you could disable DFU (global variable **NODFU=1**) or set to only downloading mode (preventing reading current firmware over USB, definition **NO\_DFUUPLOAD**).


# Details #

There some firmware control definitions and resulting firmware size


|_definition_|_feature_|_feature size (approx)_|SDK=0|SDK=0, -DNO\_DFUUPLOAD|NODFU=1|
|:-----------|:--------|:----------------------|:----|:---------------------|:------|
|DEBUG\_REGS |Helps to get internal variables|246                    |12296|11874                 |9952   |
|PRESENT\_GPS|Enables basic GPS syncronization|1708                   |12050|11628                 |9710   |
|            |Default features enabled|                       |10342|9920                  |8006   |
|UNROLL\_MACROSS\_LMX2531|Optimization in manipulating LMX registers|-186                   |10156|9734                  |7822   |
|VCO\_FIXED  |Store VCO values in firmware rather than in EEPROM (the only way to update is to re-flash)|-430                   |9726 |9304                  |7400   |
|NO\_CMDINFO |Disables INF command|-516                   |9210 |8788                  |6890   |
|NO\_CMDPIN  |Disables PIN command|-94                    |9116 |8694                  |6796   |
|NO\_CMDREG  |Disables REG command|-58                    |9058 |8636                  |6738   |
|NO\_HWINFO  |Disables HWI command|-56                    |9002 |8580                  |6682   |
|NO\_VERSION |Disables VER command|-36                    |8966 |8544                  |6646   |
|NO\_HEXVALUES|All commands accept only decimal numbers|-124                   |8842 |8420                  |6522   |