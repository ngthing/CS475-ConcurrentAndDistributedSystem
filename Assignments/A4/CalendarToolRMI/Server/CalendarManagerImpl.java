/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */

import java.rmi.*;
import java.util.*;
import java.rmi.server.UnicastRemoteObject;


public class CalendarManagerImpl extends UnicastRemoteObject 
	implements CalendarManager {
	ArrayList<CalendarImpl> allCalendarsInServer = new ArrayList<CalendarImpl>();
	
	
	public CalendarManagerImpl() throws RemoteException { };
	
	// Testing method... might be removed later ! but it is helpful for now! 
	public String EchoMessage(String Msg) throws RemoteException
    {
    	String capitalizedMsg;

            System.out.println("Server: EchoMessage() invoked...");
            System.out.println("Server: Message > " + Msg);
    	    capitalizedMsg = Msg.toUpperCase();
            return(capitalizedMsg);
    }

	/*
	 * GetCalendarRef checks if clientName is in calendarUsersList
	 * If yes, it returns a reference to the client's Calendar object
	 * Otherwise, it create a Calendar for the client and return it. 
	 */
	public Calendar GetCalendarRef(String clientName) throws RemoteException {
		System.out.println("Server: GetCalendarRef() invoked...");

		// If Calendar for client already existed
		if (this.allCalendarsInServer.size() != 0 ) {
			if (allCalendarUsers().contains(clientName)) {
				System.out.println("Client existed");
				for (CalendarImpl current: allCalendarsInServer){
					if (clientName.equals(current.GetName())) 
						return (Calendar)current;
				}
			}
		}


		// If Calendar for this Client is not existed, create a new Calendar for this client
		System.out.println("Client not exist, create new");
		CalendarImpl c = new CalendarImpl(clientName);
		this.allCalendarsInServer.add(c);
		System.out.println("created new client: " + c.GetName());
		return (Calendar)c;
	}
	
	
	// Return a list of all Calendar Users 
	public ArrayList<String> allCalendarUsers () throws RemoteException {
		ArrayList<String> users = new ArrayList<String>();
		for (CalendarImpl c: allCalendarsInServer ) {
			users.add(c.GetName());
		}
		return users;
	}
}