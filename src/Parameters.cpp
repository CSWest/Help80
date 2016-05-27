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

    min_terminal_width(p_c.min_terminal_width),
    max_terminal_width(p_c.max_terminal_width),
    terminal_width(get_terminal_width()<=max_terminal_width ? (get_terminal_width()>=min_terminal_width ? get_terminal_width() : min_terminal_width) : max_terminal_width),
    params_indent_len(p_c.params_indent_len),
    param_to_desc_len(p_c.param_to_desc_len),
    desc_indent_len(p_c.desc_indent_len),
    choice_indent_len(p_c.choice_indent_len),
    choice_desc_indent_len(p_c.choice_desc_indent_len),
    right_margin_len(p_c.right_margin_len),
    params_indent(""),
    desc_indent(""),
    choice_indent(""),
    choice_desc_indent(""),

    lang(p_c.lang),
    description_is_set(false) {
    for(int i=0 ; i<params_indent_len ; i++)      params_indent += " ";
    for(int i=0 ; i<desc_indent_len ; i++)        desc_indent += " ";
    for(int i=0 ; i<choice_indent_len ; i++)      choice_indent += " ";
    for(int i=0 ; i<choice_desc_indent_len ; i++) choice_desc_indent += " ";
}

Parameters::Parameters(const int p_argc, char const* const* const p_argv, config p_c, const int p_terminal_width):
    argc(p_argc),
    argv(p_argv),

    min_terminal_width(p_c.min_terminal_width),
    max_terminal_width(p_c.max_terminal_width),
    terminal_width(p_terminal_width<=max_terminal_width ? p_terminal_width : max_terminal_width),
    params_indent_len(p_c.params_indent_len),
    param_to_desc_len(p_c.param_to_desc_len),
    desc_indent_len(p_c.desc_indent_len),
    choice_indent_len(p_c.choice_indent_len),
    choice_desc_indent_len(p_c.choice_desc_indent_len),
    right_margin_len(p_c.right_margin_len),
    params_indent(""),
    desc_indent(""),
    choice_indent(""),
    choice_desc_indent(""),

    lang(p_c.lang),
    description_is_set(false) {
    for(int i=0 ; i<params_indent_len ; i++)      params_indent += " ";
    for(int i=0 ; i<desc_indent_len ; i++)        desc_indent += " ";
    for(int i=0 ; i<choice_indent_len ; i++)      choice_indent += " ";
    for(int i=0 ; i<choice_desc_indent_len ; i++) choice_desc_indent += " ";
}

Parameters::~Parameters() {
    for(std::pair<std::string, ParamHolder*> p: params) delete p.second;
}

/*** static functions ***/

const int Parameters::get_terminal_width() {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        /* linux, mac */
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    #elif PLATFORM == PLATFORM_WINDOWS
        /* windows */
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    #endif
}

const std::string Parameters::bold(const std::string& str) {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        return "\e[1m" + str + "\e[0m";
    #else
        return str;
    #endif
}

const std::string Parameters::underline(const std::string& str) {
    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        return "\e[4m" + str + "\e[0m";
    #else
        return str;
    #endif
}

/*** setters ***/

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
        throw DuplicateParameterException(param_name, "Parameters::define_param", lang);
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
        throw DuplicateParameterException(param_name, "Parameters::define_param", lang);
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

