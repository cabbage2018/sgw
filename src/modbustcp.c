#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifndef _MSC_VER
#include <stdint.h>
#else
#include "stdint.h"
#endif
#include "modbus/modbus.h"
#include <windef.h>
#include <errno.h>
typedef void(*cbfBits)(const uint16_t*, const int);
typedef void(*cbfRegs)(const uint16_t*, const int);

// signature for this plugin module
void onReceieBits(BOOLEAN val[], int num) {
    char buf[1024];
    int index = sprintf(buf, "[");
    for(int i=0; i<num; i++) {
        if(i == num-1 ) {
            index += sprintf(buf + index, "%d", val[i]? "true":"false");
        } else {
            index += sprintf(buf + index, "%d,", val[i]? "true":"false");
        }
    }        
    index += sprintf(buf + index, "]");
    // log
    fprintf(stdout, "toString(): %s(index:%d)\n", buf, index);
    // send to IoT edge/cloud/
    return;
}
void onReceieRegisters(int16_t val[], int num) {
    char buf[1024];
    int index = sprintf(buf, "[");
    for(int i=0; i<num; i++) {
        if(i == num-1 ) {
            index += sprintf(buf + index, "%d", val[i]);
        } else {
            index += sprintf(buf + index, "%d,", val[i]);
        }
    }
    index += sprintf(buf + index, "]");
    // log
    fprintf(stdout, "toString(): %s(index:%d)\n", buf, index);
    return;
}

void handler1(const uint16_t const *buf, const int quantity) {
    printf("registers: ");
    for (int i=0; i<quantity; i++) {
        printf("Address = %d, value %d (0x%X)\n", i, buf[i], buf[i]);
    }
}

/*
Function Codes Supported by Controllers 22 . . . . . . . . . . . . . . . . . . . . . . . . .
01 Read Coil Status 24 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
02 Read Input Status 26 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
03 Read Holding Registers 28 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
04 Read Input Registers 30 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
05 Force Single Coil 32 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
06 Preset Single Register 34 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
07 Read Exception Status 36 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
11 (0B Hex) Fetch Comm Event Ctr 38 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
12 (0C Hex) Fetch Comm Event Log 40 . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
15 (0F Hex) Force Multiple Coils 44 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
16 (10 Hex) Preset Multiple Regs 46 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
17 (11 Hex) Report Slave ID 48 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
20 (14Hex) Read General Reference 58 . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
21 (15Hex) Write General Reference 62 . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
22 (16Hex) Mask Write 4X Register 66 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
23 (17Hex) Read/Write 4X Registers 68 . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
24 (18Hex) Read FIFO Queue 70
*/
void runOnce(const char* host, const int port, const int subordinator,
    const int fc, const int addr, const int count,
    const writeAddr, const int writeCount,
    const uint16_t* values, const uint8_t* bits/**/) {
    
    modbus_t *ctx;
    int rc;

/* RTU */
/*
    ctx = modbus_new_rtu("/dev/ttyUSB0", 19200, 'N', 8, 1);
    modbus_set_slave(ctx, SERVER_ID);
*/
    /* TCP */
    ctx = modbus_new_tcp(host, port);
    modbus_set_debug(ctx, TRUE);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return;
    }
    
    // special to SIEMENS MODBUS-TCP
    rc = modbus_set_slave(ctx, subordinator);
    if (rc != 1) {
        printf("ERROR modbus_set_slave(ctx, subordinator) %d(%p, %d); ", rc, &ctx, subordinator);
        printf("register = %d\n", addr);
    }

    rc = modbus_get_slave(ctx);
    if (rc != 1) {
        printf("ERROR modbus_get_slave(modbus_t* ctx) %d(%p); ", rc, &ctx);
    }

    switch (fc) {
        case 0x17:
            uint16_t* buf = (uint16_t *) malloc(count * sizeof(uint16_t));
            memset(buf, 0, count * sizeof(uint16_t));
            rc = modbus_write_and_read_registers(ctx, addr, count,
                                               buf, writeAddr, writeCount,
                                               values);
            free(buf);
        break;

        case 16:
            rc = modbus_write_registers(ctx, writeAddr, writeCount, values);
        break;

        case 15:
            rc = modbus_write_bits(ctx, writeAddr, writeCount, bits);
        break;

        case 6:
            rc = modbus_write_register(ctx, writeAddr, values[0]);
        break;

        case 5:
            rc = modbus_write_bit(ctx, writeAddr, bits[0]);
        break;

        case 4:            
            uint16_t* buf = (uint16_t *) malloc(count * sizeof(uint16_t));
            memset(buf, 0, count * sizeof(uint16_t));
            rc = modbus_read_input_registers(ctx, addr, count, buf);
            if(rc == 1) {
                onReceieRegisters(buf, count);
            }
            free(buf);
        break;

        case 3:
            uint16_t* buf = (uint16_t *) malloc(count * sizeof(uint16_t));
            memset(buf, 0, count * sizeof(uint16_t));
            rc = modbus_read_registers(ctx, addr, count, buf);
            if(rc == 1) {
                onReceieRegisters(buf, count);
            }
            free(buf);
        break;

        case 2:
            uint8_t *bits;
            bits = (uint8_t *) malloc(count * sizeof(uint8_t));
            memset(bits, 0, count * sizeof(uint8_t));
            rc = modbus_read_input_bits(ctx, addr, 1, bits);
            if(rc == 1) {
                onReceieBits(bits, count);
            }
            free(bits);
        break;

        case 1:
            uint8_t *bits;
            bits = (uint8_t *) malloc(count * sizeof(uint8_t));
            memset(bits, 0, count * sizeof(uint8_t));
            rc = modbus_read_bits(ctx, addr, 1, bits);
            if(rc == 1) {
                onReceieBits(bits, count);
            }
            free(bits);
        break;

        default:
            fprintf(stderr, "Error function code :%d not valid\n", fc);
            rc = 0 - (1 << 15);
            // fprintf(stderr, "Error no.%d: %s\n", errno, strerror(errno));
        break;
    }

    if (rc != 1) {
        fprintf(stderr, "%d Write?read Coils?Register failed(%d): %d-%d, %d-%d...\n", fc, rc, writeAddr, writeCount, addr, count);
    }
 
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return;
}