ifndef CC
    CC=gcc
endif

MATRIXSSL_ROOT:=/home/walz/Code/matrixssl-3.8.4-afl

include $(MATRIXSSL_ROOT)/common.mk

STATIC:=\
	$(MATRIXSSL_ROOT)/matrixssl/libssl_s.a \
	$(MATRIXSSL_ROOT)/crypto/libcrypt_s.a \
	$(MATRIXSSL_ROOT)/core/libcore_s.a

CIPHER_OPTION=ID_RSA # The default cipher option
CFLAGS+=-D$(CIPHER_OPTION)

all:
	$(CC) main.c $(STATIC) $(LDFLAGS) $(CFLAGS) -o self-matrixssl

clean:
	rm $(OBJ_DIR)/*.o

