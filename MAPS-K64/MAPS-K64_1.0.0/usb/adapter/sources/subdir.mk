ADAPTER_DIR := $(USB_ROOT)/adapter/sources
SOURCES += $(wildcard $(ADAPTER_DIR)/sdk/*.c)
INCLUDES += $(ADAPTER_DIR) \
			$(ADAPTER_DIR)/sdk
