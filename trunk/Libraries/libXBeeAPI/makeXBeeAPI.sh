echo "compiling"
echo `g++ -g -Wall -fPIC -c src/XBeeAPI.cpp -o obj/XBeeAPI.o`
echo "creating shared library"
echo `g++ -g -shared -Wl,-soname,libXBeeAPI.so.1 -o libXBeeAPI.so.1.0 obj/XBeeAPI.o -lboost_system -lboost_thread`
echo "installing"
echo `sudo mv libXBeeAPI.so.1.0 /usr/local/lib`
echo `sudo ln -sf /usr/local/lib/libXBeeAPI.so.1.0 /usr/local/lib/libXBeeAPI.so`
echo `sudo ln -sf /usr/local/lib/libXBeeAPI.so.1.0 /usr/local/lib/libXBeeAPI.so.1`
echo `sudo cp src/XBeeAPI.h /usr/local/include/XBeeAPI.h`
# export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH