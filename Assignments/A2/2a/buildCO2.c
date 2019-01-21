/* Build CO2 with lock and CV */
int wC = 0; // Number of waiting C to be formed
int wO = 0; // Number of waiting O to be formed
int aC = 0; // Number of assigned C to CO2 
int aO = 0; // Number of assigned O to CO2 

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitingC = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitingO = PTHREAD_COND_INITIALIZER;

CArrives() {
	pthread_mutex_lock(&lock);
	wC = wC+1;
	pthread_mutex_unlock(&lock);
	while(aC == 0) {
		if (wO >= 2 && wC >= 1) {
			pthread_mutex_lock(&lock);
			MakeCO2();
			wC = wC - 1; 
			aC = aC + 1;
			wO = wO - 2;
			aO = aO + 2; 
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&waitingO);
			pthread_cond_signal(&waitingC);
		}
		else {
			pthread_cond_wait(&waitingC,&lock);
		}
	}
	pthread_mutex_lock(&lock);
	aC = aC - 1;
	pthread_mutex_unlock(&lock);
}

OArrives() {
	pthread_mutex_lock(&lock);
	wO = wO+1;
	pthread_mutex_unlock(&lock);
	while(aO == 0) {
		if (wO >= 2 && wC >= 1) {
			pthread_mutex_lock(&lock);
			MakeCO2();
			wC = wC - 1; 
			aC = aC + 1;
			wO = wO - 2;
			aO = aO + 2; 
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&waitingO);
			pthread_cond_signal(&waitingC);
		}
		else {
			pthread_cond_wait(&waitingO,&lock);
		}
	}
	pthread_mutex_lock(&lock);
	aO = aO - 1;
	pthread_mutex_unlock(&lock);
}