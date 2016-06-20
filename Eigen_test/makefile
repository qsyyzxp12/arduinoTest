Robot_Arm: Main.o Robot_Arm.o
	g++ -O3 Main.cpp Robot_Arm.cpp -o Robot_Arm

Main.o: Main.cpp	
	g++ Main.cpp -c

Robot_Arm.o: Robot_Arm.cpp
	g++ Robot_Arm.cpp -c
		
run:	
	./Robot_Arm

clean:	
	rm -rf  Main.o Robot_Arm.o
