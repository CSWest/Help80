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

*/

#include "Parameters.hpp"

Parameters::Parameters(const int p_argc, char const* const* const p_argv, config p_c):
    argc(p_argc),
    argv(p_argv),

    terminal_width(p_c.terminal_width),
    param_to_desc_len(p_c.param_to_desc_len),
    desc_indent_len(p_c.desc_indent_len),
    params_indent_len(p_c.params_indent_len),
    choice_indent_len(p_c.choice_indent_len),
    choice_desc_indent_len(p_c.choice_desc_indent_len),
    right_margin_len(p_c.right_margin_len),
    desc_indent(""),
    choice_indent(""),
    params_indent(""),

    lang(p_c.lang),
    description_is_set(false) {
    for(int i=0 ; i<params_indent_len ; i++) params_indent += " ";
    for(int i=0 ; i<choice_indent_len ; i++) choice_indent += " ";
    for(int i=0 ; i<desc_indent_len ; i++)   desc_indent += " ";
}

Parameters::~Parameters() {
    for(std::pair<std::string, ParamHolder*> p: params) delete p.second;
}

void Parameters::set_program_description(const std::string &p_description) {
    description        = p_description + " ";
    description_is_set = true;
}

void Parameters::set_usage(const std::string &p_usage) {
    usage        = p_usage;
    usage_is_set = true;
}

/*** build help menu ***/

void Parameters::insert_subsection(const std::string& subsection_title) {
    subsections.push_back(subsection_title);
    subs_indexes.push_back(params.size());
}

void Parameters::define_param(const std::string& param_name, const std::string& param_desc) {
    /* check if already exist */
    if(params.count("--" + param_name)) {
        if(lang==lang_fr) throw std::string("erreur : un paramètre de même nom existe déjà");
        else              throw std::string("error: a parameter with the same name already exists");
    }
    /* get type name */
    const std::string type_name = typeid(bool).name();
    /* create param */
    Param<bool>* const p = new Param<bool>("--" + param_name, param_desc);
    /* store param */
    order.insert(std::make_pair(params.size(), "--" + param_name));
    params.insert(std::make_pair("--" + param_name, p));
}

void Parameters::define_choice_param(const std::string& param_name, const std::string& value_name, const std::string& default_choice, vec_choices p_choices, const std::string& param_desc, const bool display_default_value) {
    /* check if already exist */
    if(params.count("--" + param_name)) {
        if(lang==lang_fr) throw std::string("erreur : un paramètre de même nom existe déjà");
        else              throw std::string("error: a parameter with the same name already exists");
    }
    /* get type name */
    const std::string type_name = typeid(std::string).name();
    /* append space in descriptions */
    for(std::pair<std::string, std::string>& p:p_choices) p.second += " ";
    /* create param */
    Param<std::string>* const p = new Param<std::string>("--" + param_name, param_desc, {value_name}, {default_choice}, display_default_value);
    /* store param */
    order.insert(std::make_pair(params.size(), "--" + param_name));
    params.insert(std::make_pair("--" + param_name, p));
    choices.insert(std::make_pair("--" + param_name, p_choices));
    choices_params.insert("--" + param_name);
}

/*** display help menu ***/

void Parameters::print_help(const bool p_print_usage, const bool p_print_description) const {
    if(description_is_set && p_print_description) { print_description(); }
    if(usage_is_set       && p_print_usage)       { std::cout << std::endl; print_usage(); }
    print_parameters();
}

void Parameters::print_description() const {
    std::cout << std::endl;
    /* print description */
    std::string line       = "";
    std::string word       = "";
    bool        first_l    = true;
    bool        first_word = true;
    for(std::size_t j=0 ; j<description.length() ; j++) {
        char c = description.at(j);
        if(c!=' ' && params_indent_len+word.length()<terminal_width-right_margin_len) {
            word += c;
        }
        else {
            if(params_indent_len+line.length()+word.length()+1<=terminal_width-right_margin_len) {
                if(first_word) { line = word; first_word = false; }
                else           { line += " " + word; }
                word = "";
            }
            else {
                /* line would be too long, print it */
                std::cout << params_indent;
                if(params_indent_len+word.length()<terminal_width-right_margin_len) {
                    /* prints line and take a new line */
                    std::cout << line << std::endl;
                    line = word;
                    word = "";
                }
                else {
                    /* no need to take another line, the word will be split anyways */
                    std::size_t line_len;
                    if(line!="") { std::cout << line << " "; line_len = line.length() + 1; }
                    else         { line_len = 0; }
                    std::cout << word.substr(0, terminal_width-right_margin_len-(params_indent_len+line_len)) << std::endl;
                    word = word.substr(terminal_width-right_margin_len-(params_indent_len+line_len));
                    word.push_back(c);
                    line       = "";
                    first_word = true;
                }
            }
        }
    }
    /* print last line */
    std::cout << params_indent << line << std::endl;
}

