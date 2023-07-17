#2020-10-29

##
SRC_DIR  = src
TEST_DIR = test
INC_DIR  = include
OBJ_DIR  = obj
MAIN_DIR = main

target   = example
depends  = .depends
sources  = $(wildcard $(SRC_DIR)/*.cpp $(MAIN_DIR)/$(target).cpp $(TEST_DIR)/test.cpp)
objects  = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(sources)))
headers  = $(wildcard $(INC_DIR)/*/*.h)

##
CFLAG    = -std=c++20
cpl_cmd  = gcc -g -c $$< -I $(INC_DIR) -o $$@ $(CFLAG)

##
all: $(objects)
	g++ -g $^ -o $(MAIN_DIR)/$(target).exe

$(depends):
	@echo .depends not found, building depends...
	@./deps.exe $(depends) $(INC_DIR) $(OBJ_DIR) "$(cpl_cmd)" $(sources)

include $(depends)

##
.PHONY: clean debug test depend
clean:
	del $(OBJ_DIR)/*.o
	del $(MAIN_DIR)/example.exe
	del $(depends)
debug:
	@echo sources:	$(sources)
	@echo objects:	$(objects)
	@echo headers:	$(headers)
test:
	mingw32-make all
	main/example.exe
depend:
	@del .depends
	@echo rebuilding depends...
	@./deps.exe $(depends) $(INC_DIR) $(OBJ_DIR) "$(cpl_cmd)" $(sources)
	@echo rebuilding completed