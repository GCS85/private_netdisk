# libencrypt.so
CXXFLAGS += -lcrypto -lssl -fPIC
TARGET = libencrypt.so

OBJS = encrypt.o
SRCS = encrypt.cpp

all:$(OBJS)
	$(CC) $(CXXFLAGS) -shared -o $(TARGET) $(OBJS)

$(OBJS):$(SRCS)
	$(CC) $(CXXFLAGS) -c $^

.PHONY clean:
	@rm -rf $(OBJS) $(TARGET)
