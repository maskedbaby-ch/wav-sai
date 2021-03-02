LOCAL &load_to
LOCAL &elf_file
LOCAL &load_params
ENTRY &load_to &elf_file &load_params

&erase_flash_when_error=0

;========================================================================
; Start debugging

  SYStem.RESet
  SYStem.JTAGClock 10MHz

  JTAG.PIN NRESET 0
  wait 200.ms
  JTAG.PIN NRESET 1
  wait 2.s

  GOSUB DebuggerSetupA5
  SYStem.UP
  GOSUB ClockSetup
  GOSUB MemorySetup

  ;clear shared memory
  D.S 0x3f040000++0xffff %long 0
  
  GOSUB StartM4Core

  Data.Load.Elf M4_REMOTE_APP_PATH /GLOBTYPES

  R.S PC __boot
  GO

  GOSUB DebuggerSetupM4

  Data.Load.Elf &elf_file /GLOBTYPES
  B::R.S MSP __BOOT_STACK_ADDRESS

ENDDO

;======================================================
; Included tools (procedures)
include(debug/MQX/common/lauterbach/m4/autoevb.cmm.m4)

