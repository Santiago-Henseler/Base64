#include <unistd.h>

#define STDIN 0
#define STDOUT 1

const char BASE64[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
};

const char ENTER = '\n';

int main(int argc, char* argv[]){
    unsigned char new_byte = 0x00;
    unsigned char tmp = 0x00;

    unsigned int i = 1;
    unsigned int acc = 0;
    
    read(STDIN,&new_byte,1);
    while ( new_byte != 0x0A ) {         
        write(STDOUT,&BASE64[(new_byte >> 2*i) + acc],1);

        tmp = (new_byte << 2*(4-i));  
        acc = tmp >> 2;

        i++;
        if (i == 4){
            write(STDOUT,&BASE64[acc],1);
            i = 1;        
            acc = 0;
        }
        read(STDIN,&new_byte,1);
    }
    write(STDOUT,&ENTER,1);

    
    return 0;
}
