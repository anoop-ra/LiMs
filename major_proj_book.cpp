#include <iostream>
#include<string>
#include <cassert>
#include <leveldb/db.h>
#include <sstream>

using namespace std;

class Book{
	private:
		int id, type, borrower, reserver;
		string name, author, subject;
	public:
		Book(){}
		Book(int i, string n, string a, string s, int t, int b, int r):id(i),name(n),author(a), subject(s), type(t), borrower(b), reserver(r){}
		void set_borrower(int u_id){borrower = u_id;}
		void set_reserver(int u_id){reserver = u_id;}
		string get_bname(){return name;}
		int get_btype(){return type;}
		string get_bauthor(){return author;}
		int get_bid(){return id;}
		int get_bborrower(){return borrower;}
		int get_breserver(){return reserver;}
		string get_bsubject(){return subject;}
		friend class BookManage;
};

class BookManage{
	private: 
		leveldb::DB *b_db=NULL;
		leveldb::Status b_st;
		static BookManage *bmInstance;
		BookManage(){
			if(b_db==NULL){
				leveldb::Options options;
				options.create_if_missing= true;
				b_st = leveldb::DB::Open(options, "BookRecords.db", &b_db);
				assert(b_st.ok());
			}
		}
	public: 
		static BookManage *create(){
			if(bmInstance == NULL)bmInstance = new BookManage();
			return bmInstance;
		}
		~BookManage(){delete b_db;}
		void add_book(Book& b){
			stringstream str;			
			str<<b.id<<' '<<b.name<<' '<<b.author<<' '<<b.subject<<' '<<b.type<<' '<<b.borrower<<' '<<b.reserver;
			string value = str.str();
			leveldb::Slice key((char *)&b.id,sizeof(int));
			b_st = b_db->Put(leveldb::WriteOptions(), key, value);
			if(!b_st.ok()) cout<<b_st.ToString()<<endl;
			assert(b_st.ok());
			//cout<<b.id<<' '<<b.name<<' '<<b.author<<' '<<b.subject<<' '<<b.type<<' '<<b.borrower<<' '<<b.reserver<<endl;
		}
		Book get_book(int i){
			stringstream str;
			leveldb::Slice key((char *)&i,sizeof(int));
			string value;
			b_st = b_db->Get(leveldb::ReadOptions(), key, &value);
			if(!b_st.ok())cout<<b_st.ToString()<<endl;
			assert(b_st.ok());
			int id, t, b, r;
			string n, a, s;
			str.str(value);
			str>>id>>n>>a>>s>>t>>b>>r;
			cout<<id<<' '<<n<<' '<<a<<' '<<s<<' '<<t<<' '<<b<<' '<<r<<endl;
			return Book(id, n, a, s, t, b, r);  
		}
		void borrow_book(int b_id, int u_id){
		 	Book b = get_book(b_id);
		 	b.set_borrower(u_id);
		 	add_book(b);
			return;
		}
		void reserve_book(int b_id, int u_id){
			Book b = get_book(b_id);
			b.set_reserver(u_id);
			add_book(b);
			return;
		}
		void search(string str){
			leveldb::Iterator* it = b_db->NewIterator(leveldb::ReadOptions());
			for (it->SeekToFirst(); it->Valid(); it->Next())
    		{
        		string str1 = it->value().ToString();
        		
        		char *x =strstr((char *)str1.c_str(), (char *)str.c_str());
        		if(x!=NULL) cout<<"Search Results \t"<<it->value().ToString()<<endl;
    		}
    		if (false == it->status().ok())
   			{
        		cerr << "An error was found during the scan" << endl;
        		cerr << it->status().ToString() << endl; 
    		}
		}

};
BookManage *BookManage::bmInstance = NULL;
int main(){
	
	Book b1(1236,"Harry_Potter","J.K","Fiction",1,2,3);
	Book b2(1245,"Balabhumi","Ayyappan","Story",1,4,6);
	Book b3(3425,"Balarama","Ayyappankutty","Story",1,7,2);
	Book b4(3326,"Sardarji_Jokes","Raveendra","Comic",1,4,9);
	BookManage *bm=BookManage::create();
	// bm->add_book(b1);
	// bm->add_book(b2);
	// bm->add_book(b3);
	// bm->add_book(b4);
	string str="Fic";
	bm->search(str);
	//Book b=bm->get_book(1245) ;
}