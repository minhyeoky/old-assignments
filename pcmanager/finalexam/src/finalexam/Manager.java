package finalexam;

import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.GridLayout;
import java.awt.Panel;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.table.DefaultTableModel;

public class Manager extends JFrame implements ActionListener {

	public static void main(String[] args) {
		Manager manager = new Manager();

	}

	ArrayList<Seats> als = new ArrayList<Seats>();
	ArrayList<Users> al = new ArrayList<Users>();
	ArrayList<Foods> alf = new ArrayList<Foods>();
	ArrayList<Seats> als1 = new ArrayList<Seats>();
	ArrayList<Users> al1 = new ArrayList<Users>();
	ArrayList<Foods> alf1 = new ArrayList<Foods>();

	//
	// public TextField = null;
	public JTable seatstable = null;
	public JTable userstable = null;
	public JTable foodstable = null;
	public TextField seatssearch, add, foodsearch, usersearch;
	public TextField bfn = new TextField();
	public JLabel bfl;
	private JButton list;
	private JButton buy;
	private JButton on;
	private JButton modifyseat;
	private JButton removeseats;
	private JButton addseats;
	private JButton adduser, removeuser, modifyuser, addfood, removefood,
			modifyfood, bfb;
	public Object[] seat = new Object[5];
	public Object[] user = new Object[5];
	public Object[] food = new Object[4];
	private CardLayout cardLayout = null;
	String[] labels = { "Number", "Clean", "Using", "Price", "UserName" };
	String[] userlabels = { "UserName", "birth", "id", "PhoneNumber", "used" };
	String[] foodlabels = { "Serial", "FoodName", "Price", "Number" };
	DefaultTableModel model = new DefaultTableModel(labels, 0);
	DefaultTableModel usermodel = new DefaultTableModel(userlabels, 0);
	DefaultTableModel foodmodel = new DefaultTableModel(foodlabels, 0);
	JMenuBar mb = new JMenuBar();
	public Panel seats, foods, users;
	JComboBox buyfood;
	String[] foodsname;
	int buynumber;
	int buyhow;
	int nrow;
	int totalprice;

