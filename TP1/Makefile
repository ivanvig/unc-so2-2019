CC := gcc
CFLAG := -Werror -Wall -pedantic -lcrypt
DBGFLAG := -g
COBJFLAG := $(CFLAG) -c

# path marcros
BIN_PATH := bin
OBJ_PATH := $(BIN_PATH)/obj
SRC_PATH := src
DBG_PATH := debug

# compile marcros
TARGET_NAME := client server
TARGET := $(addprefix $(BIN_PATH)/,$(TARGET_NAME))
TARGET_DEBUG := $(addprefix $(DBG_PATH)/, $(TARGET_NAME))
MAIN_SRC := src/client.c src/server.c

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(BIN_PATH) \
                  $(DBG_PATH)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: dirs all

# non-phony targets
# $(TARGET): $(OBJ)
# 	$(CC) $(CFLAG) -o $@ $?

$(BIN_PATH)/%: $(OBJ_PATH)/%.o $(OBJ_PATH)/%_utils.o
	$(CC) $(CFLAG) -o $@ $?

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c $(SRC_PATH)/remote.h
	$(CC) $(COBJFLAG) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c $(SRC_PATH)/remote.h
	$(CC) $(COBJFLAG) $(DBGFLAG) -o $@ $<

# $(TARGET_DEBUG): $(OBJ_DEBUG)
# 	$(CC) $(CFLAG) $(DBGFLAG) $? -o $@

$(DBG_PATH)/%: $(DBG_PATH)/%.o $(DBG_PATH)/%_utils.o
	$(CC) $(CFLAG) $(DBGFLAG) -o $@ $?

# phony rules
.PHONY: all
all: $(TARGET)

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
