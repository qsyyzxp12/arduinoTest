resistanceReader: resistanceReader.cpp Linux_UART.o Robot_Arm.o Filter.o
	g++ resistanceReader.cpp Filter.o Linux_UART.o Robot_Arm.o -lpthread -lwiringPi -o resistanceReader

Filter.o: Filter.cpp
	g++ Filter.cpp -c

Linux_UART.o: Linux_UART.cpp
	g++ Linux_UART.cpp -c

Robot_Arm.o: Robot_Arm.cpp
	g++ Robot_Arm.cpp -c

resistanceReaderController: resistanceReaderController.c
	gcc resistanceReaderController.c -o resistanceReaderController

install: resistanceReaderController
	sudo rm /bin/resistanceReaderController
	sudo cp resistanceReaderController /bin/

clean:
	rm -rf *.o
