# Makefile for DRouter

TARGET = Router

SRC_DIR = src
PARSER_DIR = parser
ROUTER_DIR = router
UTIL_DIR = util
GENER_DIR = generator

INC_PATH = -I$(SRC_DIR)

LIB_PATH = -L

LIBS = -lgflags

SRCS = $(SRC_DIR)/main.cpp \
	   $(SRC_DIR)/$(PARSER_DIR)/parser.cpp \
	   $(SRC_DIR)/$(PARSER_DIR)/net.cpp \
	   $(SRC_DIR)/$(PARSER_DIR)/instance.cpp \
	   $(SRC_DIR)/$(ROUTER_DIR)/router.cpp \
	   $(SRC_DIR)/$(ROUTER_DIR)/grid.cpp \
	   $(SRC_DIR)/$(UTIL_DIR)/config.cpp \
	   $(SRC_DIR)/$(UTIL_DIR)/utils.cpp \
	   $(SRC_DIR)/$(GENER_DIR)/generator.cpp \

OBJ_DIR = build

OBJS = $(addprefix $(OBJ_DIR)/, $(subst .cpp,.o, $(SRCS)))
DEPS = $(addprefix $(OBJ_DIR)/, $(subst .cpp,.d, $(SRCS)))

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	CXX = g++
endif

# for debug
# CXXFLAGS = -Wall -g -std=c++11 -MMD $(INC_PATH) -DDEBUG -DWRITEOUT
# for release
CXXFLAGS = -w -O3 -std=c++11 -MMD $(INC_PATH) -DNDEBUG

LDXXFLAGS = $(LIB_PATH) $(LIBS)

RM = rm -f

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDXXFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean distclean

clean:
	$(RM) $(OBJS)

distclean:
	$(RM) $(OBJS) $(TARGET) $(DEPS)

-include $(DEPS)
