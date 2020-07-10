#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define PRINT_NUM(token) printf(#token " is |%d|\n", token)
#define PRINT_STR(token, order) printf(#token " -%d- is |%s|\n", order, token)

#define MODULE_STR_LEN 10


enum error_types
 {
	ET_EMPTY_SEARCH_AGENT,
	ET_SEARCH_LINE_NOT_FOUND,
	ET_EMPTY_ADD_AGENT,
	ET_ADD_LINE_NOT_FOUND,
	ET_MODULES_NOT_FOUND,
	
	NUM_OF_ERROR_TYPES
 };

struct file_line_s
 {
	struct file_line_s *next_line;
	char line[500];
 };

unsigned char *str_to_search;
unsigned char *str_to_add;
unsigned char **modules_to_search;
unsigned char modules_cnt;
unsigned char log_enabled;

unsigned char *dir_to_search;

unsigned long int processed_dir_cnt;
unsigned long int processed_file_cnt;
unsigned long int modified_file_cnt;


/*
 * Date............: 13-Dec-2018  7:39
 * Function........: trim_all_spaces
 * .................
 */
void trim_all_spaces(unsigned char *str_prm)
{
unsigned short int i, k;


if (strlen(str_prm) == 0) return;

i = 0;
while ( (i < strlen(str_prm)) && (str_prm[i] != '\0') )
 {
	if (isspace(str_prm[i]))
	 {
		k = i;
		while (str_prm[k] != '\0')
		 {
			str_prm[k] = str_prm[k + 1];
			k++;
		 }
		continue;
	 }
	i++;
 }
}


/*
 * Date............: 13-Dec-2018  7:39
 * Function........: trim_leading_trailing_spaces
 * .................
 */
void trim_leading_trailing_spaces(unsigned char *str_prm)
{
unsigned short int i, k;


if (strlen(str_prm) == 0) return;

while (isspace(str_prm[0]))
 {
	k = 0;
	while (str_prm[k] != '\0')
	 {
		str_prm[k] = str_prm[k + 1];
		k++;
	 }
 }

i = strlen(str_prm) - 1;
while ( (i < strlen(str_prm)) && ( (isspace(str_prm[i])) || (str_prm[i] == '\n') ) )
 {
	k = i;
	while (str_prm[k] != '\0')
	 {
		str_prm[k] = str_prm[k + 1];
		k++;
	 }
	i--;
 }
}


/*
 * Date............: 13-Dec-2018  7:39
 * Function........: trim_config_line
 * .................
 */
void trim_config_line(unsigned char *str_prm)
{
unsigned short int i, k;
unsigned char colon_cnt = 0;


if (strlen(str_prm) == 0) return;

trim_leading_trailing_spaces(str_prm);

i = 0;
while ( (colon_cnt != 2) && (i < strlen(str_prm)) )
 {
	if (isspace(str_prm[i]))
	 {
		k = i;
		while (str_prm[k] != '\0')
		 {
			str_prm[k] = str_prm[k + 1];
		
			k++;
		 }
		continue;
	 }

	if (str_prm[i] == ':')
		colon_cnt++;

	i++;
 }
}


/*
 * Date............: 13-Dec-2018  7:39
 * Function........: configure_iteration
 * .................
 */
