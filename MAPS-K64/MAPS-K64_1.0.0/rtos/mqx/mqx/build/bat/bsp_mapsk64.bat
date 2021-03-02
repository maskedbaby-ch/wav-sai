@echo off

rem convert path to backslash format
set ROOTDIR=%1
set ROOTDIR=%ROOTDIR:/=\%
set OUTPUTDIR=%2
set OUTPUTDIR=%OUTPUTDIR:/=\%
set TOOL=%3

rem process one of label bellow
goto label_%TOOL%

:label_kds
IF NOT EXIST "%OUTPUTDIR%" mkdir "%OUTPUTDIR%"
IF NOT EXIST "%OUTPUTDIR%\." mkdir "%OUTPUTDIR%\."
IF NOT EXIST "%OUTPUTDIR%\../config" mkdir "%OUTPUTDIR%\../config"
IF NOT EXIST "%OUTPUTDIR%\config" mkdir "%OUTPUTDIR%\config"
copy "%ROOTDIR%\mqx\source\bsp\K64F12\gcc\K64FN1Mxxx12_flash.ld" "%OUTPUTDIR%\K64FN1Mxxx12_flash.ld" /Y
copy "%ROOTDIR%\config\mcu\mk64f120m\mqx_sdk_config.h" "%OUTPUTDIR%\config\mqx_sdk_config.h" /Y
copy "%ROOTDIR%\config\board\mapsk64\user_config.h" "%OUTPUTDIR%\config\user_config.h" /Y
copy "%ROOTDIR%\config\common\small_ram_config.h" "%OUTPUTDIR%\..\config\small_ram_config.h" /Y
copy "%ROOTDIR%\config\common\smallest_config.h" "%OUTPUTDIR%\..\config\smallest_config.h" /Y
copy "%ROOTDIR%\config\common\maximum_config.h" "%OUTPUTDIR%\..\config\maximum_config.h" /Y
copy "%ROOTDIR%\config\common\verif_enabled_config.h" "%OUTPUTDIR%\..\config\verif_enabled_config.h" /Y
copy "%ROOTDIR%\mqx\source\bsp\K64F12\bsp_config.h" "%OUTPUTDIR%\bsp_config.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx.h" "%OUTPUTDIR%\mqx.h" /Y
copy "%ROOTDIR%\mqx\source\bsp\bsp.h" "%OUTPUTDIR%\bsp.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_dummy\nio_dummy.h" "%OUTPUTDIR%\nio_dummy.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_mem\nio_mem.h" "%OUTPUTDIR%\nio_mem.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_null\nio_null.h" "%OUTPUTDIR%\nio_null.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_pipe\nio_pipe.h" "%OUTPUTDIR%\nio_pipe.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_serial\nio_serial.h" "%OUTPUTDIR%\nio_serial.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_tfs\nio_tfs.h" "%OUTPUTDIR%\nio_tfs.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_tty\nio_tty.h" "%OUTPUTDIR%\nio_tty.h" /Y
copy "%ROOTDIR%\platform\startup\MK64F12\system_MK64F12.h" "%OUTPUTDIR%\system_MK64F12.h" /Y
goto end_script


