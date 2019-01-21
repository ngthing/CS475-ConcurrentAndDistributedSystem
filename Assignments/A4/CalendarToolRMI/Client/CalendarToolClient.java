/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */
import java.rmi.*;
import java.rmi.server.*;
import java.util.*;
import java.text.*;  


public class CalendarToolClient {
	public static void main(String argv[]) {
		 // Validate command line parameters
        if (argv.length < 1) {
	    
		    System.out.println("Usage: java CalendarToolClient \"Calendar Name\"");
		    System.exit(1);
		}
        String clientName = argv[0];
        
        // Get a remote reference to the CalendarManagerImpl class
        String strName = "rmi://localhost/CalendarService";
        System.out.println("Client: Looking up " + strName + "...");

		CalendarManager RemCalendarManager = null; 
		Calendar MyRemCalendar = null; // The remote Calendar Object of this user

		try {
			RemCalendarManager = (CalendarManager)Naming.lookup(strName);
		}
		catch (Exception e) {
			System.out.println("Client: Exception thrown looking up " + strName);
			System.exit(1);
		}
		
		// Send a message to the remote object
		try {
			String modifiedMsg = RemCalendarManager.EchoMessage(clientName);
			System.out.println("From Server, opening calendar for: "+ modifiedMsg);
		}
		catch(Exception e) {
			System.out.println("Client: Exception thrown calling EchoMessage().");
			e.printStackTrace();
			System.exit(1);
		}

		// Get the Remote Calendar object MyRemCalendar for this client from RemCalendarManager
		try {
			MyRemCalendar = (Calendar)RemCalendarManager.GetCalendarRef(clientName);
			System.out.println("Connect Calendar for " + MyRemCalendar.GetName());
		}
		catch(Exception e) {
			System.out.println("Client: Exception thrown calling GetCalendarRef().");
			e.printStackTrace();
			System.exit(1);
		}



		// Print menu and accept input from Client
		Scanner keyboard = new Scanner(System.in);
		SimpleDateFormat formatter = new SimpleDateFormat("MM/dd/yyyy HH:mm"); 
		int menuOption = 0;
		
		// testing 
		// Date begin1 = null, end1 = null;
		// try {
		// 	begin1 = formatter.parse("12/08/2017 13:00");
		// 	end1 = formatter.parse("12/08/2017 15:00");
		// } catch (ParseException e) {e.printStackTrace();} 
		// Event open = new Event("open", begin1, end1, 0); 
		// try {
		// 	begin1 = formatter.parse("12/08/2017 14:00");
		// 	end1 = formatter.parse("12/08/2017 14:50");
		// } catch (ParseException e) {e.printStackTrace();} 
		
		// Event e1 = new Event("e", begin1, end1, 0);
		// System.out.println(open.contains(e1));

		// System.exit(1);
		while(true){
			printMenu();
			menuOption = keyboard.nextInt();
			keyboard.nextLine(); //throw away the \n not consumed by nextInt()
			switch(menuOption) {
				case 0:
					System.out.println("\nGood bye!");
					System.exit(1);
					break;
				case 1:
					try {
						System.out.println(MyRemCalendar.GetEventList());
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling GetEventList().");
						e.printStackTrace();
						System.exit(1);
					}
					
					break;
				case 2: 
				// Scheduling [0] PRIVATE, [1] PUBLIC, [3] OPEN event in this client's calendar
					try {
						System.out.println("\nScheduling a new event in your calendar...");
						Event e = createEvent();
						System.out.println("Created:\n" + e);  
						if (!MyRemCalendar.ScheduleEvent(e)) {
							System.out.println("This event is conflict with your schedule");
						}
						System.out.println("All events in your calendar ");
						System.out.println(MyRemCalendar.GetEventList());
						
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling ScheduleEvent.");
						e.printStackTrace();
						System.exit(1);
					}
					break;
				case 3:
					System.out.println("Schedule a Group Event");
					Calendar otherGroupMemRemCalendar =null;
					ArrayList<Calendar> groupMemberCals = new ArrayList<Calendar>();
					ArrayList<String> groupMemberNames = new ArrayList<String>();
					System.out.println("Other Calendar Users:");
					groupMemberNames.add(MyRemCalendar.GetName());
					groupMemberCals.add(MyRemCalendar);
					try {
						System.out.println(RemCalendarManager.allCalendarUsers().toString());
						System.out.println("How many group member besides you?");
						int numMem = keyboard.nextInt();
						keyboard.nextLine(); //throw away the \n not consumed by nextInt()
						
						String otherUser ="";
						
						while (numMem != 0) {
							numMem--;
							System.out.println("Enter member:");
							otherUser = keyboard.nextLine();
							groupMemberNames.add(otherUser);
							otherGroupMemRemCalendar = (Calendar)RemCalendarManager.GetCalendarRef(otherUser);
							System.out.println("Connect Calendar for " + otherGroupMemRemCalendar.GetName());
							groupMemberCals.add(otherGroupMemRemCalendar);
						}
						System.out.println("Member for this event: " + groupMemberNames);
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling GetCalendarRef().");
						e.printStackTrace();
						System.exit(1);
					}

					try {
						Event e = createGroupEvent(groupMemberNames);
						System.out.println("Created:\n" + e);  
						if (MyRemCalendar.ScheduleGroupEvent(groupMemberCals,e)) {
							System.out.println("Scheduled this group event");
							System.out.println("All events in your calendar ");
							System.out.println(MyRemCalendar.GetEventList());
						}
						else {
							System.out.println("Cannot schedule this group event");
						}
						
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling ScheduleEvent.");
						e.printStackTrace();
						System.exit(1);
					}

					break;	
				case 4:
					try {
						System.out.println(MyRemCalendar.GetEventList());
						System.out.println("What event you like to delete?");
						int eventToRemove = keyboard.nextInt();
						keyboard.nextLine(); //throw away the \n not consumed by nextInt()
						MyRemCalendar.DeleteEvent(eventToRemove);
					}
					catch (Exception e) {
						System.out.println("Client: Exception thrown calling GetEventList().");
						e.printStackTrace();
						System.exit(1);
					}
					break;
				case 5:
					try {
						System.out.println("\nAll Calendar Users:" + RemCalendarManager.allCalendarUsers().toString());
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling allCalendarUsers().");
						e.printStackTrace();
						System.exit(1);
					}
					break;
				case 6:
					Calendar OtherRemCalendar =null;
					try {
						System.out.println(RemCalendarManager.allCalendarUsers().toString());
						System.out.println("What Calendar you like to view?");
						String otherUser = keyboard.nextLine();
						OtherRemCalendar = (Calendar)RemCalendarManager.GetCalendarRef(otherUser);
						System.out.println("Connect Calendar for " + OtherRemCalendar.GetName());
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling GetCalendarRef().");
						e.printStackTrace();
						System.exit(1);
					}

					// Asking for time range 
					Date begin = null;
					Date  end = null;
				    try {  
				    	while (true) {
				    		System.out.println("Please enter date in format MM/dd/yyyy HH:mm");
					    	System.out.println("Begin: "); 
					        begin = formatter.parse(keyboard.nextLine());
					        System.out.println("End: ");   
					        end = formatter.parse(keyboard.nextLine()); 
					        if (begin.after(end)) {
								System.out.println("You entered begin time after end time. Try again.");
							}
							else break;
				    	}	
				    } catch (ParseException e) {e.printStackTrace();}  

					try {
						System.out.println(OtherRemCalendar.RetrieveEvent(MyRemCalendar.GetName(), begin, end));
					}
					catch(Exception e) {
						System.out.println("Client: Exception thrown calling GetCalendarRef().");
						e.printStackTrace();
						System.exit(1);
					}
					break;
			}
		}
		
		
	}

