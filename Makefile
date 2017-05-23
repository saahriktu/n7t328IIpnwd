all:
	gcc -o n7t328IIpnwd n7t328IIpnwd.c -lunistring
install:
	cp n7t328IIpnwd /usr/bin
install-scripts:
	cp n7t328IIpnwd_lynx.sh /usr/bin
clean:
	rm n7t328IIpnwd
