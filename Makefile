all:
	./compile resistanceReader.c
	gcc resistanceReaderController.c -o resistanceReaderController
	sudo rm /bin/resistanceReaderController
	sudo cp resistanceReaderController /bin/
