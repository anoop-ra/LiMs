#include <iostream>
#include <string>
#include <leveldb/db.h>
#include <sstream>
#include <cassert>
#include <map>
#include<vector>
#include <ctime>
#include<cstdlib>
#include <iterator> 
#include<algorithm>
using namespace std;


string now_date(){
	time_t t = time(NULL);
	tm* timePtr = localtime(&t);
	const string d = to_string(timePtr->tm_mday);
	const string m = to_string(timePtr->tm_mon + 1);
	const string y = to_string(timePtr->tm_year+1900);
	// strcat(d,m);
	// strcat(d,y);
	string x = d+"_"+m+"_"+y;
	//cout<<x;
	return x;
}
vector <pair<int,string>> stov(string br)
{	
	vector <pair <int,string>> v1;
	pair<int,string> p1;
	stringstream check1(br);
	string intermediate;			
		while(getline(check1, intermediate, '|')) 
			{ 
			stringstream check2(intermediate);
			string inter2;
			getline(check2, inter2, '-');
			p1.first=stoi(inter2);
			//cout<<inter2<<endl;
			getline(check2, inter2, '-');
			p1.second=inter2;
			v1.push_back(p1);
			//cout<<p1.second<<endl;		 
			}
			return v1;
			
}
class User{
	protected:
		int id;
		string name, password;
		std::vector <pair<int,string> >borrow;
		std::vector <pair<int,string> >reserved;

		int num_book=0, num_days = 0;
		string email;
		int mobile;
		string department;
	public:
		User(){}
		User(int i, string n, string p, vector<pair <int, string>> b, vector<pair <int ,string>> r,string em,int mo,string dept):id(i),password(p),name(n){
			borrow=b;
			reserved = r;
			email=em;
			mobile=mo;
			department=dept;
		}
		User(int i, string n, string p,string em,int mo,string dept):id(i),password(p),name(n),email(em),mobile(mo),department(dept){}
		//virtual void set_num_book()=0;
		//virtual void set_num_days()=0;
		int get_num_books(){
			//if(num_book ==0 ) set_num_book();
			return num_book;
		}
		int get_num_days(){
			//if(num_days==0) set_num_days();
			return num_days;
		}
		string get_pass(){
			return password;
		}
		void change_pass(){
			cout<<"Enter new password \n";
			string pass;
			cin>>pass;
			password = pass;
		}
		void see_status(){
			cout<<"Borrowed books are \n";
			for(auto b:borrow)
				cout<<b.first<<' '<<b.second<<endl;
			cout<<"Reserved books are \n";
			for(auto r:reserved)
				cout<<r.first<<' '<<r.second<<endl;
		}
		void reserve(int i){
			pair<int, string> r;
			r.first=i;
			r.second=now_date();
			reserved.push_back(r);
		}
		void cancel_reserve(int bid){
			int i=0;bool flag =false;
			for (auto r:reserved){
				if(r.first == bid){flag = true; break;}
				i++;
			}
			if(flag)reserved.erase(reserved.begin()+i);
		}
		friend class UserManage;
};

class Student:public User{
	protected:
		int year;
		bool reference = false;
	public:
		Student(){}
		Student(int i, string n, string p, vector<pair <int, string>> b, vector<pair <int ,string>> r, string em, int mo, string dept, int y):User(i, n, p, b, r, em, mo, dept),year(y){}
		void set_num_book(){ num_book = 2; return;}
		void set_num_days(){ num_days = 7; return;}
		bool get_ref_status(){return false;}
		int get_year(){return year;}
		friend class UserManage;
};

