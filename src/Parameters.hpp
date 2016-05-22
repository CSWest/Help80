/*
    
************************************************************************************************
 
LICENSE

    Help80 - Copyright (C) 2016 - Olivier Deiss - olivier.deiss@gmail.com
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
************************************************************************************************

FILE

    The classes defined in this file can be used for parameters parsing and display, while
    making sure the help menu looks nice on terminals.

    This file defines classes Parameters, ParamHolder and Param. Param is a template function
    that inherits from ParamHolder. Parameters owns a std::vector of ParamHolder, and has access
    to the derived object Param through dynamic_cast.
    
    To store parameters of multiple types, the std::string version of types is available with
    typeid(T).name(). This type is stored in ParamHolder. When arguments are parsed, each type is
    tried, and the right function is called for the right type (among stoi, stod, stold, and
    nothing for std::string parameters).
    
    
                       ---------------                 ----------------
                       | ParamHolder |---------------<>|  Parameters  |
                       ---------------                 ----------------
                              ^
                              |
                              |    ------------
                      -------------| Template |
                      |     Param  --+---------
                      ----------------
 
                         
    ParamHolder and Param are private nested classes inside class Parameters.
    
 
    How to build the menu:
        
        First you can specify a description of your program, and how to use it with functions
        set_program_description and set_usage.
 
        To build the menu, use the functions:
            - define_param :        to define a parameter with no value
            - define_num_str_param: to define a numeric or std::string parameter
            - define_choice_param:  to define a multiple choice parameter
 
        You can add structure by adding subsections with function insert_subsection.
        When the menu is ready, you can print it with print_help().
        
 
    How to use the parameters:
 
        First, call parse_params to analyse the command line arguments. Then,
        use the functions:
            - is_def  :  to know if a simple parameter is specified
            - num_val : to get a numeric value
            - str_val : to get a std::string value
            - cho_val : to get a multiple choice value
 
************************************************************************************************

*/

#ifndef Parameters_hpp
#define Parameters_hpp

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1

/* platform detection */
#if defined(_WIN32)
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

/* headers for terminal width */
#if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    #include <sys/ioctl.h>
    #include <stdio.h>
    #include <unistd.h>
#elif PLATFORM == PLATFORM_WINDOWS
    #include <windows.h>
#endif

/* other headers */
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <vector>

class Parameters {

    class ParamHolder;

    public:
    
        typedef std::vector<std::pair<std::string, std::string>> vec_choices;
    
        enum LANG {lang_fr, lang_us};
    
        struct config {
            const int max_terminal_width;                                                 // max width of terminal
            const int param_to_desc_len;                                                  // nb of spaces between longest param list and descripton
            const int desc_indent_len;                                                    // nb of characters from the left to print description
            const int params_indent_len;                                                  // nb of characters from the left to print param+values
            const int choice_indent_len;                                                  // indentation for choices descriptions
            const int choice_desc_indent_len;                                             // indentation for choice description
            const int right_margin_len;                                                   // nb of chars from the right of the terminal
            LANG      lang;                                                               // language to print the menu in
        };
    
        Parameters(const int, char const* const* const, config);
        Parameters(const int, char const* const* const, config, const int);
        ~Parameters();
    
        static const int         get_terminal_width();                                    // returns current's terminal width
        static const std::string bold(const std::string&);                                // returns the bold version of str
        static const std::string underline(const std::string&);                           // returns the underlined version of str
    
        /* use of parameters */
        template<typename T>
        const T                  num_val(const std::string&, const int=1) const;          // return n-th value for parameter. nb starts at 1
        const std::string        str_val(const std::string&, const int=1) const;          // return n-th value for parameter. nb starts at 1
        const std::string        cho_val(const std::string&)              const;          // returns choice value
        const bool               is_def(const std::string&)               const;          // tells if parameters is defined
        void                     parse_params();                                          // reads cmd line and store args
    
        /* help menu */
        void                     insert_subsection(const std::string&);                   // prints subsection when printing help menu
        void                     print_help(const bool=true, const bool=true) const;      // print help menu
        void                     set_program_description(const std::string&);             // sets program description
        void                     set_usage(const std::string&);                           // sets usage
    
