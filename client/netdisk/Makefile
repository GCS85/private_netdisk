# private netdisk client netdisk
CXXFLAGS += -ludt -lmysqlclient -lencrypt -lnwm-udt-server -lnwm-udt-client
CXXFLAGS += -Wl,-rpath,$(LIBPATH)
CXXFLAGS += -Wl,-rpath,$(TOP_DIR)/lib/libnwm-udt-client/
CXXFLAGS += -Wl,-rpath,$(TOP_DIR)/lib/libnwm-udt-server/
LD_PATH += -L/usr/lib64/mysql/

TARGET = netdisk

SRCS = $(wildcard *.cpp)

all:$(SRCS)
	$(CC) $^  $(LD_PATH) $(INCLUDE) $(CXXFLAGS) -o $(TARGET)
clean:
	rm -rf $(TARGET)
