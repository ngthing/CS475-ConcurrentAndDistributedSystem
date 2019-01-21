/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */

import java.util.*;
import java.rmi.*;

public interface CalendarManager extends Remote {
	Calendar GetCalendarRef (String clientName) throws RemoteException;
	ArrayList<String> allCalendarUsers () throws RemoteException;
	// Just for testing, will be removed. 
	String EchoMessage(String strMsg) throws RemoteException;
	
}
