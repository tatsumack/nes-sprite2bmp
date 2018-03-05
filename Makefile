CC = gcc
CFLAGS = -Wall -g
OBJS = main.o bmp.o

SRCDIR = ./src
SRCS = $(wildcard $(SRCDIR)/*.c)

OBJDIR = ./obj
OBJS = $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

TARGET = ./bin/nes2bmp

$(TARGET): $(OBJS)
		-mkdir -p ./bin
			$(CC) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
		-mkdir -p $(OBJDIR)
			$(CC) $(CFLAGS) -o $@ -c $<

all: clean $(TARGET)

clean:
		-rm -f $(OBJS) $(TARGET)

sample_img: $(TARGET)
	-$(TARGET) sample.nes sample.bmp && open sample.bmp
