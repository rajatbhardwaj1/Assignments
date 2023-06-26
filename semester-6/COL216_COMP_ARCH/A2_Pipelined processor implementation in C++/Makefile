all: compile

compile: 5stage  5stage_bypass 79stage 79stage_bypass

5stage: 5stage.cpp 5stage.hpp
	g++ 5stage.cpp 5stage.hpp -o 5stage.out

5stage_bypass: 5stage_bypass.cpp 5stage_bypass.hpp
	g++ 5stage_bypass.cpp 5stage_bypass.hpp -o 5stage_bypass.out

79stage: 79stage.cpp 79stage.hpp
	g++ 79stage.cpp 79stage.hpp -o 79stage.out

79stage_bypass: 79stage_bypass.cpp 79stage_bypass.hpp
	g++ 79stage_bypass.cpp 79stage_bypass.hpp -o 79stage_bypass.out

run_5stage: 
	./5stage.out input.asm

run_5stage_bypass:
	./5stage_bypass.out input.asm

run_79stage:
	./79stage.out input.asm

run_79stage_bypass:
	./79stage_bypass.out input.asm

clean:
	rm -rf *.exe *.out
