#pragma once
#include "threads.h"
#include <cassert>
#include "../error/errmacro.h"

namespace db {
	namespace threads {
		db::error errlog(db::last_error);

		shared_counter::shared_counter(long sc) : i(sc) {}
		shared_counter& shared_counter::operator++() {
			InterlockedIncrement(&i);
			return *this;
		}
		shared_counter& shared_counter::operator--() {
			InterlockedDecrement(&i);
			return *this;
		}
		

		task::task(int (*f)(void*), void* parameter) 
			: proc(f), param(parameter), dependency(0), parent(0), open_works(0), result(0), priority(0) {
		}
		
		void task::mark_done() {
			if(parent)
				--(parent->open_works);
			--open_works;
		}

		int (*task::get_worker())(void*) {
			return proc;
		}

		int task::work() {
			result = proc ? proc(param) : 0;
			return result;
		}

		bool task::completed() const {
			return open_works.i <= 0;
		}

		DWORD WINAPI pool::ThreadProc(LPVOID ptr) {
			(static_cast<pool*>(ptr))->worker_thread();
			return 0;
		}

		pool::pool(int max_tasks) : quit(false), running(false), adder(0), tasks(max_tasks), handles(0) {
			lobby.reserve(max_tasks);
		}
		
		pool::~pool() {
			quit_pool();
		}

		void pool::create_pool(int threads) {
			if(handles) return;
			nthreads = threads;
			quit = false;
			handles = new HANDLE[nthreads];
			InitializeCriticalSection(&q_cs);
			InitializeCriticalSection(&l_cs);

			for(int i = 0; i < nthreads; ++i) 
				err(handles[i] = CreateThread(0, 0, ThreadProc, (LPVOID*)this, 0, 0));
			running = true;
		}

		void pool::begin_task(task* t) {
			if(adder) t->parent = adder;
			adder = t;
			t->open_works = 2;
			EnterCriticalSection(&q_cs);
				tasks.push(t);
			LeaveCriticalSection(&q_cs);
		}		
		
		void pool::begin_task(task* t, task* dependency) {
			if(adder) t->parent = adder;
			adder = t;
			t->dependency = dependency;
			t->open_works = 2;
			EnterCriticalSection(&l_cs);
				lobby.push_back(t);
			LeaveCriticalSection(&l_cs);
		}
		
		void pool::child(task* t) {
			t->parent = adder;
			++(adder->open_works);
			queue_task(t);
		}

		void pool::child(task* t, task* dependency) {
			t->parent = adder;
			++(adder->open_works);
			lobby_task(t, dependency);
		}

		void pool::end_task() {
			--(adder->open_works);
			adder = adder->parent;
		}

		void pool::queue_task(task* t) {
			t->open_works = 1;
			EnterCriticalSection(&q_cs);
				tasks.push(t);
			LeaveCriticalSection(&q_cs);
		}

		void pool::queue_task(task** t, int n) { 
			EnterCriticalSection(&q_cs);
			for(int i = 0; i < n; ++i) {
				t[i]->open_works = 1;
				tasks.push(t[i]);
			}
			LeaveCriticalSection(&q_cs);
		}
		
		void pool::lobby_task(task* t, task* dependency) {
			t->open_works = 1;
			t->dependency = dependency;
			EnterCriticalSection(&l_cs);
				lobby.push_back(t);
			LeaveCriticalSection(&l_cs);
		}

		void pool::worker_thread() {
			task* mytask;
			while(!quit) {
				if(!tasks.empty()) {
					EnterCriticalSection(&q_cs);
					if(!tasks.empty()) {
						mytask = tasks.front();
						tasks.pop();
					} else mytask = 0;
					LeaveCriticalSection(&q_cs);

					if(mytask) {
						mytask->work();
						mytask->mark_done();
					}
				}
				else if(!lobby.empty()) {
					EnterCriticalSection(&l_cs);
						for(unsigned i = 0; i < lobby.size(); ++i)
							if(lobby[i]->dependency->completed()) {
								EnterCriticalSection(&q_cs);
								tasks.push(lobby[i]);
								LeaveCriticalSection(&q_cs);
								std::swap(lobby.back(), *(lobby.begin()+i)); 
								lobby.pop_back();
							}
					LeaveCriticalSection(&l_cs);
				}
			}
		}

		void pool::wait(task* t) {
			task* mytask;
			while(!t->completed() && !quit) 
				if(!tasks.empty()) {
					EnterCriticalSection(&q_cs);
					if(!tasks.empty()) {
						mytask = tasks.front();
						tasks.pop();
					} else mytask = 0;
					LeaveCriticalSection(&q_cs);

					if(mytask) {
						mytask->work();
						mytask->mark_done();
					}
				}
				else if(!lobby.empty()) {
					EnterCriticalSection(&l_cs);
						for(unsigned i = 0; i < lobby.size(); ++i)
							if(lobby[i]->dependency->completed()) {
								EnterCriticalSection(&q_cs);
								tasks.push(lobby[i]);
								LeaveCriticalSection(&q_cs);
								std::swap(lobby.back(), *(lobby.begin()+i)); 
								lobby.pop_back();
							}
					LeaveCriticalSection(&l_cs);
				}
		}
		

		void pool::join(task* t) {
			task* mytask;
			while(!t->completed() && !quit && !tasks.empty())  {
					EnterCriticalSection(&q_cs);
					if(!tasks.empty()) {
						mytask = tasks.front();
						tasks.pop();
					} else mytask = 0;
					LeaveCriticalSection(&q_cs);

					if(mytask) {
						mytask->work();
						mytask->mark_done();
					}
				}
		}

		void pool::quit_pool() {
			if(!handles) return;
			quit = true;
			WaitForMultipleObjects(nthreads, handles, true, INFINITE);

			for(int i = 0; i < nthreads; ++i)
				err(CloseHandle(handles[i]));
			
			DeleteCriticalSection(&q_cs);
			DeleteCriticalSection(&l_cs);
			running = false;

			delete [] handles;
			handles = 0;
		}

		bool pool::is_running() const {
			return running;
		}

		int get_num_cores() {
			static SYSTEM_INFO sysinfo;
			GetSystemInfo( &sysinfo );

			return sysinfo.dwNumberOfProcessors;
		}
	}
}