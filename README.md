# Help80

The classes defined in this project can be used for parameters parsing and display, while making sure the help menu looks nice on terminals.

The name comes from the default width of terminals. Indeed, most terminals have a default width of 80 characters.

### Use

To use the classes in your own project, just include the files *Parameters.hpp* and *Parameters.cpp* in your project.

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
