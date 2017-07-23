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
#ifdef DEBUG
            ledOn(RED_LED);
#endif
            return 0;
        }
        
        if (error != 0){
            if (buffer_find(error)){
#ifdef DEBUG
                ledOn(RED_LED);
#endif
                return -1;
            }
        }
    } while(! buffer_find(ans));
    
#ifdef DEBUG
    ledOff(RED_LED);
#endif
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

void generatePayload(unsigned char *dst, float distance1, float distance2, float battery){
    unsigned char d1_i, d1_r, d2_i, d2_r, b_i, b_r, i;
    
    floatToBytes(distance1, &d1_i, &d1_r);
    floatToBytes(distance2, &d2_i, &d2_r);
    floatToBytes(battery, &b_i, &b_r);
    
    
    
    dst[1] = d1_r;
    dst[2] = d2_r;
    dst[3] = b_r;
    dst[4] = b_i;
    dst[5] = d2_i;
    dst[6] = d1_i;
    
    unsigned char hash = 0;
    
    for (i=6; i>0; i--){
        hash ^= dst[i]; 
    }
    
    hash = hash >> 1;
    hash += 69;
    
    dst[0] = hash;
    
    return;
}