	Manager() {
		super("PCManager");
		menubar();
		cardLayout = new CardLayout();
		setLayout(cardLayout);
		JPanel seats = new JPanel();
		// /////////////////////////////////////////buyfood panel
		// Panel bf = new Panel();
		// bf.setSize(300, 200);
		// bf.setLayout(new GridLayout(1, 4));
		// foodsname = new String[alf.size()];
		// for (int i = 0; i < alf.size(); i++) {
		// foodsname[i] = alf.get(i).fn + ":" + alf.get(i).price;
		// }
		// buyfood=new JComboBox(foodsname);
		// bfl=new JLabel("갯수");
		// bfn = new TextField();
		// bfb.addActionListener(this);
		// buyfood.addActionListener(this);
		// bf.add(buyfood);
		// bf.add(bfl);
		// bf.add(bfn);
		// bf.add(bfb);
		//
		// add("buyfood",bf);
		// ////////////////userssssssssssssssssssssssssssssssss
		JPanel users = new JPanel();
		users.setLayout(new BorderLayout());
		userstable = new JTable(usermodel);
		JScrollPane usersp = new JScrollPane(userstable);
		usersearch = new TextField();

		usersearch.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				String userc = usersearch.getText();
				usersearch.setText("");
				// TODO Auto-generated method stub

				if (userc.equals("")) {
					resetu();
					return;
				}
				for (int j = usermodel.getRowCount(); j > 0; j--) {
					usermodel.removeRow(0);
				}
				for (int i = 0; i < al.size(); i++) {
					System.out.println(userc);
					if (al.get(i).username.contains(userc)
							|| al.get(i).birth.contains(userc)
							|| al.get(i).id.contains(userc)
							|| al.get(i).pn.contains(userc)) {

						System.out.println(userc);

						user[0] = al.get(i).username;
						user[1] = al.get(i).birth;
						user[2] = al.get(i).id;
						user[3] = al.get(i).pn;
						user[4] = al.get(i).used;
						usermodel.addRow(user);
					}
				}
			}

		});
		Panel userbtn = new Panel();
		userbtn.setLayout(new GridLayout(3, 1));
		adduser = new JButton("adduser");
		removeuser = new JButton("removeuser");
		modifyuser = new JButton("modifyuser");
		userbtn.add(adduser);
		userbtn.add(removeuser);
		userbtn.add(modifyuser);
		users.add(usersp, "Center");
		users.add(usersearch, "South");
		users.add(userbtn, "East");
		add("users", users);
		// /////////////////FFFFFFFOOOOOOOOOOOD
		JPanel foods = new JPanel();
		foods.setLayout(new BorderLayout());
		foodstable = new JTable(foodmodel);
		JScrollPane foodsp = new JScrollPane(foodstable);
		foodsearch = new TextField();
		foodsearch.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				// TODO Auto-generated method stub
				String foodc = foodsearch.getText();
				foodsearch.setText("");
				for (int j = foodmodel.getRowCount(); j > 0; j--) {
					foodmodel.removeRow(0);
				}
				if (foodc.equals("")) {
					resetf();
					return;
				}
				for (int j = foodmodel.getRowCount(); j > 0; j--) {
					foodmodel.removeRow(0);
				}
				for (int i = 0; i < alf.size(); i++) {
					if (alf.get(i).serial.contains(foodc)
							|| alf.get(i).fn.contains(foodc)) {

						food[0] = alf.get(i).serial;
						food[1] = alf.get(i).fn;
						food[2] = alf.get(i).price;
						food[3] = alf.get(i).number;
						foodmodel.addRow(food);
					}
				}
			}

		});
		Panel foodbtn = new Panel();
		foodbtn.setLayout(new GridLayout(3, 1));
		addfood = new JButton("addfood");
		removefood = new JButton("removefood");
		modifyfood = new JButton("modifyfood");
		foodbtn.add(addfood);
		foodbtn.add(removefood);
		foodbtn.add(modifyfood);
		foods.add(foodsp, "Center");
		foods.add(foodsearch, "South");
		foods.add(foodbtn, "East");
		add("foods", foods);
		// ///////////////////
		seats.setLayout(new BorderLayout());
		for (int i = 0; i < als.size(); i++) {
			for (int j = 0; j < als.size(); j++) {
				if (i == als.get(j).number) {
					seat[0] = als.get(j).number;
					seat[1] = als.get(j).clean;
					seat[2] = als.get(j).using;
					seat[3] = als.get(j).price;
					seat[4] = als.get(j).username;
					model.addRow(seat);
				}
			}
		}
		seatstable = new JTable(model);

		JScrollPane seatsp = new JScrollPane(seatstable);

		Panel seatsbtn = new Panel();
		seatsbtn.setLayout(new GridLayout(6, 1));
		addseats = new JButton("add");
		removeseats = new JButton("remove");
		removeseats.addActionListener(this);
		modifyseat = new JButton("modify");
		modifyseat.addActionListener(this);
		on = new JButton("on");
		buy = new JButton("buy");
		list = new JButton("list");
		addseats.addActionListener(this);
		seatsbtn.add(addseats);
		seatsbtn.add(removeseats);
		seatsbtn.add(modifyseat);
		seatsbtn.add(on);
		seatsbtn.add(buy);
		seatsbtn.add(list);
		seatssearch = new TextField();
		seats.add(seatsp, "Center");
		seats.add(seatsbtn, "East");
		add("seats", seats);
		adduser.addActionListener(this);
		addfood.addActionListener(this);
		removeuser.addActionListener(this);
		removefood.addActionListener(this);
		modifyuser.addActionListener(this);
		modifyfood.addActionListener(this);
		on.addActionListener(this);
		buy.addActionListener(this);
		list.addActionListener(this);

		cardLayout.show(getContentPane(), "seats");
		//

		// setJMenuBar(mb);
		setSize(600, 400);
		setVisible(true);

	}

	public void menubar() {
		JMenu menuFile = new JMenu("File");
		JMenuItem menuitemopen = new JMenuItem("OPEN");
		JMenuItem menuitemsave = new JMenuItem("SAVE");
		JMenuItem menuitemexit = new JMenuItem("EXIT");
		menuFile.add(menuitemopen);
		menuFile.add(menuitemsave);
		menuFile.add(menuitemexit);
		JMenu menuLook = new JMenu("Look");
		JMenuItem menuitemseats = new JMenuItem("SEATS");
		JMenuItem menuitemusers = new JMenuItem("USERS");
		JMenuItem menuitemfoods = new JMenuItem("FOODS");
		menuLook.add(menuitemseats);
		menuLook.add(menuitemusers);
		menuLook.add(menuitemfoods);
		mb.add(menuFile);
		mb.add(menuLook);
		menuitemopen.addActionListener(this);
		menuitemsave.addActionListener(this);
		menuitemexit.addActionListener(this);
		menuitemseats.addActionListener(this);
		menuitemusers.addActionListener(this);
		menuitemfoods.addActionListener(this);
		setJMenuBar(mb);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		// TODO Auto-generated method stub
		String cmd = e.getActionCommand();
		//
		if (cmd.equals("OPEN")) {
			input();
			System.out.println("OPENED");
		}
		if (cmd.equals("SAVE")) {
			output();
			System.out.println("OPENED");
		}
		if (cmd.equals("EXIT")) {
			exit();
			System.out.println("OPENED");
		}
		if (cmd.equals("SEATS")) {
			seeseats();
			System.out.println("OPENED");
		}
		if (cmd.equals("USERS")) {
			seeusers();
			System.out.println("OPENED");
		}
		if (cmd.equals("FOODS")) {
			seefoods();
			System.out.println("OPENED");
		}

		if (e.getSource() == addseats) {
			addseat();
		}
		if (e.getSource() == removeseats) {
			removeseat();
		}
		if (e.getSource() == adduser) {
			adduser();
		}
		if (e.getSource() == removeuser) {
			removeuser();
		}
		if (e.getSource() == modifyuser) {
			modifyuser();
		}
		if (e.getSource() == addfood) {
			addfood();
		}
		if (e.getSource() == removefood) {
			removefood();
		}
		if (e.getSource() == modifyfood) {
			modifyfood();
		}
		if (e.getSource() == modifyseat) {
			System.out.println("dd");
			modifyseat();
		}
		if (e.getSource() == on) {
			System.out.println("dd");
			on();
		}
		if (e.getSource() == buy) {
			System.out.println("dd");
			buy();
		}
		if (e.getSource() == list) {
			System.out.println("dd");
			list();
		}
		if (e.getSource() == bfb) {
			System.out.println("dd");
			buynumber = buyfood.getSelectedIndex();
			buyhow = Integer.parseInt(bfn.getText());
			setSize(600, 400);
			System.out.println(buynumber + buyhow);
			seeseats();
			if (alf.get(buynumber).number - buyhow < 0) {
				JOptionPane.showMessageDialog(null, "수량이부족합니다.", "Message",
						JOptionPane.ERROR_MESSAGE);
			} else {
				alf.get(buynumber).number -= buyhow;
				for (int i = 0; i < als.size(); i++) {
					if (als.get(i).number == nrow) {
						als.get(i).price += buyhow * alf.get(buynumber).price;
						for (int j = 0; j < al.size(); j++) {
							if (als.get(i).username.equals(al.get(j).id)) {
								al.get(j).used += buyhow
										* alf.get(buynumber).price;
								break;
							}
						}
						break;
					}
				}
				totalprice += buyhow * alf.get(buynumber).price;
				reset();
			}

		}
	}

	public void reset() {
		for (int i = model.getRowCount(); i > 0; i--) {
			model.removeRow(0);
		}
		int k = 0;
		for (int i = 0; i < als.size(); i++) {
			if (k < als.get(i).number) {
				k = als.get(i).number;
			}
		}
		for (int i = 0; i <= k; i++) {
			for (int j = 0; j < als.size(); j++) {
				if (i == als.get(j).number) {
					seat[0] = als.get(j).number;
					seat[1] = als.get(j).clean;
					seat[2] = als.get(j).using;
					seat[3] = als.get(j).price;
					seat[4] = als.get(j).username;
					model.addRow(seat);
				}
			}
		}
	}

	public void resetf() {
		for (int i = foodmodel.getRowCount(); i > 0; i--) {
			foodmodel.removeRow(0);
		}

		for (int j = 0; j < alf.size(); j++) {

			food[0] = alf.get(j).serial;
			food[1] = alf.get(j).fn;
			food[2] = alf.get(j).price;
			food[3] = alf.get(j).number;
			foodmodel.addRow(food);

		}

	}

	public void resetu() {

		for (int i = usermodel.getRowCount(); i > 0; i--) {
			usermodel.removeRow(0);
		}
		for (int j = 0; j < al.size(); j++) {
			user[0] = al.get(j).username;
			user[1] = al.get(j).birth;
			user[2] = al.get(j).id;
			user[3] = al.get(j).pn;
			user[4] = al.get(j).used;
			usermodel.addRow(user);
		}

	}

	public void addseat() {
		int number = 0;
		for (int i = 0; i < als.size(); i++) {
			for (int j = 0; j < als.size(); j++) {
				if (number == als.get(j).number) {
					number++;

					break;
				}
			}
		}
		boolean clean = false;
		boolean using = false;
		int price = 0;
		String username = null;
		Seats s = new Seats(number, clean, using, price, username);
		als.add(s);
		reset();
	}

	//
	public void adduser() {
		String username = JOptionPane.showInputDialog("이름을 입력하세요.");
		if (username.equals("")) {
			System.out.println("dd");
			return;
		}
		String birth = JOptionPane.showInputDialog("생년월일을 입력하세요.");
		if (birth.equals("")) {
			return;
		}
		String id = JOptionPane.showInputDialog("ID을 입력하세요.");
		if (id.equals("")) {
			return;
		}
		if (isID(id)) {
			return;
		}
		String pn = JOptionPane.showInputDialog("핸드폰 번호을 입력하세요.");
		if (pn.equals("")) {
			return;
		}
		int used = 0;
		Users users = new Users(username, birth, id, pn, used);
		al.add(users);
		resetu();

	}

	public boolean isID(String newID) {
		for (int i = 0; i < al.size(); i++) {
			if (newID.equals(al.get(i).id)) {
				JOptionPane.showMessageDialog(null, "이미존재하는 ID입니다!", "Message",
						JOptionPane.ERROR_MESSAGE);
				return true;
			}
		}

		return false;
	}

	//
	public void addfood() {
		System.out.println("d");
		String serial = JOptionPane.showInputDialog("일련번호를 입력하세요.");
		if (isFood(serial)) {
			return;
		}
		System.out.println("d");
		String fn = JOptionPane.showInputDialog("이름을 입력하세요.");
		System.out.println("d");
		int price = Integer.parseInt(JOptionPane.showInputDialog("가격을 입력하세요."));
		System.out.println("d");
		int number = Integer
				.parseInt(JOptionPane.showInputDialog("갯수를 입력하세요."));
		System.out.println("d");
		Foods foods = new Foods(serial, fn, price, number);
		alf.add(foods);
		resetf();

	}

	public boolean isFood(String a) {
		for (int i = 0; i < alf.size(); i++) {
			if (a.equals(alf.get(i).serial)) {
				JOptionPane.showMessageDialog(null, "이미존재하는 일련번호입니다!",
						"Message", JOptionPane.ERROR_MESSAGE);
				return true;
			}
		}
		return false;
	}

	//
	public void removeseat() {
		int a = seatstable.getSelectedRow();
		int row = (int) model.getValueAt(a, 0);
		System.out.println(a);
		if (a < 0)
			return; // 선택이 안된 상태면 -1리턴
		for (int i = 0; i < als.size(); i++) {
			if (als.get(i).number == row) {
				als.remove(i);
				break;
			}
		}
		reset();

	}

	//
	public void removeuser() {
		int a = userstable.getSelectedRow();
		System.out.println(a);
		if (a < 0)
			return;
		int b = userstable.getSelectedColumn();
		String c = (String) userstable.getValueAt(a, 2);
		for (int i = 0; i < al.size(); i++) {
			if (al.get(i).id.equals(c)) {
				al.remove(i);
			}
		}
		resetu();
		System.out.println("rm");
	}

	//
	public void removefood() {
		int a = foodstable.getSelectedRow();
		if (a < 0)
			return;
		String c = (String) foodstable.getValueAt(a, 0);
		for (int i = 0; i < alf.size(); i++) {
			if (alf.get(i).serial.equals(c)) {
				alf.remove(i);
			}
		}
		resetf();
	}

	//
	public boolean isSeat(int a) {
		for (int i = 0; i < als.size(); i++) {
			if (als.get(i).number == a) {
				JOptionPane.showMessageDialog(null, "이미존재하는 좌석입니다!", "Message",
						JOptionPane.ERROR_MESSAGE);
				return true;
			}
		}
		return false;
	}

	public void modifyseat() {
		System.out.println("dd");
		int a = seatstable.getSelectedRow();
		if (a < 0)
			return;
		System.out.println("dd");
		int b = seatstable.getSelectedColumn();
		int row = (int) model.getValueAt(a, 0);
		if (b == 0) {
			System.out.println("dd");
			int newnumber = Integer.parseInt(JOptionPane
					.showInputDialog("새로운 번호를 입력하세요"));
			if (isSeat(newnumber)) {
				return;
			}
			for (int i = 0; i < als.size(); i++) {
				if (als.get(i).number == row) {
					als.get(i).setnumber(newnumber);
				}
			}
		}
		if (b == 1) {
			for (int i = 0; i < als.size(); i++) {
				if (als.get(i).number == row) {
					if (als.get(i).clean == true) {
						als.get(i).clean = false;
					} else {
						als.get(i).clean = true;
					}
				}
			}
		}
		if (b == 2) {
			for (int i = 0; i < als.size(); i++) {
				if (als.get(i).number == row) {
					if (als.get(i).using == true) {
						als.get(i).using = false;
						als.get(i).username = null;
					}
				}
			}
		}

		reset();
	}

	//
	public void modifyuser() {
		int a = userstable.getSelectedRow();
		if (a < 0)
			return;
		int b = userstable.getSelectedColumn();
		if (b == 0) {// name
			String name = JOptionPane.showInputDialog("새로운 이름를 입력하세요");
			al.get(a).setname(name);
		}
		if (b == 1) {// birth
			String name = JOptionPane.showInputDialog("새로운 생년월일를 입력하세요");
			al.get(a).setbirth(name);
		}
		if (b == 2) {// ID
			String name = JOptionPane.showInputDialog("새로운 ID를 입력하세요");
			if (isID(name)) {
				return;
			}
			al.get(a).setid(name);
		}
		if (b == 3) {// phoneNumber
			String name = JOptionPane.showInputDialog("새로운 핸드폰번호를 입력하세요");
			al.get(a).setpn(name);
		}
		if (b == 4) {// used
			int name = Integer.parseInt(JOptionPane
					.showInputDialog("새로운 사용액를 입력하세요"));
			al.get(a).setused(name);
		}
		resetu();
	}

	//
	public void modifyfood() {
		int a = foodstable.getSelectedRow();
		if (a < 0)
			return;
		int b = foodstable.getSelectedColumn();
		if (b == 0) {
			String serial = JOptionPane.showInputDialog("새로운 일련번호를 입력하세요.");
			if (isFood(serial)) {
				return;
			}
			alf.get(a).setserial(serial);
		}
		if (b == 1) {
			String serial = JOptionPane.showInputDialog("새로운 음식명를 입력하세요.");
			alf.get(a).setfn(serial);
		}
		if (b == 2) {
			int c = Integer.parseInt(JOptionPane
					.showInputDialog("새로운 가격를 입력하세요."));
			alf.get(a).setsprice(c);
		}
		if (b == 3) {
			int serial = Integer.parseInt(JOptionPane
					.showInputDialog("새로운 남은갯수를 입력하세요."));
			alf.get(a).setnumber(serial);
		}
		resetf();
	}

	//
	public void on() {
		int a = seatstable.getSelectedRow();
		if (a < 0) {
			return;
		}
		String id = JOptionPane.showInputDialog("ID를 입력하세요");
		if (login(id)) {
			int row = (int) model.getValueAt(a, 0);
			for (int i = 0; i < als.size(); i++) {
				if (als.get(i).number == row) {
					als.get(i).setname(id);
					als.get(i).using = true;
					als.get(i).clean = true;
					break;
				}
			}
		}
		reset();

	}

	//
	public boolean login(String a) {
		for (int i = 0; i < al.size(); i++) {
			if (al.get(i).id.equals(a)) {

				return true;
			}
		}
		JOptionPane.showMessageDialog(null, "존재하지않는 ID입니다.", "Message",
				JOptionPane.ERROR_MESSAGE);
		return false;
	}

	//
	// public void off() {
	//
	// }
	//
	public void buy() {
		int a = seatstable.getSelectedRow();
		System.out.println("ok");
		if (a < 0) {
			return;
		}
		int row = (int) model.getValueAt(a, 0);
		for (int i = 0; i < als.size(); i++) {
			if (als.get(i).number == row) {
				if (als.get(i).using == false) {
					JOptionPane.showMessageDialog(null, "빈 좌석입니다.", "Message",
							JOptionPane.ERROR_MESSAGE);
					return;
				}
			}
		}
		Panel bf = new Panel();
		bf.setLayout(new GridLayout(1, 4));
		foodsname = new String[alf.size()];
		System.out.println("ok");
		for (int i = 0; i < alf.size(); i++) {
			foodsname[i] = alf.get(i).fn + ":" + alf.get(i).price + "원:"
					+ alf.get(i).number + "개";
		}
		System.out.println("ok");
		buyfood = new JComboBox(foodsname);
		System.out.println("ok");
		bfl = new JLabel("갯수");
		bfb = new JButton("확인");
		bfb.addActionListener(this);
		buyfood.addActionListener(this);
		bf.add(buyfood);
		bf.add(bfl);
		bf.add(bfn);
		bf.add(bfb);
		add("buyfood", bf);
		setSize(600, 100);
		cardLayout.show(getContentPane(), "buyfood");
		nrow = row;

	}

	// public void clean(){
	//
	// }
	//
	public int totalprice() {
		totalprice = 0;
		for (int i = 0; i < al.size(); i++) {
			totalprice += al.get(i).used;
		}
		return totalprice;
	}

	public void list() {
		int usernumber = al.size();
		int allseats = als.size();
		int eq = 0;
		for (int i = 0; i < als.size(); i++) {
			if (als.get(i).using == true) {
				eq++;
			}
		}
		JOptionPane.showMessageDialog(this, "전체 사용자 수는" + usernumber + "이며"
				+ "\n" + "판매한 총 가격은" + totalprice() + "원, \n" + "총" + allseats
				+ "개의 좌석중" + eq + "개가 작동중입니다.", "total",
				JOptionPane.INFORMATION_MESSAGE);

	}

	//
	public void input() {
		FileInputStream fin = null;
		ObjectInputStream ois = null;

		try {
			fin = new FileInputStream("Seats.dat");
			ois = new ObjectInputStream(fin);

			als = (ArrayList) ois.readObject();
			als1 = (ArrayList) ois.readObject();
			System.out.println(als.get(0).number);
		} catch (Exception ex) {
		} finally {
			try {
				ois.close();
				fin.close();
			} catch (IOException ioe) {
			}
		}
		try {
			fin = new FileInputStream("Foods.dat");
			ois = new ObjectInputStream(fin);

			alf = (ArrayList) ois.readObject();
			alf1 = (ArrayList) ois.readObject();
			System.out.println(alf);

		} catch (Exception ex) {
		} finally {
			try {
				ois.close();
				fin.close();
			} catch (IOException ioe) {
			}
		}
		try {
			fin = new FileInputStream("Users.dat");
			ois = new ObjectInputStream(fin);

			al = (ArrayList) ois.readObject();
			al1 = (ArrayList) ois.readObject();
			System.out.println(al);

		} catch (Exception ex) {
		} finally {
			try {
				ois.close();
				fin.close();
			} catch (IOException ioe) {
			}
		}
		System.out.println("dd");
		reset();
		resetu();
		resetf();
	}

	// public void system(){
	//
	// }
	//
	public void output() {
		FileOutputStream fout = null;
		ObjectOutputStream oos = null;
		try {
			fout = new FileOutputStream("Seats.dat");
			oos = new ObjectOutputStream(fout);

			oos.writeObject(als);
			oos.reset();
			oos.writeObject(als);
			oos.reset();
			System.out.println(als);
			System.out.println("저장되었습니다.");

		} catch (Exception ex) {
		} finally {
			try {
				oos.close();
				fout.close();
			} catch (IOException ioe) {
			}
		}
		try {
			fout = new FileOutputStream("Foods.dat");
			oos = new ObjectOutputStream(fout);

			oos.writeObject(alf);
			oos.reset();
			oos.writeObject(alf);
			oos.reset();

			System.out.println("저장되었습니다.");

		} catch (Exception ex) {
		} finally {
			try {
				oos.close();
				fout.close();
			} catch (IOException ioe) {
			}
		}
		try {
			fout = new FileOutputStream("Users.dat");
			oos = new ObjectOutputStream(fout);

			oos.writeObject(al);
			oos.reset();
			oos.writeObject(al);
			oos.reset();

			System.out.println("저장되었습니다.");

		} catch (Exception ex) {
		} finally {
			try {
				oos.close();
				fout.close();
			} catch (IOException ioe) {
			}
		}
	}

	//
	public void exit() {
		System.exit(0);
	}

	public void seeusers() {
		cardLayout.show(getContentPane(), "users");
		resetu();
	}

	public void seeseats() {
		cardLayout.show(getContentPane(), "seats");
		reset();
	}

	public void seefoods() {
		cardLayout.show(getContentPane(), "foods");
		resetf();

	}
}
