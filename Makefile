CC = g++
CXXFLAGS := -std=c++11

TOP_DIR := $(PWD)
LIBPATH := $(TOP_DIR)/lib/libencrypt/

INCLUDE += -I $(TOP_DIR)/lib/libencrypt/
LD_PATH := -L $(TOP_DIR)/lib/libencrypt/

SUB_DIR := 	lib/libencrypt \
		lib/libnwm-udt-client \
		lib/libnwm-udt-server \
#		server \
#		client/netdisk \

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
	
