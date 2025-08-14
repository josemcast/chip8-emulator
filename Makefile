MAJOR_REV = 0
MINOR_REV = 5

INC = inc
SRC := $(shell find . -name '*.c')
BUILDIR = build
CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS =  -I$(INC) -DMAJOR_REV=$(MAJOR_REV) -DMINOR_REV=$(MINOR_REV) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTBIN = out


all: $(SRC)
	@mkdir -p $(BUILDIR)
	@$(CC) -o $(BUILDIR)/$(OUTBIN) $(CFLAGS) $^ $(LDFLAGS)
	@objdump -d -M intel $(BUILDIR)/$(OUTBIN) > $(BUILDIR)/"$(OUTBIN).dis"

run: all
	@./build/$(OUTBIN)

clean:
	@rm -rf $(BUILDIR)
	@rm -rf *.log