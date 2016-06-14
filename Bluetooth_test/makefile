Bluetooth: Main.o Linux_UART.o
	g++ Main.cpp Linux_UART.cpp -lpthread -o Bluetooth

Main.o: Main.cpp	
	g++ Main.cpp -c -lpthread

Linux_UART.o: Linux_UART.cpp
	g++ Linux_UART.cpp -c

run:	
	./Bluetooth

clean:	
	rm -rf  Main.o Linux_UART.o
