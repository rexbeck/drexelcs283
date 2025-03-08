1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_The for loop that handles child processes won't continue until the waitpid() is finished. Forgetting to call waitpid() would result in out of sync command calls. The results for the previous call would appear after the prompt for the new command. _

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_Forgetting to close them can result in descriptor leaks. This would leave the program without avaiable file descriptors. It could also cause the program to hang._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_We have to run external commands in a child process. If we were to run cd in a child process, only the child process's directory would change, leaving the parent process as is._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_Instead of having a fixed size array, we would need to allocate memory for every execution and then free the memory after the execution. The trade off would be that it would require an even more complex implementation that could result in fragmentation of commands._