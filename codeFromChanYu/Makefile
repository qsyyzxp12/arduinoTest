Bluetooth: Main.o Linux_UART.o
	g++ -O3 -m64 Main.cpp Linux_UART.cpp -lpthread -o Bluetooth

Main.o: Main.cpp	
	g++ -m64 Main.cpp -c -lpthread

Linux_UART.o: Linux_UART.cpp
	g++ -m64 Linux_UART.cpp -c

run:	
	./Bluetooth

clean:	
	rm -rf  Main.o Linux_UART.o
