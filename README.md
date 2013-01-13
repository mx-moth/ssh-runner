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

Examples
--------

SSHing to some hosts defined in your `~/.ssh/config`:

![SSHing to some already defined hosts](https://github.com/maelstrom/ssh-runner/raw/master/examples/ssh-map.png)

SSHing to an IP address:

![SSHing to an IP address](https://github.com/maelstrom/ssh-runner/raw/master/examples/ssh-ip.png)

An option amongst others:

![Multiple options for xen](https://github.com/maelstrom/ssh-runner/raw/master/examples/ssh-xen.png)
