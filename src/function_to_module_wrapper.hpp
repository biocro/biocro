/* It would be nice to have a general class that can be given a function and the names of keys in a state_map that match the functions arguments.
 * Then you could do something like the following:
 * IModule * my_module = new f_to_m(some_function, "parm1", "parm2");
 * state_map s {{"parm1", 5}, {"parm2"}, 7}};
 * state_map result = my_module(s);
 *
 */

template<typename F, typename... Args>
class f_to_m : public IModule {
    public:
        f_to_m(F f, Args... args) :
            IModule("f_to_m",
                    std::vector<std::string> {},
                    std::vector<std::string> {}),
           tuple_(std::make_tuple(std::forward<Args>(args)...)),
           wrapped_f(f),
           args_length(sizeof...(args))
       {};

       state_map run(state_map s)
       {

           std::tuple<> some_tuple;
           for (auto element : tuple_) {
               some_tuple = std::tuple_cat(some_tuple, std::make_tuple(s[element]));
           }
           auto some_f = std::bind(wrapped_f, some_tuple);
           state_map result = some_f();
           //state_map result = some_f();
           return result;
       };
    private:
       std::tuple<Args...> tuple_;
       F wrapped_f;
       size_t args_length;
};

