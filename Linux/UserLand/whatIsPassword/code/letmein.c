#include <stdio.h>
#include <string.h>
#include "secret.h"

void flag(){
	printf("Here is your flag: Y0u 4r3 4w3$0m3 \n");
}

void authenticate(char* user_input){

	char buf[16];
	const char* password = get_password();
	strcpy(buf, user_input);
	if (strcmp(buf, password) == 0)
		flag(); // equal
	else
		printf("Incorrect Passowrd. Try again... \n"); // unequal
}


int main(int argc, char* argv[]){

	if (argc != 2) {
		printf("Usage: %s <password>\n", argv[0]);
		return 1;
	}
	
	authenticate(argv[1]);

	return 0;
}
