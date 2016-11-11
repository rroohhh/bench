#include <iostream>

template<int N>
struct flag {
  friend constexpr int adl_flag (flag<N>);
};
template<int N>
struct writer {
  friend constexpr int adl_flag (flag<N>) {
    return N;
  }

  static constexpr int value = N;
};
template<int N, int = adl_flag (flag<N> {})>
int constexpr reader (int, flag<N>) {
  return N;
}

template<int N>
int constexpr reader (float, flag<N>, int R = reader (0, flag<N-1> {})) {
  return R;
}

int constexpr reader (float, flag<0>) {
  return 0;
}
template<int N = 1>
int constexpr next (int R = writer<reader (0, flag<32> {}) + N>::value) {
  return R;
}

namespace foonathan {
    namespace string_id {
        namespace detail
        {
            using hash_type = std::uint64_t;
            
            constexpr hash_type fnv_basis = 14695981039346656037ull;
            constexpr hash_type fnv_prime = 109951162821ull;
            
            // FNV-1a 64 bit hash
            constexpr hash_type sid_hash(const char *str, hash_type hash = fnv_basis) noexcept
            {
                return *str ? sid_hash(str + 1, (hash ^ *str) * fnv_prime) : hash;
            }
        }
    }
} 

	template <typename Hash, typename N = std::integral_constant<int, next()>>
       	struct string_int_map {
		using hash = Hash;
		using value = N();
		int n = N();
	};

/*
    namespace fn_detail {
        
         /// Named parameter (could be empty!)
        template <typename Hash, typename... Ts>
        struct named_param : public std::tuple<std::decay_t<Ts>...> {
            using hash = Hash;                                                              ///< key
            
            named_param(Ts&&... ts) : std::tuple<std::decay_t<Ts>...>(std::forward<Ts>(ts)...){ };        ///< constructor
            
            template <typename P>
            named_param<Hash,P> operator=(P&& p){ return named_param<Hash,P>(std::forward<P>(p)); };
            
            
        };
    
        template <typename Hash>
        using make_named_param = named_param<Hash>;
        

        
        /// Named tuple is just tuple of named params
        template <typename... Params>
        struct named_tuple : public std::tuple<Params...>
        {
            
            template <typename... Args>
            named_tuple(Args&&... args) : std::tuple<Args...>(std::forward<Args>(args)...) {}
            
            static const std::size_t error = -1;
            
            template<std::size_t I = 0, typename Hash>
            constexpr typename std::enable_if<I == sizeof...(Params), const std::size_t>::type
            static get_element_index()
            {
                return error;
            }
            
            template<std::size_t I = 0, typename Hash>
            constexpr typename std::enable_if<I < sizeof...(Params), const std::size_t>::type
            static get_element_index()
            {
                using elementType = typename std::tuple_element<I, std::tuple<Params...>>::type;
                return (std::is_same<typename elementType::hash, Hash>::value) ? I : get_element_index<I + 1, Hash>();
            }
            
            template<typename Hash>
            const auto& get() const
            {
                constexpr std::size_t index = get_element_index<0, Hash>();
                static_assert((index != error), "Wrong named tuple key");
                auto& param = (std::get< index >(static_cast<const std::tuple<Params...>&>(*this)));
                return std::get<0>( param );
            }
            
            template<typename NP>
            const auto& operator[](NP&& param)
            {
                return get<typename NP::hash>();
            }
             
        };
        
            

    }
    
    template <typename... Args>
    auto make_named_tuple(Args&&... args)
    {
        return fn_detail::named_tuple<Args...>(std::forward<Args>(args)...);
    }

#define param(x) fn_detail::make_named_param< std::integral_constant<foonathan::string_id::detail::hash_type, foonathan::string_id::detail::sid_hash(x)> >{}
#define key_value(x) fn_detail::make_named_param< std::integral_constant<foonathan::string_id::detail::hash_type, foonathan::string_id::detail::sid_hash(x)> >{}
*/
int main(void) {
	std::cout << string_int_map<std::integral_constant<int, 'a'>>().n << std::endl;
	std::cout << string_int_map<std::integral_constant<int, 'b'>>().n << std::endl;
	std::cout << string_int_map<std::integral_constant<int, 'a'>>().n << std::endl;
}
