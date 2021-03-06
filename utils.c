#include "utils.h"

void uart_send_int(int value){
    char string[5];

    sprintf(string, "%d", value);
    uart_send(string);
}

int buffer_find(const char *string) {
  unsigned int i, j, k;

  for (i=0; i<UART_INDEX; i++) {
    j = i;
    k = 0;
    while (1){
        if (UART_BUFFER[j++] != string[k++]) break;
        if (string[k] == '\0') return j;
    }
  }
  return 0;
}

int waitFor(const char *ans, const char *error, unsigned short time){
    unsigned char counter = 0;

    do{
        delay(300);

        if (counter++ > (time / 300)){
            return 0;
        }

        if (error != 0){
            if (buffer_find(error)){
                return -1;
            }
        }
    } while(! buffer_find(ans));

    return 1;
}

void floatToBytes(float value, unsigned char *integ, unsigned char* dec){
    int integer;
    int decimal;

    integer = (int)value;
    decimal = (int)((value - integer) * 100);

    *integ = (unsigned char)integer;
    *dec = (unsigned char)decimal;

    return;
}

void uitoa(char *dst, unsigned int n){
    uint16_t i = n;
    int8_t count = 0;

    while (i > 0){
        i = i / 10;
        count++;
    }

    dst[count] = '\0';
    count--;

    i = n;
    while (count >= 0){
        dst[count] = (i % 10) + '0';
        i = i / 10;
        count--;
    }
}

void generatePayload(unsigned char *dst, float distance, float battery){
    unsigned char d_i, d_r, b_i, b_r, i;

    floatToBytes(distance, &d_i, &d_r);
    floatToBytes(battery, &b_i, &b_r);

    dst[1] = d_r;
    dst[2] = b_r;
    dst[3] = b_i;
    dst[4] = d_i;

    unsigned char hash = 0;

    for (i=4; i>0; i--){
        hash ^= dst[i];
    }

    hash = hash >> 1;
    hash += 69;

    dst[0] = hash;

    return;
}
