all : libc.a
	# libc.a finish!!

libc.a : OBJS
	#ライブラリ生成
	$(AR) rv $@  *.o
	$(AR) t libc.a
	mv libc.a ../libc.a
	
OBJS :
	## ライブラリのコンパイル ##
	cd ./ctype; $(MAKE)
	cd ./stdlib; $(MAKE)
	cd ./string; $(MAKE)

clean :
	rm -f *.o *.a
	cd ./ctype; $(MAKE) clean
	cd ./stdlib; $(MAKE) clean
	cd ./string; $(MAKE) clean
	