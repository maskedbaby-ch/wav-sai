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
copy "%ROOTDIR%\mqx\source\psp\cortex_m\compiler\gcc_arm\asm_mac.h" "%OUTPUTDIR%\asm_mac.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\compiler\gcc_arm\comp.h" "%OUTPUTDIR%\comp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp.h" "%OUTPUTDIR%\psp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_abi.h" "%OUTPUTDIR%\psp_abi.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_comp.h" "%OUTPUTDIR%\psp_comp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_math.h" "%OUTPUTDIR%\psp_math.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_rev.h" "%OUTPUTDIR%\psp_rev.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_supp.h" "%OUTPUTDIR%\psp_supp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_time.h" "%OUTPUTDIR%\psp_time.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psptypes.h" "%OUTPUTDIR%\psptypes.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_cpu.h" "%OUTPUTDIR%\psp_cpu.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_cpudef.h" "%OUTPUTDIR%\psp_cpudef.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\cortex.h" "%OUTPUTDIR%\cortex.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\kinetis_mpu.h" "%OUTPUTDIR%\kinetis_mpu.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\nvic.h" "%OUTPUTDIR%\nvic.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\kinetis.h" "%OUTPUTDIR%\kinetis.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\cpu\kinetis_fcan.h" "%OUTPUTDIR%\kinetis_fcan.h" /Y
copy "%ROOTDIR%\config\mcu\mk64f120m\mqx_sdk_config.h" "%OUTPUTDIR%\..\config\mqx_sdk_config.h" /Y
copy "%ROOTDIR%\config\board\mapsk64\user_config.h" "%OUTPUTDIR%\..\config\user_config.h" /Y
copy "%ROOTDIR%\config\common\small_ram_config.h" "%OUTPUTDIR%\small_ram_config.h" /Y
copy "%ROOTDIR%\mqx\source\include\gen_rev.h" "%OUTPUTDIR%\gen_rev.h" /Y
copy "%ROOTDIR%\mqx\source\include\eds.h" "%OUTPUTDIR%\eds.h" /Y
copy "%ROOTDIR%\mqx\source\include\edserial.h" "%OUTPUTDIR%\edserial.h" /Y
copy "%ROOTDIR%\mqx\source\include\event.h" "%OUTPUTDIR%\event.h" /Y
copy "%ROOTDIR%\mqx\source\include\charq.h" "%OUTPUTDIR%\charq.h" /Y
copy "%ROOTDIR%\mqx\source\include\klog.h" "%OUTPUTDIR%\klog.h" /Y
copy "%ROOTDIR%\mqx\source\include\log.h" "%OUTPUTDIR%\log.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwsem.h" "%OUTPUTDIR%\lwsem.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwevent.h" "%OUTPUTDIR%\lwevent.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwlog.h" "%OUTPUTDIR%\lwlog.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwmem.h" "%OUTPUTDIR%\lwmem.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwmsgq.h" "%OUTPUTDIR%\lwmsgq.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwtimer.h" "%OUTPUTDIR%\lwtimer.h" /Y
copy "%ROOTDIR%\mqx\source\include\message.h" "%OUTPUTDIR%\message.h" /Y
copy "%ROOTDIR%\mqx\source\include\mmu.h" "%OUTPUTDIR%\mmu.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx.h" "%OUTPUTDIR%\mqx.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_cnfg.h" "%OUTPUTDIR%\mqx_cnfg.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_cpudef.h" "%OUTPUTDIR%\mqx_cpudef.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_inc.h" "%OUTPUTDIR%\mqx_inc.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_ioc.h" "%OUTPUTDIR%\mqx_ioc.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_str.h" "%OUTPUTDIR%\mqx_str.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_macros.h" "%OUTPUTDIR%\mqx_macros.h" /Y
copy "%ROOTDIR%\mqx\source\include\mutex.h" "%OUTPUTDIR%\mutex.h" /Y
copy "%ROOTDIR%\mqx\source\include\name.h" "%OUTPUTDIR%\name.h" /Y
copy "%ROOTDIR%\mqx\source\include\partition.h" "%OUTPUTDIR%\partition.h" /Y
copy "%ROOTDIR%\mqx\source\include\pcb.h" "%OUTPUTDIR%\pcb.h" /Y
copy "%ROOTDIR%\mqx\source\include\psptypes_legacy.h" "%OUTPUTDIR%\psptypes_legacy.h" /Y
copy "%ROOTDIR%\mqx\source\include\posix.h" "%OUTPUTDIR%\posix.h" /Y
copy "%ROOTDIR%\mqx\source\include\queue.h" "%OUTPUTDIR%\queue.h" /Y
copy "%ROOTDIR%\mqx\source\include\sem.h" "%OUTPUTDIR%\sem.h" /Y
copy "%ROOTDIR%\mqx\source\include\timer.h" "%OUTPUTDIR%\timer.h" /Y
copy "%ROOTDIR%\mqx\source\include\watchdog.h" "%OUTPUTDIR%\watchdog.h" /Y
copy "%ROOTDIR%\mqx\source\include\unicode.h" "%OUTPUTDIR%\unicode.h" /Y
copy "%ROOTDIR%\mqx\source\nio\nio.h" "%OUTPUTDIR%\nio.h" /Y
copy "%ROOTDIR%\mqx\source\nio\errno.h" "%OUTPUTDIR%\errno.h" /Y
copy "%ROOTDIR%\mqx\source\nio\ioctl.h" "%OUTPUTDIR%\ioctl.h" /Y
copy "%ROOTDIR%\mqx\source\nio\fcntl.h" "%OUTPUTDIR%\fcntl.h" /Y
copy "%ROOTDIR%\mqx\source\nio\fs\fs_supp.h" "%OUTPUTDIR%\fs_supp.h" /Y
copy "%ROOTDIR%\mqx\source\tad\tad.h" "%OUTPUTDIR%\tad.h" /Y
goto end_script


