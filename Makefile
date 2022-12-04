all: nested-BBG nested-PI 

nested-BBG:
	make --directory=BBG
nested-PI:
	make --directory=PI