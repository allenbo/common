TEST_SRC_DIR := ./test
INCLUDE_DIR := ./include

TESTBIN_DIR := test_bin

CPP := g++
CC := gcc

CFLAG := -g
LFLAG := -g -flto -lpthread

TEST_SRC := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_TARGET := $(patsubst %.cpp, %, $(subst $(TEST_SRC_DIR),$(TESTBIN_DIR), $(TEST_SRC)))


.PHONY:all $(TESTBIN_DIR)
all: $(TESTBIN_DIR) $(TEST_TARGET)

$(TESTBIN_DIR):
	mkdir -p $@

$(TESTBIN_DIR)/%:$(TEST_SRC_DIR)/%.cpp
	$(CPP) $^ $(CFLAG) $(LFLAG) -o $@ -I$(INCLUDE_DIR)

clean:
	rm -rf $(TESTBIN_DIR)
