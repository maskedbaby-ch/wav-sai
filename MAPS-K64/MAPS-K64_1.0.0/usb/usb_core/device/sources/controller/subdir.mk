CONTROLLER_DIR := $(USBD_ROOT)/sources/controller

SOURCES += $(wildcard $(CONTROLLER_DIR)/*.c)
SOURCES += $(wildcard $(CONTROLLER_DIR)/khci/*.c)
INCLUDES += $(CONTROLLER_DIR)
INCLUDES += $(CONTROLLER_DIR)/khci
