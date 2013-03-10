#include "db.h"
#include "redirectio.h"

// todo:
// jakis error przy serwie tcp przy exchange - kurwo zrob prevention completion port notification bo sie wypierdala jak send completion traktuje jako recv completion
// 

using namespace db;

struct user : public db::network::overlapped {
	network::ip* _ip;
	char* buffer;
};

int work(db::threads::iocp* io) {
	threads::completion comp;
	user* u;

	while(true) {
		if(io->get_completion(comp)) {
			if(comp.get_user(u)) {
				if(u->buffer) { // is recv not send
					cout << "from: " << u->_ip->get_ipv4() << endl;
					cout << u->buffer << endl;
				}
			}
			else {
				io->post_completion(threads::iocp::QUIT); // quit
				break;
			}
		}
		else if (!comp.get_user(u)) {
			io->post_completion(threads::iocp::QUIT); // quit
			// timeout
		}
		else { // error
			io->post_completion(threads::iocp::QUIT); // quit
		}
	}

	return 0;
}


int main() {

	//network::errlog.open("network.txt");
	//threads::errlog.open("threads.txt");

	threads::iocp io;

	//RedirectIOToConsole();
	if(network::init()) 
	{	
		using namespace network;
		char buffer[1000];
		udp s;

		if(s.open()) { 
			io.open();
			io.associate(s, 0);
			io.create_pool(work, &io);

			if(s.bind(27017)) {
				ip from, to(27017, ip::get_local_ipv4());

				user trecv, tsend;
				trecv.create_event();
				trecv._ip = &from, trecv.buffer = buffer;
				tsend.buffer = 0;

				int rres = s.recv(from, buf(buffer, 1000), &trecv);
				int sres = s.send(to, buf("he he hellmanns", strlen("he he hellmanns")+1), &tsend);

				cout << rres << endl << sres << endl;
				Sleep(1000);
			}
		}
	}
	io.close();
	network::deinit();

	//network::errlog.close();
	//threads::errlog.close();


	system("pause");
	return 0;
}