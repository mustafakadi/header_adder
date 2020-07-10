# Header Adder

Header Adder is a simple shell application that solves the header dependency issues in C source files. In complex systems that are developed with C, when a new header dependency is added to another C header file, it is pain to add that header file to those all C source files that includes the dependant header file one by one. Header Adder simply adds the given headers to all of the C source files that include the dependant header in the given directories.

### Essential Components :
### ----------------------
- These components are needed to compile and run the code
	- header_adder.c
	- iteration_config.txt
	- makefile

### How to Compile?
### ---------------
- If the executable out file, which is named as "headadder", is not present and the source file "header_adder.c" and the makefile "makefile" are present, then it can be compiled as:
	- Executing "make" command from a terminal or shell on the path of the makefile and source file
- Compiling creates an executable out file named as "headadder"
- Once the executable out file is created, it does not have to be compiled again, fields inside the "iteration_config.txt" can be updated as desired

### How to Run?
### -----------
- There are two ways:
	- 1st: The created executable out file can be called by executing the command "./headadder" from the terminal or the shell, while the shell execution path is on the directory of the out file
	- 2nd: The created executable out file can be directly called by double clicking on the executable file
- After running the executable out file, the configurations will be printed on the screen to receive the approval of the user to proceed further
- The necessary configurations are gathered from the "iteration_config.txt"

### Content of "iteration_config.txt" :
### -----------------------------------
- "Search::"
	- Essential
	- The string is given in that line will be used as the search agent in the files
	- Example:
		- "Search::#include "header_to_be_searched.h"

- "Add::"
	- Essential
	- The string is given in that line will be added to the file, where the search agent is found, before the search agent
	- Example:
		- "Add::#include "header_to_be_added.h"

- "Directory::"
	- Essential
	- The string is given in that line will be used as the initial directory to begin searching from
	- Example:
		- "Directory::/dir1/dir2/dir3/dir4/dir5"
	
- "Modules::"
	- Optional
	- If "Modules::" line is missing in the configuration file, then it will be considered as "All Modules"
	- Searching will be done over only the directories that have the corresponding module name that is given in this line
	- If the line is empty, than there will be no restriction over the directories to be searched
	- Module names need to be separated by commas
	- Not mandatory but to be more effective in terms of searching, it would be better if the module names begin with a slash '/'
	- Examples:
		- "Module::"
		- "Module::m1"
		- "Module::/m1"
		- "Module::/m1/m2"
		- "Module::/m1,/m2,/m3"
		
- "Log::"
	- Optional
	- If "Log" line is missing in the configuration file, then it will be considered as "FALSE"
	- This part will be used to whether enable to keep a log or not
	- If it is 0, no log will be kept
	- If it is 1, an external txt file will be used to keep log. The name of the txt file is "iteration_log.txt"
	- Disabling logging makes the code approximately 1 minute faster than enabling it (these values may vary according the system features)
		- Test:
			- Logging Enabled :
				- real 2m35.52s
			- Logging Disabled :
				- real 1m39.66s
		- These values are gathered for only iterating through all parts of the directory
	- Examples:
		- "Log::1"
		- "Log::0"
