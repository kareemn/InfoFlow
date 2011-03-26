import java.io.*;
import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;

public class EmailParser {

	/**
	 * @param args
	 */
	
	HashMap<String, List<String> > theMap;
	
	public EmailParser(){
		theMap = new HashMap<String, List<String> >();
	}
	
	public static void main(String[] args) {
		
		if( args.length <= 0 ){
			System.out.println(" Usage: invoke <email file name>");
			return;
		}
		EmailParser theParser = new EmailParser();
		
		System.out.println("Parsing emails in directory: " + args[0]);
		theParser.parseDirectory(args[0]);
		theParser.printCommunications();
		System.out.println("done");
		

		
	}
	
	public void parseDirectory(String emailFolder){
		System.out.println("Parsing emails in directory: " + emailFolder);
		File dir = new File(emailFolder);
		String[] children = dir.list();
		
		if(children == null){

		   try {
			
			   FileInputStream fis = new FileInputStream(emailFolder);
			   addToHashMap(fis);
	        
		   } catch (FileNotFoundException e) {

		      e.printStackTrace();
		   }
		}
		
		else {
			for (int i = 0; i < children.length; i++){
				parseDirectory(emailFolder + "/" + children[i]);
			}
		}
	}
	
	public void addToHashMap(InputStream in){
		try {

			Session s = Session.getDefaultInstance(new Properties());
			MimeMessage message = new MimeMessage(s, in);
			
			Address[] fromAddr = message.getFrom();
			Address[] toAddr = message.getAllRecipients();
			if(toAddr == null)
				return;
			
			
			for(int i = 0; i < toAddr.length; i++){
			   List<String> values = theMap.get(fromAddr[0].toString());
			   if(values == null){
				   values = new ArrayList<String>();
				   values.add(toAddr[i].toString());
			       theMap.put(fromAddr[0].toString(), values);
			   }
			   
			   else {
				   values.add(toAddr[i].toString());
				   theMap.put(fromAddr[0].toString(), values);
			   }
			}

		
			
			
		} catch (MessagingException e) {
			
			e.printStackTrace();
			
		}
		
		
	}
	
	public void printCommunications(){
		String formattedMap = theMap.toString().replaceAll("],", "],\n");
		//System.out.print(formattedMap);
		
	      try{
	    	    // Create file 
	         FileWriter fstream = new FileWriter("EnronEmailParse-output");
	         BufferedWriter out = new BufferedWriter(fstream);
	    	 out.write(formattedMap);
	    	 out.close();
	      } catch (Exception e){//Catch exception if any
	         System.err.println("Error: " + e.getMessage());
	      }
	}
	

}