unsigned char configure_iteration()
{
FILE			*config_file;
unsigned char		curr_line[200];
unsigned char		*exist_ptr = 0;
unsigned char		search_str_len = 8, add_str_len = 5, modules_str_len = 9, log_str_len = 5, directory_str_len = 11;
unsigned short int	i;
unsigned char		tmp_str_index, tmp_module_index;
unsigned short int	str_to_search_len = 0, str_to_add_len = 0, dir_len = 0;


config_file = fopen("iteration_config.txt", "r");

if (config_file == NULL)
 {
	printf("\"iteration_config.txt\" Cannot be Found!");
	return (0);
 }

str_to_search = NULL;
str_to_add = NULL;
modules_to_search = NULL;
modules_cnt = 0;
log_enabled = 0;

while (fgets(curr_line, sizeof(curr_line), config_file))
 {
	trim_config_line(curr_line);
	exist_ptr = strstr(curr_line, "Search::");
	if (exist_ptr)
	 {
		str_to_search_len = strlen(curr_line) - search_str_len;
		str_to_search = (unsigned char *) malloc(sizeof(unsigned char) * str_to_search_len);
		memcpy(str_to_search, &curr_line[search_str_len], str_to_search_len);
		str_to_search[str_to_search_len] = '\0';
		trim_leading_trailing_spaces(str_to_search);
	 }
	
	exist_ptr = strstr(curr_line, "Add::");
	if (exist_ptr)
	 {
		str_to_add_len = strlen(curr_line) - add_str_len;
		str_to_add = (unsigned char *) malloc(sizeof(unsigned char) * str_to_add_len);
		memcpy(str_to_add, &curr_line[add_str_len], str_to_add_len);
		str_to_add[str_to_add_len] = '\0';
		trim_leading_trailing_spaces(str_to_add);
	 }
	
	exist_ptr = strstr(curr_line, "Modules::");
	if (exist_ptr)
	 {
		trim_all_spaces(curr_line);
		tmp_str_index = modules_str_len;
		for (i = 0; curr_line[i] != '\0'; i++)
		 {
			if (curr_line[i] == ',')
			 {
				if (i == tmp_str_index)
				 {
					tmp_str_index = i + 1;
					continue;
				 }
				else
				 {
					modules_cnt++;
					tmp_str_index = i + 1;
				 }
			 }
		 }
		
		if (i != tmp_str_index)
			modules_cnt++;
		
		if (modules_cnt == 0) continue;

		modules_to_search = malloc( (modules_cnt) * sizeof(char *));
		for (i = 0; i <= modules_cnt; i++)
		 {
			modules_to_search[i] = malloc(MODULE_STR_LEN * sizeof(char));
		 }

		tmp_str_index = modules_str_len;
		tmp_module_index = 0;
		for (i = modules_str_len; curr_line[i] != '\0'; i++)
		 {
			if (curr_line[i] == ',')
			 {
				if (i == tmp_str_index)
				 {
					tmp_str_index = i + 1;
					continue;
				 }
				else
				 {
					memcpy(modules_to_search[tmp_module_index], &curr_line[tmp_str_index], i - tmp_str_index);
					modules_to_search[tmp_module_index][i - tmp_str_index] = '\0';
					trim_all_spaces(modules_to_search[tmp_module_index]);
					tmp_module_index++;
					tmp_str_index = i + 1;
				 }
			 }
		 }

		if (i != tmp_str_index)
		 {
			memcpy(modules_to_search[tmp_module_index], &curr_line[tmp_str_index], i - tmp_str_index);
			modules_to_search[tmp_module_index][i - tmp_str_index] = '\0';
			trim_all_spaces(modules_to_search[tmp_module_index]);
		 }
	 }

	exist_ptr = strstr(curr_line, "Log::");
	if (exist_ptr)
	 {
		trim_all_spaces(curr_line);

		if (curr_line[log_str_len] == '1')
			log_enabled = 1;
		else	log_enabled = 0;
	 }

	exist_ptr = strstr(curr_line, "Directory::");
	if (exist_ptr)
	 {
		dir_len = strlen(curr_line) - directory_str_len;
		dir_to_search = (unsigned char *) malloc(sizeof(unsigned char) * dir_len);
		memcpy(dir_to_search, &curr_line[directory_str_len], dir_len);
		dir_to_search[dir_len] = '\0';
		trim_leading_trailing_spaces(dir_to_search);
	 }
 }

fclose(config_file);

return (1);
}


/*
 * Date............: 13-Dec-2018  7:39
 * Function........: free_dynamic_var
 * .................
 */
void free_dynamic_var()
{
unsigned char i;


free(str_to_search);
free(str_to_add);

for (i = 0; i < modules_cnt; i++)
 {
	free(modules_to_search[i]);
 }

free(modules_to_search);
}


/*
 * Date............: 13-Dec-2018  7:39
 * Function........: is_dir_contains_module
 * .................
 */
unsigned char is_dir_contains_module(unsigned char *dir_path_ptr)
{
unsigned char *exist_ptr = 0;
unsigned char i;


if (modules_cnt == 0) return (1);

for (i = 0; i < modules_cnt; i++)
 {
	exist_ptr = strstr(dir_path_ptr, modules_to_search[i]); 
	
	if (exist_ptr) return (1);
 }

return (0);
}


