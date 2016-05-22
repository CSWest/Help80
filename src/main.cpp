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
    Parameters::config p_c {90, 2, 13, 5, 3, 10, 2, Parameters::lang_us};
    Parameters p(argc, argv, p_c);
    
    /* set program description and usage */
    p.set_program_description("These classes are a set of tools for the management of command line arguments. By using these classes in your project, you do not need to worry about designing a help menu or about retrieving the parameters' values anymore. Everything is handled by the class Parameters and its functions, and make sure your menu looks nice even in 80-characters-wide terminals.");
    p.set_usage("software --width <value>");
    
    /* irst subsection */
    p.insert_subsection("FIRST SUBSECTION");
    p.define_param("help", "Displays this help.");
    p.define_num_str_param<int>("width", {"value"}, {80}, "This parameter would allow the user to set the width of the terminal. The default value, 80, is automatically printed under this short description since the function is called with optional parameter 'display_default_value' set to true. This description, although longer than 80 characters, keeps a nice appearence in the help menu.", true);
    p.define_num_str_param<double>("multiple", {"v1", "v2", "v3"}, {0.5, 0.7, 10}, "This parameter expects three values. Since the parameter's name and the list of values is a long string, this description paragraph is printed on a new line. Default values are defined but aren't listed below.");
    p.define_choice_param("mode", "choice", "second_mode",
                             {{"first_mode", "this mode is the first available mode. The description of the choices also fits in the terminal."},
                              {"second_mode", "this mode is another available mode. This is the default mode, as printed just below."},
                              {"third_mode", "the third mode. If_there_is_a_really_long_word_that_wouldnt_fit_on_one_line_it_is_split_at_the_end_of_this_line_and_this_works_on_multiple_lines."},
                              {"fourth_mode_super_super_long", "last one. If_there_is_a_long_word that would fit on the next line, it goes on the next line."}},
                          "You can also define multiple choices parameters. You need to give the multiple choices in an array of pairs (choice, choice_description):", true);
    
    /* new subsection */
    p.insert_subsection("SECOND SUBSECTION");
    p.define_param("first_option", "You can also define parameters with no values, just like this one. No default value is printed.");
    p.define_param("second_option", "If there is_a_really_long_word_that_wouldnt_fit_on_one_line_it_is_split_at_the_end_of_this_line_and_this_works_on_multiple_lines.");
    p.define_param("third_option", "If there is_a_really_long_word_that_would_fit_on_the_next_line it goes on the next line.");
    p.define_param("fourth_really_long_option", "If there is_a_really_long_word_that_wouldnt_fit_on_one_line_it_is_split_at_the_end_of_this_line_and_this_works_on_multiple_lines.");
    p.define_param("fifth_really_long_option", "If there is_a_really_long_word_that_would_fit_on_the_next_line it goes on the next line.");
    p.define_param("sixth_really_long_option", "One_word.");
    p.define_param("seventh_really_long_option", "Normal option.");
    
    /* display description, usage, help */
    p.print_help(true, true);
    
    /* parse p from command line */
    p.parse_params();
    
    /* retrieve values */
    std::cout << std::endl;
    std::cout << "Now we can print some values:" << std::endl;
    std::cout << "  help: "                      << p.is_def("help") << std::endl;
    std::cout << "  width: "                     << p.num_val<int>("width") << std::endl;
    std::cout << "  multiple: "                  << p.num_val<double>("multiple", 1) << ", "
                                                 << p.num_val<double>("multiple", 2) << ", "
                                                 << p.num_val<double>("multiple", 3) << std::endl;
    std::cout << "  mode: \""                    << p.cho_val("mode") << "\"" << std::endl;
    std::cout << "  first_option: "              << p.is_def("first_option") << std::endl;
    std::cout << "  second_option: "             << p.is_def("second_option") << std::endl;
    std::cout << "  third_option: "              << p.is_def("third_option") << std::endl;
    std::cout << "  fourth_really_long_option: " << p.is_def("fourth_really_long_option") << std::endl;

    return 0;    
    
}
