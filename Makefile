
# 
# FF_ROOT     pointing to the FastFlow root directory (i.e.
#             the one containing the ff directory).
ifndef FF_ROOT
FF_ROOT		= $(HOME)/fastflow
endif

CXX ?= g++ -std=c++17 

SRC_PATH = src
BUILD_PATH = build
OBJECTS_PATH = $(BUILD_PATH)/objects

BIN_NAME = bfs

SOURCES = $(shell find $(SRC_PATH) -name '*.cpp' | sort -k 1nr | cut -f2-)
OBJECTS = $(SOURCES:$(SRC_PATH)/%.cpp=$(OBJECTS_PATH)/%.o)
DEPS = $(OBJECTS:.o=.d)

COMPILE_FLAGS = -std=c++17 -Wall -O3 -finline-functions -DNDEBUG
INCLUDES = -I include/ -I /usr/local/include -I $(FF_ROOT) 

# Space-separated pkg-config libraries
LIBS = 

.PHONY: release
release: export CXXFLAGS := $(CXXFLAGS) $(COMPILE_FLAGS)
release: dirs
	@$(MAKE) all

.PHONY: dirs
dirs:
	@echo "Creating directories"
	@mkdir -p $(dir $(OBJECTS))
	@mkdir -p $(BUILD_PATH)

.PHONY: clean
clean:
	@echo "Deleting $(BIN_NAME) symlink"
	@$(RM) $(BIN_NAME)
	@echo "Deleting directories"
	@$(RM) -r $(OBJECTS_PATH)
	@$(RM) -r $(BUILD_PATH)

# Executable
.PHONY: all
all: $(BUILD_PATH)/$(BIN_NAME)
	@echo "Making symlink: $(BIN_NAME) -> $<"
	@$(RM) $(BIN_NAME)
	@ln -s $(BUILD_PATH)/$(BIN_NAME) $(BIN_NAME)

$(BUILD_PATH)/$(BIN_NAME): $(OBJECTS)
	@echo "Linking: $@"
	$(CXX) $(OBJECTS) -o $@ ${LIBS}

# Add dependency files
-include $(DEPS)

# Source file ruless
$(OBJECTS_PATH)/%.o: $(SRC_PATH)/%.cpp
	@echo "Compiling: $< -> $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MP -MMD -c $< -o $@