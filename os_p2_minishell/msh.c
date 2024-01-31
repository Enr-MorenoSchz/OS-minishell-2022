//  MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_COMMANDS 8

#define PERM 0666

#define READ_PIPE 0
#define WRITE_PIPE 1
#define BUF_SIZE 2048

// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
	printf("****  Exiting MSH **** \n");
	//signal(SIGINT, siginthandler);
	exit(0);
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
	//reset first
	for(int j = 0; j < 8; j++)
		argv_execvp[j] = NULL;

	int i = 0;
	for ( i = 0; argvv[num_command][i] != NULL; i++)
		argv_execvp[i] = argvv[num_command][i];
}


/**
 * Main sheell  Loop  
 */
int main(int argc, char* argv[])
{
	/**** Do not delete this code.****/
	int end = 0; 
	int executed_cmd_lines = -1;
	char *cmd_line = NULL;
	char *cmd_lines[10];

	if (!isatty(STDIN_FILENO)) {
		cmd_line = (char*)malloc(100);
		while (scanf(" %[^\n]", cmd_line) != EOF){
			if(strlen(cmd_line) <= 0) return 0;
			cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
			strcpy(cmd_lines[end], cmd_line);
			end++;
			fflush (stdin);
			fflush(stdout);
		}
	}

	/*********************************/

	char ***argvv = NULL;
	int num_commands, Acc;


	while (1) 
	{
		int status = 0;
		int command_counter = 0;
		int in_background = 0;
		signal(SIGINT, siginthandler);

		// Prompt 
		write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

		// Get command
		//********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
		executed_cmd_lines++;
		if( end != 0 && executed_cmd_lines < end) {
			command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
		}
		else if( end != 0 && executed_cmd_lines == end) {
			return 0;
		}
		else {
			command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
		}
		//************************************************************************************************


		/************************ STUDENTS CODE ********************************/	
		//We need to check  the number of commands enter in the minishell
	 	if ((command_counter > 0) && (command_counter <= MAX_COMMANDS)){
	 		
	 		//We get the complete commands entered in the minishell (with its program name and arguments)
	 		//We will use a for loop to get each command completly
			for (int i = 0; i < command_counter; i++){
                  		getCompleteCommand(argvv, i);
               	 }
                
                
                //As the internal commands  belong to the minishell and cannot be run in a command sequence, we first need to.
                //check if the entered command is an internal one, meaning we will execute it instead of a simple/sequence command
                
            	//First, we implement MYCALC
            	
            	
            	//Check the first command is "mycalc". If so, run the code inside this If-Statement
                if (strcmp(argv_execvp[0], "mycalc") == 0){
                	//Check that the 3 next arguments exist, that is, they are different from NULL so we know it satisfies one
                	//one of the requirements to apply this internal command (having 3 arguments)
                	if ((argv_execvp[1] != NULL) && (argv_execvp[2] != NULL) && (argv_execvp[3] != NULL)){
                		//Now we check the second argument in order to know the operator that is going to be used
				
				//Check if the operator is "add". If so, run the code inside this If-Statement 
				if (strcmp(argv_execvp[2], "add") == 0){
					//Create 2 variables to be the operands and 1 to store the result
					int operand_1, operand_2, sum;
					//Cast the 1st and 3rd arguments (they are operands) and store them in their var	
					operand_1 = atoi(argv_execvp[1]);
                      			operand_2 = atoi(argv_execvp[3]);
                      			//Store the result from the addition
                      			sum = operand_1 + operand_2;
                      			//We create the env variable "Acc" that will store the accumulative sum
                      			//NOTE: This env variable was declared before the while(1) for applying the minishell
                      			Acc = Acc + sum;
                      			//We cast this int env variable to char data_type and then set the environgment with it
                      			const char env_var = (char) Acc;
					if (setenv("Acc", &env_var, 1) < 0){
						perror("ERROR: The environment variable could not be stated.\n");
						exit(-1);
					}
					//Now its time to print the results into the STD error output
					
					//Create a char array that will store the pattern that needs to be followed to print to
					//STD error output, as well as with the values of the variables for each case
                      			char results[100];
                      			snprintf(results, 100, "[OK] %d + %d = %d; Acc %s\n", operand_1, operand_2, sum, getenv("Acc"));
                      			//Check that there is no error while writing into the STD error output
					if((write(STDERR_FILENO, results, strlen(results)))<0){
						perror ("ERROR: Writing in STD error output was not possible\n");
						exit(-1);
					}
				
				} 
				//If we reached this part, then "add" was not our operator". Now, check if the operator is "mod"
				
				//Check if the operator is "mod". If so, run the code inside this If-Statement
				else if (strcmp(argv_execvp[2], "mod") == 0){
					//Declare the 2 operands and the variables to store the reminder and quotient
					int operand_1, operand_2, reminder, quotient;
					//Create the char array "results" to later store the output to be shown
					char results[150];
					//Like in "mycalc", do the respective cast to integer
					operand_1 = atoi(argv_execvp[1]);
					operand_2 = atoi(argv_execvp[3]);
					//Perform the requested computations
					reminder = operand_1 % operand_2;
					quotient = operand_1 / operand_2;
					//Store into "results" the output of this command. It will be syntax to be followed as well
					//as the respective values from the operands and the values from the 2 operations
					snprintf(results, 150, "[OK] %d %% %d = %d; Quotient %d\n", operand_1, operand_2, reminder, quotient);
					//Check there is no errors while writing into the STD error output
					if((write(STDERR_FILENO, results, strlen(results)))<0){
						perror ("ERROR: Writing in STD error output was not possible");
						exit(-1);
					}
				} 
				//If reached this part, then it means that even having 3 arguments, the operator is not valid
				else{
					//Print an error showing the user how the mycalc command works
					perror("[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>");
				}
			//If reached this part, then it means that there was a problem with the 3 required arguments (at least one
			//of them being NULL
			} 
			else{
				//Print an error showing the user how the mycalc command must be entered
				perror("[ERROR] The structure of the command is mycalc <operand_1> <add/mod> <operand_2>");
			}
		} 
		
		
		//The second internal command that can be entered is "mycp". If we reached this part, it means the entered command
		//was not the internal command "mycalc". So now, it is time to check if the entered command is mycp or not
		
		//Second, implement MYCP
		
		//Check the first command is "mycp". If so, run the code inside this If-Statement
		else if (strcmp(argv_execvp[0], "mycp") == 0){
			//Check that the 2 next arguments exist, that is, they are different from NULL so we know it satisfies one
                	//one of the requirements to apply this internal command (having 2 arguments)
			if ((argv_execvp[1] != NULL) && (argv_execvp[2] != NULL)){
				//Declare the variable to store the fd of the original file
				int file_origin;
				//Check there are not errors while opening the original file
				if((file_origin = open(argv_execvp[1], O_RDONLY, PERM)) < 0){
					//If erros appear, then write into the STD output the following message
					char open_error[] = "[ERROR] Error opening original file";
					//Check this error message is correctly written into the terminal, without problems
					if((write(STDOUT_FILENO, open_error, strlen(open_error))) < 0){
						perror ("ERROR: Writing in STD output was not possible\n");
						exit(-1);
					}
				//Now, start the procces to copy the file
				}
				else{
					//Declare the var to store the fd of the copy. Also, declare 2 var to know how many
					//bytes were read and written
					int file_copy, bytes_read, bytes_write; 
					//Declare a buffer that will be used to first store the read content from the original file
					//and then to read from when writing into the copy
					char buffer[BUF_SIZE];
					//Check there are no erros while opening the copy
					if ((file_copy = open(argv_execvp[2], O_CREAT | O_TRUNC | O_WRONLY, PERM)) < 0){
						//If erros appear, then write into STD output the following message
						char close_error[] = "[ERROR] Error opening the copied file";
						//Check this error message is correctly written into the terminal, without problems
						if((write(STDOUT_FILENO, close_error, strlen(close_error))) < 0){
							perror ("ERROR: File could not be written");
							exit(-1);
						}
					}
					//Now it is time to read the original file and perform the copy
					else{
					//Read the original file and store its content into buffer, while bytes_read var stores
					//the amount of bytes read from the original file
					while ((bytes_read = read(file_origin, buffer, BUF_SIZE)) > 0){
						//Write the content stored in the buffer into the copy, while bytes_write stores
						//the amount of bytes that are written
						
						//Use an If-Statement to check if there is any problem while writing into the copy
						//If there is an error, then close both the original and copy file descriptors
						if ((bytes_write = write(file_copy, buffer, bytes_read)) < 0){
							//Close the original file and also check if there is any error while closing
							if (close(file_origin) < 0){
								perror("ERROR: File could not be closed\n");
								exit(-1);
							}
							//Close the copy file and also check if there is any error while closing
							if (close(file_copy) < 0){
								perror("ERROR: File could not be closed\n");
								exit(-1);
							}
							perror("ERROR: Writing in the copy file was not possible\n");
							exit(-1);
						}
					}
					//If reached this part, it means the while as finished or that there was some error while
					//reading. Therefore, it is needed to check what happend with the following If-Statement
					if (bytes_read < 0){
						perror("ERROR: Original file could not be read\n");
						if (close(file_origin) < 0){
							perror("ERROR: File could not be closed\n");
						}
					}
					//If reached this part, then no problems were found while copying the file
					
					//Now, close the files descriptors of both files and check that there are no erros while
					//doing so
					if (close(file_origin) < 0){
						perror("ERROR: File could not be closed\n");
					}
					if (close(file_copy) < 0){
						perror("ERROR: File could not be closed\n");
					}
					//Now we have to show into the STD output that the mycp command was successful
					
					//In order to do so, create a char array that then will be used to store the syntax
					//used for the successful message, as well as the names of both files
					char achivement[150];
					snprintf(achivement, 150, "[OK] Copy has been successful between <original_file> and <copied_file> %s a %s\n", argv_execvp[1], argv_execvp[2]);
					//Check that there is no errors while writing into STD output the content of "achivement"
					if (write(STDOUT_FILENO, achivement, strlen(achivement)) < 0){
						perror("ERROR: mycp output could not be printed in the STD output\n");
					};
					
				}
				} 
			} 
			//If reached this part, then it means there was some error in the syntax of the mycp command
			else{
				//Show the user how the mycp command works
				char syntax_error[] = "[ERROR] The structure of the command is mycp <original_file><copied_file>";
				if((write(STDOUT_FILENO, syntax_error, strlen(syntax_error))) <0){
					perror("ERROR: Writing in STD output could not be possible\n");
					exit(-1);
				}
			}
		} 	
		
		
		//If reached this part, then it means the introduced command was not an internal one and therefore it must be
		//either just a simple command or a sequence of them
		
		//WARNING: For this Lab Assignment the sequence of commands is limited up to 3 commands!
		
		else{	
			//First, we declared the pid variable that will be used for storing the processes id after
			//the fork service		
			pid_t pid;
			//Then, declare the 2 pipes and 3 other variables that will represent the file descriptors
			//of input file, output file and error output file respectively entered by the user with <, >, !>
			//NOTE: Depending on the situation some of this variables may be used or not
			int fd1[2], fd2[2], file_in, file_out, file_err;
			//Check the number of commands	
			switch(command_counter){
				//1 command was entered
				case 1:
				//Apply fork and then store the result into pid variable. Depending on the value stored, then
				//different actions will be performed in the switch that follows
				pid = fork();
				switch (pid){
					//Some error happend, we notify it to the user
					case -1:
					perror("ERROR: Fork operation was not possible.\n");
					exit(-1);
					
					//We are in the CHILD process	
					case 0:
					//Check if there is any input file received. If so, open it and use it instead of STD input
					if (strcmp(filev[0], "0") != 0){
						file_in = open(filev[0], O_RDONLY);
						dup2(file_in, STDIN_FILENO);
					}
					//Check if there is any output file received. If so, open it and use it instead of STD 
					//output
					if (strcmp(filev[1], "0") != 0){
						file_out = open(filev[1], O_CREAT | O_RDONLY, PERM);
						dup2(file_out, STDOUT_FILENO);
					}
					//Check if there is any error output file received. If so, open it and use it instead of 
					//STD error input
					if (strcmp(filev[2], "0") != 0){
						file_err = open(filev[2], O_CREAT | O_RDONLY, PERM);
						dup2(file_err, STDERR_FILENO);
					}
					//Now, run the new command. Also, check if there is some error while doing the exec service
					if (execvp(argvv[0][0], argvv[0]) < 0){
						//Executing a new program inside the child process using execvp() resulted in an 
						//error.
						perror("");
						exit(-1);
					}
					
					//We are in the PARENT process (Minishell)
					default:
					//Check if its child process ir run in background. If it is not, then wait for it to finish
					if (in_background == 0) {
						wait(&status);
						//Check if there was some problem with the child process
						if (status < 0){
							perror("ERROR: There was some problem while running the child process.");
						}
					}
					//If the child process is run in background, then print its process id
					else{
						printf("[%d]\n", pid);
					}
				}
				break;
						
				//2-command sequence	
				case 2:
				//We create just one pipe with the pipe() service. Then we use fork()
				pipe(fd1);
				pid = fork();
				switch(pid){
					//Some error happend, we notify it to the user
					case -1:
					perror("ERROR: Fork operation was not possible.\n");
					exit(-1);
					
					//We are in the child that has the write end of the pipe open
					case 0:
					//Check if there is any input file received. If so, open it and use it instead of STD input
					if (strcmp(filev[0], "0") != 0){
						file_in = open(filev[0], O_RDONLY);
						dup2(file_in, STDIN_FILENO);
					}
					//Check if there is any error output file received. If so, open it and use it instead of 
					//STD error input
					if (strcmp(filev[2], "0") != 0){
						file_err = open(filev[2], O_CREAT | O_RDONLY, PERM);
						dup2(file_err, STDERR_FILENO);
					}
					close(fd1[READ_PIPE]);
					dup2(fd1[WRITE_PIPE], STDOUT_FILENO);
					close(fd1[WRITE_PIPE]);
					if (execvp(argvv[0][0], argvv[0]) < 0){
					//Executing a new program inside the child process using execvp() resulted in an error.
						perror("");
						exit(-1);
					}
					
					default: 
					close(fd1[WRITE_PIPE]);
					pid = fork();
					switch (pid){
						//Some error happend, we notify it to the user
						case -1:
						perror("ERROR: Fork operation was not possible.\n");
						exit(-1);
						
						//We are in the child that has the read end of the pipe open
						case 0:
						//Check if there is any output file received. If so, open it and use it instead of 
						//STD output
						if (strcmp(filev[1], "0") != 0){
							file_out = open(filev[1], O_CREAT | O_RDONLY, PERM);
							dup2(file_out, STDOUT_FILENO);
						}
						//Check if there is any error output file received. If so, open it and use it 
						//instead of STD error input
						if (strcmp(filev[2], "0") != 0){
							file_err = open(filev[2], O_CREAT | O_RDONLY, PERM);
							dup2(file_err, STDERR_FILENO);
						}
						dup2(fd1[READ_PIPE], STDIN_FILENO);
						close(fd1[READ_PIPE]);
						if (execvp(argvv[1][0], argvv[1]) < 0){
						//Executing a new program inside the child process using execvp() resulted in an 
						//error.
							perror("");
							exit(-1);
						}
						
						default:
						close(fd1[READ_PIPE]);
						//Check if its child processes are running in background. If it is not, then wait 
						//for them to finish
						if (in_background == 0){
							wait(&status);
							wait(&status);
							//Check if there was some problem with any of the child process
							if(status < 0){
								perror("ERROR: There was some problem while running the child process.");
							}
						}
						//If the child process is run in background, then print its process id
						else{
							printf("[%d]\n", pid);
						}
					}
				}
				break;
				
				//3-command sequence	
				case 3:
				//For this situation we will use 2 pipes. We will create the first one now
				pipe(fd1);
				pid = fork();
				switch(pid){
					//Some error happend, we notify it to the user
					case -1:
					perror("ERROR: Fork operation was not possible.\n");
					exit(-1);
					
					//This child has only the write end of the first pipe opened
					case 0:
					//Check if there is any input file received. If so, open it and use it instead of STD input
					if (strcmp(filev[0], "0") != 0){
						file_in = open(filev[0], O_RDONLY);
						dup2(file_in, STDIN_FILENO);
					}
					//Check if there is any error output file received. If so, open it and use it 
					//instead of STD error input
					if (strcmp(filev[2], "0") != 0){
						file_err = open(filev[2], O_CREAT | O_RDONLY, PERM);
						dup2(file_err, STDERR_FILENO);
					}
					close(fd1[READ_PIPE]);
					dup2(fd1[WRITE_PIPE], STDOUT_FILENO);
					close(fd1[WRITE_PIPE]);
					if (execvp(argvv[0][0], argvv[0]) < 0){
					//Executing a new program inside the child process using execvp() resulted in an error.
						perror("");
						exit(-1);
					}
					
					default: 
					close(fd1[WRITE_PIPE]);
					//Now we create the second pipe
					pipe(fd2);
					pid = fork();
					switch (pid){
						//Some error happend, we notify it to the user
						case -1:
						perror("ERROR: Fork operation was not possible.\n");
						exit(-1);
						
						//This child only has the read end of the first pipe and the write end of the
						//second pipe opened
						case 0:
						//Check if there is any error output file received. If so, open it and use it 
						//instead of STD error input
						if (strcmp(filev[2], "0") != 0){
							file_err = open(filev[2], O_CREAT | O_RDONLY, PERM);
							dup2(file_err, STDERR_FILENO);
						}
						close(fd2[READ_PIPE]);
						dup2(fd1[READ_PIPE], STDIN_FILENO);
						close(fd1[READ_PIPE]);
						dup2(fd2[WRITE_PIPE], STDOUT_FILENO);
						close(fd2[WRITE_PIPE]);
						if (execvp(argvv[1][0], argvv[1]) < 0){
						//Executing a new program inside the child process using execvp() resulted in an 
						//error.
							perror("");
							exit(-1);
						}
						
						default:
						close(fd1[READ_PIPE]);
						close(fd2[WRITE_PIPE]);
						pid = fork();
						switch(pid){
							//Some error happend, we notify it to the user
							case -1:
							perror("ERROR: Fork operation was not possible.\n");
							exit(-1);
							
							//This child only has the read end of the second pipe opened
							case 0:
							//Check if there is any output file received. If so, open it and use it 
							//instead of STD output
							if (strcmp(filev[1], "0") != 0){
								file_out = open(filev[1], O_CREAT | O_RDONLY, PERM);
								dup2(file_out, STDOUT_FILENO);
							}
							//Check if there is any error output file received. If so, open it and use 
							//it instead of STD error input
							if (strcmp(filev[2], "0") != 0){
								file_err = open(filev[2], O_CREAT | O_RDONLY, PERM);
								dup2(file_err, STDERR_FILENO);
							}
							dup2(fd2[READ_PIPE], STDIN_FILENO);
							close(fd2[READ_PIPE]);
							if (execvp(argvv[2][0], argvv[2]) < 0){
							//Executing a new program inside the child process using execvp() resulted 
							//in an error.
								perror("");
								exit(-1);
							}
							
							default:
							close(fd2[READ_PIPE]);
							//Check if its child processes are running in background. If it is not, 
							//then wait for them to finish
							if (in_background == 0) {
								wait(&status);   
   								wait(&status);	
    								wait(&status);
    								//Check if there was some problem with any of the child process
								if (status < 0){
									perror("ERROR: There was some problem while running the child process.");
								}
							}
							//If the child process is run in background, then print its process id
							else{
								printf("[%d]\n", pid);
							}
											
						}
					}
				}
				break;
				
				//n-command sequence
				//NOTE: For this Lab Assignment the number of commands in sequence is limited up to 3. Notify it
				//to the user
				default:
					perror("WARNING: This MSH version only supports up to 3 commands in the sequence!");
			}
			}
	}
		//If reached this part, then it means that no command was entered. Report it to the user
		else if (command_counter == 0){
			perror("ERROR: No command was entered.");
		}
		//If reached this part, the nit means that the number of commands entered in sequence exceededthe Maximum amount
		//allowed. Notift it to the user
		else{
			perror("ERROR: Maximum number of commands has been exceeded.");
		}     
	}
	return 0;
}
