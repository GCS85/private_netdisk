CC = g++
CXXFLAGS := -std=c++11

TOP_DIR := $(PWD)
LIBPATH := $(TOP_DIR)/lib/libencrypt/

INCLUDE += -I $(TOP_DIR)/lib/libencrypt/ -I $(TOP_DIR)/lib/libnwm-udt-server/ -I $(TOP_DIR)/lib/libnwm-udt-client/
LD_PATH := -L $(TOP_DIR)/lib/libencrypt/ -L $(TOP_DIR)/lib/libnwm-udt-server/ -L $(TOP_DIR)/lib/libnwm-udt-client/

SUB_DIR := 	lib/libencrypt \
		lib/libnwm-udt-client \
		lib/libnwm-udt-server \
		server \
		client/netdisk \

export

.PHONY : all $(SUB_DIR)

all : $(SUB_DIR)

$(SUB_DIR):
	make -C $@

clean:
	for d in $(SUB_DIR); \
	do \
		make --directory=$$d clean; \
	done 
	
