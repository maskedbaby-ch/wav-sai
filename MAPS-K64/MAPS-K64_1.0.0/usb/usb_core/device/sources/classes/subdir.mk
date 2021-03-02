USBD_CLASS_ROOT := $(USBD_ROOT)/sources/classes

include $(USBD_CLASS_ROOT)/audio/subdir.mk
include $(USBD_CLASS_ROOT)/cdc/subdir.mk
include $(USBD_CLASS_ROOT)/common/subdir.mk
include $(USBD_CLASS_ROOT)/composite/subdir.mk
include $(USBD_CLASS_ROOT)/hid/subdir.mk
include $(USBD_CLASS_ROOT)/msd/subdir.mk
include $(USBD_CLASS_ROOT)/phdc/subdir.mk

INCLUDES += $(USBD_CLASS_ROOT)/include
