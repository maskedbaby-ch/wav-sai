#===================================================================================================
#
#    @brief   Main makefile for KsdkMqxLib library
#
#===================================================================================================

# Include user_config.h
INCLUDE_DIRS += $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)

# Include generated INCLUDE_DIRS, SRC_FILES lists
-include $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/KsdkMqxLib/KsdkMqxLib_$(PLATFORM).mak

# Copy build LIB from out folder
POST_BUILD_CMD += $(CP) $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/KsdkMqxLib/out/KsdkMqxLib.$(EXE_EXT) \
$(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/KsdkMqxLib/KsdkMqxLib.$(EXE_EXT)
