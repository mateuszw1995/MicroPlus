#include "db.h"
#include "redirectio.h"

// dopracowac shutting down

using namespace db;
using namespace network;

tcp s;

struct user : public db::network::overlapped {
	char buffer[1000];
	buf b;
	tcp client;
	user() : b(buffer, 1000) {
		trecv.who = tsend.who = this;
	}

	struct uoverlapped : public db::network::overlapped {
		user* who;
	} trecv, tsend;

} users[10];
int ui = -1;

int work(db::threads::iocp* io) {
	threads::completion comp;
	overlapped_accept* acc, tacc;
	user* u, *root;
	user::uoverlapped *uov;
	buf bb("jeb sie hehe", 13);

	while(true) {
		if(io->get_completion(comp)) {
			
			if(comp.get_user(u)) {
				if(comp.get_key() == 0) {
					++ui; ui %= 10;
					comp.get_overlapped(acc);
					users[ui].client.open(*acc);
					
					s.accept(&tacc);

					users[ui].client.linger(true, 100);

					io->associate(users[ui].client, 1);
					
					users[ui].client.recv(&users[ui].b, 1, &users[ui].trecv);
					users[ui].client.send(&bb, 1, &users[ui].tsend);

				//	users[ui].client.close();
				}
				else if(comp.get_key() == 1) {
					uov = (user::uoverlapped*)u;
					root = (uov->who);
					if(uov == &(root->tsend)) {
						cout << uov->get_result() << " bytes sent." << endl;
					}
					else {
						cout << "from: " << root->client.addr.get_ipv4() << endl;
						cout << root->buffer << endl;
					}
				}
			}
			else {
				io->post_completion(io->QUIT); // quit
				break;
			}
		}
		else if (!comp.get_user(u)) {
			// timeout
		}
		else {
			
		}// error
	}

	return 0;
}


int main() {
	
	network::errlog.open("network.txt");
	threads::errlog.open("threads.txt");
	
	
	//RedirectIOToConsole();
	if(network::init()) 
	{	
		using namespace network;
		char buffer[1000];
		
		overlapped_accept tacc;
		threads::iocp io;

		if(s.open()) { 

			s.linger(true, 100);
			s.nagle(false);
			s.bind(27017);
			if(s.listen(27017)) {
				io.open();
				io.associate(s, 0);
				io.create_pool(work, &io);
	
				int accres = s.accept(&tacc);

				cout << accres << endl;
				work(&io);
			}
		}

		s.close();
		for(int i = 0; i < 10; ++i) 
			users[i].~user();

		io.close();
	}
	network::deinit();
	
	network::errlog.close();
	threads::errlog.close();
	

	system("pause");
	return 0;
}