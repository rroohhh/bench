#include <iostream>
#include <array>
#include <cassert>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

namespace string_id {
	namespace detail
	{
		using hash_type = std::uint64_t;

		constexpr hash_type fnv_basis = 14695981039346656037ull;
		constexpr hash_type fnv_prime = 109951162821ull;

		// FNV-1a 64 bit hash
		constexpr hash_type sid_hash(const char *str, hash_type hash = fnv_basis) noexcept {
			return *str ? sid_hash(str + 1, (hash ^ *str) * fnv_prime) : hash;
		}
	}
}

namespace platform {
    inline uint64_t rdtsc() noexcept {
	unsigned int lo,hi;
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32) | lo;
   }
}

#ifndef NPROFILE
namespace profiling {
	using time_type = std::uint64_t;
	void print_profile_stats() noexcept;

	struct profile_info {
		std::string name;
		std::string function;
		time_type time = 0;
		std::uint64_t count = 0;
	};

	inline std::ostream &operator<<(std::ostream &os, profile_info const &p) noexcept {
		return os << p.name << ", " << p.function << ", " << p.time << ", " << p.count << ", " << ((p.count != 0) ? (p.time / p.count) : 0);
	}

	namespace detail {
		constexpr int max_profilings = 1024;
		std::array<profile_info, max_profilings> profiling_info;
		static int counter = 0;

		template<typename Hash>
		struct index_provider {
			static bool inited;
			static int index;

			inline index_provider() noexcept {
				if(!inited) {
					assert(detail::counter < detail::max_profilings);

					index = detail::counter;
					detail::counter++;
					inited = true;
				}
			}

			inline operator int() const noexcept {
				return index;
			}

			inline static bool exists() noexcept {
				return inited;
			}
		};
		
		template<typename Hash>
		bool index_provider<Hash>::inited = false;

		template<typename Hash>
		int index_provider<Hash>::index = 0;

		struct __profile__init__ {
			__profile__init__() {
				std::atexit([](){ profiling::print_profile_stats(); });
			}
		} __profile__init_;
	}

	template<typename Hash>
	struct profiler {
		inline profiler(const char * name, const char * function) noexcept{
			if(first) {
				index = detail::index_provider<Hash>();
				detail::profiling_info[index].name = name;
				detail::profiling_info[index].function = function;

				first = false;
			}

			start = platform::rdtsc();
		}

		inline void end() noexcept {
			if(!ended) {
				detail::profiling_info[index].time += platform::rdtsc() - start; 	
				detail::profiling_info[index].count++;
				ended = true;
			}
		}

		inline ~profiler() noexcept {
			end();		
		}

		bool ended = false;
		static bool first;
		time_type start;
		int index;
	};

	template<typename Hash>
	bool profiler<Hash>::first = true;	
	
	template<typename Hash>
	inline static const profile_info get_profile_info() noexcept {
		profile_info info;
		if(detail::index_provider<Hash>::exists()) {
			info = detail::profiling_info[detail::index_provider<Hash>()];
		}

		return info;
	}
	
	void print_profile_stats() noexcept {
		std::cout << std::left << std::setw(20) << std::setfill(' ') << "name"; 
		std::cout << std::left << std::setw(20) << std::setfill(' ') << "function"; 
		std::cout << std::left << std::setw(20) << std::setfill(' ') << "total_time";
		std::cout << std::left << std::setw(20) << std::setfill(' ') << "count"; 
		std::cout << std::left << std::setw(20) << std::setfill(' ') << "avg" << std::endl;

		for(int i = 0; i < detail::counter; i++) {
			const auto & info = detail::profiling_info[i];
			if(info.name == "" && info.function == "") continue; 
			std::cout << std::left << std::setw(20) << std::setfill(' ') << info.name;
			std::cout << std::left << std::setw(20) << std::setfill(' ') << info.function;
			std::cout << std::left << std::setw(20) << std::setfill(' ') << info.time; 
			std::cout << std::left << std::setw(20) << std::setfill(' ') << info.count;
			std::cout << std::left << std::setw(20) << std::setfill(' ') << ((info.count != 0) ? (info.time / info.count) : 0) << std::endl;
		}
	}
}

#define profile(x) profiling::profiler<std::integral_constant<string_id::detail::hash_type, string_id::detail::sid_hash(#x)>> paste(__profile_,x)(#x,__PRETTY_FUNCTION__)
#define get_profile(x) profiling::get_profile_info<std::integral_constant<string_id::detail::hash_type, string_id::detail::sid_hash(#x)>>()
#define profile_end(x) paste(__profile_,x).end()

#else
#define profile(x)
#define get_profile(x)
#define profile_end(x)
#endif

#define _paste(x,y) x##y
#define paste(x,y) _paste(x,y)

int main () {
	profile(main);
	for(int i = 0; i < 1<<4; i++) {
		profile(single_sleep);
		std::this_thread::sleep_for(200ms);
	}
}
