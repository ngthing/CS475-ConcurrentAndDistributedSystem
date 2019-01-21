void handler (int sig) 
{
	int olderrno = errno;
	sigset_t mask_all, prev_all;
	pid_t pid;

	Sigfillset(&mask_all);
	while ((pid = waitpid(-1, NULL, 0)) > 0 ) { // Real a zombie child 
		Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
		deletejob(pid); // delete the child from the job list
		Sigprocmask(SIG_SETMASK, &prev_all, NULL);
	}
	if (errno != ECHILD)
		printf("waitpid error\n");
	errno = olderrno;
}

int main(int argc, char **argv)
{
	int pid; 
	sigset_t mask_all, mask_one, prev_one;

	Sigfillset(&mask_all);
	Sigemptyset(&mask_one);
	Sigaddset(&mask_one, SIGCHLD);
	Signal(SIGCHLD, handler);
	initjobs(); // initialize the job list

	while(1) {
		Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); // block sigchld
		if ((pid = fork()) == 0) { // child process
			Sigprocmask(SIG_SETMASK, &prev_one, NULL);
			Execve("program...");
		}
		Sigprocmask(SIG_BLOCK, &mask_all, NULL); // parent process
		addjob(pid);
		Sigprocmask(SIG_SETMASK, &prev_one, NULL); // unblock SIGCHLD
	}
	exit(0);
}