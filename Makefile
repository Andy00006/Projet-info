CC = gcc
CFLAGS = -Wall -Wextra -O2 -g -std=c11
OBJDIR = build/obj
BINDIR = build
TARGET = $(BINDIR)/eau_analyse

SOURCES = avl.c data.c main.c
OBJS = $(SOURCES:%.c=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c
	mkdir -p $(OBJDIR)
    # Note: Utilise une tabulation réelle ici si tu copies-colles
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build vol_*.dat vol_*.png leaks_history.dat
