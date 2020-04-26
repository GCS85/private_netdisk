# private netdisk client API
CXXFLAGS += -ludt -lencrypt -shared -fPIC 

TARGET = libnwm-udt-client.so

#CUR_DIR := $(PWD)
#OBJS = $(patsubst %.c,%.o,$(CUR_DIR))
SRCS = $(wildcard *.cpp)

$(TARGET):$(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CXXFLAGS) $(LD_PATH) $(INCLUDE)

.PHONY clean:
	@rm -rf $(TARGET)

