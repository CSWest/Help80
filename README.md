# Help80

The classes defined in this project can be used for parameters parsing and display, while making sure the help menu looks nice on terminals. The name comes from the default width of terminals. Indeed, most terminals have a default width of 80 characters.

### Use

To use the classes in your own project, just include the files [*Parameters.hpp*](src/Parameters.hpp) and [*Parameters.cpp*](Parameters.cpp) in your project.

#### How to build the menu
        
First you can specify a description of your program, and how to use it with the functions:
* `set_program_description`
* `set_usage`.
 
To build the menu, use the functions:
* `define_param` : to define a parameter with no value
* `define_num_str_param`: to define a numeric or `std::string` parameter
* `define_choice_param`: to define a multiple choice parameter
 
You can add structure by adding subsections with function `insert_subsection`.
When the menu is ready, you can print it with `print_help`.
        
#### How to use the parameters
 
First, call `parse_params` to analyse the command line arguments. Then, use the functions:
* `is_def` :  to know if a simple parameter is specified
* `num_val` : to get a numeric value
* `str_val` : to get a `std::string` value
* `cho_val` : to get a multiple choice value

***

### Example

The file [*main.cpp*](src/main.cpp) provides an example of how to use this project. Here is the output of the `print_help` function. No formatting is made by the user:

	These classes are a set of tools for the management of command line arguments.
	By using these classes in your project, you do not need to worry about designing
	a help menu or about retrieving the parameters' values anymore. Everything is
	handled by the class Parameters and its functions, and make sure your menu looks
	nice even in 80-characters-wide terminals.

	USAGE:

		software --width <value>


	FIRST SUBSECTION:

		--help             Displays this help.

		--width <value>    This parameter would allow the user to set the width of
                      	   the terminal. The default value, 80, is automatically
                      	   printed under this short description since the function is
                      	   called with optional parameter 'display_default_value' set
                      	   to true. This description, although longer than 80
                      	   characters, keeps a nice appearence in the help menu.
                      	   Default: 80

		--multiple <v1> <v2> <v3>
                      	   This parameter expects three values. Since the parameter's
                      	   name and the list of values is a long string, this
                      	   description paragraph is printed on a new line. Default
                      	   values are defined but aren't listed below.

		--mode <choice>    You can also define multiple choices parameters. You need
                      	   to give the multiple choices in an array of pairs (choice,
                      	   choice_description):
                         	   "first_mode": this mode is the first available mode.
                                       	     The description of the choices also fits
                                       	     in the terminal.
                         	   "second_mode": this mode is another available mode.
                                        	  This is the default mode, as printed
                                       	      just below.
                         	   "third_mode": the third mode. If_there_is_a_really_long
                                       	     _word_that_wouldnt_fit_on_one_line_it_is_
                                       	     split_at_the_end_of_this_line_and_this_wo
                                       	     rks_on_multiple_lines.
                         	   "fourth_mode_super_long": last one.
                                                  	     If_there_is_a_long_word that
                                                   	     would fit on the next line,
                                                   	     it goes on the next line.
                      	   Default: "second_mode"


	SECOND SUBSECTION:

		--first_option     You can also define parameters with no values, just like
                      	   this one. No default value is printed.

		--second_option    If there is_a_really_long_word_that_wouldnt_fit_on_one_lin
                      	   e_it_is_split_at_the_end_of_this_line_and_this_works_on_mu
                      	   ltiple_lines.

		--third_option     If there
                      	   is_a_really_long_word_that_would_fit_on_the_next_line it
                      	   goes on the next line.

		--fourth_really_long_option
                      	   If there is_a_really_long_word_that_wouldnt_fit_on_one_lin
                      	   e_it_is_split_at_the_end_of_this_line_and_this_works_on_mu
                      	   ltiple_lines.

		--fifth_really_long_option
                      	   If there
                      	   is_a_really_long_word_that_would_fit_on_the_next_line it
                      	   goes on the next line.

		--sixth_really_long_option
                      	   One_word.

		--seventh_really_long_option
                      	   Normal option.

***

### License

Help80 - Copyright (C) 2016 -  Olivier Deiss

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received [a copy of the GNU General Public License](COPYING)
along with this program. If not, see <http://www.gnu.org/licenses/>.

***

### Contact

olivier . deiss [at] gmail . com
