#include <stdio.h> //for I/O
#include <sys/types.h> //for fork()
#include <unistd.h> //for sleep()
#include <stdlib.h> //for rand()
int length_word=11;
char word[] = "Hello World";
void print_word(char word[], int index){
	if(index<length_word){
		int pid=fork();
		// Then we print for the Parent Process(pid>0)
		if(pid>0){
	    		printf("%c <PID: %d> <Letter No: %d>\n",word[index],pid,index); // we're printing letter followed by the PID of the process printing it
	    		sleep(rand()%4+1); //for random 1 to 4 sec wait between printing each letter
	    		print_word(word,index+1);
    		}
	}
	return;
}
int main(){ 
    print_word(word,0);
    return 0; 
}
