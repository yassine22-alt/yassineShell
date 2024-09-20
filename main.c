#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//reading a line

#define YSH_RL_BUFSIZE 1024
char *ysh_read_line(void){
  int bufsize = YSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) *bufsize);
  int c;

  if(!buffer) {
    fprintf(stderr, "ysh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while(1) {
    c = getchar();
    // if we get to the end of file/line we exit
    if(c== EOF || c=='\n'){
	buffer[position] = '\0';
	return buffer;
    }else{
	buffer[position] = c;
    }
    position++;
   // if we need more space to allocate
    if(position >= bufsize) {
	bufsize += YSH_RL_BUFSIZE;
	buffer = realloc(buffer, bufsize);
	if(!buffer) {
		fprintf(stderr, "ysh: allocation error\n");
		exit(EXIT_FAILURE);
	}

    }

}


}

#define YSH_TOK_BUFSIZE 64
#define YSH_TOK_DELIM " \t\r\n\a"

char **ysh_split_line(char *line){
  int bufsize = YSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if(!tokens){
	fprintf(stderr, "ysh: allocation error\n");
	exit(EXIT_FAILURE);
  }
  token = strtok(line, YSH_TOK_DELIM);
  while(token != NULL){
	tokens[position]=token;
	position++;

	if(position >= bufsize){
	  bufsize+=YSH_TOK_BUFSIZE;
	  tokens = realloc(tokens, bufsize * sizeof(char*));
	  if(!tokens){
		fprintf(stderr, "ysh: allocation error \n");
		exit(EXIT_FAILURE);
	  }
	}
	token = strtok(NULL, YSH_TOK_DELIM);
  }
  tokens[position]=NULL;
  return tokens;


}


int ysh_launch(char **args){
  pid_t pid, wpid;
  int status;
  pid = fork();

  if(pid == 0){
    if(execvp(args[0],args) == -1){
	perror("ysh");
    }
    exit(EXIT_FAILURE);
  }else if(pid<0){
	perror("ysh");
  }else{
	do {
	  wpid=waitpid(pid,&status,WUNTRACED);
	}while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}


int ysh_cd(char **args);
int ysh_help(char **args);
int ysh_exit(char **args);


char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int(*builtin_func[]) (char **) = {
  &ysh_cd,
  &ysh_help,
  &ysh_exit

};

int ysh_num_builtins(){
  return sizeof(builtin_str) / sizeof(char *);
}


int ysh_cd(char **args){
  if(args[1]==NULL){
	fprintf(stderr, "ysh: expected argument to \"cd\"\n");
  }else{
	if(chdir(args[1]) != 0){
	  perror("ysh");
	}
  }
 return 1;

}

int ysh_help(char **args){
  int i;
  printf("Yassine BLALI's Shell\n");
  printf("Have fun using it. Any recommandations are welcomed! find me in gtihub /yassine22-alt.\n");
  printf("Builtin commands:\n");

  for(i=0;i<ysh_num_builtins();i++){
 	printf(" %s\n", builtin_str[i]);
  }

  printf("Use man command for info on other programs\n");
  return 1;
}

int ysh_exit(char **args){
  return 0;
}


int ysh_execute(char **args){

  if(args[0] == NULL){
	return 1;
  }

  for(int i=0;i<ysh_num_builtins();i++){
	if(strcmp(args[0], builtin_str[i]) == 0){
		return (*builtin_func[i])(args);
	}
  }
  return ysh_launch(args);

}


void ysh_loop(void){
  char *line;
  char **args;
  int status;

  do{
    printf(">");
    line = ysh_read_line();
    args = ysh_split_line(line);
    status = ysh_execute(args);

    free(line);
    free(args);
  }while(status);
}




int main(int argc, char **argv){
  ysh_loop();
  return EXIT_SUCCESS;


}
