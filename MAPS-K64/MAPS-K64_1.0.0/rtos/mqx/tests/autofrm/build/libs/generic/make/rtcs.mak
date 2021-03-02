#===================================================================================================
#
#    @brief   Main makefile for RTCS library
#
#===================================================================================================

# Include user_config.h
INCLUDE_DIRS += $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)

# Include generated INCLUDE_DIRS, SRC_FILES lists
-include $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/rtcs/rtcs_$(PLATFORM).mak

# Copy default user_config, and apply test specific and platform specific settings
POST_CLEAN_CMD += \
if ! test -d $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG); then \
  mkdir -p $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG); \
fi; \
cat $(DEV_DIR)/config/board/$(PLATFORM)/user_config.h > $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)/user_config.h; \
if test -f $(DEV_DIR)/tests/$(MODULE)/specific/test_specific.h; then \
  cat $(DEV_DIR)/tests/$(MODULE)/specific/test_specific.h >> $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)/user_config.h; \
fi; \
if test -f $(DEV_DIR)/tests/configurations/output/$(CNFG).h; then \
  cat $(DEV_DIR)/tests/configurations/output/$(CNFG).h >> $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)/user_config.h; \
fi; \
if test -f $(DEV_DIR)/tests/$(MODULE)/specific/$(PLATFORM)/board_specific.h; then \
  cat $(DEV_DIR)/tests/$(MODULE)/specific/$(PLATFORM)/board_specific.h >> $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)/user_config.h; \
fi

# Copy build LIB from out folder
POST_BUILD_CMD += $(CP) $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/rtcs/out/rtcs.$(EXE_EXT) \
$(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/rtcs/rtcs.$(EXE_EXT) && 

# Run post-build LIB bat file
POST_BUILD_CMD += $(call winpath,$(DEV_DIR))/rtcs/build/bat/rtcs_$(PLATFORM).bat \
$(call winpath,$(DEV_DIR)) \
$(call winpath,$(DEV_DIR))/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/rtcs \
$(TOOLCHAIN) &&
POST_BUILD_CMD += $(CP) -f $(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/$(CNFG)/user_config.h \
$(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/user_config.h || exit 0;
#ignore error if files are the same (no CNFG was defined and thus default user_config.h was used)