        template<typename T>  // in the order: add a parameter with values, with choices, with no values
        void define_num_str_param(const std::string&, const std::vector<std::string>&, const std::vector<T>&, const std::string&, const bool=false);
        void define_choice_param(const std::string&, const std::string&, const std::string&, vec_choices, const std::string&, const bool=false);
        void define_param(const std::string&, const std::string&);
    
    
    private:
    
        typedef std::map<std::string, ParamHolder* const> map_params;
        typedef std::map<std::string, vec_choices>        map_choices;
        typedef std::map<size_t, std::string>             map_order;
    
    
        template<typename T> void pr_def(ParamHolder* const, const bool=false)   const;   // prints default value
        template<typename T> void pr_ran(const std::string&, const std::string&) const;   // prints expected range
        void                      pr_exi(const std::string&, const std::string&) const;   // prints expected an integer value
        void                      pr_exd(const std::string&, const std::string&) const;   // prints expected a decimal value
        void                      print_description()                            const;   // print program description
        void                      print_usage()                                  const;   // print usage
        void                      print_parameters()                             const;   // print list of parameters
    
        Parameters(const Parameters&);
        Parameters& operator=(const Parameters&);
    
        /* cmd line */
        const int                argc;                                                    // command line args number
        char const* const* const argv;                                                    // command line args values
    
        /* display parameters */
        const int                max_terminal_width;                                      // max width of the terminal
        const int                terminal_width;                                          // the width of the terminal
        const int                param_to_desc_len;                                       // nb of spaces between longest param list and descripton
        const int                desc_indent_len;                                         // nb of characters form the left to print desccription
        const int                params_indent_len;                                       // nb of characters from the left to print param+values
        const int                choice_indent_len;                                       // indentation for choices
        const int                choice_desc_indent_len;                                  // indentation for choices descriptions
        const int                right_margin_len;                                        // nb of chars from the right of the terminal
        std::string              desc_indent;                                             // spaces for indentation of big description
        std::string              choice_indent;                                           // indentation for choices descriptions
        std::string              params_indent;                                           // string of 'params_indent_len' spaces
    
        /* internal vars */
        const LANG               lang;                                                    // language to print the menu in
        std::string              description;                                             // description of the program
        bool                     description_is_set;                                      // true if set_description() is called
        std::string              usage;                                                   // usage of the program
        bool                     usage_is_set;                                            // true if set_usage() is called
        std::vector<std::string> subsections;                                             // sub sections titles in the help menu
        std::vector<std::size_t> subs_indexes;                                            // indexes of the subsections (where to print them)
        map_params               params;                                                  // data structure that stores all the parameters
        map_order                order;                                                   // data structure to store order of parameters
        map_choices              choices;                                                 // stores choices associated to choice-parameters
        std::set<std::string>    choices_params;                                          // stores all the params that are multiple choice
    
    
    private:
    
        class ParamHolder {

            public:
            
                ParamHolder(const std::string& p_name, const std::string& p_description, const std::string& p_type_name, const std::vector<std::string>& p_values_names={}, const bool p_display_default_value=false):
                    name(p_name),
                    description(p_description + " "),
                    nb_values(p_values_names.size()),
                    values_names(p_values_names),
                    type_name(p_type_name),
                    display_default_value(p_display_default_value),
                    is_defined(false) {}
                virtual ~ParamHolder() {}
            
                const std::string              name;                                      // param name with added suffix '--'
                const std::string              description;                               // long description paragraph, can't start or end with ' '
                const std::size_t              nb_values;                                 // nb of values expected for the parameters
                const std::vector<std::string> values_names;                              // name of these values, without '<' and '>'
                const std::string              type_name;                                 // to distinguish the type of the values
                const bool                     display_default_value;                     // if default value has to be displayed in help menu
                bool                           is_defined;                                // if the arg was specified by the user
            
            
            private:
            
                ParamHolder(const ParamHolder&);
                ParamHolder& operator=(const ParamHolder&);

        };

        template<typename T>
        class Param: public ParamHolder {

            public:
            
