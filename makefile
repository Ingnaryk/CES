#2020-10-29

##
SRC_DIR  = src
INC_DIR  = inc
OBJ_DIR  = obj
MAIN_DIR = main

target   = example
depends  = .depends
sources  = $(wildcard $(SRC_DIR)/*.cpp $(MAIN_DIR)/$(target).cpp)
objects  = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(sources)))
headers  = $(wildcard $(INC_DIR)/*.h)

##
CFLAG    = -std=c++20
cpl_cmd  = gcc -g -c $$< -I $(INC_DIR) -o $$@ $(CFLAG)

##
all: $(objects)
	g++ -g $(filter-out $(depends),$^) -o $(MAIN_DIR)/$(target).exe

$(depends):
	@./deps.exe $(depends) $(INC_DIR) $(OBJ_DIR) "$(cpl_cmd)" $(sources)
	@echo building depends...

sinclude $(depends)

##
.PHONY: clean debug test
clean:
	del $(OBJ_DIR)\\*.o
	del $(MAIN_DIR)\\example.exe
	del $(depends)
debug:
	@echo sources:  	         $(sources)
	@echo objects:  	         $(objects)
	@echo headers:               $(headers)
test:
	mingw32-make all
	main\\example.exe