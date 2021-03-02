################################################################################
# Include configuration file
################################################################################
include local_config_$(LOCAL_CONF).mak

################################################################################
# Include configuration file to override default setting
################################################################################
-include $(DEV_DIR)/tests/autofrm/station/$(shell hostname)/board_settings.mak

################################################################################
# Power switch settings
################################################################################
ifeq ($(PS),ON)
PRE_RUN_CMD += $(PERL) debug/MQX/common/tools/telnet.pl $(PS_IP) ps $(PS_ID) 0 &&
PRE_RUN_CMD += $(PERL) debug/MQX/common/tools/telnet.pl $(PS_IP) ps $(PS_ID) 1 ;
endif

################################################################################
# Set additional variables
################################################################################
USER_CONFIG=$(DEV_DIR)/tests/$(MODULE)/user_configs/$(PLATFORM)
TOOLCHAIN_CFG_DIR=$(DEV_DIR)/tests/autofrm/beart/toolchain/dummy
DEBUG_CFG_DIR=debug/MQX/$(PLATFORM)
REPORT_DIR=$(OUTPUT_DIR)/target/test-results


################################################################################
# New generaiton scripts require lowercase configuration
# Beart require uppercase
################################################################################
TO_LC = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))
CONFIGURATION_LC := $(call TO_LC,$(CONFIGURATION))


################################################################################
# BUILD_TYPE switches between library and test application
# CFG_DIR - directory for library makefile definition
################################################################################
ifeq ($(BUILD_TYPE),APP)
BUILD_DIR=$(OUTPUT_DIR)/target/build
CFG_DIR=$(DEV_DIR)/tests/$(MODULE)
# $(error $(CFG_DIR))
else
# BUILD_TYPE=LIB
BUILD_DIR=$(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)
CFG_DIR=$(DEV_DIR)/tests/autofrm/build/libs/generic
endif


################################################################################
# Compiler settings
################################################################################
ifeq ($(strip $(TOOLCHAIN)),cw10)
TOOLCHAIN_DIR=$(CW10_DIR)
COMPILER_FAMILY=cw
endif
ifeq ($(strip $(TOOLCHAIN)),cwcf72)
TOOLCHAIN_DIR=$(CWCF72_DIR)
COMPILER_FAMILY=cw
endif
ifeq ($(strip $(TOOLCHAIN)),iar)
TOOLCHAIN_DIR=$(IARARM_DIR)
COMPILER_FAMILY=iar
endif
ifeq ($(strip $(TOOLCHAIN)),uv4)
TOOLCHAIN_DIR=$(UV4_DIR)
COMPILER_FAMILY=uv4
endif
ifeq ($(strip $(TOOLCHAIN)),cwmpc92)
TOOLCHAIN_DIR=$(CW92_DIR)
COMPILER_FAMILY=cw
endif
ifeq ($(strip $(TOOLCHAIN)),ds5)
TOOLCHAIN_DIR=$(DS5_DIR)
COMPILER_FAMILY=ds
endif
ifeq ($(strip $(TOOLCHAIN)),cw10gcc)
TOOLCHAIN_DIR=$(CW10GCC_DIR)
COMPILER_FAMILY=gcc
endif
ifeq ($(strip $(TOOLCHAIN)),kds)
TOOLCHAIN_DIR=$(KDS_DIR)
COMPILER_FAMILY=gcc
endif
ifeq ($(strip $(TOOLCHAIN)),gcc_cs)
TOOLCHAIN_DIR=$(GCC_CS_DIR)
COMPILER_FAMILY=gcc
endif
ifeq ($(strip $(TOOLCHAIN)),gcc_arm)
TOOLCHAIN_DIR=$(GCC_ARM_DIR)
COMPILER_FAMILY=gcc
endif
ifeq ($(strip $(TOOLCHAIN)),cw)
ifeq ($(PLATFORM),twrmpc5125)
TOOLCHAIN_DIR=$(CW92_DIR)
COMPILER_FAMILY=cw
endif
endif


################################################################################
# Platform settings
################################################################################
ifeq ($(PLATFORM),twrmcf52259)
PLATFORM_FAMILY =coldfire
endif
# Kinetis twrk60n512
ifeq ($(PLATFORM),twrk60n512)
PLATFORM_FAMILY =cortex4
endif
ifeq ($(PLATFORM),twrmpc5125)
PLATFORM_FAMILY =powerpc
endif


################################################################################
# Debugger settings
################################################################################
# Load to settings
ifeq ($(TEST_PARAMS),)
  TEST_PARAMS=@LOAD_TO=$(LOAD_TO)
else
  TEST_PARAMS+=LOAD_TO=$(LOAD_TO)
endif

_EMPTY_CHAR_ :=
_COMMA_CHAR_ := $(_EMPTY_CHAR_),$(_EMPTY_CHAR_)
_SPACE_CHAR_ := $(_EMPTY_CHAR_) $(_EMPTY_CHAR_)

TEST_PARAMS:=$(subst $(_SPACE_CHAR_),$(_COMMA_CHAR_),$(TEST_PARAMS))


#PE debugger
ifeq ($(strip $(DEBUGGER)),pe)
  DEBUGGER_DIR=$(PE_DIR)
  CONNECTION_DEVICE=EMPTY
endif

#CW10 debugger
ifeq ($(strip $(DEBUGGER)),cw10)
  DEBUGGER_DIR=$(CW10_DEBUGGER_DIR)
  CONNECTION_DEVICE=EMPTY
endif

#CW92 debugger
ifeq ($(strip $(DEBUGGER)),cw92)
  DEBUGGER_DIR=$(CW92_DIR)
  CONNECTION_DEVICE=EMPTY
endif

#IAR debugger
ifeq ($(strip $(DEBUGGER)),iar)
  DEBUGGER_DIR=$(IARARM_DIR)
  CONNECTION_DEVICE=EMPTY
endif

# UV4 debugger
ifeq ($(strip $(DEBUGGER)),uv4)
  DEBUGGER_DIR=$(UV4_DIR)
  CONNECTION_DEVICE=EMPTY
endif

# Lauterbach debugger
ifeq ($(strip $(DEBUGGER)),lauterbach)
  DEBUGGER_DIR=$(T32_DIR)
  CONNECTION_DEVICE=TRACE32_ICD
endif


###############################################################################
# HTML reports enable/disable
################################################################################
TR2HTML_DIR=junit

################################################################################
# Not used variables
################################################################################
COM_MODULE=NOT_USED
PACKAGE=NOT_USED
DERIVATIVE=NOT_USED
AR_PKG_NAME=NOT_USED
#TRESOS_DIR=NOT_USED due to new params.mak checking

FILTERING = $(foreach v,$(2),$(if $(findstring $(1),$(v)),$(v),))
TEST_BLOCK_SIZE = $(strip $(subst -DEUNIT_TRES_ARRAY_SIZE=,,$(call FILTERING,EUNIT_TRES_ARRAY_SIZE, $(CCDEF))))
TEST_TIMEOUT = $(strip $(subst -DTEST_TIMEOUT=,,$(call FILTERING,TEST_TIMEOUT, $(CCDEF))))
