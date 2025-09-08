#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <fcntl.h>

#define MASK2BITS 0x03 // 00000011
#define MASK4BITS 0x0F // 00001111
#define MASK6BITS 0x3F // 00111111

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

char * encode(char * input){

    char * output = malloc(sizeof(char));

    int packs = (int)(floor(strlen(input) / 3) * 3);
    int i;
    for(i = 0; i < packs; i += 3){

        output = realloc(output, 4*sizeof(char));
        int len = strlen(output);

        output[len] = BASE64[input[i] >> 2];

        // Agarro los 2 ultimos bits del primer octeto
        char lastBits1 = (input[i] & MASK2BITS) << 6; 
        // Agrego los 2 ultimos bits al principio del segundo octeto
        char arg1 = ((input[i+1] >> 2) | lastBits1); 
        output[len+1] = BASE64[arg1 >> 2];

        // Agarro los 4 ultimos bits del segundo octeto
        char lastBits2 = (input[i+1] & MASK4BITS) << 4; 
        // Agrego los 4 ultimos bits al principio del tercer octeto
        char arg2 = ((input[i+2] >> 4) | lastBits2); 
        output[len+2] = BASE64[arg2 >> 2];

        // Elimino los primeros 2 bits del tercer octeto
        output[len+3] = BASE64[input[i+2] & MASK6BITS];
    }

    if(strlen(input) - i == 1){
        output = realloc(output, 4*sizeof(char));
        int len = strlen(output);

        output[len] = BASE64[input[i] >> 2];
        // Agarro los 2 ultimos bits del primer octeto
        output[len+1] = BASE64[(input[i] & MASK2BITS) << 4];
        output[len+2] = '=';
        output[len+3] = '=';
    }else if(strlen(input) - i == 2){
        output = realloc(output, 4*sizeof(char));
        int len = strlen(output);

        output[len] = BASE64[input[i] >> 2];

        // Agarro los 2 ultimos bits del primer octeto
        char lastBits = (input[i] & MASK2BITS) << 6; 
        // Agrego los 2 ultimos bits al principio del segundo octeto
        char arg = ((input[i+1] >> 2) | lastBits); 
        output[len+1] = BASE64[arg >> 2];

        // Agarro los 2 ultimos bits del segundo octeto y los desplazo
        output[len+2] = BASE64[(input[i+1] & MASK2BITS) << 2];
        output[len+3] = '=';
    }

    return output;
}

int base64Position(char code){

    int position = -1;
    for (int i = 0; i < 64; i++){
        if(BASE64[i] == code)
            position = i;
    }
    
    return position;
}

char * decode(char * input){

    char * output = malloc(sizeof(char));

    int i;
    for(i = 0; i < strlen(input); i += 4){
        output = realloc(output, 3*sizeof(char));
        int len = strlen(output);

        int letter1 = base64Position(input[i+3]); 
        int letter2 = base64Position(input[i+2]); 
        int letter3 = base64Position(input[i+1]); 
        int letter4 = base64Position(input[i]); 

        output[len] = (letter4 << 2) | (letter3 >> 4);
        
        if(letter2 != -1){
           output[len+1] = ((letter3 & MASK4BITS) << 4) | (letter2 >> 2);
        }
        if(letter1 != -1){
            output[len+2] =  ((letter2 & MASK2BITS) << 6) | letter1;
        }
        
    }

    return output;
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

int syntax_error(char* msg) {
        printf("\033[1;31mSyntax error\033[0m: %s\n",msg);
        return -1;	
}

int main(int argc, char* argv[]){

    if(argc < 2)
        return syntax_error("./base64 -h for more help");

    char *(*fnc)(char *) = NULL;
    int fdIn = STDIN;
    int fdOut = STDOUT;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0){
            helpOptions();
            return 0;
        }
        if(strcmp(argv[i], "-e") == 0){
            if(fnc != NULL)
                return syntax_error("you can only select encode or decode");
            fnc = encode;
        }
        if(strcmp(argv[i], "-i") == 0){
            if(argc > i+1)
	        return syntax_error("the file name is not indicated");

            fdIn = open(argv[i+1], O_RDONLY);
            if(fdIn == -1)
                return syntax_error("the file could  not be opened");
        }
        if(strcmp(argv[i], "-d") == 0){
            if(fnc != NULL)
                return syntax_error("./base64 -h for more help");
            fnc = encode;
        }
        if(strcmp(argv[i], "-o") == 0){
            if(argc > i+1)
		return syntax_error("the file name is not indicated");
            
            fdOut = open(argv[i+1], O_CREAT | O_WRONLY);
            if(fdOut == -1)
		return syntax_error("the file could not be opened");
            }
    }

    return 0;
}
