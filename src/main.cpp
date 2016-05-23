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

#include <iostream>

#include "Parameters.hpp"

int main(int argc, char** argv) {

    /* create object */
    Parameters::config p_c {80, 3, 1, 23, 5, 7, 2, Parameters::lang_us};
    Parameters p(argc, argv, p_c);
    
    /* set program description and usage */
    p.set_program_description("These classes are a set of tools for the management of command line arguments. By using these classes in your project, you do not need to worry about designing a help menu or about retrieving the parameters' values anymore. Everything is handled by the class Parameters and its functions, and make sure your menu looks nice even in 80-characters-wide terminals.");
    p.set_usage("software --width <value>");
    
    /* irst subsection */
    try {
        p.insert_subsection("FIRST SUBSECTION");
        p.define_param("help", "Displays this help.");
        p.define_num_str_param<int>("parameter", {"value"}, {80}, "This parameter would allow the user to set the value for parameter. The default value, 80, is automatically printed under this short description since the function is called with optional parameter 'display_default_value' set to true. This description, although longer than 80 characters, keeps a nice appearence in the help menu.", true);
        p.define_num_str_param<double>("multiple", {"v1", "v2", "v3"}, {0.5, 0.7, 10}, "This parameter expects three values. Since the parameter's name and the list of values is a long string, this description paragraph is printed on a new line. Default values are defined but aren't listed below.");
        p.define_choice_param("mode", "choice", "second_mode",
                                 {{"first_mode", "This mode is the first available mode. The description of the choices also fits in the terminal."},
                                  {"second_mode", "This mode is another available mode. This is the default mode, as printed just below."},
                                  {"third_mode", "The third mode. If there_is_a_really_long_word_that_wouldnt_fit_on_one_line_it_is_split_at_the_end_of_this_line_and_this_works_on_multiple_lines."},
                                  {"fourth_mode_super_super_long", "Last one. If there_is_a_long_word_that_would fit on the next line, it goes on the next line."}},
                              "You can also define multiple choices parameters. You need to give the multiple choices in an array of pairs (choice, choice_description):", true);
        p.define_param("simple_parameter", "This parameter doesn't expect any value.");
        p.define_num_str_param<std::string>("path", {"folder"}, {"./"}, "This parameter expects a std::string.", true);
        
        /* new subsection */
        p.insert_subsection("SECOND SUBSECTION");
        p.define_num_str_param<short int>("short_int", {"value"}, {0}, "Below is the list of supported types. No default value is printed.");
        p.define_num_str_param<unsigned short int>("unsigned_short_int", {"value"}, {0}, "If there is_a_really_long_word_that_wouldnt_fit_on_one_line_it_is_split_at_the_end_of_this_line_and_this_works_on_multiple_lines.");
        p.define_num_str_param<int>("int", {"value"}, {0}, "If there is_a_really_long_word_that_wouldnt_fit_on_one_line_it_is_split_at_the_end_of_this_line_and_this_works_on_multiple_lines.");
        p.define_num_str_param<unsigned int>("unsigned_int", {"value"}, {0}, "If there is_a_really_long_word_that_would_fit_on_the_next_line it goes on the next line.");
        p.define_num_str_param<long int>("long_int", {"value"}, {0}, "If there is_a_really_long_word_that_would_fit_on_the_next_line it goes on the next line.");
        p.define_num_str_param<unsigned long int>("unsigned_long_int", {"value"}, {0}, "Another parameter.");
        p.define_num_str_param<long long int>("long_long_int", {"value"}, {0}, "Another parameter.");
        p.define_num_str_param<unsigned long long int>("unsigned_long_long_int", {"value"}, {0}, "In C++, data types are declarations for memory locations or variables that determine the characteristics of the data that may be stored and the methods (operations) of processing that are permitted involving them. From Wikipedia.");
        p.define_num_str_param<float>("float", {"value"}, {0}, "In computing, floating point is the formulaic representation that approximates a real number so as to support a trade-off between range and precision. Well said.");
        p.define_num_str_param<double>("double", {"value"}, {0}, "Double.");
        p.define_num_str_param<long double>("long_double", {"value"}, {0}, "Last.");
    }
    catch(const std::exception& e) {
        std::cerr << "error while defining arguments: " << e.what() << std::endl;
    }
    
    /* parse p from command line */
    try {
        p.parse_params();
    }
    /* check errors */
    catch(const std::exception& e) {
        std::cerr << "error while parsing arguments:" << std::endl << "   " << e.what() << std::endl;
        std::cerr << "You can get help with parameter \"--help\"." << std::endl;
        return 0;
    }
    /* print help and stop if help is required */
    if(p.is_spec("help") || argc==1) {
        p.print_help();
        return 0;
    }
    
    /* retrieve values */
    try {
        std::cout << std::endl;
        std::cout << "FIRST SUBSECTION:" << std::endl;
        std::cout << "   help:             " << p.is_spec("help") << std::endl;
        std::cout << "   parameter:        " << p.num_val<int>("parameter") << std::endl;
        std::cout << "   multiple:         " << p.num_val<double>("multiple", 1) << ", "
                                             << p.num_val<double>("multiple", 2) << ", "
                                             << p.num_val<double>("multiple", 3) << std::endl;
        std::cout << "   mode:             \"" << p.cho_val("mode") << "\"" << std::endl;
        std::cout << "   simple_parameter: " << p.is_spec("simple_parameter") << std::endl;
        std::cout << "   path:             " << p.str_val("path") << std::endl;
        std::cout << std::endl;
        
        std::cout << "SECOND SUBSECTION:" << std::endl;
        std::cout << "   short_int:              " << p.num_val<short int>("short_int") << std::endl;
        std::cout << "   unsigned_short_int:     " << p.num_val<unsigned short int>("unsigned_short_int") << std::endl;
        std::cout << "   int:                    " << p.num_val<int>("int") << std::endl;
        std::cout << "   unsigned_int:           " << p.num_val<unsigned int>("unsigned_int") << std::endl;
        std::cout << "   long_int:               " << p.num_val<long int>("long_int") << std::endl;
        std::cout << "   unsigned_long_int:      " << p.num_val<unsigned long int>("unsigned_long_int") << std::endl;
        std::cout << "   long_long_int:          " << p.num_val<long long int>("long_long_int") << std::endl;
        std::cout << "   unsigned_long_long_int: " << p.num_val<unsigned long long int>("unsigned_long_long_int") << std::endl;
        std::cout << "   float:                  " << p.num_val<float>("float") << std::endl;
        std::cout << "   double:                 " << p.num_val<double>("double") << std::endl;
        std::cout << "   long double:            " << p.num_val<long double>("long_double") << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "error while retrieving arguments:" << std::endl << "   " << e.what() << std::endl;
    }
    
    return 0;    
    
}
