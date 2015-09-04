#include "command.h"
#include "command-internals.h"
#include <stdio.h>
#include "error.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>


/* FIXME: You may need to add #include directives, macro definitions,
 static function definitions, etc.  */


int
command_status (command_t c)
{
    return c->status;
}

void
execute_command (command_t c)
{
    
    if(c->type == SIMPLE_COMMAND)
    {
        
        pid_t processID;
        
        int status;
        
        processID = fork();
		//processID  =0; //added
        if(processID==0) //this is the child process
        {
            
            if(c->output)
            {
                //0644 was put here because the default mask is
                //0022 so we need total of 0666 to give RW permission to all
                //adding these together we get the 0666
                int fileOpen = open(c->output,O_RDWR|O_CREAT|O_TRUNC,0644);
                
                if(fileOpen == -1)
                {
                    perror("File Open Error");
                    exit(errno);
                }
                
                dup2(fileOpen,1); // this redirects output to file
                
                
                int closeStatus =  close(fileOpen);
                
                if(closeStatus == -1)
                {
                    perror("Closing of File Error");
                    exit(errno);
                } //this checks for file closing mistake
                
            }
            
            if(c->input)
            {
                
                int fileInput = open(c->input,O_RDONLY,0);
                
                if(fileInput == -1)
                {
                    perror("Input File Error");
                    exit(errno);
                    
                }
                
                dup2(fileInput,0); //this redirects input to be FROM a file
                
                int closeStatus2 = close(fileInput);
                if(closeStatus2 == -1)
                {
                    perror("Closing of File Error");
                    exit(errno);
                }
                
            }
            
            
            status = execvp(c->u.word[0],c->u.word); //this feeds our command executing function the command to EXECUTE, and the rest of the word is the arguments supplied to command
            
            if(status == -1)
            {
                //perror("Command Execution Error"); //here we have an error from exec and must return the exit status error
                exit(0); //returns the status
            }
            else
            {
                exit(0); //otherwise we succeed in command
            }
            
            
        }
        
        
        
        if(processID>0) //this is the parent process
        {
            
            if(waitpid(processID,&status,0) == -1) //wait for any children
            {
                perror("Child Did Not Exit Correctly");
                exit(1);
                
            }
            
            if(WIFEXITED(status)) //if child exited correctly
            {
                c->status = WEXITSTATUS(status); //set the child's exit status as the command's exit status
            }
            
            
        }
        else //else this is an error for forking
            
        {
            perror("Child Thread Creation Failed");
            exit(1);
        }
        
        
    }
    
    if(c->type == AND_COMMAND)
    {
        execute_command(c->u.command[0]);
        int status1 = c->u.command[0]->status;
        
        if(status1 == 0) //first command is succesful
        {
            execute_command(c->u.command[1]);
            int status2 = c->u.command[1]->status;
            
            c->status = status2;
            
        }
        else
        {
            c->status = status1;
        }
        
    }
    
    if(c->type == OR_COMMAND)
    {
        execute_command(c->u.command[0]);
        int status1 = c->u.command[0]->status;
        
        if(status1==0)
        {
            c->status = status1;
        }
        else
        {
            execute_command(c->u.command[1]);
            int status2 = c->u.command[1]->status;
            c->status = status2;
            
            
        }
        
        
    }
    
    if(c->type == SUBSHELL_COMMAND)
    {
        if(c->output)
        {
            //0644 was put here because the default mask is
            //0022 so we need total of 0666 to give RW permission to all
            //adding these together we get the 0666
            int fileOpen = open(c->output,O_RDWR|O_CREAT|O_TRUNC,0644);
            
            if(fileOpen == -1)
            {
                perror("File Open Error");
                exit(errno);
            }
            
            dup2(fileOpen,1); // this redirects output to file
            
            
            int closeStatus =  close(fileOpen);
            
            if(closeStatus == -1)
            {
                perror("Closing of File Error");
                exit(errno);
            } //this checks for file closing mistake
            
        }
        
        if(c->input)
        {
            
            int fileInput = open(c->input,O_RDONLY, 0);
            
            if(fileInput == -1)
            {
                perror("Input File Error");
                exit(errno);
                
            }
            
            dup2(fileInput,0); //this redirects input to be FROM a file
            
            int closeStatus2 = close(fileInput);
            if(closeStatus2 == -1)
            {
                perror("Closing of File Error");
                exit(errno);
            }
            
        }
        
        execute_command(c->u.subshell_command);
        c->status = c->u.subshell_command->status;
        
    }
    
    if(c->type == SEQUENCE_COMMAND)
    {
        execute_command(c->u.command[0]);
        execute_command(c->u.command[1]);
        
        c->status = c->u.command[1]->status;
        
    }
    
    if(c->type == PIPE_COMMAND)
    {
        int pipeArray[2];
        
        
        int pipeResult = pipe(pipeArray);
        
        if((pipeResult) < 0) //there was a mistake opening the pipe
        {
            perror("Couldnt Establish Pipe");
            exit(1);
            
        }
        
        
        //now we will open up one child thread to run through the first end of the pipe
        
        
        pid_t processIDOne = fork();
        
        if(processIDOne < 0) //check for fork mistake
        {
            
            perror("Forking Error in Pipe");
            exit(1);
            
        }
        
        if(processIDOne == 0) //in the child 1st process
            
        {
            dup2(pipeArray[1],1); //here we redirect the stdrd output (the 1) to write to the pipe[1] which is the write end
            
            close(pipeArray[0]); // we close the read end of the pipe to avoid mistakes
            
            execute_command(c->u.command[0]); //run the 1st command in the top end of the pipe
            
            int statusExitOne = c->u.command[0]->status;
            
            exit(statusExitOne);
            
        }
        
        //close(pipeArray[0]); // this is closed in the parent function because we are only receiving input from child in this parent, we need to close the bottom of the pipe
        
        pid_t processIDTwo = fork();
        
        if(processIDTwo < 0)
        {
            perror("Forking Error in Pipe");
            exit(1);
        }
        
        if(processIDTwo == 0) //in 2nd child
            
        {
            dup2(pipeArray[0],0); //now we redirect the input to the bottom of the pipe, and now this is our new output;
            
            close(pipeArray[1]); //close the unecessary end
            
            execute_command(c->u.command[1]); //run the 2nd command in this pipe after we set up the pipe redirect of output and input
            
            int statusExitTwo = c->u.command[1]->status;
            
            exit(statusExitTwo);
        }
        
		close(pipeArray[0]);
        close(pipeArray[1]); //here we close the top end of the pipe as the parent receives from the child the output so we dont need the top end
        
        
        //
        
        //
        
        // NOTE CHECK THIS CLOSING LOGIC MAKE SURE THIS IS THE CORRECT WAY TO CLOSE THE PIPES, should be right
        
        
        //
        
        
        int childOneStatus;
        int childTwoStatus;
        
        waitpid(processIDOne,&childOneStatus,0); //in the parent we wait on two children
        waitpid(processIDTwo,&childTwoStatus,0);
        
        if(WIFEXITED(childOneStatus)) // if bothexit
        {
            if(WIFEXITED(childTwoStatus))
            {
                c->status = WEXITSTATUS(childTwoStatus); //set the command's status as the exit status of the 2nd child, the bottom of the pipe command
            }
        }
        
        
    }
    
    
    
    
}