	public static void printMenu() {
		System.out.println("\nWhat would you like to do?");
		System.out.println("[0] Exit");
		System.out.println("[1] View All Events in My Calendar");
		System.out.println("[2] Schedule an Event in My Calendar");
		System.out.println("[3] Schedule a Group Event"); // will ask for list of user, validate, then create remoteref  
		System.out.println("[4] Delete an Event"); 
		System.out.println("[5] List all Calendar Users"); 
		System.out.println("[6] View other Calendar"); 
		System.out.print("Please enter an option: ");

	}

	// Get Event info from user and create an Event. 
	public static Event createEvent() {
		SimpleDateFormat formatter = new SimpleDateFormat("MM/dd/yyyy HH:mm"); 
		Scanner keyboard = new Scanner(System.in);
		System.out.println("Event's Description:");
		String des = keyboard.nextLine();
		Date begin = null;
		Date  end = null;
	    try {  
	    	while (true) {
	    		System.out.println("Please enter date in format MM/dd/yyyy HH:mm");
		    	System.out.println("Event begin: "); 
		        begin = formatter.parse(keyboard.nextLine());
		        System.out.println("Event end: ");   
		        end = formatter.parse(keyboard.nextLine()); 
		        if (begin.after(end)) {
					System.out.println("Cannot create event with begin time after end time. Try again.");
				}
				else break;
	    	}	
	    } catch (ParseException e) {e.printStackTrace();}  
	    
		System.out.println("Events Access Control: [0] PRIVATE, [1] PUBLIC, [3] OPEN");
		int ac = keyboard.nextInt();
		keyboard.nextLine(); //throw away the \n not consumed by nextInt()
		Event event = new Event(des,begin,end,ac);
		return event;
	}

	public static Event createGroupEvent(ArrayList<String> groupMemberNames) {
		SimpleDateFormat formatter = new SimpleDateFormat("MM/dd/yyyy HH:mm"); 
		Scanner keyboard = new Scanner(System.in);
		System.out.println("Event's Description:");
		String des = keyboard.nextLine();
		Date begin = null;
		Date  end = null;
	    try {  
	    	while (true) {
	    		System.out.println("Please enter date in format MM/dd/yyyy HH:mm");
		    	System.out.println("Event begin: "); 
		        begin = formatter.parse(keyboard.nextLine());
		        System.out.println("Event end: ");   
		        end = formatter.parse(keyboard.nextLine()); 
		        if (begin.after(end)) {
					System.out.println("Cannot create event with begin time after end time. Try again.");
				}
				else break;
	    	}	
	    } catch (ParseException e) {e.printStackTrace();}  
	   
		Event event = new Event(des,begin,end,2,groupMemberNames);
		return event;
	}
}
