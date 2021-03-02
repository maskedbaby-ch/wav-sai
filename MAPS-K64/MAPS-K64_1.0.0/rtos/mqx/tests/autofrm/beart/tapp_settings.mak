################################################################################
# Test result base address
################################################################################
TEST_BASE_ADDR := eunit_tres_array

################################################################################
# Test execution timeout in tenth of seconds
################################################################################
# TEST_TIMEOUT is now defined in .yml
# TEST_TIMEOUT := 3000

################################################################################
# Other options
################################################################################
EU_CUSTOM_DEFINES=

################################################################################
# Add include path to EUnit_Assertion_Ids.h file made during "make parse" target
################################################################################
ifeq ($(BUILD_TYPE), APP)
INCLUDE_DIRS += \
$(BUILD_DIR)/$(TEST_NAME)/debug
endif
################################################################################
# EUNIT print assertion details
################################################################################
ifeq ($(PRINT_DETAILS),ON)
CCDEF+=-DEU_PRINT_DETAILS=TRUE
endif

################################################################################
# Re-run feature enablement
################################################################################
ifeq ($(BUILD_TYPE),APP)
-include $(BUILD_DIR)/$(TEST_NAME)/debug/suppress_run.mak
endif  # ($(BUILD_TYPE),APP)

################################################################################
# VectorCAST enablement
################################################################################
ifeq ($(CCOV_ENABLE),VCAST)

VCAST_WORK_DIR=$(DEV_DIR)/lib/$(CNFG)/$(PLATFORM).$(TOOLCHAIN)/$(CONFIGURATION_LC)/_utils/vcast
VCAST_PROJECT=$(PLATFORM)_$(TOOLCHAIN)_$(CONFIGURATION_LC)_$(CNFG)

CCDEF+=-DEU_CCOV_ENABLE=VCAST

# USBH and USBD share same make file folder
ifeq ($(TEST),$(filter $(TEST),usbh usbd))
MAK_FILE_PATH=$(call winpath,$(BUILD_DIR))/usb/$(TEST)_$(PLATFORM).mak
else
MAK_FILE_PATH=$(call winpath,$(BUILD_DIR))/$(TEST)/$(TEST)_$(PLATFORM).mak
endif

ifeq ($(BUILD_TYPE), LIB)
POST_PARSE_CMD=python $(call winpath,$(DEV_DIR))/tests/autofrm/utils/vcast/vcast_instrument.py \
    --toolchain "$(TOOLCHAIN)" \
    --toolchain_dir "$(call winpath,$(TOOLCHAIN_DIR))" \
    --compile_options "$(CCOPT)" \
    --compile_defines "$(foreach v,$(CCDEF),$(subst -D,,$(v)))" \
    --source_files "$(call winpath,$(SRC_FILES))" \
    --library "$(TEST)" \
    --make_file_path "$(MAK_FILE_PATH)" \
    --vc_project_name "$(VCAST_PROJECT)" \
    --vc_work_dir "$(call winpath,$(VCAST_WORK_DIR))" \
    --dev_dir "$(call winpath,$(DEV_DIR))" \
    --keep_sources_inplace "Y" \
    --instr_type "$(CCOV_TYPE)" \
    --filter_library "$(CCOV_FILTER_LIB)" \
    --filter_out "$(CCOV_FILTER_OUT)" \
    --filter_in "$(CCOV_FILTER_IN)" 

endif

ifeq ($(BUILD_TYPE), APP)
SRC_FILES+=$(DEV_DIR)/tests/autofrm/utils/vcast/c_cover_io.c
INCLUDE_DIRS+=$(VCAST_WORK_DIR)/$(VCAST_PROJECT)

POST_REPORT_CMD=python $(call winpath,$(DEV_DIR))/tests/autofrm/utils/vcast/vcast_result.py \
    --coverage_data_dir $(call winpath,$(DEBUG_DIR)) \
    --vc_test_name $(TEST) \
    --vc_project_name $(VCAST_PROJECT) \
    --vc_work_dir $(call winpath,$(VCAST_WORK_DIR))

endif

endif

################################################################################
# Defines for linker file tests
################################################################################
ifeq ($(LOAD_TO),INTRAM)
CCDEF+=-DLINKER_USED=1
endif

ifeq ($(LOAD_TO),INTFLASH)
CCDEF+=-DLINKER_USED=2
endif
ifeq ($(LOAD_TO),INTFLASH-SRAM)
CCDEF+=-DLINKER_USED=2
endif
ifeq ($(LOAD_TO),INTFLASH-DDR)
CCDEF+=-DLINKER_USED=2
endif

ifeq ($(LOAD_TO),EXTRAM)
CCDEF+=-DLINKER_USED=3
endif
ifeq ($(LOAD_TO),EXTMRAM)
CCDEF+=-DLINKER_USED=3
endif

ifeq ($(LOAD_TO),EXTFLASH)
CCDEF+=-DLINKER_USED=4
endif

ifeq ($(LOAD_TO),DDR)
CCDEF+=-DLINKER_USED=5
endif

################################################################################
# Use PREPROCESS_SRC
################################################################################
ifeq ($(BUILD_TYPE), APP)
  PREPROCESS_SRC=$(call FILTERING,/test.c,$(SRC_FILES))
endif