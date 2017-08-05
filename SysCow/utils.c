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

void floatToBytes(float value, uint16_t *integ, uint8_t* dec){
    int integer;
    int decimal;
    
    integer = (int)value;
    decimal = (int)((value - integer) * 100);
    
    *integ = (uint16_t)integer;
    *dec = (uint8_t)decimal;
    
    return;
}

void generatePayload(uint8_t *dst, float distance1, float distance2, float battery){
    uint8_t d1_r, d2_r, b_i, b_r, i;
    uint16_t d1_i, d2_i;
    
    floatToBytes(distance1, &d1_i, &d1_r);
    floatToBytes(distance2, &d2_i, &d2_r);
    floatToBytes(battery, &b_i, &b_r);
    
    dst[1] = d1_i;
    dst[2] = d2_i;
    dst[3] = b_r;
    dst[4] = b_i;
    dst[5] = (d2_i >> 8);
    dst[6] = d2_r;
    dst[7] = (d1_i >> 8);
    dst[8] = d1_r;
    
    uint8_t hash = 0;
    for (i=8; i>0; i--){
        hash ^= dst[i]; 
    }
    
    hash = hash >> 1;
    hash += 69;
    
    dst[0] = hash;
    
    return;
}
