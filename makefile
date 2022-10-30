#2020-10-29

src=$(wildcard ./*.cpp ./src/*.cpp)
obj=$(patsubst %.cpp,%.o,$(src))

example:$(obj)
	g++ -g -o $@ $^

%.o:%.cpp
	g++ -g -c $< -I./include -o $@

.PHONY:clean

clean:
	del *.o
	del src\\*.o
	del example.exe

test:
	@echo $(src)
	@echo $(obj)