:label_iar
IF NOT EXIST "%OUTPUTDIR%" mkdir "%OUTPUTDIR%"
IF NOT EXIST "%OUTPUTDIR%\." mkdir "%OUTPUTDIR%\."
IF NOT EXIST "%OUTPUTDIR%\../config" mkdir "%OUTPUTDIR%\../config"
copy "%ROOTDIR%\mqx\source\psp\cortex_m\compiler\iar\asm_mac.h" "%OUTPUTDIR%\asm_mac.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\compiler\iar\comp.h" "%OUTPUTDIR%\comp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp.h" "%OUTPUTDIR%\psp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_abi.h" "%OUTPUTDIR%\psp_abi.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_comp.h" "%OUTPUTDIR%\psp_comp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_math.h" "%OUTPUTDIR%\psp_math.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_rev.h" "%OUTPUTDIR%\psp_rev.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_supp.h" "%OUTPUTDIR%\psp_supp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_time.h" "%OUTPUTDIR%\psp_time.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psptypes.h" "%OUTPUTDIR%\psptypes.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_cpu.h" "%OUTPUTDIR%\psp_cpu.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_cpudef.h" "%OUTPUTDIR%\psp_cpudef.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\cortex.h" "%OUTPUTDIR%\cortex.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\kinetis_mpu.h" "%OUTPUTDIR%\kinetis_mpu.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\nvic.h" "%OUTPUTDIR%\nvic.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\kinetis.h" "%OUTPUTDIR%\kinetis.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\cpu\kinetis_fcan.h" "%OUTPUTDIR%\kinetis_fcan.h" /Y
copy "%ROOTDIR%\config\mcu\mk64f120m\mqx_sdk_config.h" "%OUTPUTDIR%\..\config\mqx_sdk_config.h" /Y
copy "%ROOTDIR%\config\board\mapsk64\user_config.h" "%OUTPUTDIR%\..\config\user_config.h" /Y
copy "%ROOTDIR%\config\common\small_ram_config.h" "%OUTPUTDIR%\small_ram_config.h" /Y
copy "%ROOTDIR%\mqx\source\include\gen_rev.h" "%OUTPUTDIR%\gen_rev.h" /Y
copy "%ROOTDIR%\mqx\source\include\eds.h" "%OUTPUTDIR%\eds.h" /Y
copy "%ROOTDIR%\mqx\source\include\edserial.h" "%OUTPUTDIR%\edserial.h" /Y
copy "%ROOTDIR%\mqx\source\include\event.h" "%OUTPUTDIR%\event.h" /Y
copy "%ROOTDIR%\mqx\source\include\charq.h" "%OUTPUTDIR%\charq.h" /Y
copy "%ROOTDIR%\mqx\source\include\klog.h" "%OUTPUTDIR%\klog.h" /Y
copy "%ROOTDIR%\mqx\source\include\log.h" "%OUTPUTDIR%\log.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwsem.h" "%OUTPUTDIR%\lwsem.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwevent.h" "%OUTPUTDIR%\lwevent.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwlog.h" "%OUTPUTDIR%\lwlog.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwmem.h" "%OUTPUTDIR%\lwmem.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwmsgq.h" "%OUTPUTDIR%\lwmsgq.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwtimer.h" "%OUTPUTDIR%\lwtimer.h" /Y
copy "%ROOTDIR%\mqx\source\include\message.h" "%OUTPUTDIR%\message.h" /Y
copy "%ROOTDIR%\mqx\source\include\mmu.h" "%OUTPUTDIR%\mmu.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx.h" "%OUTPUTDIR%\mqx.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_cnfg.h" "%OUTPUTDIR%\mqx_cnfg.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_cpudef.h" "%OUTPUTDIR%\mqx_cpudef.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_inc.h" "%OUTPUTDIR%\mqx_inc.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_ioc.h" "%OUTPUTDIR%\mqx_ioc.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_str.h" "%OUTPUTDIR%\mqx_str.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_macros.h" "%OUTPUTDIR%\mqx_macros.h" /Y
copy "%ROOTDIR%\mqx\source\include\mutex.h" "%OUTPUTDIR%\mutex.h" /Y
copy "%ROOTDIR%\mqx\source\include\name.h" "%OUTPUTDIR%\name.h" /Y
copy "%ROOTDIR%\mqx\source\include\partition.h" "%OUTPUTDIR%\partition.h" /Y
copy "%ROOTDIR%\mqx\source\include\pcb.h" "%OUTPUTDIR%\pcb.h" /Y
copy "%ROOTDIR%\mqx\source\include\psptypes_legacy.h" "%OUTPUTDIR%\psptypes_legacy.h" /Y
copy "%ROOTDIR%\mqx\source\include\posix.h" "%OUTPUTDIR%\posix.h" /Y
copy "%ROOTDIR%\mqx\source\include\queue.h" "%OUTPUTDIR%\queue.h" /Y
copy "%ROOTDIR%\mqx\source\include\sem.h" "%OUTPUTDIR%\sem.h" /Y
copy "%ROOTDIR%\mqx\source\include\timer.h" "%OUTPUTDIR%\timer.h" /Y
copy "%ROOTDIR%\mqx\source\include\watchdog.h" "%OUTPUTDIR%\watchdog.h" /Y
copy "%ROOTDIR%\mqx\source\include\unicode.h" "%OUTPUTDIR%\unicode.h" /Y
copy "%ROOTDIR%\mqx\source\nio\nio.h" "%OUTPUTDIR%\nio.h" /Y
copy "%ROOTDIR%\mqx\source\nio\errno.h" "%OUTPUTDIR%\errno.h" /Y
copy "%ROOTDIR%\mqx\source\nio\ioctl.h" "%OUTPUTDIR%\ioctl.h" /Y
copy "%ROOTDIR%\mqx\source\nio\fcntl.h" "%OUTPUTDIR%\fcntl.h" /Y
copy "%ROOTDIR%\mqx\source\nio\fs\fs_supp.h" "%OUTPUTDIR%\fs_supp.h" /Y
copy "%ROOTDIR%\mqx\source\tad\tad.h" "%OUTPUTDIR%\tad.h" /Y
goto end_script


