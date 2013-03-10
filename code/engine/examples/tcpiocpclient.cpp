#include "db.h"
#include "redirectio.h"

using namespace db;
using namespace network;

struct user : public db::network::overlapped {
	tcp* client;
	bool connected;
	user() { trecv.who = tsend.who = this; connected = false; }
	struct uoverlapped : public db::network::overlapped {
	tcp* client;
		char buffer[1000];
		buf b;
		uoverlapped() : b(buffer, 1000) { }
		user* who;
	} trecv, tsend;
};

int work(db::threads::iocp* io) {
	threads::completion comp;
	user* u, *root;
	user::uoverlapped *uov;
	buf huj("spierdalaj", 11);
	while(true) {
		if(io->get_completion(comp)) {
			if(comp.get_user(u)) {
				if(comp.get_key() == 0) {
					if(!u->connected) {
						u->connected = true;
						u->trecv.client = u->client;
						int rres = u->client->recv(&u->trecv.b, 1, &u->trecv);
						int sres = u->client->send(&huj, 1, &u->tsend);
						cout << rres << endl << sres << endl;
					}
					else { 
						uov = (user::uoverlapped*)u;
						root = (uov->who);
						if(uov == &(root->tsend)) {
							cout << uov->get_result() << " bytes sent." << endl;
						}
						else {
							cout << "from: " << uov->client->addr.get_ipv4() << endl;
							cout << uov->buffer << endl;
						}
					}
				}

			}
			else if(!comp.get_user(u)) {
				io->post_completion(io->QUIT); // quit
				break;
			}
		}
		else if (!comp.get_user(u)) {
			io->post_completion(io->QUIT); // quit
			// timeout
		}
		else { // error
			io->post_completion(io->QUIT); // quit
		}
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
		
		user tconn;
		threads::iocp io;
		tcp c;

		if(c.open()) { 
			cout <<  ip::get_local_ipv4() << endl;
			c.linger(true, 1);
			c.nagle(false);

			c.bind();

			io.open();
			io.associate(c, 0);
			io.create_pool(work, &io);
			ip to(27017, ip::get_local_ipv4());

			tconn.client = &c;
			int connres = c.connect(to, &tconn);

			cout << connres << endl;
			work(&io);
		}
		c.close();
		io.close();
	}
	network::deinit();

	network::errlog.close();
	threads::errlog.close();


	system("pause");
	return 0;
}