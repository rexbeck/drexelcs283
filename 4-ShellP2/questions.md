1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  _If we only called execvp, our c program would close after execvp finished. Using fork, we let the thread close upon completing execvp._

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  _If fork fails, it returns -1. My implementation reports a ERR_EXEC_CMD error code._

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  _execvp uses the PATH environmental variable to replace the current process running with the process specified._

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  _Calling wait() in the parent process forces the parent to wait until the child process finishes. If we didn't wait, then the parent process could end before the child one finished._

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  _WEXITSTATUS returns the exit status of the child process. This is important because you need to know whether the child process failed or not._

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  _It handles it by using strtok to get the argument inside the quotes. This is needed because any command that takes a string as an argument uses quotes to define what is inside the string._

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  _I did not do the last assignment... sorry lol_

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  _Signals are used to reach a running process. They allow you to notify a running process for reasons like failing gracefully. This differs from other IPC because it reaches the process while it's already running._

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  _SIGKILL is used to force a process to end without any clean up methods running. A use case might be if a process is unresponsive. SIGTERM is used to have a process end gracefully. A use case would be when the process is no longer needed. SIGINT interrupts a signal. It's used when the user clicks control + c._

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  _SIGSTOP pauses a process and can later be restarted. But nothing in the process runs until it receives SIGCONT. It can't be caught or ignored so that the kernel can reliably pause the process._