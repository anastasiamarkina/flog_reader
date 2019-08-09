ccflags-y	+= -iquote include -fPIC

CFLAGS_src1.o	+= -DONE

obj-y		+= src1.o
obj-y		+= src2.o
obj-y		+= sub/src3.o
obj-y		+= ./abs/abs.o

HOSTCC		:= gcc
HOSTLD		:= ld

host-ccflags-y	+= -iquote include

hostprogs-y	+= h1
h1-objs		+= h1.o h2.o

libso-y		+= some-lib
some-lib-objs	+= src1.o src2.o