/*
 * Date............: 12-Dec-2018 18:06
 * Function........: add_file
 * .................
 */
void add_file(struct file_line_s **lines_param, unsigned char *dir_path_ptr)
{
FILE *new_file;
struct file_line_s *new_line, *past_line;
char line[1000];


new_file = fopen(dir_path_ptr, "r");
(*lines_param) = (struct file_line_s *) malloc(sizeof(struct file_line_s));
fgets(line, sizeof(line), new_file);
(*lines_param) -> next_line = 0;
strcpy((*lines_param) -> line, line);
past_line = (*lines_param); 

while(fgets(line, sizeof(line), new_file))
 {
	new_line = (struct file_line_s *) malloc(sizeof(struct file_line_s));
	past_line -> next_line = new_line;
	new_line -> next_line = 0;
	strcpy(new_line -> line, line);
	past_line = new_line;
 }

fclose(new_file);
}


/*
 * Date............: 12-Dec-2018 18:06
 * Function........: add_line
 * .................
 */
void add_line(struct file_line_s *lines_param, unsigned char *file_name)
{
FILE *new_file;
struct file_line_s *curr_line;
char *ptr;


if (lines_param == 0) return;

new_file = fopen(file_name, "w");
fprintf(new_file, "%s", lines_param -> line);
curr_line = lines_param -> next_line;

while (curr_line)
 {
	ptr = strstr(curr_line -> line, str_to_search);
	if (ptr)
	 {
		fprintf(new_file, "%s", str_to_add);
		fprintf(new_file, "\n");
	 }
	
	fprintf(new_file, "%s", curr_line -> line);
	curr_line = curr_line -> next_line;
 }

fclose(new_file);
}


/*
 * Date............: 12-Dec-2018 18:06
 * Function........: free_file_line_ptr
 * .................
 */
void free_file_line_ptr(struct file_line_s **lines_param)
{
struct file_line_s *curr_fl_ptr, *next_fl_ptr;


if ((*lines_param) == 0)
	return;

curr_fl_ptr = *lines_param;
next_fl_ptr = curr_fl_ptr -> next_line;

free(curr_fl_ptr);

while(next_fl_ptr != 0)
 {
	curr_fl_ptr = next_fl_ptr;
	next_fl_ptr = curr_fl_ptr -> next_line;

	free(curr_fl_ptr);
 }
}


/*
 * Date............: 12-Dec-2018 18:06
 * Function........: is_dir
 * .................
 */
unsigned char is_dir(unsigned char *name)
{
DIR *tmp_dir;


tmp_dir = opendir(name);
if (tmp_dir != NULL)
 {
	closedir(tmp_dir);
	return (1);
 }
else	return (0);
}


/*
 * Date............: 12-Dec-2018 18:30
 * Function........: recursive_file_iteration
 * .................
 */
