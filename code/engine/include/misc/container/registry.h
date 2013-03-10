#pragma once
#include <vector>

namespace db {
	namespace misc {
		namespace container {
			template <typename T>
			struct registry {
				static bool clear_flag;
				struct entry { 
					T t;
					unsigned usage;
					entry(const T& tt) : usage(1) { t = tt; }
					~entry() { t.~T(); };
				};
			protected:
				static std::vector<entry*> reg;

				entry* p;

				bool assign() {
					for(unsigned i = 0; i < reg.size(); ++i) {
						if(((T*)this)->operator==(reg[i]->t)) {
						   ((T*)this)->operator =(reg[i]->t);
						   ((T*)this)->p = reg[i]->t.p;
							++(reg[i]->usage);
							return false;
						}
					}
					((T*)this)->p = new entry(*(T*)this);
					reg.push_back(p);
					return true;
				}

				bool deassign() {
					if(!p->usage) return true;

					if(!(--(p->usage)) && clear_flag) {
						for(unsigned i = 0; i < reg.size(); ++i) {
							if(reg[i] == p) {
								delete p;
								reg.erase(reg.begin()+i);
								return true;
							}
						}
					}
					return false;
				}
			public:
				static int clear_unused() {
					int cnt = 0;
					for(unsigned i = 0; i < reg.size(); ++i) {
						if(!reg[i]->usage) {
							delete reg[i];
							reg.erase(reg.begin()+i);
							++cnt;
						}
					}
					return cnt;
				}

				static int clear_unused(bool flag) {
					clear_flag = flag;
					if(flag) return clear_unused();
					return 0;
				}
			};

			template<typename T> bool registry<T>::clear_flag = false;
			template<typename T> std::vector<typename registry<T>::entry*> registry<T>::reg;
		}
	}
}

