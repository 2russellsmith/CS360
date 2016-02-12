#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

main(){
    int pid = fork();
    std::cout << pid << std::endl;
    if(pid == 0){
	std::cout << "About to exec" << std::endl;
        execl("/bin/ls", "/bin/ls",(char *)0);
    }else{
        int status;
        wait(&status);
	std::cout << "after wait" << std::endl;
    }
}
