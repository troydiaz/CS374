make clean && make all

./enc_server <port number1> &
./dec_server <port number2> &

./p5testscript <port number11> <port number2> > mytestresults 2>&1

then view mytestresults