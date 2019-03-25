package finalexam;

import java.io.Serializable;

public class Seats implements Serializable{
	public int number;
	public boolean clean;
	public boolean using;
	public int price;
	public String username;
	public Seats(int number,boolean clean,boolean using,int price,String username){
		this.number=number;
		this.clean=clean;
		this.using=using;
		this.price=price;
		this.username=username;
		
	}
	public void setnumber(int a){
		this.number=a;
	}public void setprice(int a){
		this.price=a;
	}public void setname(String a){
		this.username=a;
	}
}
