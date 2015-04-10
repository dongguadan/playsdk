all:
	make -C./dgdplaysdk
	make -C./sdktest2

clean:
	make clean -C dgdplaysdk
	make clean -C sdktest2
