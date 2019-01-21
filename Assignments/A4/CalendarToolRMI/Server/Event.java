/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */

import java.util.*;
import java.io.Serializable; 

public class Event implements Serializable {
    Date begin, end;
	String description;
	int accessControl; // 0: PRIVATE, 1: PUBLIC, 2: GROUP, 3: OPEN
	ArrayList<String> groupMembers = new ArrayList<String> (); // For GROUP event
	
	// constructors 
	public Event(String des, Date begin, Date end, int ac) {
		this.description = des;
		this.begin = begin;
		this.end = end;
		this.accessControl = ac;
	}
	public Event(String des, Date begin, Date end, int ac, ArrayList<String> groupMembers ) {
		this.description = des;
		this.begin = begin;
		this.end = end;
		this.accessControl = ac;
		this.groupMembers = groupMembers;
	}
	public Event(Event e) {
		this.description = e.description;
		this.begin = e.begin;
		this.end = e.end;
		this.accessControl = e.accessControl;
		this.groupMembers = e.groupMembers;
	}
	// get methods 
	public String getDescription() {
		return this.description;
	}
	
	public Date getBeginTime() {
		return this.begin;
	}

	public Date getEndTime() {
		return this.end;
	}

	public ArrayList<String> getGroupMembers() {
		return this.groupMembers;
	}
	public void addGroupMember(String member) {
		this.groupMembers.add(member);
	}
	public int getAccessControl() {
		return this.accessControl;
	}

	// Set methods 
	public void setDescription(Event e) {
		this.description = e.description;
	}

	public void setDescription(String newDes) {
		this.description = newDes;
	}
	
	public void setBeginTime(Event e) {
		this.begin = e.begin;
	}

	public void setBeginTime(Date newBegin) {
		this.begin = newBegin;
	}

	public void setEndTime(Event e) {
		this.end = e.end;
	}

	public void setEndTime(Date newEnd) {
		this.end = newEnd;
	}

	public void setGroupMembers(Event e) {
		this.groupMembers = e.groupMembers;
	}
	public void setGroupMembers(ArrayList<String> newGroup) {
		this.groupMembers = newGroup;
	}
	
	public void setAccessControl(Event e) {
		this.accessControl = e.accessControl;
	}

	public void setEvent(Event e) {
		this.description = e.description;
		this.begin = e.begin;
		this.end = e.end;
		this.accessControl = e.accessControl;
		this.groupMembers = e.groupMembers;
	}

	// other helper methods
	public String getAccessControlString() {
		String result = "";
		switch(this.accessControl) {
		case 0:
			result = "PRIVATE";
			break;
		case 1:
			result = "PUBLIC";
			break;
		case 2:
			result = "GROUP";
			break;
		case 3:
			result = "OPEN";
			break;
		}	
		return result;	
	}
	@Override
    public String toString() {
    	String result = getAccessControlString() + " - " + getDescription() + ": " + getBeginTime() + " - " + getEndTime();
    	if (this.getGroupMembers().size() != 0 ) {
    		result += "\nMembers: " + this.getGroupMembers();
    	}
    	return result;
    }

    public boolean contains(Event e) {
    	return ((this.getBeginTime().equals(e.getBeginTime()) || this.getBeginTime().before(e.getBeginTime()))
    		&& (this.getEndTime().equals(e.getEndTime()) || this.getEndTime().after(e.getEndTime())));
    }
}
