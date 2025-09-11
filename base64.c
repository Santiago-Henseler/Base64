#include <unistd.h>

#define STDIN 0
#define STDOUT 1

const char BASE64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 

const unsigned int MASK1 = 0x00FC0000;
const unsigned int MASK2 = 0x003F0000;
const unsigned int MASK3 = 0x0000FC00;
const unsigned int MASK4 = 0x0000003F;

int buf_in;
char buf_out[4];

void toBase64(int len) {
	buf_out[0] = BASE64[ ( buf_in & MASK1 ) >> 18 ];
	buf_out[1] = BASE64[ ( buf_in & MASK2 ) >> 12 ];
	buf_out[2] = (len > 1)? BASE64[ ( buf_in & MASK3 ) >>  6 ]: '='; 
	buf_out[3] = (len > 2)? BASE64[ ( buf_in & MASK4 )       ]: '=';
}

int main(int argc, char* argv[]){
	int len;
	while ( (len = read(STDIN,&buf_in,3) ) ) {
		toBase64(len);		
		write(STDOUT,&buf_out,4);				
	}
    
	return 0;
}