:label_iar
IF NOT EXIST "%OUTPUTDIR%" mkdir "%OUTPUTDIR%"
IF NOT EXIST "%OUTPUTDIR%\." mkdir "%OUTPUTDIR%\."
IF NOT EXIST "%OUTPUTDIR%\../config" mkdir "%OUTPUTDIR%\../config"
IF NOT EXIST "%OUTPUTDIR%\config" mkdir "%OUTPUTDIR%\config"
copy "%ROOTDIR%\mqx\source\bsp\K64F12\iar\K64FN1Mxxx12_flash.icf" "%OUTPUTDIR%\K64FN1Mxxx12_flash.icf" /Y
copy "%ROOTDIR%\config\mcu\mk64f120m\mqx_sdk_config.h" "%OUTPUTDIR%\config\mqx_sdk_config.h" /Y
copy "%ROOTDIR%\config\board\mapsk64\user_config.h" "%OUTPUTDIR%\config\user_config.h" /Y
copy "%ROOTDIR%\config\common\small_ram_config.h" "%OUTPUTDIR%\..\config\small_ram_config.h" /Y
copy "%ROOTDIR%\config\common\smallest_config.h" "%OUTPUTDIR%\..\config\smallest_config.h" /Y
copy "%ROOTDIR%\config\common\maximum_config.h" "%OUTPUTDIR%\..\config\maximum_config.h" /Y
copy "%ROOTDIR%\config\common\verif_enabled_config.h" "%OUTPUTDIR%\..\config\verif_enabled_config.h" /Y
copy "%ROOTDIR%\mqx\source\bsp\K64F12\bsp_config.h" "%OUTPUTDIR%\bsp_config.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx.h" "%OUTPUTDIR%\mqx.h" /Y
copy "%ROOTDIR%\mqx\source\bsp\bsp.h" "%OUTPUTDIR%\bsp.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_dummy\nio_dummy.h" "%OUTPUTDIR%\nio_dummy.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_mem\nio_mem.h" "%OUTPUTDIR%\nio_mem.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_null\nio_null.h" "%OUTPUTDIR%\nio_null.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_pipe\nio_pipe.h" "%OUTPUTDIR%\nio_pipe.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_serial\nio_serial.h" "%OUTPUTDIR%\nio_serial.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_tfs\nio_tfs.h" "%OUTPUTDIR%\nio_tfs.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_tty\nio_tty.h" "%OUTPUTDIR%\nio_tty.h" /Y
copy "%ROOTDIR%\platform\startup\MK64F12\system_MK64F12.h" "%OUTPUTDIR%\system_MK64F12.h" /Y
goto end_script


:label_uv4
IF NOT EXIST "%OUTPUTDIR%" mkdir "%OUTPUTDIR%"
IF NOT EXIST "%OUTPUTDIR%\." mkdir "%OUTPUTDIR%\."
IF NOT EXIST "%OUTPUTDIR%\../config" mkdir "%OUTPUTDIR%\../config"
IF NOT EXIST "%OUTPUTDIR%\config" mkdir "%OUTPUTDIR%\config"
copy "%ROOTDIR%\mqx\source\bsp\K64F12\arm\K64FN1Mxxx12_flash.scf" "%OUTPUTDIR%\K64FN1Mxxx12_flash.scf" /Y
copy "%ROOTDIR%\config\mcu\mk64f120m\mqx_sdk_config.h" "%OUTPUTDIR%\config\mqx_sdk_config.h" /Y
copy "%ROOTDIR%\config\board\mapsk64\user_config.h" "%OUTPUTDIR%\config\user_config.h" /Y
copy "%ROOTDIR%\config\common\small_ram_config.h" "%OUTPUTDIR%\..\config\small_ram_config.h" /Y
copy "%ROOTDIR%\config\common\smallest_config.h" "%OUTPUTDIR%\..\config\smallest_config.h" /Y
copy "%ROOTDIR%\config\common\maximum_config.h" "%OUTPUTDIR%\..\config\maximum_config.h" /Y
copy "%ROOTDIR%\config\common\verif_enabled_config.h" "%OUTPUTDIR%\..\config\verif_enabled_config.h" /Y
copy "%ROOTDIR%\mqx\source\bsp\K64F12\bsp_config.h" "%OUTPUTDIR%\bsp_config.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx.h" "%OUTPUTDIR%\mqx.h" /Y
copy "%ROOTDIR%\mqx\source\bsp\bsp.h" "%OUTPUTDIR%\bsp.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_dummy\nio_dummy.h" "%OUTPUTDIR%\nio_dummy.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_mem\nio_mem.h" "%OUTPUTDIR%\nio_mem.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_null\nio_null.h" "%OUTPUTDIR%\nio_null.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_pipe\nio_pipe.h" "%OUTPUTDIR%\nio_pipe.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_serial\nio_serial.h" "%OUTPUTDIR%\nio_serial.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_tfs\nio_tfs.h" "%OUTPUTDIR%\nio_tfs.h" /Y
copy "%ROOTDIR%\mqx\source\nio\drivers\nio_tty\nio_tty.h" "%OUTPUTDIR%\nio_tty.h" /Y
copy "%ROOTDIR%\platform\startup\MK64F12\system_MK64F12.h" "%OUTPUTDIR%\system_MK64F12.h" /Y
goto end_script



:end_script

