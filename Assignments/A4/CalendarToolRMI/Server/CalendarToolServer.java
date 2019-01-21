/***
 * CS475-Fall2017 - Dr. Setia @ GMU
 * A Calendar Tool for Workgroups using RMI 
 * Author: Thi Nguyen 
 */
import java.rmi.*;


public class CalendarToolServer {            
    public static void main(String argv[])
    {
        try {
        
			System.setSecurityManager(new SecurityManager());


			System.out.println("Server: Registering Calendar Service");
			CalendarManagerImpl remote = new CalendarManagerImpl();
			Naming.rebind("CalendarService", remote);

			//	   Replace with line below if rmiregistry is using port 2934	
			//	   Naming.rebind("rmi://localhost:2934/EchoService", remote);

			System.out.println("Server: Ready...");
        }
        catch (Exception e)
	    {
			System.out.println("Server: Failed to register Calendar Service: " + e);
	    }
    }
}