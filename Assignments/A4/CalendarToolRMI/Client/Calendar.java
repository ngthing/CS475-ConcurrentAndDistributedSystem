/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */

import java.rmi.*;
import java.util.*;

public interface Calendar extends Remote {
	/* 
	 * Return the name of this Calendar
	 */
	String GetName() throws RemoteException;
	String GetEventList() throws RemoteException;
	/* 
	 * ScheduleEvent returns true if event can be scheduled in current Calendar
	 * Otherwise, returns false
	 */
	boolean ScheduleEvent(Event e) throws RemoteException;

	boolean ScheduleGroupEvent(ArrayList<Calendar> groupMemberCals, Event e ) throws RemoteException;
	/*
	 * RetrieveEvent returns a list of events in this Calendar that is visible to the requestedUser 
	 * in the time range [begin,end]
	 * If the requestedUser is the owner of this Calendar, she can view all events in the time range
	 * Otherwise, she can only view PUBLIC, OPEN, and shared GROUP events of this Calendar
	 */
	ArrayList<Event> RetrieveEvent (String requestedUser, Date begin, Date end) throws RemoteException;

	/* 
	 * DeleteEvent removes an event at index i in eventList
	 */
	void DeleteEvent(int i) throws RemoteException;

	void InsertThisGroupEvent(Event e) throws RemoteException; 
	boolean OkToScheduleGroupEvent(ArrayList<Calendar> groupMemberCals, Event e) throws RemoteException;
	boolean thereIsOpenEventForThisEvent(Event e) throws RemoteException;
}