/////////////////////////////////// TIME TRAVEL MODE //////////////////////////
/////////////////////////////////// LAB 1C ////////////////////////////////////

//the dependent
void grab_all_io(command_t command, char*** all_io_array, int *all_io_array_size) {
	if (command->type == SIMPLE_COMMAND) {
        if (command->input) {
            (*all_io_array_size)++;
            *all_io_array =  realloc(*all_io_array, sizeof(char*) * (*all_io_array_size));
            (*all_io_array)[*all_io_array_size - 1] = command->input;
        }
        
		if (command->output) {
			(*all_io_array_size)++;
			*all_io_array =  realloc(*all_io_array, sizeof(char*) * (*all_io_array_size));
			(*all_io_array)[*all_io_array_size - 1] = command->output;
		}
	}
    else if (command->type == SUBSHELL_COMMAND)
    {
        if (command->input) {
            (*all_io_array_size)++;
            *all_io_array =  realloc(*all_io_array, sizeof(char*) * (*all_io_array_size));
            (*all_io_array)[*all_io_array_size - 1] = command->input;
        }
        
		if (command->output) {
			(*all_io_array_size)++;
			*all_io_array =  realloc(*all_io_array, sizeof(char*) * (*all_io_array_size));
			(*all_io_array)[*all_io_array_size - 1] = command->output;
		}
        grab_all_io(command->u.subshell_command, all_io_array, all_io_array_size);
        
    }
    else {
		grab_all_io(command->u.command[0], all_io_array, all_io_array_size);
		grab_all_io(command->u.command[1], all_io_array, all_io_array_size);
	}
}

















