// This is the header file for all helper functions
extern char ** input_split;
extern int num_args; //number of arguments passed in command line
extern int num_token; //number of tokens in input_split
void help();
void func_help(char * str);
char ** split(char * input, char c);
char * get_first_arg();
char * get_builtin();
char * get_cur_dir(char * cur_dir);
int is_builtin(char * str);
int find_pipe();
int contains_right_hoinky();
int contains_left_hoinky();
int get_right_hoinky_index();
int get_left_hoinky_index();
int countpipes(char ** argv);
void pipeline(char ** argv);
int find_next_pipe(char ** argv);
