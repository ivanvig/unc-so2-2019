CC := gcc
CFLAG := -Werror -Wall -pedantic -lcrypt
DBGFLAG := -g
COBJFLAG := $(CFLAG) -c -MMD

# path marcros
BIN_PATH := bin
OBJ_PATH := $(BIN_PATH)/obj
SRC_PATH := src
DBG_PATH := debug


# clean files list
DISTCLEAN_LIST := $(BIN_PATH) \
                  $(DBG_PATH)

CLEAN_LIST := $(OBJ_PATH) \
							$(DBG_PATH)

# default rule
default: all

$(BIN_PATH)/%: $(OBJ_PATH)/%.o $(OBJ_PATH)/%_utils.o
	$(CC) $(CFLAG) -o $@ $?

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) $(COBJFLAG) -o $@ $<

$(DBG_PATH)/%: $(DBG_PATH)/%.o $(DBG_PATH)/%_utils.o
	$(CC) $(CFLAG) $(DBGFLAG) -o $@ $?

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c $(SRC_PATH)/%.h
	$(CC) $(COBJFLAG) $(DBGFLAG) -o $@ $<

-include $(OBJ_PATH)/*.d

# phony rules
.PHONY: all
all: dirs client server

.PHONY: client
client: dirs $(BIN_PATH)/client

.PHONY: server
server: dirs $(BIN_PATH)/server

.PHONY: dirs
dirs:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: debug
debug: dirs $(TARGET_DEBUG)

.PHONY: clean
clean:
	rm -rf $(CLEAN_LIST)

.PHONY: distclean
distclean:
	rm -rf $(DISTCLEAN_LIST)
