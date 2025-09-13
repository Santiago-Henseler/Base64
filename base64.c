#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#define MASK2BITS 0x03 // 00000011
#define MASK4BITS 0x0F // 00001111
#define MASK6BITS 0x3F // 00111111

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

void encode(char input[3], char* output, int len){
    output[0] = BASE64[input[0] >> 2];                        // Elimino los ultimos 2 bits del primer octeto
    output[3] = (len>2) ? BASE64[input[2] & MASK6BITS] : '='; // Elimino los primeros 2 bits del tercer octeto
    
    if(len == 3){
        // Agarro los 2 ultimos bits del primer octeto
        char lastBits1 = (input[0] & MASK2BITS) << 6; 
        // Agrego los 2 ultimos bits al principio del segundo octeto
        char arg1 = ((input[1] >> 2) | lastBits1); 
        output[1] = BASE64[arg1 >> 2];
        // Agarro los 4 ultimos bits del segundo octeto
        char lastBits2 = (input[1] & MASK4BITS) << 4; 
        // Agrego los 4 ultimos bits al principio del tercer octeto
        char arg2 = ((input[2] >> 4) | lastBits2); 
        output[2] = BASE64[arg2 >> 2];
    }else if(len == 2){
        // Agarro los 2 ultimos bits del primer octeto
        char lastBits = (input[0] & MASK2BITS) << 6; 
        // Agrego los 2 ultimos bits al principio del segundo octeto
        char arg = ((input[1] >> 2) | lastBits); 
        output[1] = BASE64[arg >> 2];

        // Agarro los 2 ultimos bits del segundo octeto y los desplazo
        output[2] = BASE64[(input[1] & MASK2BITS) << 2];
    }else{
        // Agarro los 2 ultimos bits del primer octeto
        output[1] = BASE64[(input[0] & MASK2BITS) << 4];
        output[2] = '=';
    }
}

int base64Position(char code){

    int position = -1;
    for (int i = 0; i < 64; i++){
        if(BASE64[i] == code)
            position = i;
    }
    
    return position;
}

void decode(char input[4], char* output){

    int letter1 = base64Position(input[0]); 
    int letter2 = base64Position(input[1]); 
    int letter3 = base64Position(input[2]); 
    int letter4 = base64Position(input[3]); 
    
    output[0] = (letter1 << 2) | (letter2 >> 4);
        
    if(letter3 != -1){ 
        output[1] = ((letter2 & MASK4BITS) << 4) | (letter3 >> 2);
    }
    if(letter4 != -1){
        output[2] =  ((letter3 & MASK2BITS) << 6) | letter4;
    }
}

void operate(bool encod){
    char buff[encod?3:4];
    char output[encod?4:3];

    int len;
    while((len = read(STDIN_FILENO, &buff, encod?3:4))){
        encod? encode(buff,output, len) : decode(buff, output);
	    write(STDOUT_FILENO,&output,encod?4:3);
    }
}

void helpOptions(){
    printf("./base64 [OPTIONS]\n");
        printf("    -e --encode Encodes to Base64\n");
        printf("    -d --decode Decodes from Base64\n");
        printf("    -i --input file Reads from file or stdin\n");
        printf("    -o --output file Writes to file or stdout\n");
        printf("    -v --version Show version string\n");
        printf("    -h --help Print this message and quit\n");
}

int main(int argc, char* argv[]){

    if(argc < 1){
        perror("\033[1;31mSyntax error\033[0m: write ./base64 -h for more help\n");
        return -1;
    }

    bool encode = true;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0){
            helpOptions();
            return 0;
        }
        if(strcmp(argv[i], "-e") == 0){
            encode = true;
        }
        if(strcmp(argv[i], "-d") == 0){
            encode = false;
        }
        if(strcmp(argv[i], "-i") == 0){
            if(argc < i+1){
                perror("\033[1;31mSyntax error\033[0m: the file name is not indicated\n");
                return -1;
            }
            dup2(open(argv[i+1], O_RDONLY), STDIN_FILENO);
        }
        if(strcmp(argv[i], "-o") == 0){
            if(argc < i+1){
                perror("\033[1;31mSyntax error\033[0m: the file name is not indicated\n");
                return -1;
            }
            
            dup2(open(argv[i+1], O_CREAT | O_WRONLY, 0644), STDOUT_FILENO);
        }
    }

    operate(encode);

    return 0;
}
