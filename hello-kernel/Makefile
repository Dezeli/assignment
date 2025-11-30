KDIR            ?= /lib/modules/$(shell uname -r)/build

BUILD_DIR       := .build
KERNEL_SRC_DIR  := kernel
USER_SRC_DIR    := user
INCLUDE_DIR     := include

KERNEL_MODULE   := $(BUILD_DIR)/hello_kernel.ko
USER_BIN        := $(BUILD_DIR)/hello_user

CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -O2

.PHONY: all kernel user clean

all: $(KERNEL_MODULE) $(USER_BIN)

$(BUILD_DIR):
	@mkdir -p $@

$(KERNEL_MODULE): $(KERNEL_SRC_DIR)/hello_kernel.c $(KERNEL_SRC_DIR)/Makefile | $(BUILD_DIR)
	$(MAKE) -C $(KDIR) M=$(PWD)/$(KERNEL_SRC_DIR) modules
	cp $(KERNEL_SRC_DIR)/hello_kernel.ko $@

$(USER_BIN): $(USER_SRC_DIR)/main.c $(INCLUDE_DIR)/abi.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $(USER_SRC_DIR)/main.c

kernel: $(KERNEL_MODULE)
user:   $(USER_BIN)

clean:
	$(MAKE) -C $(KDIR) M=$(PWD)/$(KERNEL_SRC_DIR) clean
	rm -rf $(BUILD_DIR)