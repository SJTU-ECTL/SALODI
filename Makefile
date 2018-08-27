all: insert2 insert3

#insert: DFFinsertion.cpp structure.h 
#	g++ -o insert DFFinsertion.cpp 


insert2: DFFinsertion2.cpp structure.h 
	g++ -o insert2 DFFinsertion2.cpp 
insert3: DFFinsertion3.cpp structure.h 
	g++ -o insert3 DFFinsertion3.cpp 
clean:
	rm insert insert2
