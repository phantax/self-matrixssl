#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "errno.h"
#include <sys/stat.h>

#include "matrixssl/matrixsslApi.h"

const static char certpath[] = "cert.pem";
const static char keypath[] = "key.pem";
const static char capath[] = "cacert.pem";
const static char password[] = "";


void stimulateServer(uint8_t* data, size_t len);

void hexdump(uint8_t* data, size_t len);

void tohex(uint8_t* str, uint8_t* data, size_t len);


int main(int argc , char* argv[]) {

    /* input */
    uint8_t input[1024*16];
    ssize_t n = read(STDIN_FILENO, &input[0], sizeof(input));
    printf("Read %d bytes:\n", n);

    FILE* file = 0;

    if (argc > 1 && strcmp(argv[1], "-s") == 0) {
        char* defFilename = "stimuli_head.txt";

        /* get stimuli output file size */
        struct stat st;
        stat(defFilename, &st);
        
        if (st.st_size > 16*1024*1024) {

            int iFile = 0;
            char filename[128];
            memset(&filename[0], 0, sizeof(filename));

            while (strlen(&filename[0]) == 0 || access(&filename[0], F_OK) != -1) {
                snprintf(&filename[0], sizeof(filename),
                        "stimuli_%08d.txt", ++iFile);
            }
            rename(defFilename, &filename[0]);
        }
        file = fopen(defFilename, "a");
    }
  

    if (n > 0) {
        printf("===== input =====\n");
        hexdump(&input[0], n);
        printf("=================\n");
        stimulateServer(input, (size_t)n);

        if (file != 0) {
            uint8_t hex[1024*16];
            tohex(&hex[0], &input[0], n);
            n = strlen(&hex[0]);
            hex[n] = '\n';
            fwrite(hex, 1, n + 1, file);
        }
    }

    if (file != 0) {
        fclose(file);
    }

    return 0;
}



void hexdump(uint8_t* data, size_t len) {

    size_t i = 0;
    for (i = 0; i < len; ++i) {
        printf("%.2X ", data[i]);
        if ((i % 16) == 15 || (i + 1) == len) {
            printf("\n");
        }
    }
}



void tohex(uint8_t* str, uint8_t* data, size_t len) {

    size_t i = 0;
    for (i = 0; i < len; ++i) {
        snprintf(str + (2*i), 3, "%.2X", data[i]);
    }
}


void stimulateServer(uint8_t* data, size_t len) {
 
    sslSessOpts_t options;
    sslKeys_t* keys = NULL;
    ssl_t* ssl = NULL;

    int32 rc = 0;

    int32 tmplen = 0;

    int32 lbuf = 0;
    uint8_t* buf;

    memset(&options, 0x0, sizeof(sslSessOpts_t));

	if ((rc = matrixSslOpen()) < 0) {
		printf("MatrixSSL library init failure. Exiting\n");
		return;
	}

    if (matrixSslNewKeys(&keys, NULL) < 0) {
		printf("Unable to allocate keys\n");
	    return;
    }


	if ((rc = matrixSslLoadRsaKeys(keys, certpath, keypath, password, capath)) < 0) {
		printf("Unable to load key material (%i). Exiting\n", rc);
		return;
	}
    printf("matrixSslLoadRsaKeys(...) done!\n");


    options.userPtr = keys;

    if ((rc = matrixSslNewServerSession(&ssl, keys,	NULL, &options)) < 0) {
		printf("Failed to create new session (%i). Exiting\n", rc);
        return;
    }
    printf("matrixSslNewServerSession(...) done!\n");


    /* client -> server */
    
    if ((lbuf = matrixSslGetReadbuf(ssl, &buf)) <= 0) {
        printf("Invalid read buffer (%i). Exiting\n", lbuf);
        return;
    }
    printf("matrixSslGetReadbuf(...) returned %i\n", lbuf);

    if ((int)len > lbuf) {
        printf("Insufficient buffer capacity. Exiting\n", rc);
        return;
    }

    memcpy(buf, data, len);

    if ((rc = matrixSslReceivedData(ssl, len, &buf, (uint32*)&lbuf)) < 0) {
        printf("Failed to transfer data (%i). Exiting\n", rc);
        return;
    }
    printf("matrixSslReceivedData(...) returned %i\n", rc);


    if (rc == MATRIXSSL_REQUEST_SEND) {

        if ((tmplen = matrixSslGetOutdata(ssl, &buf)) > 0) {
            printf("===== output =====\n");
            hexdump(buf, tmplen);
            printf("==================\n");
        }
    }

}

