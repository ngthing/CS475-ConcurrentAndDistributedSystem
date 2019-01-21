import java.util.*;
import java.rmi.*;
import java.rmi.server.UnicastRemoteObject;

public class CalendarImpl extends UnicastRemoteObject 
	implements Calendar {
	String name; // Name of this Calendar's client
	ArrayList<Event> eventList = new ArrayList<Event>(); // List of all events in this calendar in ascending order.  
	

	public CalendarImpl() throws RemoteException { };

	public CalendarImpl(String name) throws RemoteException { 
		super();
		this.name = name;
	}

	public String GetName() throws RemoteException {
		return this.name;
	}
	public String GetEventList() throws RemoteException {
		return this.toString();
	}

	public synchronized boolean ScheduleEvent(Event e) throws RemoteException {
		if (OkToSchedule(e))
		{
			this.addEventToList(e);
			return true;
		}
		else {
			return false;
		}
	}

	/*
		addEventToList adds an event to eventList in ascending order.
	*/
	public synchronized void addEventToList(Event e ) {
		if (this.eventList.size() != 0) {
			for (int i = 0; i< this.eventList.size() ; i++ ) {
				if ((eventList.get(i)).getBeginTime().after(e.getBeginTime())) {
					eventList.add(i, e);
					return;
				}
			}
		}
		// If event is the first or last		
		this.eventList.add(e);


	}

	/*
		OkToSchedule returns true if this event can be scheduled in this calendar, e.i. no conflict with any other event
		else return false. 
		The algorithm to check if OfToSchedule:
		Let B and E are the Begin and End time of the event we want to schedule to calendar
		and B1 and E1 are the Begin and End time of any event that exist in calendar
		Event e is not ok to schedule to Calendar when:
		B1 < B < E < E1 or B1 < B < E1 < E => B1 < B < E
		or B < B1 < E < E1 or B < B1 < E1 < E 
	*/
	public synchronized boolean OkToSchedule(Event e) {
		if (!eventList.isEmpty()) { 
			for (Event eventInCalendar : eventList ) {
				if (
					// B1 = B
					e.getBeginTime().equals(eventInCalendar.getBeginTime()) ||
					// B1 < B < E 
					(e.getBeginTime().after(eventInCalendar.getBeginTime()) 
						&& e.getBeginTime().before(eventInCalendar.getEndTime()))
					// B < B1 < E < E1
					|| (e.getBeginTime().before(eventInCalendar.getBeginTime())
						&& eventInCalendar.getBeginTime().before(e.getEndTime())) 
					// B < B1 < E1 < E 
						|| (e.getBeginTime().before(eventInCalendar.getBeginTime())
							&& e.getEndTime().after(eventInCalendar.getEndTime()))
						)
					return false; 
			}
		}
		return true;
	}
	/*
	 * RetrieveEvent returns a list of events in this Calendar that is visible to the requestedUser 
	 * in the time range [begin,end]
	 * If the requestedUser is the owner of this Calendar, he/she can view all events in the time range
	 * Otherwise, he/she can only view PUBLIC, OPEN, and shared GROUP events of this Calendar
	 */
	@Override
	public synchronized ArrayList<Event> RetrieveEvent(String requestedUser, Date begin, Date end) throws RemoteException {
		ArrayList<Event> resultList = new ArrayList<Event>();
		if (requestedUser.equals(this.GetName())) {
			return this.eventList;
		}
		// If event is 1: PUBLIC, 2: GROUP, 3: OPEN
		for (Event eventInCalendar : eventList ) {
			if (eventInCalendar.getAccessControl() == 1 || 
				eventInCalendar.getAccessControl() == 3 ||
				(eventInCalendar.getAccessControl() == 2 
				&& eventInCalendar.getGroupMembers().contains(requestedUser))) {
				resultList.add(eventInCalendar);
			} 
		}
		return resultList;
	}

	@Override
	public synchronized void DeleteEvent(int index) throws RemoteException {
		int eventToDeleteIndex = index -1;
		this.eventList.remove(eventToDeleteIndex);
	}
	
	@Override
    public String toString() {
    	// String result="Events in " + this.GetName() + " Calendar:\n ";
    	String result="";
    	for (int i=0; i <eventList.size(); i++ ) {
    		result += "[" + (i + 1) +"] " + eventList.get(i).toString() + "\n" ;
    	}
    	return result;
    }

    public synchronized boolean ScheduleGroupEvent(ArrayList<Calendar> groupMemberCals, Event e ) throws RemoteException {
    	System.out.println("in ScheduleGroupEvent(members, e)");
    	if (OkToScheduleGroupEvent(groupMemberCals,e)) {
    		for (Calendar memberCal : groupMemberCals ) {
    			memberCal.InsertThisGroupEvent(e);
    		}
    		return true;
    	}
    	else { 
    		System.out.println("Cannot schedule this group event");
    		return false;
    	}
    }

    public void InsertThisGroupEvent(Event e) throws RemoteException {
    	System.out.println("InsertThisGroupEvent for " + this.GetName());
    	// Find the appropriate open event for this group event
    	for (Event openEvent: this.eventList) {
    		if (openEvent.getAccessControl() == 3 && openEvent.contains(e)) 
    			{
    				// If the groupEvent interval is the same as the interval of openEvent
    				// set the openEvent to the groupEvent
    				if ((openEvent.getBeginTime().equals(e.getBeginTime())) && 
    					(openEvent.getEndTime().equals(e.getEndTime()))) 
    					openEvent.setEvent(e);
    				// If open begin < group begin && group end < open end
    				// Get the index of old open event = oldOpenIndex 
    				// Create a new open with new open begin = group end , new open end = old open end
    				// Then set the old open end = group begin 
    				// insert group at oldOpenIndex + 1, insert new open at oldOpenIndex +2 
    				else if ((openEvent.getBeginTime().before(e.getBeginTime())) &&
    					(openEvent.getEndTime().after(e.getEndTime()))) {
    					int oldOpenIndex = this.eventList.indexOf(openEvent);
    					Event newOpen = new Event("Open", e.getEndTime(), openEvent.getEndTime(),3);
    					openEvent.setEndTime(e.getBeginTime());
    					this.eventList.add(oldOpenIndex + 1, e);
    					this.eventList.add(oldOpenIndex + 2, newOpen);
    				}

    				// If open begin = group begin && group end < open end 
    				// Get the index of old open event = oldOpenIndex 
    				// set old open begin = group end
    				// insert group to oldOpenIndex
    				
    				else if ((openEvent.getBeginTime().equals(e.getBeginTime())) &&
    					(openEvent.getEndTime().after(e.getEndTime()))) {
    					int oldOpenIndex = this.eventList.indexOf(openEvent);
    					openEvent.setBeginTime(e.getEndTime());
    					this.eventList.add(oldOpenIndex, e);
    				}

    				// If open begin < group begin && open end == group end
    				// Get the index of old open event = oldOpenIndex 
    				// set open end = group begin 
    				// add group to oldOpenIndex + 1
    				else if ( (openEvent.getBeginTime().before(e.getBeginTime())) &&
    							(openEvent.getEndTime().equals(e.getEndTime()))) {
    					int oldOpenIndex = this.eventList.indexOf(openEvent);
    					openEvent.setEndTime(e.getBeginTime());
    					this.eventList.add(oldOpenIndex + 1, e);
    				}
    				break;
    			}
    	}
    }

    public boolean OkToScheduleGroupEvent(ArrayList<Calendar> groupMemberCals, Event e ) throws RemoteException {
    	System.out.println("in OkToScheduleGroupEvent");
    	for (Calendar memberCal : groupMemberCals) {
    		System.out.println("Checking " + memberCal.GetName());
    		System.out.println("result " + memberCal.thereIsOpenEventForThisEvent(e));
    		// if (!memberCal.thereIsOpenEventForThisEvent(e)) { return false;}
    	}
    	return true;
    }

    public boolean thereIsOpenEventForThisEvent(Event e) throws RemoteException {
    	System.out.println("in thereIsOpenEventForThisEvent");
    	for (Event openEvent: this.eventList) {
    		if (openEvent.getAccessControl() == 3 && openEvent.contains(e)) 
    			{return true;}
    	}
    	return false;
    }

}
