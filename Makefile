INC = inc
SRCDIR = src
BUILDIR = build
SRC := $(shell find . -name '*.c')
OBJ := $(patsubst ./$(SRCDIR)/%.c,$(BUILDIR)/%.o,$(SRC))

CC = gcc
CFLAGS = -g -Wall -Werror -I$(INC)
LDFLAGS = -lgcov -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTBIN = chip8

ifdef GENERATE_COV
CFLAGS += --coverage
LDFLAGS += --coverage
endif

all: $(OBJ)
	@$(CC) -o $(BUILDIR)/$(OUTBIN) $^ $(LDFLAGS)
	@objdump -d -M intel $(BUILDIR)/$(OUTBIN) > $(BUILDIR)/"$(OUTBIN).dis"

$(BUILDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDIR)
	@$(CC) -c -o $@ $^ $(CFLAGS)

run: all
	@./build/$(OUTBIN)

debug: all
	@./build/$(OUTBIN) -d

clean:
	@rm -rf $(BUILDIR)
	@rm -rf *.log
	@rm -rf *.gcov
