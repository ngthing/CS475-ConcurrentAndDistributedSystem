void ArriveBridge(int vid, int direc)
{
	pthread_mutex_lock(&lock);
	fprintf(stderr, "vid=%d dir=%d arrives at the bridge. Bridge num_car=%d curr_dir=%d\n", 
		vid, direc, br.num_car, br.curr_dir);
	/* While cannot get on the  bridge, wait */
	while ((br.num_car == 5) || (br.num_car > 0 && br.curr_dir != direc)) {
		waiters[direc]++;
		if (direc == 0) { pthread_cond_wait(&waitingToGoEast,&lock); }
		else { pthread_cond_wait(&waitingToGoWest,&lock); }
		/* Once the car is woke up */ 
		waiters[direc]--;
	}
	/* Car get on the bridge */
	br.num_car++;
	br.curr_dir = direc;
	pthread_mutex_unlock(&lock);
	return;
}


void CrossBridge(int vid, int direc, int time_to_cross)
{
	pthread_mutex_lock(&lock);
	fprintf(stderr, "vid=%d dir=%d starts crossing. Bridge num_car=%d curr_dir=%d\n", 
		vid, direc, br.num_car, br.curr_dir);
	pthread_mutex_unlock(&lock);
	sleep(time_to_cross);
	return;
}

void ExitBridge(int vid, int direc)
{
	pthread_mutex_lock(&lock);
	/* car gets off the bridge */
	br.num_car--;
	br.dept_idx++;
	/* If anybody wants to go on the same direction of the bridge curr_dir, wake them up */
	if (waiters[br.curr_dir] > 0) {
		if (br.curr_dir == 0) { pthread_cond_signal(&waitingToGoEast); }
		else { pthread_cond_signal(&waitingToGoWest); }
	}
	/* Else if no other car is waiting to pass in the curr_dir and there is no car on the bridge 
		wake up the cars waiting in the other side
	*/
	else if (br.num_car == 0) {
		if (br.curr_dir == 0) { pthread_cond_broadcast(&waitingToGoWest); }
		else { pthread_cond_broadcast(&waitingToGoEast); }
	}

	fprintf(stderr, "vid=%d dir=%d exit with departure idx=%d\n", 
		vid, direc, br.dept_idx);
	pthread_mutex_unlock(&lock);
	return;
}