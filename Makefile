INC = inc
SRC := $(shell find . -name '*.c')
BUILDIR = build
CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS =  -I$(INC) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTBIN = chip8


all: $(SRC)
	@mkdir -p $(BUILDIR)
	@$(CC) -o $(BUILDIR)/$(OUTBIN) $(CFLAGS) $^ $(LDFLAGS)
	@objdump -d -M intel $(BUILDIR)/$(OUTBIN) > $(BUILDIR)/"$(OUTBIN).dis"

run: all
	@./build/$(OUTBIN)

debug: all
	@./build/$(OUTBIN) -d

clean:
	@rm -rf $(BUILDIR)
	@rm -rf *.log