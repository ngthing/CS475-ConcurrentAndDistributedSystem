/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline)
{   
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)        
        return; /* Ignore empty lines */
    
    if (!builtin_cmd(argv)) { 
        if ((pid = fork()) == 0) { /* Child runs user job */
                setpgid(0,0); /* Put the child in a new process group whose group ID is identical to the child's PID */
                if (execve(argv[0], argv, environ) < 0) {
                        printf("%s: Command not found. \n", argv[0]);
                        exit(0);
                }
        }
        
        /* Parent waits for foreground job to terminate */
        if (!bg) {
                //addjob(jobs, pid, FG, cmdline);
                //waitfg(pid);
                int status;
                if (waitpid(pid, &status, 0) < 0)
                        unix_error("waitfg: waitpid error");
        }
        else {  
                addjob(jobs, pid, BG, cmdline);
                printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
        }
    }
    return;
}