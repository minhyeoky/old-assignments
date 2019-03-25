package finalexam;

import java.io.Serializable;

public class Users implements Serializable{
	public String username;
	public String birth;
	public String id;
	public String pn; //전화번호
	public int used;
	public Users(String username,String birth,String id,String pn
			,int used){
		this.username=username;
		this.birth=birth;
		this.id=id;
		this.pn=pn;
		this.used=used;
	}
	public void setname(String a){
		this.username=a;
	}
	public void setbirth(String a){
		this.birth=a;
	}public void setid(String a){
		this.id=a;
	}public void setpn(String a){
		this.pn=a;
	}public void setused(int a){
		this.used=a;
	}
	
}
