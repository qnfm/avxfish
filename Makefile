#CC = clang

CFLAGS = -O3 -fPIC -g -Wall -Wextra -std=gnu99 -pedantic -mavx512f -mavx512vl
LDFLAGS = -static
INCLUDES = include
SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin
LIBDIR = ./lib

.PHONY: all
all: $(LIBDIR)/libavxfish.a $(BINDIR)/bench $(BINDIR)/test

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

$(OBJDIR) $(LIBDIR) $(BINDIR):
	mkdir -p $@

$(OBJDIR)/%.S.o: $(SRCDIR)/%.S | $(OBJDIR)
	$(CC) -o $@ -c $<

$(OBJDIR)/%.c.o: $(SRCDIR)/%.c $(shell find $(INCLUDES) -name '*.h') | $(OBJDIR)
	$(CC) -o $@  -I$(INCLUDES) $(CFLAGS) -c $<

$(LIBDIR)/libavxfish.a: $(OBJDIR)/avxfish.S.o | $(LIBDIR)
	$(AR) rcs $@ $^

$(BINDIR)/bench: $(OBJDIR)/avxfish.S.o $(OBJDIR)/bench.c.o | $(BINDIR)
	$(CC) -o $@ $(LDFLAGS) $^

$(BINDIR)/test: $(OBJDIR)/avxfish.S.o $(OBJDIR)/test.c.o | $(BINDIR)
	$(CC) -o $@ $(LDFLAGS) $^
