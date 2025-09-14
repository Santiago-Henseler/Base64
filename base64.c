#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

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
        char lastBits1 = (input[0] & MASK2BITS) << 4; 
        // Agrego los 2 ultimos bits al principio del segundo octeto
        int arg1 = ((input[1] >> 4) | lastBits1); 
        output[1] = BASE64[arg1];

        // Agarro los 4 ultimos bits del segundo octeto
        char lastBits2 = (input[1] & MASK4BITS) << 2; 
        // Agrego los 4 ultimos bits al principio del tercer octeto
        int arg2 = ((input[2] >> 6) | lastBits2); 
        output[2] = BASE64[arg2];
    }else if(len == 2){
        // Agarro los 2 ultimos bits del primer octeto
        char lastBits = (input[0] & MASK2BITS) << 4; 

        // Agrego los 2 primeros bits al principio del segundo octeto
        int arg = ((input[1] >> 4) | lastBits); 
        output[1] = BASE64[arg];

        // Agarro los 2 ultimos bits del segundo octeto y los desplazo
        output[2] = BASE64[(input[1] & MASK4BITS) << 2];
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
        encode(buff,output, len);
        encod? encode(buff,output, len) : decode(buff, output);
	    write(STDOUT_FILENO,&output,encod?4:3);
    }
    printf("\n");
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
    
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"encode", no_argument, 0, 'e'},
        {"decode", no_argument, 0, 'd'},
        {"help",   no_argument, 0, 'h'},
        {"input file",   required_argument, 0, 'i'},
        {"output file",   required_argument, 0, 'o'},
        {"version",   no_argument, 0, 'v'},
        {0,        0,           0,  0 }
    };

    if (argc < 2) {
        helpOptions();
        return -1;
    }

    bool encode = true;
    int fd;

    while ((opt = getopt_long(argc, argv, "edhi:o:v", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'e':
                encode = true;
                break;
            case 'd':
                encode = false; 
                break;
            case 'o':
                fd = open(optarg, O_CREAT | O_WRONLY, 0644);
                if(fd < 0){
                    printf("%s", optarg);
                    return -1;
                }
                if(dup2(fd, STDOUT_FILENO) < 0){
                    perror("");
                    return -1;
                }
            break;
            case 'i':
                fd = open(optarg, O_RDONLY);
                if(fd < 0){
                    perror("");
                    return -1;
                }
                if(dup2(fd, STDIN_FILENO) < 0){
                    perror("");
                    return -1;
                }
                break;
            case 'v':
                printf("Base64 verssion 1.0\n");
                return 0;
            case 'h':
                helpOptions();
                return 0;
            default:
                helpOptions();
                return -1;
        }
    }
    
    operate(encode);

    return 0;
}
