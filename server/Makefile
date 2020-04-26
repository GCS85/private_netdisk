# private netdisk server
CXXFLAGS += -ludt -lmysqlclient -lencrypt
CXXFLAGS += -Wl,-rpath,$(LIBPATH)
LD_PATH += -L/usr/lib64/mysql/

TARGET = server

SRCS = $(wildcard *.cpp)

all:$(SRCS)
	$(CC) $^  $(LD_PATH) $(INCLUDE) $(CXXFLAGS) -o $(TARGET)
clean:
	rm -rf $(TARGET)
