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

//state_map update_state(state_map const &state, state_map const &change_in_state);
void update_state(state_map &state, state_map const &change_in_state);

state_vector_map allocate_state(state_map const &m, size_t n);

void append_state_to_vector(state_map const &state, state_vector_map &state_vector);

std::vector<std::string> keys(state_map const &state);

std::vector<std::string> keys(state_vector_map const &state);

state_map at(state_vector_map const &vector_map, std::vector<double>::size_type const n);



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

state_map abs(state_map x);

#endif

