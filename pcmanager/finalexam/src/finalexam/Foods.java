package finalexam;

import java.io.Serializable;

public class Foods implements Serializable{
	public String serial;
	public String fn; //À½½Ä¸í
	public int price;
	public int number; ///°¹¼ö
	public Foods(String serial,String a,int b,int c){
		this.fn=a;
		this.price=b;
		this.number=c;
		this.serial=serial;
	}
	public void setserial(String a){
		this.serial=a;
	}
	public void setsprice(int a){
		this.price=a;
	}public void setnumber(int a){
		this.number=a;
	}public void setfn(String a){
		this.fn=a;
	}




}
