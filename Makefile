all:
	gcc -lwiringPi -lpthread resistanceReader.c -o resistanceReader 
	gcc resistanceReaderController.c -o resistanceReaderController
	sudo rm /bin/resistanceReaderController
	sudo cp resistanceReaderController /bin/
