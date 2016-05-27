# Help80

![Screenshot](media/Screenshot.png)

The classes defined in this project can be used for parameters parsing and display, while making sure the help menu looks nice on terminals. The name comes from the default width of terminals. Indeed, most terminals have a default width of 80 characters.

### Features

**Help80** allows you to:
* parse the command line arguments
* automatically design a help menu that fits the terminal's width, with bold parameters and underlined values for parameters
* make sure all the input arguments respect the size of the built-in type you assign them to 

Supported types:
`std::string` `short int` `unsigned short int` `int` `unsigned int` `long int` `unsigned long int` `long long int` `unsigned long long int` `float` `double` `long double`

Note:
* type `bool` is reserved for parameters with no value
* type `std::string` can be used, but is also used for multiple choices parameters

### Use

To use the classes in your own project, just include the files [*Parameters.hpp*](src/Parameters.hpp) and [*Parameters.cpp*](src/Parameters.cpp) in your project and follow these steps:

#### Create the `Parameters` object

When you create the `Parameters` object, you need to pass a `Parameters::config` structure to its constructor. This structure defines all the indentation and dimensions of the help menu. The structure has the following fields (in that order):

Field | Description
----- | -----------
`const int max_terminal_width` | The maximum width that the help menu can take. If the terminal is full screen, using 100% of its width can lead to long lines that are hard to read. This parameter should be around 90-120.
`const int params_indent_len` | Number of characters between the left side of the terminal and the list of parameters.
`const int param_to_desc_len` | Minimal number of characters between the end of a parameter's name and its description. If this space is not respected, the description begins on a new line.
`const int desc_indent_len` | Number of characters between the left side of the terminal and the paragraph that describes the parameters.
`const int choice_indent_len` | Indentation of the available choices for multiple choices parameters.
`const int choice_desc_indent_len` | Indentation of the choices' description from the choice's name.
`const int right_margin_len` | Number of characters between last character on screen and right side of the terminal.
`LANG lang` (`lang_fr` or `lang_us`) | To adapt to the differences of these two languages. 

Two constructors are available:

Constructor | Use
----------- | ---
`Parameters(const int, char const* const* const, config)` | The help menu width will be the minimum between `max_terminal_width` and the terminal's width.
`Parameters(const int, char const* const* const, config, const int)` | The help menu width will be the minimum between `max_terminal_width` and the value given by the constructor's fourth argument.

When the object is created, you can start defining parameters. This will automatically start building the help menu.

#### Specify the parameters
        
First you can specify a description of your program, and how to use it with the functions:
* `void set_program_description(const std::string&)`
* `void set_usage(const std::string&)`
 
Then you can use the following functions to define your program's parameters:

##### `void define_param`
To define a parameter that doesn't need any value. This function takes two arguments, listed below:
  1. `const std::string&`: the parameter's name.
  1. `const std::string&`: its description.

##### `void define_num_str_param`
To define a numeric or `std::string` parameter:
  1. `const std::string&`: the parameter's name.
  1. `const std::vector<std::string>&`: the name of the associated values. This is what will be written between `<` and `>`, like `<value1>` next to the parameter's name.
  1. `const std::vector<T>&`: the default values associated with the above names.
  1. `const std::string&`: the parameter's description.
  1. `const bool=false`: specifies if the default value has to be printed. This will mean to the user that if he does not specify this argument, the default value will be used.

##### `void define_choice_param`
To define a multiple choice parameter. This is a parameter that can only take a finite set of (`std::string`) values:
  1. `const std::string&`: the parameter's name.
  1. `const std::string&`: the name of the associated value.
  1. `const std::string&`: the default choice.
  1. `vec_choices`: a vector of `std::pairs<std::string, std::string>` where the `first` element is the choice's name, and the `second` element is the choice's description.
  1. `const std::string&`: the parameter's decription.
  1. `const bool`: specifies if the default choice has to be printed.

The order in which you define the parameters will be the same as the order of the parameters on the help menu. You can add structure by adding subsections between parameters with function `void insert_subsection(const std::string&)`.

When the menu is ready, you can print it with if you wish with `void print_help()`. This function is usually only called when asked by the user though.
        
#### Get the entered values

So far, the parameters have only been defined. After a call to `void parse_params()` the command line arguments will be parsed and the parameter's values will be updated from their default value to the one provided by the user. If no exception is thrown so far, the command line is correct. You can then use the following functions to retrieve the parameters' values:

function | description
-------- | -----------
`const bool is_spec(const std::string&)` | To know if a simple parameter is specified.
`const T num_val(const std::string&, const int=1)` | To get the n-th numeric value of a numeric parameter. The first one is accessed with index 1, the default value.
`const std::string str_val(const std::string&, const int=1)` | To get the value of a `std::string` parameter. The first one is accessed with index 1, the default value.
`const std::string cho_val(const std::string&)` | To get the value of a multiple choice parameter.

#### Catch the following exceptions

Along the way, exceptions can be thrown. They mean something bad was entered by the user:

Exception | Meaning
--------- | -------
`ValueOutOfRangeException` | Thrown if the value given by the user is not in the range of the built-in type specified by the coder.
`NotEnoughValuesException` | Thrown when the command line doesn't have as many arguments as required.
`DecimalExpectedException` | Thrown when a decimal value is expected but something else is given by the user.
`IntegerExpectedException` | Thrown when an integer value is expected but something else is given by the user.
`UnknownParameterException` | Thrown when there is an unknown parameter in the command line.

They can also mean something was badly coded (by the coder):

Exception | Meaning
--------- | -------
`UndefinedValueException` | Thrown when you are trying to access (n+1)-th value of a parameter that only has n values.
`UnsupportedParameterTypeException` | Thrown when you try to create a parameter of (so far) unsupported type.
`DuplicateParameterException` | Thrown when you try to create a parameter with an already existing name.
`UndefinedParameterException` | Thrown When you try to retrieve value for a parameter that doesn't exist.
`DynamicCastFailedException`  | Thrown when calling `num_val` with the wrong template type argument, or when a `dynamic_cast` fails.

***

### Text Decoration

In the description of the program, the parameters or the choices, you can use the following combinations:
* `$p(text)` will be replaced by *text*, prefixed by `--` and surrounded by double quotes `"`, in bold: "**--text**". This is useful to refer in other parameters in a description. Do not use that command, but the following one instead, to refer to the parameter this description is refering to.
* `$_0` will be replaced by the name of the parameter the description refers to, prefixed by `--` and surrounded by double quotes `"`, in bold: "**--parameter_name**".
* `$_n` will be replaced by the name of the n-th parameter's value surrounded by `<` and `>`: *<parameter_name>*.

***

### Guarantees

These classes guarantees the following things if no exception is thrown:
* The command line entered by the user only calls defined parameters, with acceptable values.
* The parameters you defined and how you use them is correct.

However, there is no guarantee that:
* The input parameters specified by the user are compatible. It is possible that a combination of parameters doesn't make sense in your program. This is your task to check this kind of incompatibilities.
* The values for the parameters are acceptable for your application. They respect the size of the built-in type, but if you want to make sure a value is not above 100 or below 50, this is also your job.

***

### Example

The file [*main.cpp*](src/main.cpp) provides an example of how to use this project.

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
