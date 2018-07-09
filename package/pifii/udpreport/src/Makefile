SOURCES=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SOURCES))
EXEC += udpreport

CFLAGS += -DDEBUG

all: $(EXEC)

$(EXEC):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -luci

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
