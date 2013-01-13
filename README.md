KRunner SSH helper
==================

Easily connect to remote servers

Installing
----------

	sudo apt-get install build-essential cmake kdelibs5-dev
	cd kde-addons-krunner-ssh/
	mkdir build/
	cd build/
	cmake .. -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix`
	make
	make install