void recursive_file_iteration(unsigned char *dir_path_str, FILE *log_file, unsigned char inner_dir)
{
DIR			*curr_dir;
FILE			*curr_file;
struct dirent		*dir_content;
struct file_line_s	*file_ptr;
unsigned char		curr_dir_str[250];
unsigned char		curr_line[1000];
unsigned char		co_command[500] = "cleartool co -nc \0";
unsigned char		ci_command[500] = "cleartool ci -nc \0";
unsigned char		*exist_ptr = 0;
unsigned char		includes_found, header_already_exist;


if (is_dir(dir_path_str))
 {
	curr_dir = opendir(dir_path_str);
	processed_dir_cnt++;

	if (log_enabled && log_file)
	 {
		fprintf(log_file, "\nCurrent Directory \"%s\":\n-----------------\n", dir_path_str);
		fprintf(log_file, "\t- Directory Opened\n");
	 }
 }
else	return;

if (curr_dir == NULL)
 {
	if (log_enabled && log_file)
		fprintf(log_file, "\n- LOG:\n  ----\n\t- Directory \"%s\" Cannot Be Opened!\n\n", dir_path_str);
	
	return;
 }

while ((dir_content = readdir(curr_dir)) != NULL)
 {
	if ( (strcmp(dir_content -> d_name, ".") == 0) || (strcmp(dir_content -> d_name, "..") == 0) ) 
		continue;

	strcpy(curr_dir_str, dir_path_str);
	strcat(curr_dir_str, "/");
	strcat(curr_dir_str, dir_content -> d_name);

	if (is_dir(curr_dir_str))
	 {
		if (inner_dir == 1)
		 {
			recursive_file_iteration(&curr_dir_str[0], log_file, inner_dir);
		 }
		else
		 {
			if (is_dir_contains_module(curr_dir_str))
				recursive_file_iteration(&curr_dir_str[0], log_file, 1);
		 }
		continue;
	 }

	exist_ptr = strstr(dir_content -> d_name, ".c");
	if (exist_ptr)
	 {
		curr_file = fopen(curr_dir_str, "r");
		
		if (curr_file == NULL) continue;

		processed_file_cnt++;

		if (log_enabled && log_file)
		 {
			fprintf(log_file, "\tCurrent File \"%s\":\n\t------------\n", dir_content -> d_name);
			fprintf(log_file, "\t\t- Opened\n");
		 }

		includes_found = 0;
		header_already_exist = 0;

		while (fgets(curr_line, sizeof(curr_line), curr_file))
		 {
			exist_ptr = strstr(curr_line, "#include");
			if (exist_ptr)
				includes_found = 1;
			else
			 {
				if (includes_found == 1)
					break;
			 }

			exist_ptr = strstr(curr_line, str_to_add);
			if (exist_ptr)
				header_already_exist = 1;

			exist_ptr = strstr(curr_line, str_to_search);
			if (exist_ptr)
			 {
				if (header_already_exist)
				 {
					if (log_enabled && log_file)
						fprintf(log_file, "\t\t- Add Line Already Exists\n");

					break;
				 }

				modified_file_cnt++;

				strcpy(co_command, "cleartool co -nc \0");
				strcpy(ci_command, "cleartool ci -nc \0");

				strcat(co_command, curr_dir_str);
				strcat(ci_command, curr_dir_str);

				file_ptr = (struct file_line_s *) malloc(sizeof(struct file_line_s));
				system(co_command);

				if (log_enabled && log_file)
					fprintf(log_file, "\t\t- Checked Out\n");

				add_file(&file_ptr, curr_dir_str);

				if (log_enabled && log_file)
					fprintf(log_file, "\t\t- File Stored To Buffer\n");

				add_line(file_ptr, curr_dir_str);

				if (log_enabled && log_file)
					fprintf(log_file, "\t\t- The Line Add Line is Added and File Restored From Buffer\n");

				free_file_line_ptr(&file_ptr);
				system(ci_command);

				if (log_enabled && log_file)
					fprintf(log_file, "\t\t- Checked In\n");

				break;
			 }
		 }

		if (includes_found == 1)
		 {
			fclose(curr_file);

			if (log_enabled && log_file)
				fprintf(log_file, "\t\t- Closed\n");

			includes_found = 0;
			continue;
		 }

		fclose(curr_file);

		if (log_enabled && log_file)
			fprintf(log_file, "\t\t- Closed\n");
	 }
 }

closedir(curr_dir);

if (log_enabled && log_file)
	fprintf(log_file, "\t- Directory Closed: \"%s\"\n\n", dir_path_str);
}


/*
 * Date............: 12-Dec-2018 18:06
 * Function........: main
 * .................
 */
