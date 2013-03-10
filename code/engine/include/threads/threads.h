#pragma once
#include <Windows.h>
#include <vector>

#include "../misc/container/circular_buffer.h"
#include "../error/errmacro.h"

/* todo: rozbudowac pool o priority i critical path */

namespace db {
	namespace network {
		class udp;
		class tcp;
		struct overlapped;
		class overlapped_accept;
	}
}

namespace db {
	namespace threads {
		extern db::error errlog;

		int get_num_cores();

		struct shared_counter {
			long i;
			shared_counter(long i = 0);
			shared_counter& operator+=(long);
			shared_counter& operator++();
			shared_counter& operator--();
		};

		class overlapped {
			friend class completion;
			friend class db::network::udp;
			friend class db::network::tcp;

			OVERLAPPED overlap;
			DWORD result;
		public:

			class multiple_waits {
				HANDLE* events;
				int n;
				multiple_waits(const multiple_waits&); // disable copy
			public:
				multiple_waits(overlapped*, int cnt);
				~multiple_waits();
				bool wait(DWORD timeout, bool all);
			};

			overlapped(); ~overlapped();
			void create_event();
			bool wait(DWORD timeout = INFINITE);
			int get_result() const;
		};
			
		class completion {
			friend class iocp;
			OVERLAPPED* overlap;
			DWORD result, key;
		public:
			completion(overlapped* o = 0, DWORD result = 0, DWORD key = 0);

			template<class T>
			bool get_user(T*& p) {
				return (p = (T*)overlap) != 0;
			}

			DWORD& get_key(), &get_result();
			
			void get_overlapped(overlapped*&);
			void get_overlapped(db::network::overlapped*&);
			void get_overlapped(db::network::overlapped_accept*&);
		};

		class iocp {
			HANDLE cport, *workers;
			int cval, nworkers;
			void* argp, *workerf;

			template<class T>
			static DWORD WINAPI ThreadProc(LPVOID p) {
				iocp* io = (iocp*)p;
				((int (*)(T*))(io->workerf))((T*)(io->argp));
				return 0;
			}
		public:
			iocp();

			static completion QUIT;

			void open(int concurrency_value = get_num_cores());
	
			bool associate(db::network::udp&, int key);
			bool associate(db::network::tcp&, int key);

			bool get_completion(completion& out, DWORD ms_timeout = INFINITE);
			bool post_completion(completion& in);

			template <class argument>
			void create_pool(int (*worker)(argument*), argument* arg, int n = get_num_cores()) {
				if(workers) return;
				argp = (void*)arg;
				workerf = (void*)worker;
				nworkers = n;
				workers = new HANDLE[n];
				for(int i = 0; i < n; ++i)
					err(workers[i] = CreateThread(0, 0, ThreadProc<argument>, this, 0, 0));
			}

			void quit_pool(void);
			
			void close();
			~iocp();
		};

		class task {
			friend struct pool;
			int (*proc)(void*);
			void* param;
			task *dependency, *parent;
			shared_counter open_works;
			int result, priority;
			void mark_done();

		public:
			task(int (*)(void*), void* parameter);
			int (*get_worker())(void*);
			int work();
			bool completed() const;
		}; 
		
		struct pool {
			pool(int max_tasks = 10000);
			~pool();
			
			void create_pool(int threads = (get_num_cores()-1));
			
			void begin_task(task* t);
			void begin_task(task* t, task* dependency);
			void child(task*);
			void child(task*, task* dependency);
			void end_task();
			
			void queue_task(task*);
			void queue_task(task**, int n);
			void lobby_task(task*);
			void lobby_task(task*, task* dependency);

			void join(task*); // join work while waiting for task; return when there are no jobs left and/or the task is finished.
			void wait(task*); // join work while waiting for task; return only when the task is finished.

			void quit_pool(); // finish off active tasks and quit
			bool is_running() const;
		
		private:
			misc::container::circular_buffer<task*> tasks; 
			std::vector<task*> lobby;
			CRITICAL_SECTION q_cs, l_cs;
			task* new_t;

			HANDLE* handles;

			task* adder;
			int nthreads;
			bool running, quit;
			static DWORD WINAPI ThreadProc(LPVOID);
			void worker_thread(); // current thread as worker

		};

	}
}