void Parameters::print_text(const std::string& text, const bool start_on_new_line, const int indent_len, const std::string& indent, ParamHolder* const p) const {
    std::string line       = "";
    std::string word       = "";
    bool        first_line = true;
    bool        first_word = true;
    int         word_pad   = 0;     // used when replacing $_x by value name
    bool        in_par_val = false;
    /* lambda function to replace $_x by value name */
    auto replace_by_value_name = [&word, &p, &in_par_val] () {
        std::string value_num_str = word.substr(2);
        int         value_num     = stoi(value_num_str);
        if(value_num>0) {
            if(word.size()>3) word = "<" + underline(p->values_names.at(value_num-1)) + ">" + word.substr(3);
            else              word = "<" + underline(p->values_names.at(value_num-1)) + ">";
            #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
                in_par_val = true;
            #endif
        }
        else {
            if(word.size()>3) word = "\"" + bold(p->name) + "\"" + word.substr(3);
            else              word = "\"" + bold(p->name) + "\"";
        }
    };
    /* for each character in the text */
    for(std::size_t j=0 ; j<text.length() ; j++) {
        char c = text.at(j);
        if(c!=' ' && c!= '\n' && indent_len+static_cast<int>(word.length())<terminal_width-right_margin_len) {
            /* we are in a middle of a word and we still have space so far */
            word += c;
        }
        else {
            /* text decoration */
            if(word.find("$_")==0) {
                replace_by_value_name();
                #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
                    word_pad = 8;
                #endif
            }
            else if(word.find("$p(")==0) {
                std::size_t end_ind = word.find(")");
                if(word.at(word.length()-1)!=')') word = "\"--" + bold(word.substr(3, end_ind-3)) + "\"" + bold(word.substr(end_ind+1));
                else                              word = "\"--" + bold(word.substr(3, end_ind-3)) + "\"";
                #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
                    word_pad = 8;
                #endif
            }
            /* display word */
            if(indent_len+static_cast<int>(line.length()+word.length())-word_pad+1<=terminal_width-right_margin_len) {
                /* if this word plus the previous space can fit, print it */
                if(c!='\n') {
                    if(first_word) { line = word; first_word = false; }
                    else           { line += " " + word; }
                    word = "";
                }
                /* if a new line is requested, take a new line */
                else {
                    if(first_word) { line = word; first_word = false; }
                    else           { line += " " + word; }
                    if(!first_line || start_on_new_line) std::cout << indent;
                    if(first_line)                       first_line = false;
                    std::cout << line << std::endl;
                    first_word = true;
                    line       = "";
                    word       = "";
                }
            }
            else {
                /* line would be too long, we need to go on next line */
                if(!first_line || start_on_new_line) std::cout << indent;
                if(first_line)                       first_line = false;
                if(indent_len+static_cast<int>(word.length())-word_pad<terminal_width-right_margin_len) {
                    /* the next word can fit on a line in its entirety, so print the current line and take a new line */
                    std::cout << line << std::endl;
                    line = word;
                    word = "";
                }
                else {
                    /* the next word cannot fit on a line in its entirety, so fill the current line and take a new line */
                    /* the word will be split. Careful if first word is too long */
                    int line_len;
                    if(line!="") { std::cout << line << " "; line_len = static_cast<int>(line.length() + 1); }
                    else         { line_len = 0; }
                    int ind = terminal_width-(right_margin_len+indent_len+line_len);
                    if(ind<0) ind=0;
                    std::cout << word.substr(0, static_cast<std::size_t>(word_pad/2 + ind));
                    /* disable underline */
                    if(in_par_val) std::cout << "\e[0m";
                    std::cout << std::endl;
                    if(in_par_val) { word = "\e[4m" + word.substr(static_cast<std::size_t>(word_pad/2 + ind)); word_pad = 4; }
                    else           { word = word.substr(static_cast<std::size_t>(word_pad/2 + ind)); }
                    word.push_back(c);
                    line       = "";
                    first_word = true;
                }
            }
            if(word_pad==8) word_pad   = 0;
            in_par_val = false;
        }
    }
    /* print last line */
    if(!first_line || start_on_new_line) std::cout << indent;
    std::cout << line << std::endl;
}

void Parameters::print_help(const bool p_print_usage, const bool p_print_description) const {
    if(description_is_set && p_print_description) { print_description(); }
    if(usage_is_set       && p_print_usage)       { print_usage(); }
    print_parameters();
}

void Parameters::print_description() const {
    std::cout << std::endl;
    if(lang==lang_fr) std::cout << bold("DESCRIPTION :") << std::endl;
    else              std::cout << bold("DESCRIPTION:") << std::endl;
    /* print description */
    print_text(description, true, params_indent_len, params_indent, nullptr);
}

void Parameters::print_usage() const {
    std::cout << std::endl;
    if(lang==lang_fr) std::cout << bold("UTILISATION :") << std::endl;
    else              std::cout << bold("USAGE:") << std::endl;
    std::cout << params_indent << usage << std::endl;
}

