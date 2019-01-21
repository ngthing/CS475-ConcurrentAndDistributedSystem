/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */

import java.rmi.*;
import java.util.*;

public interface Alarm extends Remote {

	void notifyEvent() throws RemoteException;
	
}