int main(int argc, char *argv[])
{
FILE		*log_file;
unsigned char	error_line[1000] = "\nERROR:\n------\n";
unsigned char	config_sane = 1, error_type = NUM_OF_ERROR_TYPES, module_no;
char		answer;


if (configure_iteration() == 0)
 {
	free_dynamic_var();
	printf("- CONFIGURATION FILE ERROR\n");
	return (0);
 }

if (log_enabled)
	log_file = fopen("iteration_log.txt", "w");

printf("\n**Configuration Results**\n-------------------------\n");
printf("Note: The character '|' is used to show the exact content of the strings! \n");

if (str_to_search)
 {
	if (strlen(str_to_search) == 0)
	 {
		config_sane = 0;
		error_type = ET_EMPTY_SEARCH_AGENT;
		strcat(error_line, "! Search Agent is Empty in Configuration File\n");
		printf("- String to Search\t= ERROR\n");
	 }
	else	printf("- String to Search\t= |%s|\n", str_to_search);
 }
else
 {
	config_sane = 0;
	error_type = ET_SEARCH_LINE_NOT_FOUND;
	strcat(error_line, "! \"Search\" Line is Missing in Configuration File\n");
	printf("- String to Search\t= ERROR\n");
 }

if (str_to_add)
 {
	if (strlen(str_to_add) == 0)
	 {
		config_sane = 0;
		error_type = ET_EMPTY_ADD_AGENT;
		strcat(error_line, "! Add Agent is Empty in Configuration File\n");
		printf("- String to Add\t\t= ERROR\n");
	 }
	else	printf("- String to Add\t\t= |%s|\n", str_to_add);
 }
else
 {
	config_sane = 0;
	error_type = ET_ADD_LINE_NOT_FOUND;
	strcat(error_line, "! \"Add\" Line is Missing in Configuration File\n");
	printf("- String to Add\t\t= ERROR\n");
 }

if (dir_to_search)
 {
	if (is_dir(dir_to_search) == 0)
	 {
		config_sane = 0;
		strcat(error_line, "! Failed to Open the Given \"Directory to Search\" (Corresponding ClearCase View May Not Be Loaded)\n  (Please, Check the Related Line in the Configuration File)\n");
		printf("- Directory to Search\t= |%s|\n", dir_to_search);
	 }
	else
	 {
		if (strlen(dir_to_search) == 0)
		 {
			config_sane = 0;
			error_type = ET_EMPTY_ADD_AGENT;
			strcat(error_line, "! Directory is Empty in Configuration File\n");
			printf("- Directory to Search\t= ERROR\n");
		 }
		else	printf("- Directory to Search\t= |%s|\n", dir_to_search);
	 }
 }
else
 {
	config_sane = 0;
	error_type = ET_ADD_LINE_NOT_FOUND;
	strcat(error_line, "! \"Directory\" Line is Missing in Configuration File\n");
	printf("- Directory to Search\t= ERROR\n");
 }


if (log_enabled == 1)
	printf("- Log Enabled\t\t= TRUE\n");
else	printf("- Log Enabled\t\t= FALSE\n");

if (modules_to_search)
 {
	printf("- Modules (Modules Count = %d):\n", modules_cnt);

	if (modules_cnt == 0)
		printf("\t- All Modules\n");
	else
	 {
		int i;
		for (i = 0; i < modules_cnt; i++)
		 {
			if (modules_to_search[i])
				printf("\t- Module-%d = |%s|\n", i, modules_to_search[i]);
			else
			 {
				config_sane = 0;
				error_type = ET_MODULES_NOT_FOUND;
				module_no = 0x30 + i;

				strcat(error_line, "! Unknown Error at Module-");
				strcat(error_line, &module_no);
				strcat(error_line, "\n");

				printf("\t- Module-%d = ERROR\n", i);
			 }
		 }
	 }
 }
else	printf("- Modules\t\t:\n\t- All Modules\n");

if (config_sane == 0)
 {
	free_dynamic_var();
	printf("%s\n", error_line);
 	return (0);
 }

printf("\nDo you approve the current configuration? [y|n] ");
scanf("%c", &answer);

while ((answer != 'n') && (answer != 'N') && (answer != 'y') && (answer != 'Y'))
 {
	scanf("%c", &answer);
 }

if ((answer == 'y') || (answer == 'Y'))
 {
	processed_dir_cnt = 0;
	processed_file_cnt = 0;
	modified_file_cnt = 0;

	recursive_file_iteration(dir_to_search, log_file, 0);

	printf("\nProcessed Directory Count:\t%lu\n", processed_dir_cnt);
	printf("Processed File Count:\t\t%lu\n", processed_file_cnt);
	printf("Modified File Count:\t\t%lu\n\n", modified_file_cnt);
 }

if (log_enabled && log_file)
	fclose(log_file);

free_dynamic_var();

return (1);
}