void Parameters::print_parameters() const {
    std::cout << std::endl;
    for(std::size_t i=0 ; i<params.size() ; i++) {
    
        /* print subsection if needed */
        for(std::size_t j=0 ; j<subs_indexes.size() ; j++) {
            if(subs_indexes[j]==i) {
                if(lang==lang_fr) std::cout << bold(subsections[j] + " :") << std::endl;
                else              std::cout << bold(subsections[j] + ":") << std::endl;
            }
        }
        
        /* retrieve param */
        ParamHolder* p = params.at(order.at(i));
        /* build use string */
        std::string use = params_indent + bold(p->name);
        for(std::string value_name: p->values_names) use += " <" + underline(value_name) + ">";
        
        /* print param and values to take */
        bool desc_on_new_line = false;
        int  len_adjust       = 0;
        #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            len_adjust = 8 + p->nb_values*8;
        #endif
        if(static_cast<int>(use.length())-len_adjust+param_to_desc_len>desc_indent_len) {
            /* print param list now if too long */
            std::cout << use << std::endl;
            desc_on_new_line = true;
        }
        else {
            /* print list and spaces if it fits */
            /* minus one because one space will be added by first description line */
            std::string spaces = "";
            for(int j=0 ; j<desc_indent_len-static_cast<int>(use.length())+len_adjust ; j++) spaces += " ";
            std::cout << use << spaces;
        }
        
        /* print description */
        print_text(p->description, desc_on_new_line, desc_indent_len, desc_indent, p);
        
        /* print choices */
        if(choices_params.count(p->name)) {
            for(const std::pair<std::string, std::string>& pc: choices.at(p->name)) {
                /* print choice and new line */
                if(lang==lang_fr) std::cout << desc_indent << choice_indent << "\"" << bold(pc.first) << "\" :" << std::endl;
                else              std::cout << desc_indent << choice_indent << "\"" << bold(pc.first) << "\":" << std::endl;
                /* print choice description */
                const std::string indent = desc_indent + choice_indent + choice_desc_indent;
                print_text(pc.second, true, static_cast<int>(indent.size()), indent, p);
            }
        }
        
        /* print default value */
        if(p->display_default_value) {
            if(p->type_name==typeid(short int).name())                   pr_def<short int>(p);
            else if(p->type_name==typeid(unsigned short int).name())     pr_def<unsigned short int>(p);
            else if(p->type_name==typeid(int).name())                    pr_def<int>(p);
            else if(p->type_name==typeid(unsigned int).name())           pr_def<unsigned int>(p);
            else if(p->type_name==typeid(long int).name())               pr_def<long int>(p);
            else if(p->type_name==typeid(unsigned long int).name())      pr_def<unsigned long int>(p);
            else if(p->type_name==typeid(long long int).name())          pr_def<long long int>(p);
            else if(p->type_name==typeid(unsigned long long int).name()) pr_def<unsigned long long int>(p);
            else if(p->type_name==typeid(float).name())                  pr_def<float>(p);
            else if(p->type_name==typeid(double).name())                 pr_def<double>(p);
            else if(p->type_name==typeid(long double).name())            pr_def<long double>(p);
            else if(p->type_name==typeid(std::string).name())            pr_def<std::string>(p, true);
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
            for(std::size_t j=0 ; j<static_cast<std::size_t>(p->nb_values) ; j++) {
                if(++i<argc) {
                    std::string arg_value(argv[i]);
                    if(p->type_name==typeid(short int).name()) {
                        Param<short int>* const p_reint = dynamic_cast<Param<short int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        int                     tmp_val = 0;
                        try { tmp_val = std::stoi(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<short int>(line_param, arg_value, "Parameters::parse_params", lang); }
                        /* arg_value is a valid unsigned long int, but is it a valid unsigned int */
                        short int min = std::numeric_limits<short int>::min();
                        short int max = std::numeric_limits<short int>::max();
                        if(tmp_val<static_cast<int>(min) || tmp_val>static_cast<int>(max)) { throw ValueOutOfRangeException<short int>(line_param, arg_value, "Parameters::parse_params", lang); }
                        else { p_reint->values[j] = static_cast<short int>(tmp_val); }
                    }
                    else if(p->type_name==typeid(unsigned short int).name()) {
                        Param<unsigned short int>* const p_reint = dynamic_cast<Param<unsigned short int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        unsigned long int                tmp_val = 0;
                        try { tmp_val = std::stoul(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<unsigned short int>(line_param, arg_value, "Parameters::parse_params", lang); }
                        /* arg_value is a valid unsigned long int, but is it a valid unsigned int */
                        unsigned short int min = std::numeric_limits<unsigned short int>::min();
                        unsigned short int max = std::numeric_limits<unsigned short int>::max();
                        if(tmp_val<static_cast<unsigned long int>(min) || tmp_val>static_cast<unsigned long int>(max)) { throw ValueOutOfRangeException<unsigned short int>(line_param, arg_value, "Parameters::parse_params", lang); }
                        else { p_reint->values[j] = static_cast<unsigned short int>(tmp_val); }
                    }
                    else if(p->type_name==typeid(int).name()) {
                        Param<int>* const p_reint = dynamic_cast<Param<int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stoi(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<int>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(unsigned int).name()) {
                        Param<unsigned int>* const p_reint = dynamic_cast<Param<unsigned int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        unsigned long int          tmp_val = 0;
                        try { tmp_val = std::stoul(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<unsigned int>(line_param, arg_value, "Parameters::parse_params", lang); }
                        /* arg_value is a valid unsigned long int, but is it a valid unsigned int */
                        unsigned int min = std::numeric_limits<unsigned int>::min();
                        unsigned int max = std::numeric_limits<unsigned int>::max();
                        if(tmp_val<static_cast<unsigned long int>(min) || tmp_val>static_cast<unsigned long int>(max)) { throw ValueOutOfRangeException<unsigned int>(line_param, arg_value, "Parameters::parse_params", lang); }
                        else { p_reint->values[j] = static_cast<unsigned int>(tmp_val); }
                    }
                    else if(p->type_name==typeid(long int).name()) {
                        Param<long int>* const p_reint = dynamic_cast<Param<long int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stol(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<long int>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(unsigned long int).name()) {
                        Param<unsigned long int>* const p_reint = dynamic_cast<Param<unsigned long int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stoul(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<unsigned long int>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(long long int).name()) {
                        Param<long long int>* const p_reint = dynamic_cast<Param<long long int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stoll(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<long long int>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(unsigned long long int).name()) {
                        Param<unsigned long long int>* const p_reint = dynamic_cast<Param<unsigned long long int>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stoull(arg_value); }
                        catch(const std::invalid_argument& e) { throw IntegerExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<unsigned long long int>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(float).name()) {
                        Param<float>* const p_reint = dynamic_cast<Param<float>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stof(arg_value); }
                        catch(const std::invalid_argument& e) { throw DecimalExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<float>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(double).name()) {
                        Param<double>* const p_reint = dynamic_cast<Param<double>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stod(arg_value); }
                        catch(const std::invalid_argument& e) { throw DecimalExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<double>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(long double).name()) {
                        Param<long double>* const p_reint = dynamic_cast<Param<long double>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        try { p_reint->values[j] = std::stold(arg_value); }
                        catch(const std::invalid_argument& e) { throw DecimalExpectedException(line_param, arg_value, "Parameters::parse_params", lang); }
                        catch(const std::out_of_range& e)     { throw ValueOutOfRangeException<long double>(line_param, arg_value, "Parameters::parse_params", lang); }
                    }
                    else if(p->type_name==typeid(std::string).name()) {
                        Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
                        if(p_reint==0) throw DynamicCastFailedException(line_param, "Parameters::parse_params", lang);
                        p_reint->values[j] = arg_value;
                        /* check if available value for multiple choice */
                        if(choices_params.count(p->name)) {
                            bool ok = false;
                            for(const std::pair<std::string, std::string> choice: choices.at(p->name)) {
                                if(choice.first==arg_value) {
                                    ok = true;
                                    break;
                                }
                            }
                            if(!ok) {
                                throw UnknownChoiceException(p->name, arg_value, "Parameters::parse_params", lang);
                            }
                        }
                    }
                }
                else {
                    throw NotEnoughValuesException(p->name, p->nb_values, static_cast<int>(j), "Parameters::parse_params", lang);
                }
            }
            /* arg is defined */
            p->is_defined = true;
        }
        else {
            throw UnknownParameterException(line_param, "Parameters::parse_params", lang);
        }
    }
}

const bool Parameters::is_spec(const std::string& param_name) const {
    if(params.count("--" + param_name)) {
        ParamHolder* const p = params.at("--" + param_name);
        return p->is_defined;
    }
    else {
        throw UndefinedParameterException(param_name, "Parameters::is_spec", lang);
    }
}

const std::string Parameters::str_val(const std::string& param_name, const int value_number) const {
    if(params.count("--" + param_name)) {
        Parameters::ParamHolder* const p = params.at("--" + param_name);
        if(value_number>p->nb_values) {
            throw UndefinedValueException(param_name, p->nb_values, value_number, "Parameters::str_val", lang);
        }
        else {
            /* reinterpret with the good type */
            Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
            if(p_reint==0) throw DynamicCastFailedException(param_name, "Parameters::str_val", lang);
            /* return value */
            return p_reint->values[static_cast<std::size_t>(value_number-1)];
        }
    }
    else {
        throw UndefinedParameterException(param_name, "Parameters::is_def", lang);
    }
}

const std::string Parameters::cho_val(const std::string& param_name) const {
    if(params.count("--" + param_name)) {
        Parameters::ParamHolder* const p = params.at("--" + param_name);
        /* reinterpret with the good type */
        Param<std::string>* const p_reint = dynamic_cast<Param<std::string>* const>(p);
        if(p_reint==0) throw DynamicCastFailedException(param_name, "Parameters::cho_val", lang);
        /* return value */
        return p_reint->values[0];
    }
    else {
        throw UndefinedParameterException(param_name, "Parameters::is_def", lang);
    }
}
