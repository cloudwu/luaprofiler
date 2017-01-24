LUA_INC = -I/usr/local/include
LUA_LIB = -L/usr/local/bin -llua53

mingw : profiler.dll
linux : profiler.so

profiler.dll : profiler.c	
	gcc -Wall --shared -o $@ $^ $(LUA_INC) $(LUA_LIB)

profiler.so : profiler.c
	gcc -Wall -fPIC --shared -o $@ $^ $(LUA_INC)

clean :
	rm -f profiler.dll profiler.so