                Param(const std::string& p_name, const std::string& p_description, const std::vector<std::string>& p_values_names, const std::vector<T>& p_default_values, const bool p_display_default_value):
                    ParamHolder(p_name, p_description, typeid(T).name(), p_values_names, p_display_default_value),
                    values(p_default_values),
                    def_values(p_default_values) {}
                Param(const std::string& p_name, const std::string& p_description):
                    ParamHolder(p_name, p_description, typeid(T).name()) {}
                virtual ~Param() {}
                
                std::vector<T>       values;                                              // parameter values
                const std::vector<T> def_values;                                          // parameter default values

        };

};


/*** template functions definition ***/

template<typename T>
void Parameters::pr_def(ParamHolder* const p, const bool add_quotes) const {
    if(typeid(T).name()!=typeid(int                   ).name() && typeid(T).name()!=typeid(long int         ).name()
    && typeid(T).name()!=typeid(long long int         ).name() && typeid(T).name()!=typeid(unsigned long int).name()
    && typeid(T).name()!=typeid(unsigned long long int).name() && typeid(T).name()!=typeid(float            ).name()
    && typeid(T).name()!=typeid(double                ).name() && typeid(T).name()!=typeid(long double      ).name()
    && typeid(T).name()!=typeid(std::string           ).name()) {
        throw std::string("type not supported yet");
    }
    else {
        /* reinterpret with the good type */
        const Param<T>* const p_reint = dynamic_cast<Param<T>* const>(p);
        if(lang==lang_fr) std::cout << desc_indent << bold("Défaut :");
        else              std::cout << desc_indent << bold("Default:");
        for(std::size_t j=0 ; j<p->nb_values ; j++) {
            if(!add_quotes) { std::cout << " "   << p_reint->def_values[j];         if(j<p->nb_values-1) std::cout << ","; }
            else            { std::cout << " \"" << p_reint->def_values[j] << "\""; if(j<p->nb_values-1) std::cout << ","; }
        }
        std::cout << std::endl;
    }
}

template<typename T>
void Parameters::pr_ran(const std::string& line_param, const std::string& arg_value) const {
    T max = std::numeric_limits<T>::max();
    T min = std::numeric_limits<T>::min();
    if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
    else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
}

template<typename T>
const T Parameters::num_val(const std::string& param_name, const int value_number) const {
    if(params.count("--" + param_name)) {
        Parameters::ParamHolder* const p = params.at("--" + param_name);
        if(value_number>p->nb_values) {
            throw std::string("parameter \"--" + param_name + "\" only has " + std::to_string(p->nb_values) + " values");
        }
        else {
            if(typeid(T).name()!=typeid(int                   ).name() && typeid(T).name()!=typeid(long int         ).name()
            && typeid(T).name()!=typeid(long long int         ).name() && typeid(T).name()!=typeid(unsigned long int).name()
            && typeid(T).name()!=typeid(unsigned long long int).name() && typeid(T).name()!=typeid(float            ).name()
            && typeid(T).name()!=typeid(double                ).name() && typeid(T).name()!=typeid(long double      ).name()) {
                throw std::string("type not supported yet");
            }
            else {
                /* reinterpret with the good type */
                Param<T>* const p_reint = dynamic_cast<Param<T>* const>(p);
                /* return value */
                return p_reint->values[value_number-1];
            }
        }
    }
    else {
        throw std::string("error: unknown parameter \"--" + param_name + "\"");
    }
}

template<typename T>
void Parameters::define_num_str_param(const std::string& param_name, const std::vector<std::string>& values_names, const std::vector<T>& default_param_values, const std::string& param_desc, const bool display_default_value) {
    /* check if already exist */
    if(params.count("--" + param_name)) {
        if(lang==lang_fr) throw std::string("erreur : un paramètre de même nom existe déjà");
        else              throw std::string("error: a parameter with the same name already exists");
    }
    /* get type name */
    const std::string type_name = typeid(T).name();
    /* create param */
    Param<T>* const p = new Param<T>("--" + param_name, param_desc, values_names, default_param_values, display_default_value);
    /* store param */
    order.insert(std::make_pair(params.size(), "--" + param_name));
    params.insert(std::make_pair("--" + param_name, p));
}

#endif