:label_uv4
IF NOT EXIST "%OUTPUTDIR%" mkdir "%OUTPUTDIR%"
IF NOT EXIST "%OUTPUTDIR%\." mkdir "%OUTPUTDIR%\."
IF NOT EXIST "%OUTPUTDIR%\../config" mkdir "%OUTPUTDIR%\../config"
copy "%ROOTDIR%\mqx\source\psp\cortex_m\compiler\rv_uv4\asm_mac.h" "%OUTPUTDIR%\asm_mac.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\compiler\rv_uv4\comp.h" "%OUTPUTDIR%\comp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp.h" "%OUTPUTDIR%\psp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_abi.h" "%OUTPUTDIR%\psp_abi.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_comp.h" "%OUTPUTDIR%\psp_comp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_math.h" "%OUTPUTDIR%\psp_math.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_rev.h" "%OUTPUTDIR%\psp_rev.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_supp.h" "%OUTPUTDIR%\psp_supp.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_time.h" "%OUTPUTDIR%\psp_time.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psptypes.h" "%OUTPUTDIR%\psptypes.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_cpu.h" "%OUTPUTDIR%\psp_cpu.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\psp_cpudef.h" "%OUTPUTDIR%\psp_cpudef.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\cortex.h" "%OUTPUTDIR%\cortex.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\kinetis_mpu.h" "%OUTPUTDIR%\kinetis_mpu.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\nvic.h" "%OUTPUTDIR%\nvic.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\kinetis.h" "%OUTPUTDIR%\kinetis.h" /Y
copy "%ROOTDIR%\mqx\source\psp\cortex_m\cpu\kinetis_fcan.h" "%OUTPUTDIR%\kinetis_fcan.h" /Y
copy "%ROOTDIR%\config\mcu\mk64f120m\mqx_sdk_config.h" "%OUTPUTDIR%\..\config\mqx_sdk_config.h" /Y
copy "%ROOTDIR%\config\board\mapsk64\user_config.h" "%OUTPUTDIR%\..\config\user_config.h" /Y
copy "%ROOTDIR%\config\common\small_ram_config.h" "%OUTPUTDIR%\small_ram_config.h" /Y
copy "%ROOTDIR%\mqx\source\include\gen_rev.h" "%OUTPUTDIR%\gen_rev.h" /Y
copy "%ROOTDIR%\mqx\source\include\eds.h" "%OUTPUTDIR%\eds.h" /Y
copy "%ROOTDIR%\mqx\source\include\edserial.h" "%OUTPUTDIR%\edserial.h" /Y
copy "%ROOTDIR%\mqx\source\include\event.h" "%OUTPUTDIR%\event.h" /Y
copy "%ROOTDIR%\mqx\source\include\charq.h" "%OUTPUTDIR%\charq.h" /Y
copy "%ROOTDIR%\mqx\source\include\klog.h" "%OUTPUTDIR%\klog.h" /Y
copy "%ROOTDIR%\mqx\source\include\log.h" "%OUTPUTDIR%\log.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwsem.h" "%OUTPUTDIR%\lwsem.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwevent.h" "%OUTPUTDIR%\lwevent.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwlog.h" "%OUTPUTDIR%\lwlog.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwmem.h" "%OUTPUTDIR%\lwmem.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwmsgq.h" "%OUTPUTDIR%\lwmsgq.h" /Y
copy "%ROOTDIR%\mqx\source\include\lwtimer.h" "%OUTPUTDIR%\lwtimer.h" /Y
copy "%ROOTDIR%\mqx\source\include\message.h" "%OUTPUTDIR%\message.h" /Y
copy "%ROOTDIR%\mqx\source\include\mmu.h" "%OUTPUTDIR%\mmu.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx.h" "%OUTPUTDIR%\mqx.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_cnfg.h" "%OUTPUTDIR%\mqx_cnfg.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_cpudef.h" "%OUTPUTDIR%\mqx_cpudef.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_inc.h" "%OUTPUTDIR%\mqx_inc.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_ioc.h" "%OUTPUTDIR%\mqx_ioc.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_str.h" "%OUTPUTDIR%\mqx_str.h" /Y
copy "%ROOTDIR%\mqx\source\include\mqx_macros.h" "%OUTPUTDIR%\mqx_macros.h" /Y
copy "%ROOTDIR%\mqx\source\include\mutex.h" "%OUTPUTDIR%\mutex.h" /Y
copy "%ROOTDIR%\mqx\source\include\name.h" "%OUTPUTDIR%\name.h" /Y
copy "%ROOTDIR%\mqx\source\include\partition.h" "%OUTPUTDIR%\partition.h" /Y
copy "%ROOTDIR%\mqx\source\include\pcb.h" "%OUTPUTDIR%\pcb.h" /Y
copy "%ROOTDIR%\mqx\source\include\psptypes_legacy.h" "%OUTPUTDIR%\psptypes_legacy.h" /Y
copy "%ROOTDIR%\mqx\source\include\posix.h" "%OUTPUTDIR%\posix.h" /Y
copy "%ROOTDIR%\mqx\source\include\queue.h" "%OUTPUTDIR%\queue.h" /Y
copy "%ROOTDIR%\mqx\source\include\sem.h" "%OUTPUTDIR%\sem.h" /Y
copy "%ROOTDIR%\mqx\source\include\timer.h" "%OUTPUTDIR%\timer.h" /Y
copy "%ROOTDIR%\mqx\source\include\watchdog.h" "%OUTPUTDIR%\watchdog.h" /Y
copy "%ROOTDIR%\mqx\source\include\unicode.h" "%OUTPUTDIR%\unicode.h" /Y
copy "%ROOTDIR%\mqx\source\nio\nio.h" "%OUTPUTDIR%\nio.h" /Y
copy "%ROOTDIR%\mqx\source\nio\errno.h" "%OUTPUTDIR%\errno.h" /Y
copy "%ROOTDIR%\mqx\source\nio\ioctl.h" "%OUTPUTDIR%\ioctl.h" /Y
copy "%ROOTDIR%\mqx\source\nio\fcntl.h" "%OUTPUTDIR%\fcntl.h" /Y
copy "%ROOTDIR%\mqx\source\nio\fs\fs_supp.h" "%OUTPUTDIR%\fs_supp.h" /Y
copy "%ROOTDIR%\mqx\source\tad\tad.h" "%OUTPUTDIR%\tad.h" /Y
goto end_script



:end_script

