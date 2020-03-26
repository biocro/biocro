#ifndef STATE_MAP_H
#define STATE_MAP_H

#include <unordered_map>
#include <vector>
#include <string>

typedef std::unordered_map<std::string, std::vector<double>> state_vector_map;
typedef std::unordered_map<std::string, double> state_map;

/*
class state_map
{
    typedef std::unordered_map<std::string, double> sm;

public:
    typedef sm::iterator iterator;
    typedef sm::const_iterator const_iterator;

public:
    state_map()
        : map_()
    {
    }

// ... [ implement container interface ]
//]
    double& operator[]( std::string const& key )
    { return map_[key]; }

    double& at(std::string const& key)
    { return map_.at(key); }

    const double& at(std::string const& key) const
    { return map_.at(key); }

    iterator begin()
    { return map_.begin(); }

    const_iterator begin() const
    { return map_.begin(); }

    iterator end()
    { return map_.end(); }
    
    const_iterator end() const
    { return map_.end(); }

    size_t size() const 
    { return map_.size(); }


private:
    std::unordered_map<std::string, double> map_;

};
*/

state_map combine_state(state_map state_a, state_map const &state_b);

void output_map(state_map const &m);

state_map replace_state(state_map const &state, state_map const &newstate);

state_map replace_or_insert_state(state_map const &state, state_map const &new_state);

//state_map update_state(state_map const &state, state_map const &change_in_state);
void update_state(state_map &state, state_map const &change_in_state);

state_vector_map allocate_state(state_map const &m, size_t n);

void append_state_to_vector(state_map const &state, state_vector_map &state_vector);

template <typename map_with_string_key_type>
std::vector<std::string> keys(map_with_string_key_type const& map)
{
    std::vector<std::string> result;
    for (auto const& it : map) {
        result.push_back(it.first);
    }
    return result;
}

state_map at(state_vector_map const &vector_map, std::vector<double>::size_type const n);

template <typename name_list_type>
state_map state_map_from_names(name_list_type names)
{
    state_map m;
    for (std::string const& n : names) {
        m[n] = 0;
    }
    return m;
}



state_map& operator+=(state_map &lhs, state_map const &rhs);

inline state_map operator+(state_map lhs, state_map const &rhs)
{
    lhs += rhs;
    return lhs;
}

state_map& operator+=(state_map &x, double const a);

inline state_map operator+(state_map x, double const a)
{
    x += a;
    return x;
}

inline state_map operator+(double const a, state_map x)
{
    x += a;
    return x;
}

state_map& operator*=(state_map &x, double const a);

inline state_map operator*(state_map x, double const a)
{
    x *= a;
    return x;
}

inline state_map operator*(double const a, state_map x)
{
    x *= a;
    return x;
}

state_map operator/(state_map lhs, state_map const& rhs);

state_map& operator/=(state_map& lhs, double const a);

state_map abs(state_map x);


class safe_state_map {
private:
        state_map s;
public:
        safe_state_map(state_map s_) : s(s_)
        {};

        double at(std::string const& str) {
            try {
                return s.at(str);
            } catch (std::out_of_range const& e) {
                throw std::out_of_range(std::string(e.what()) + " " + str);
            }
        };
};

class check_state : public state_map {
    public:
    check_state(state_map s) : state_map(s) {};
    double at(std::string key)
    {
        try {
            return state_map::at(key);
        } catch (std::exception &e) {
            throw std::out_of_range(key);
        }
    }
};

#endif

