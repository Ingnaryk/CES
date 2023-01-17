#2020-10-29

.PHONY:clean

##
SRC_DIR=src
OBJ_DIR=obj
MAIN_DIR=main
INC_DIR=inc

src=$(wildcard $(SRC_DIR)/*.cpp)
src_obj=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(src))
tpl_h=$(INC_DIR)/RawArray.h $(INC_DIR)/StdArray.h $(INC_DIR)/Async.h $(INC_DIR)/Test.h
main=$(MAIN_DIR)/example.cpp
main_obj=$(OBJ_DIR)/example.o
target=$(MAIN_DIR)/example.exe
all_obj=$(src_obj) $(main_obj)

##
CFLAG=-std=c++20

##
$(target):$(all_obj)
	g++ -g $^ -o $@

$(main_obj):$(main) $(tpl_h)
	gcc -g -c $< -o $@ $(CFLAG)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cpp $(INC_DIR)/%.h
	gcc -g -c $< -o $@ $(CFLAG)

##
clean:
	del $(OBJ_DIR)\\*.o
	del $(MAIN_DIR)\\example.exe