//the depender
void grab_prev_output(command_t command, char*** prev_output_array, int *prev_output_array_size) {
	if (command->type == SIMPLE_COMMAND) {
		if (command->output) {
			(*prev_output_array_size)++;
			*prev_output_array =  realloc(*prev_output_array, sizeof(char*) * (*prev_output_array_size));
			(*prev_output_array)[*prev_output_array_size - 1] = command->output;
		}
	}
    else if (command->type == SUBSHELL_COMMAND){
        if (command->output) {
			(*prev_output_array_size)++;
			*prev_output_array =  realloc(*prev_output_array, sizeof(char*) * (*prev_output_array_size));
			(*prev_output_array)[*prev_output_array_size - 1] = command->output;
        }
        grab_prev_output(command->u.subshell_command, prev_output_array, prev_output_array_size);
    }
    
    else {
		grab_prev_output(command->u.command[0], prev_output_array, prev_output_array_size);
		grab_prev_output(command->u.command[1], prev_output_array, prev_output_array_size);
	}
}




void execute_timeTravel(command_stream_t c)
{
    command_stream_t start; //get the head of the list
    command_stream_t commandCount; //use this guy to travel along list to see how many cmns i have to make the matrix
    start = commandCount = c;
    
    int commandCounter = 0; //int to count commands
    
    while(commandCount!=NULL) //loop to count commands
    {
        commandCounter++;
        commandCount = commandCount->next;
        
    }
    
    
    
    
    

    
	command_stream_t where_i_am, catch_up;
	where_i_am = catch_up = c;
    
	int *w =  calloc(commandCounter, sizeof (int));	 //this w will be the array on the side of the matrix that adds all the values in the row
    int **dep_matrix =  malloc(commandCounter * sizeof(int *));
	int tmp=0;
	while(tmp < commandCounter) {
		dep_matrix[tmp] =  calloc(commandCounter, sizeof(int));
		tmp++;
	}
    
	int i = 0;
	int j= 0;
    char** where_i_am_dep =  malloc(sizeof(char*));
	char** catch_up_dep =  malloc(sizeof(char*));
    int im=0;
	int jm=0;
	while (where_i_am != NULL) {
		free (where_i_am_dep);
		char** where_i_am_dep = malloc(sizeof(char*));
		int where_i_am_dep_size = 0;
		grab_all_io(where_i_am->tree, &where_i_am_dep, &where_i_am_dep_size);
		while (catch_up != where_i_am)
		{
			free(catch_up_dep);
			char** catch_up_dep =  malloc(sizeof(char*));
			int catch_up_dep_size = 0;
			grab_prev_output(catch_up-> tree, &catch_up_dep, &catch_up_dep_size);
		 	im=0;
			while (where_i_am_dep[im] != NULL) {
				jm = 0;
				while (catch_up_dep[jm] != NULL) {
					if (!strcmp(where_i_am_dep[im], catch_up_dep[jm])) {
					    dep_matrix[i][j]=1;
                        w[i]++;
					}
					++jm;
				}
				++im;
			}
            
			catch_up = catch_up->next;
			j++;
		}
		where_i_am = where_i_am->next;
		i++;
		j= 0;
		catch_up = c;
	}
    
    //DEBUG PURPOSES//
    
    
    
    
    /*
    
    
    
    
    
    
    int x = 0;
    for(x; x<commandCounter;x++)
    {
        int y=0;
        for(y;y<commandCounter;y++)
        {
            printf("%d", dep_matrix[x][y]);
        }
        printf("\n");
    }
    
    
    int o=0;
    for(o; o<commandCounter; o++)
    {
        printf("%d %d", o, w[o]);
        printf("\n");
    }
    //  printf("command counter %d", commandCounter);
    
    */
    
    //debug purposses
    
    
    
    
    
    
    
    
    
    
    
    
    int *runYet = malloc(commandCounter * sizeof (int)); //initilaize the array that will keep track of did we run this command yet
	memset(runYet, -1, sizeof(int) * commandCounter);
    
    
    int *toRun = malloc(commandCounter * sizeof (int)); //initialize the array that will keep track of what we need to run next
	memset(toRun, -1, sizeof(int) * commandCounter);
    
    
    command_t* arrayOfCommands = malloc(commandCounter * sizeof(command_t));
                                        
    command_stream_t iterator;
    iterator = start;
                                        
    int r=0;
    
    for(r;r<commandCounter;++r)
     {
       arrayOfCommands[r] = (*iterator).tree;
        iterator= iterator->next;
        if(iterator==NULL)
        {
            break;
        }
    }//at this point we filled our arrayOfCommands with the commmands in our tree
    
    
    //debug purposes this tests that we made a correct array with all our commands in it
    /*
	printf("this is command count %d\n",commandCounter);
	printf("this is the r var %d\n",r);
    	
	printf("cmnd 0 word %s",(*arrayOfCommands[0]).u.word[0]);
	printf("cmnd 1 word %s",(*arrayOfCommands[1]).u.word[0]);
	printf("cmnd 2 word %s",(*arrayOfCommands[2]).u.word[0]);
     */

    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    /////////
    /////////must figure out how to make this a while loop starting here
    ////////
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////

    //this section of code we will see which commands need to be ran
    //we look for w[i] being a 0 meaning it has no dep so its a go
    //also we look for runYet[i] == -1 so that we havent ran this command before
    //if both conditions are true, set our toRun array to 1 for that PARTICULAR command at the slot of mover
    //this will correspond to the array slot in arrayOfCommands
    
    int loopCount = commandCounter;
    while(loopCount>0)
    {
    int mover =0;
    for(mover;mover<commandCounter;mover++)
    {
        if(w[mover]==0 && runYet[mover]==-1)
        {
            toRun[mover]=1;
        }
    }
    
    //now we must count how many kids we will need to fork in this go around
    
    int momma = 0;
    int lilKids = 0;
    for(momma;momma<commandCounter;momma++)
    {
        if(toRun[momma]==1)
        {
            lilKids++;
        }
    }
    
    //lilKids now tells how many process we are to fork into
    
    //here we fork
    
    int var = 0;
        
        
        
    
    for(var;var<lilKids;var++)
    {
        
        loopCount--;
        
        
        
        int runMe =0;
        for(runMe;runMe<commandCounter;runMe++)
        {
            if(toRun[runMe]==1 && runYet[runMe]==-1)
            {
                runYet[runMe]=1; //HERE WE MIGHT NEED A LOCK because this can get touched by different kids at same time
                
                
                
                
                goto exitLoop; //break from the for loop
            }
        }
        
        
        
        exitLoop:
        
        
        
        switch (fork()) {
            case 0:
            {



            
                //now we left the loop so we know who to run, lets run this bad boy
            
                execute_command(arrayOfCommands[runMe]); //chose runMe to be the guy we run in this loop
      		exit((*arrayOfCommands[runMe]).status);          
            }
                break;
                
            case -1:
            {
                perror("Input File Error");
                exit(errno);
            }
                break;
            default: //parent process..so do nothing
                break;
        }
    }
    
    
    
  
    //DONE FORKING
    
    int startWait = 0;
    int status;
    for (startWait; startWait<lilKids; startWait++) {
        waitpid(-1, &status, 0);
    }
    //here we wait for all lil kids to return
    
    
    
    
    
    //we have done forking we have done everything up to this point NOW we must update all our booj
    
    
    //1st we use toRun's info to see who we just ran and update the matrix according to this because since we
    //just ran them
    
    int col = 0;
    int row;
    for(col;col<commandCounter;col++)
    {
        if(toRun[col] == 1)
        {
            row = 0;
            for(row;row<commandCounter;row++)
            {
                dep_matrix[row][col] = 0; //we reset the entire column in matrix to 0's because we ran this guy
            }
        }
    }
    //now our matrix is up to date
    
    int reset= 0;
    for(reset;reset<commandCounter;reset++)
    {
        toRun[reset] = -1; //this resets our array of things to run so that we are blank, we are ready to run again
    }
    
    col = 0;
    row = 0;
    int sum = 0;
    
    for(row;row<commandCounter;row++)
    {
       col = 0; 
        for(col;col<commandCounter;col++)
        {
            sum +=dep_matrix[row][col] ;
        }
        w[row] = sum;
        sum = 0;
    }
    }
    
}

