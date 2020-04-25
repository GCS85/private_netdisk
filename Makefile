CC = g++
CXXFLAGS := -std=c++11

TOP_DIR := $(PWD)
LIBPATH := $(TOP_DIR)/lib/libencrypt/

INCLUDE += -I $(TOP_DIR)/lib/libencrypt/
LD_PATH := -L $(TOP_DIR)/lib/libencrypt/


SUB_DIR := 	server
#		client	\
#		lib/libencrypt \

export

$(SUB_DIR):ECHO
	make -C $@
ECHO:
	@echo $(SUB_DIR)
	@echo begin compile
clean:
	make clean -C $(SUB_DIR)