class Faculty:public User{
	protected:
		string role;
		bool reference=true;
	public:
		Faculty(){}
		Faculty(int i, string n, string p, vector<pair <int, string>> b, vector<pair <int ,string>> r, string em, int mo, string dept, string ro):User(i, n, p, b, r, em, mo, dept) ,role(ro){}
		void set_num_book(){num_book=4;return;}
		void set_num_days(){num_days=14;return;}
		bool get_ref_status(){return true;}
		string get_role(){return role;}
		friend class UserManage;
};
class UserManage{
	private:
		leveldb::DB *db=NULL;
		leveldb::Status st;
		static UserManage *um_Instance;
		UserManage(){
			if(db==NULL){
				leveldb::Options options;
				options.create_if_missing=true;
				st=leveldb::DB::Open(options,"User.db",&db);
				assert(st.ok());
			}
		}
	public:
		static UserManage *create(){
			if(um_Instance==NULL)
				um_Instance=new UserManage;
			return um_Instance;
		}
		~UserManage(){delete db;}
		void add_record(Faculty& f ){
			ostringstream str,str2;
			str<<f.id<<' '<<f.name<<' '<<f.password<<' '<<f.num_book<<' '<<f.num_days<<' '<<f.email<<' '<<f.mobile<<' '<<f.department<<' '<<f.role<<' '<<f.reference;
			for(auto b:f.borrow){
				string f1 = to_string(b.first);
				str<<f1<<'-'<<b.second<<'|';
			}
			for(auto b:f.reserved){
				string f1 = to_string(b.first);
				str<<f1<<'-'<<b.second<<'|';
			}
  			string value=str.str();
  			leveldb::Slice key((char *)&f.id,sizeof(int));
  			st=db->Put(leveldb::WriteOptions(),key,value);
  			if(!st.ok())cout<<st.ToString()<<endl;
  			assert(st.ok());
  		}
  		void add_record(Student& f ){
			stringstream str;
			str<<f.id<<' '<<f.name<<' '<<f.password<<' '<<f.num_book<<' '<<f.num_days<<' '<<f.email<<' '<<f.mobile<<' '<<f.department<<' '<<f.reference<<' '<<f.year<<' ';
			for(auto b:f.borrow){
				string f1 = to_string(b.first);
				str<<f1<<'-'<<b.second<<'|';
			}
			str<<' ';
			for(auto b:f.reserved){
				string f1 = to_string(b.first);
				str<<f1<<'-'<<b.second<<'|';
			}
  			string value=str.str();
  			leveldb::Slice key((char *)&f.id,sizeof(int));
  			st=db->Put(leveldb::WriteOptions(),key,value);
  			if(!st.ok())cout<<st.ToString()<<endl;
  			assert(st.ok());
  		}
  		bool check_if(int id){
  			stringstream str;
  			leveldb::Slice key((char *)&id, sizeof(id));
  			string value;
  			st = db->Get(leveldb::ReadOptions(), key, &value);
  			if(!st.ok()) cout<<st.ToString()<<endl;
  			assert(st.ok());
  			int i;
			string n, p;
			std::vector <pair<int,string>> b;
			std::vector <pair<int,string> >r;
			int nb, nd ;
			string em;
			int mo,y;
			string dept;
			bool ref;
			string br,re;
			str.str(value);
			str>>i>>n>>p>>nb>>nd>>em>>mo>>dept>>ref>>y>>br>>re;
			//cout<<i<<' '<<n<<' '<<p<<' '<<nb<<' '<<nd<<' '<<em<<' '<<mo<<' '<<dept<<' '<<ref<<' '<<y<<' '<<br<<' '<<re<<' ';
			return ref;
  		}
  		//emplate <class T, class U>
  		User* get_record(int id){
  			stringstream str;
  			leveldb::Slice key((char *)&id, sizeof(id));
  			string value;
  			st = db->Get(leveldb::ReadOptions(), key, &value);
  			if(!st.ok()) cout<<st.ToString()<<endl;
  			assert(st.ok());
  			int i;
			string n, p;
			std::vector <pair<int,string>> b;
			std::vector <pair<int,string> >r;
			int nb, nd ;
			string em;
			int mo,y;
			string dept;
			bool ref;
			string br,re;
			str.str(value);
			str>>i>>n>>p>>nb>>nd>>em>>mo>>dept>>ref>>y>>br>>re;
			//cout<<i<<' '<<n<<' '<<p<<' '<<nb<<' '<<nd<<' '<<em<<' '<<mo<<' '<<dept<<' '<<ref<<' '<<y<<' '<<br<<' '<<re<<' ';
	        char s1[2] = "-";
			char s2[2] = "|";
			if(ref){
				string ro;
				str>>i>>n>>p>>nb>>nd>>em>>mo>>dept>>ref>>br>>re>>ro;
				//cout<<br<<'\t'<<re<<endl;
				r = stov(re);
				b = stov(br);
				User* u  =  new Faculty(i,n,p,b,r,em,mo,dept,ro);
				return u;
			}
			else{
				int y;
				str>>i>>n>>p>>nb>>nd>>em>>mo>>dept>>ref>>br>>re>>y;
				//cout<<endl<<br<<'\t'<<re<<endl;
				r = stov(re);
				b = stov(br);
				User* u = new Student(i,n,p,b,r,em,mo,dept,y);
				return u;
			}

  			
  		}
  		friend class Librarian;
};
UserManage *UserManage::um_Instance = NULL;

void user_login(){
	User *u;int a;
	UserManage *um = UserManage::create();
	string passwd;
	int u_Id;
	
	cout<<"UserId: ";
	cin>>u_Id;
	cout<<"Password: ";
	cin>>passwd;
	bool temp=um->check_if(u_Id);
	User *s=um->get_record(u_Id);
	if(s!=NULL&&s->get_pass()==passwd)
		{	
			system("clear");
			while(1){
			cout<<"\t1.STATUS \t2.SEARCH \n\t3.RESERVE \t4.CANCEL RESERVATION \n\t5.CHANGE PASSWORD \t6.EXIT"<<endl;
			cout<<"Enter choice";
			cin>>a;
			u=um->get_record(u_Id);
			switch(a){
				case 1:
						u->see_status();
					break;
				//case 2:search();
				//case 3:reserve();
				// case 4:u->cancel_reserve();
				// 	break;
				// case 5:u.change_pass();
				// 	break;
				case 6:exit(0);
					break;
			}}
		}
	else{cout<<"Invalid Password";}
	//f->see_status();
	}
	

int main(){
	vector <pair<int,string>> v;
	pair <int,string> pair1;
	pair1.first=4;
	pair1.second=now_date();
	v.push_back(pair1);
	v.push_back(pair1);
	Student u(1026764,"Gayathri","quest123",v,v,"gau2586@gmail.com",8281,"eci",2015);
	// u1(1026764,"Gayathri","quest",v,v,"gau2586@gmail.com",8281,"eci","Prof");
	u.reserve(10334);
	u.reserve(27122);
	//u.see_status();
	//u.cancel_reserve(10334);
	//u.see_status();
	now_date();
	UserManage *um = UserManage::create();
	um->add_record(u);
	cout<<"\t****************************************************"<<endl;
	cout<<"\t\tLIBRARY MANAGEMENT SYSTEM\t"<<endl;
	cout<<"\t****************************************************"<<endl;
	user_login();
	//um->add_record(u);
	bool temp=um->check_if(1026764);
	if(!temp){
	User *s=um->get_record(1026764);
	}
	if(temp){
	User *f=um->get_record(1026764);
	f->see_status();
	}
}