void Parameters::print_usage() const {
    if(lang==lang_fr) std::cout << "UTILISATION :" << std::endl << std::endl;
    else              std::cout << "USAGE:" << std::endl << std::endl;
    std::cout << params_indent << usage << std::endl << std::endl;
}

void Parameters::print_parameters() const {
    for(std::size_t i=0 ; i<params.size() ; i++) {
        /* print subsection if needed */
        for(std::size_t j=0 ; j<subs_indexes.size() ; j++) {
            if(subs_indexes[j]==i) {
                if(lang==lang_fr) std::cout << std::endl << subsections[j] << " :" << std::endl << std::endl;
                else              std::cout << std::endl << subsections[j] << ":" << std::endl << std::endl;
            }
        }
        
        /* retrieve param */
        ParamHolder* p = params.at(order.at(i));
        /* build use string */
        std::string use = params_indent + p->name;
        for(std::string value_name: p->values_names) use += " <" + value_name + ">";
        
        /* print param and values to take */
        bool desc_on_new_line = false;
        if(use.length()+param_to_desc_len>desc_indent_len) {
            /* print param list now if too long */
            std::cout << use << std::endl;
            desc_on_new_line = true;
        }
        else {
            /* print list and spaces if it fits */
            /* minus one because one space will be added by first description line */
            std::string spaces = "";
            for(int j=0 ; j<desc_indent_len-use.length() ; j++) spaces += " ";
            std::cout << use << spaces;
        }
        
        /* print description */
        std::string line       = "";
        std::string word       = "";
        bool        first_l    = true;
        bool        first_word = true;
        for(std::size_t j=0 ; j<p->description.length() ; j++) {
            char c = p->description.at(j);
            if(c!=' ' && desc_indent_len+word.length()<terminal_width-right_margin_len) {
                word += c;
            }
            else {
                if(desc_indent_len+line.length()+word.length()+1<=terminal_width-right_margin_len) {
                    if(first_word) { line = word; first_word = false; }
                    else           { line += " " + word; }
                    word = "";
                }
                else {
                    /* line would be too long, print it */
                    if(!first_l || desc_on_new_line) std::cout << desc_indent;
                    if(first_l)                      first_l = false;
                    if(desc_indent_len+word.length()<terminal_width-right_margin_len) {
                        /* prints line and take a new line */
                        std::cout << line << std::endl;
                        line = word;
                        word = "";
                    }
                    else {
                        /* no need to take another line, the word will be split anyways */
                        std::size_t line_len;
                        if(line!="") { std::cout << line << " "; line_len = line.length() + 1; }
                        else         { line_len = 0; }
                        std::cout << word.substr(0, terminal_width-right_margin_len-(desc_indent_len+line_len)) << std::endl;
                        word = word.substr(terminal_width-right_margin_len-(desc_indent_len+line_len));
                        word.push_back(c);
                        line       = "";
                        first_word = true;
                    }
                }
            }
        }
        /* print last line */
        if(!first_l || desc_on_new_line) std::cout << desc_indent;
        std::cout << line << std::endl;
        
        /* print choices */
        if(choices_params.count(p->name)) {
            for(const std::pair<std::string, std::string>& p: choices.at(p->name)) {
                /* print choice */
                std::size_t other_len    = (lang==lang_fr) ? 5+p.first.length() : 4+p.first.length();
                bool        sub_desc_onl = false;
                if(other_len+choice_indent_len>30) {
                    /* print choice and new line */
                    if(lang==lang_fr) std::cout << desc_indent << choice_indent << "\"" << p.first << "\" :" << std::endl;
                    else              std::cout << desc_indent << choice_indent << "\"" << p.first << "\":" << std::endl;
                    sub_desc_onl = true;
                    other_len    = choice_indent_len+choice_desc_indent_len;
                }
                else {
                    /* print choice */
                    if(lang==lang_fr) std::cout << desc_indent << choice_indent << "\"" << p.first << "\" : ";
                    else              std::cout << desc_indent << choice_indent << "\"" << p.first << "\": ";
                }
                /* print choice description */
                /* print description */
                std::string spaces     = desc_indent + choice_indent;
                std::string line       = "";
                std::string word       = "";
                bool        first_l    = true;
                bool        first_word = true;
                for(int j=0 ; j<other_len ; j++) spaces += " ";
                for(std::size_t j=0 ; j<p.second.length() ; j++) {
                    char c = p.second.at(j);
                    if(c!=' ' && desc_indent_len+choice_indent_len+other_len+word.length()<terminal_width-right_margin_len) {
                        word += c;
                    }
                    else {
                        if(desc_indent_len+choice_indent_len+other_len+line.length()+word.length()+1<=terminal_width-right_margin_len) {
                            if(first_word) { line = word; first_word = false; }
                            else           { line += " " + word; }
                            word = "";
                        }
                        else {
                            /* line would be too long, print it */
                            if(!first_l || sub_desc_onl) std::cout << spaces;
                            if(first_l)                      first_l = false;
                            if(desc_indent_len+choice_indent_len+other_len+word.length()<terminal_width-right_margin_len) {
                                /* prints line and take a new line */
                                std::cout << line << std::endl;
                                line = word;
                                word = "";
                            }
                            else {
                                /* no need to take another line, the word will be split anyways */
                                std::size_t line_len;
                                if(line!="") { std::cout << line << " "; line_len = line.length() + 1; }
                                else         { line_len = 0; }
                                std::cout << word.substr(0, terminal_width-right_margin_len-(desc_indent_len+choice_indent_len+other_len+line_len)) << std::endl;
                                word = word.substr(terminal_width-right_margin_len-(desc_indent_len+choice_indent_len+other_len+line_len));
                                word.push_back(c);
                                line       = "";
                                first_word = true;
                            }
                        }
                    }
                }
                /* print last line */
                if(!first_l || sub_desc_onl) std::cout << spaces;
                std::cout << line << std::endl;
            }
        }
        
        /* print default value */
        if(p->display_default_value) {
            if(p->type_name==typeid(int).name()) {
                /* reinterpret with the good type */
                const Param<int>* const p_reint = dynamic_cast<Param<int>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(long int).name()) {
                /* reinterpret with the good type */
                const Param<long int>* const p_reint = dynamic_cast<Param<long int>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(long long int).name()) {
                /* reinterpret with the good type */
                const Param<long long int>* const p_reint = dynamic_cast<Param<long long int>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(unsigned long int).name()) {
                /* reinterpret with the good type */
                const Param<unsigned long int>* const p_reint = dynamic_cast<Param<unsigned long int>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(unsigned long long int).name()) {
                /* reinterpret with the good type */
                const Param<unsigned long long int>* const p_reint = dynamic_cast<Param<unsigned long long int>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(float).name()) {
                /* reinterpret with the good type */
                const Param<float>* const p_reint = dynamic_cast<Param<float>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(double).name()) {
                /* reinterpret with the good type */
                const Param<double>* const p_reint = dynamic_cast<Param<double>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(long double).name()) {
                /* reinterpret with the good type */
                const Param<long double>* const p_reint = dynamic_cast<Param<long double>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " " << p_reint->def_values[j]; if(j<p->nb_values-1) std::cout << ","; }
                std::cout << std::endl;
            }
            else if(p->type_name==typeid(std::string).name()) {
                const Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
                if(lang==lang_fr) std::cout << desc_indent << "Défaut :";
                else              std::cout << desc_indent << "Default:";
                for(std::size_t j=0 ; j<p->nb_values ; j++) { std::cout << " \"" << p_reint->def_values[j] << "\""; if(j<p->nb_values-1) std::cout << "\","; }
                std::cout << std::endl;
            }
        }
        
        /* skip line */
        std::cout << std::endl;
    }
}

