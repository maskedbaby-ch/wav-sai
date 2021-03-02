ifeq ($(PLATFORM), twrpxs30)
  DEBUGGER=lauterbach
  PS=ON
  PS_ID=4
  PS_IP=169.254.3.3
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-3023
endif
ifeq ($(PLATFORM), twrk60d100m)
  DEBUGGER=lauterbach
  PS=OFF
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-1110
endif
ifeq ($(PLATFORM), twrk70f120m)
  DEBUGGER=lauterbach
  PS=OFF
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-3032
endif
ifeq ($(PLATFORM), twrvf65gs10_a5)
  DEBUGGER=lauterbach
  PS=OFF
  PS_ID=3
  PS_IP=169.254.3.3    
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-3033
endif
ifeq ($(PLATFORM), twrvf65gs10_m4)
  DEBUGGER=lauterbach
  PS=OFF
  PS_ID=3
  PS_IP=169.254.3.3
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-3033
endif
ifeq ($(PLATFORM), vybrid_autoevb_a5)
  DEBUGGER=lauterbach
  PS=OFF
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-3024
endif
ifeq ($(PLATFORM), vybrid_autoevb_m4)
  DEBUGGER=lauterbach
  PS=OFF
  RRUN=ON
  RRUN_DEVICE=0
  RRUN_PORT=49175
  RRUN_IP=zcz01-3024
endif
