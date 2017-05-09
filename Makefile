ifndef CC
    CC=gcc
endif

ifndef MATRIXSSL_ROOT
    $(error Please set MATRIXSSL_ROOT)
endif

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
	rm self-matrixssl