/*** use parameters ***/

void Parameters::parse_params() {
    for(int i=1 ; i<argc ; i++) {
        /* get arg name */
        const std::string line_param(argv[i]);
        if(params.count(line_param)) {
            /* retrieve param */
            ParamHolder* const p = params[line_param];
            /* read param values */
            for(int j=0 ; j<p->nb_values ; j++) {
                if(++i<=argc) {
                    std::string arg_value(argv[i]);
                    if(p->type_name==typeid(int).name()) {
                        /* reinterpret with the good type */
                        Param<int>* const p_reint = dynamic_cast<Param<int>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stoi(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            int max = std::numeric_limits<int>::max();
                            int min = std::numeric_limits<int>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(long int).name()) {
                        /* reinterpret with the good type */
                        Param<long int>* const p_reint = dynamic_cast<Param<long int>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stol(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            long int max = std::numeric_limits<long int>::max();
                            long int min = std::numeric_limits<long int>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(long long int).name()) {
                        /* reinterpret with the good type */
                        Param<long long int>* const p_reint = dynamic_cast<Param<long long int>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stoll(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            long long int max = std::numeric_limits<long long int>::max();
                            long long int min = std::numeric_limits<long long int>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(unsigned long int).name()) {
                        /* reinterpret with the good type */
                        Param<unsigned long int>* const p_reint = dynamic_cast<Param<unsigned long int>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stoul(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            unsigned long int max = std::numeric_limits<unsigned long int>::max();
                            unsigned long int min = std::numeric_limits<unsigned long int>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(unsigned long long int).name()) {
                        /* reinterpret with the good type */
                        Param<unsigned long long int>* const p_reint = dynamic_cast<Param<unsigned long long int>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stoull(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            unsigned long long int max = std::numeric_limits<unsigned long long int>::max();
                            unsigned long long int min = std::numeric_limits<unsigned long long int>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(float).name()) {
                        /* reinterpret with the good type */
                        Param<float>* const p_reint = dynamic_cast<Param<float>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stof(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            float max = std::numeric_limits<float>::max();
                            float min = std::numeric_limits<float>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(double).name()) {
                        /* reinterpret with the good type */
                        Param<double>* const p_reint = dynamic_cast<Param<double>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stod(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            double max = std::numeric_limits<double>::max();
                            double min = std::numeric_limits<double>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(long double).name()) {
                        /* reinterpret with the good type */
                        Param<long double>* const p_reint = dynamic_cast<Param<long double>* const>(p);
                        /* update value */
                        try { p_reint->values[j] = std::stold(arg_value); }
                        catch(const std::invalid_argument& e) {
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" attend une valeur entière, et a reçu \"" << arg_value << "\"";
                            else              std::cerr << "parameter \"" << line_param << "\" expects an integer value, received \"" << arg_value << "\"";
                        }
                        catch(const std::out_of_range& e) {
                            long double max = std::numeric_limits<long double>::max();
                            long double min = std::numeric_limits<long double>::min();
                            if(lang==lang_fr) std::cerr << "le paramètre \"" << line_param << "\" doit être compris entre " << min << " et " << max << " mais a reçu " << arg_value;
                            else              std::cerr << "parameter \"" << line_param << "\" must be between " << min << " and " << max << " but received " << arg_value;
                        }
                    }
                    else if(p->type_name==typeid(std::string).name()) {
                        /* reinterpret with the good type */
                        Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
                        /* update value */
                        p_reint->values[j] = arg_value;
                    }
                }
                else {
                    if(lang==lang_fr) std::cerr << "erreur : le paramètre \"" << line_param << "\" attend " << p->nb_values << " valeurs" << std::endl;
                    else              std::cerr << "error: parameter \"" << line_param << "\" expects " << p->nb_values << " values" << std::endl;
                }
            }
            /* arg is defined */
            p->is_defined = true;
        }
        else {
            if(lang==lang_fr) std::cerr << "erreur : paramètre \"--" << line_param << "\" inconnu" << std::endl;
            else              std::cerr << "unknown parameter \"" << line_param << "\"" << std::endl;
        }
    }
}

const bool Parameters::is_def(const std::string& param_name) const {
    if(params.count("--" + param_name)) {
        ParamHolder* const p = params.at("--" + param_name);
        return p->is_defined;
    }
    else {
        if(lang==lang_fr) throw std::string("erreur : paramètre \"--" + param_name + "\" inconnu");
        else              throw std::string("error: unknown parameter \"--" + param_name + "\"");
    }
}

const std::string Parameters::str_val(const std::string& param_name, const int value_number) const {
    if(params.count("--" + param_name)) {
        Parameters::ParamHolder* const p = params.at("--" + param_name);
        if(value_number>p->nb_values) {
            throw std::string("parameter \"--" + param_name + "\" only has " + std::to_string(p->nb_values) + " values");
        }
        else {
            /* reinterpret with the good type */
            Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
            /* return value */
            return p_reint->values[value_number-1];
        }
    }
    else {
        throw std::string("error: unknown parameter \"--" + param_name + "\"");
    }
}

const std::string Parameters::cho_val(const std::string& param_name) const {
    if(params.count("--" + param_name)) {
        Parameters::ParamHolder* const p = params.at("--" + param_name);
        /* reinterpret with the good type */
        Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
        /* return value */
        return p_reint->values[0];
    }
    else {
        throw std::string("error: unknown parameter \"--" + param_name + "\"");
    }
}
