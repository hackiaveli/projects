
// UCLA CS 111 Lab 1 command reading


// shai segal and andrej ivanovic

#include "command.h"
#include "command-internals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "alloc.h"

#include <ctype.h>


#include "error.h"

/* FIXME: You may need to add #include directives, macro definitions,
 static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
 complete the incomplete type declaration in command.h.  */


struct commandLine
{
    char * completeCommand;
    int lineNumber;
    
};



char* cleanLeadingWhiteSpace(char * buffer)
{
    int i = 0;
    int returner = 0;
    
    char *retString = malloc(strlen(buffer));
    
    while( buffer[i] !='\0' )
    {
        while(buffer[i]==' ')
        {
            i++;
        }
        retString[returner] = buffer[i];
        returner++;
        i++;
        
    }
    retString[returner] = '\0';
    
    return retString;
    
}

struct signal
{
    char *buffer;
    enum command_type type;
    struct signal *next;
    struct signal *last;
    char *input;
    char *output;

};

char * newLineKilla(char *buffer)
{
    char * returner = malloc(strlen(buffer));
    int i = 0;
    int inReturner = 0;
    while (buffer[i]!='\0') {
        if(buffer[i]=='\n')
        {
            returner[inReturner] = buffer[i];
            inReturner++;
            i++;
            while (buffer[i] == '\n' || buffer[i] == ' ') {
                i++;
            }
        }
        returner[inReturner] = buffer[i];
        i++;
        inReturner++;
    }
    
    return returner;
}

typedef struct signal *signalTree;

char * whiteSpaceKilla(char * buffer)
{
    char * returner;
    returner = (char *) checked_malloc(sizeof(char));
    int sizeOfReturner =1;
    while(buffer[0]!='\0')
    {
        
        
        if(buffer[0] == ' ')
        {
            buffer++;
        }
        else
        {
            returner [sizeOfReturner-1] = buffer[0];
            sizeOfReturner++;
            returner=(char*)checked_realloc(returner, sizeOfReturner*sizeof(char));
            //returner++;
            buffer++;
        }
        
    }
    return returner;
}

int isWord(char symbol)
{
    if(isalpha(symbol))
        return 1;
    if(isdigit(symbol))
        return 1;
    switch(symbol)
    {
        case '!':
        case '%':
        case '+':
        case '-':
        case ',':
        case '.':
        case '/':
        case ':':
        case '@':
        case ' ':
        case '^':
        case '_':
            return 1;
        default:
            return 0;
            
    }
    
}

int isToken(char symbol)
{
    switch (symbol)
    {
        case '|':
        case '&':
        case '(':
        case ')':
        case ';': //ADD for sequence
            return 1;
        default:
            return 0;
    }
}

void getByte(int(*get_next_byte)(void *), void *get_byte, char *symbol, int *numlines) {
    *symbol = (char) get_next_byte(get_byte);
    if(*symbol == '\n' || *symbol == EOF)
        (*numlines)++;
    
}

struct commandLine createCommandLine(char *buffer, int numLine, int bufSize)
{
    struct commandLine singleCommandLine;
    
    singleCommandLine.completeCommand = (char *) checked_malloc((bufSize+1)*sizeof(char));
    
    strcpy(singleCommandLine.completeCommand, buffer);
    
    singleCommandLine.lineNumber = numLine;
    
    return singleCommandLine;
    
}


void linkNewSignal(struct signal **head, struct signal **current, char* buffer, enum command_type type)
{
    struct signal *returner = malloc(sizeof(struct signal));
    returner->buffer = malloc(strlen(buffer));
    strcpy(returner->buffer, buffer);
    returner->type = type;
    returner->input = NULL;
    returner->output = NULL;
    if(*head==NULL)
    {
        *head = returner;
        returner->next = NULL;
        returner->last = NULL;
    }
    else
    {
        returner->last = *current;
        (*current)->next = returner;
    }
    
    *current = returner;
    
}

char * giveWholeWord(char **buffer)
{
    //char *returner = malloc(strlen(*buffer));
    char *returner = malloc(sizeof(char));
    int letters = 0;
    while (isWord(**buffer) || (**buffer) == '<' || (**buffer) == '>') //if its an entire string, send whole thing to be tokenized
    {
        returner[letters] = **buffer;
        letters++;
        returner = realloc(returner, (letters+1)*sizeof(char));
        (*buffer)++;
    }
    return returner;
}

signalTree turnIntoSignals(struct commandLine commandLine)
{
    
    char * string = commandLine.completeCommand;
    int size = strlen(commandLine.completeCommand);
    
    
    struct signal* head=NULL;
    struct signal* current = NULL;
    
    int tempLetter = 0;
    while(*string!='\0')
    {
        
        if(isWord(*string))
        {

            linkNewSignal(&head, &current, giveWholeWord(&string), SIMPLE_COMMAND); //create a token with the word in it
        }
        
        if(isToken(*string))
        {
            
            if((*string)=='&') //if we hit an AND token
            {
                string++;
                if ((*string)=='&')
                {
                    linkNewSignal(&head, &current, "&&", AND_COMMAND);
                   
                }
                else
                {
                    //errorChange
                    
                    //error(1,0,"Error Report on Line 1");
                    
                    
                }
            }
            
            else if((*string)=='|')
            {
                string++;
                if((*string) == '|')
                {
                    linkNewSignal(&head, &current, "||", OR_COMMAND);
                }
                else
                {
                    linkNewSignal(&head, &current, "|", PIPE_COMMAND);
                    string--;
                }
                
            }
            else if((*string) == '(')
            {
                linkNewSignal(&head, &current, "(", SUBSHELL_COMMAND);
            }
            else if((*string) == ')')
            {
                linkNewSignal(&head, &current, ")", SUBSHELL_COMMAND);
            }
            else if((*string) == ';')
            {
                linkNewSignal(&head, &current, ";", SEQUENCE_COMMAND);
            }
            
            
        }
        
        string++;
        
 
    }
    
    
    
    return head;
}

void checkError (signalTree* input)//using a pointer because I don't want to advance the head returned in TurnIntoSignals
{
    int numClosing = 0;
    int numOpening = 0;
    while (*input != NULL) //I want to check every signal in linked list
    {
        if((**input).type == SIMPLE_COMMAND) //now I need to see if the signal we're on has IO
        {
           if((**input).next == NULL || (*(**input).next).type == AND_COMMAND || (*(**input).next).type == SEQUENCE_COMMAND || (*(**input).next).type == OR_COMMAND || (*(**input).next).type == PIPE_COMMAND || !strcmp((*(**input).next).buffer,")") )
           {
               //
           }
            else
            { //error(1, 0, "error");
            }
        }
        else if((**input).type == AND_COMMAND)
        {
             if((*(**input).next).type == SIMPLE_COMMAND || !strcmp((*(**input).next).buffer,"(") )
             {
                 
             }
            else
            {
            //error(1, 0, "error");
            }
            
        }
        else if((**input).type == OR_COMMAND)
        {
            if((*(**input).next).type == SIMPLE_COMMAND || !strcmp((*(**input).next).buffer,"(") )
            {
                
            }
            else
            {
                //error(1, 0, "error");
            }
            
        }
        else if(  (**input).type == PIPE_COMMAND||(**input).type==SEQUENCE_COMMAND  )
        {
            if((*(**input).next).type == SIMPLE_COMMAND || !strcmp((*(**input).next).buffer,"(") )
            {
                
            }
            else
            {
                //error(1, 0, "error");
            }
        }
        else if(!strcmp((**input).buffer,"("))
        {
            if((*(**input).next).type == SIMPLE_COMMAND)
            {
                numOpening++;
            }
            else
            {
                //error(1, 0, "error");
            }
            
        }
        else if(!strcmp((**input).buffer,")"))
        {
            int i=0;
            while(  (**input).next != NULL  &&   (*(**input).next).buffer[i] == ' ')
            {
                i++;
            } //ADD extra check for if the thing after ) is a < or >
            
            if( (**input).next ==NULL ||  (*(**input).next).type == AND_COMMAND || (*(**input).next).type == SEQUENCE_COMMAND || (*(**input).next).type == OR_COMMAND || (*(**input).next).type == PIPE_COMMAND || (**input).next == NULL || (*(**input).next).buffer[i] == '<' ||  (*(**input).next).buffer[i] == '>') //ADD here too
            {
                numClosing++;
                if(numClosing > numOpening)
                {
                //error(1, 0, "error");
                }
            }
            else
            {
                //error(1, 0, "error");
            }
            
        }
        
        input = &((**input).next); //move to next token
    }
    if (numOpening != numClosing)
        {
            //error(1, 0, "error");
        }
}


void checkIO (signalTree* input)//using a pointer because I don't want to advance the head returned in TurnIntoSignals
{
    //
    //
    //ADD
    int flag=0; //set a flag to see if we need to set a subshell's input and output
    signalTree* temp;
    //
    //
    //ADD
    int whereInBuffer = 0;
    int whereInNew = 0;
    int spotInBuffer=0;
    int numRight=0;//keep track of number of redirects (max 1)
    int numLeft=0;//keep track on number of redirects (max 1)
    while (*input != NULL) //I want to check every signal in linked list
    {
        //NOTE: ASSUMING CORRECT STRING
        //
        //ADD; idea: need to access start of subshell in order to set it's i/o
        if(!strcmp((**input).buffer,"("))
        {
            temp=input; //have pointer to subshell
            (**input).input = (char*) checked_malloc(strlen((**input).buffer)); //instantiate it's i/o
            (**input).output = (char*) checked_malloc(strlen((**input).buffer));
            (**input).input=0;
            (**input).output=0;
        }
        //
        //
        //ADD
        if((**input).type == SIMPLE_COMMAND) //now I need to see if the signal we're on has IO
        {
            while((**input).buffer[spotInBuffer]!='\0') //checking for 3 cases a<b, a>b, a<b>c; these are only cases
            {
                if((**input).buffer[spotInBuffer] == '<')
                    numLeft++;
                else if((**input).buffer[spotInBuffer] == '>')
                    numRight++;
                spotInBuffer++;
            }
            //case 1 a<b>c
            if(numRight && numLeft)
            {
                //
                //
                //ADD
                if((**input).last!=NULL &&!strcmp((*((**input).last)).buffer,")")) //the thing before it is this ) need to set the signal to empty and give ) the proper input/output
                {
                    flag=1;
                }
                //
                //
                //ADD
                whereInBuffer = 0;
                whereInNew = 0;
                //creating a string to put into the token's word
                char* newWord = (char*)checked_malloc(strlen((**input).buffer));
                //creating a string to put into the token's input (need to malloc space for token's input)
                char* newInput = (char*)checked_malloc(strlen((**input).buffer));
                (**input).input = (char*) checked_malloc(strlen((**input).buffer));
                //creating a string to put into the token's output (need to malloc space for token's output)
                char* newOutput = (char*)checked_malloc(strlen((**input).buffer));
                (**input).output = (char*) checked_malloc(strlen((**input).buffer));
                while((**input).buffer[whereInBuffer] != '<')
                {
                    newWord[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                whereInNew = 0;
                whereInBuffer++;
                while((**input).buffer[whereInBuffer] != '>')
                {
                    newInput[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                whereInNew = 0;
                whereInBuffer++;
                while((**input).buffer[whereInBuffer] != '\0')
                {
                    newOutput[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                //
                //
                //ADD
                if (flag) //set subshell's i/o and set the current signals buffer to space so we get rid of it later
                {
                    free((**input).buffer);
                    (**input).buffer = malloc(sizeof(char));
                    (**input).buffer = " "; //my attempt to set buffer to a space
                    free((**temp).input);  //need to give it i/o, instantiated to have 0 i/o
                    free((**temp).output);
                    (**temp).input = malloc(strlen(newInput));
                    (**temp).output= malloc(strlen(newOutput));
                    strcpy((**temp).input, newInput);
                    strcpy((**temp).output, newOutput);
                }
                //
                //
                //ADD
                //free word and malloc it
                else
                {
                    free((**input).buffer);
                    (**input).buffer = malloc(strlen(newWord));
                    strcpy((**input).buffer, newWord);
                    strcpy((**input).input, newInput);
                    strcpy((**input).output, newOutput);
                }
                flag=0;
                //
                //
                //ADD
            }
            //case 2 a<b
            else if (numLeft)
            {
                //
                //
                //ADD
                if((**input).last!=NULL && !strcmp((*((**input).last)).buffer,")")) //the thing before it is this ) need to set the signal to empty and give ) the proper input/output
                {
                    flag=1;
                }
                //
                //
                //ADD
                whereInBuffer = 0;
                whereInNew = 0;
                //creating a string to put into the token's input (need to malloc space for token's input)
                char* newInput = (char*)checked_malloc(strlen((**input).buffer));
                (**input).input = (char*) checked_malloc(strlen((**input).buffer));
                //creating a string to put into the token's output (need to malloc space for token's output)
                char* newOutput = (char*)checked_malloc(strlen((**input).buffer));
                (**input).output = (char*) checked_malloc(strlen((**input).buffer));
                while((**input).buffer[whereInBuffer] != '<')
                {
                    newOutput[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                whereInNew = 0;
                whereInBuffer++;
                while((**input).buffer[whereInBuffer] != '\0')
                {
                    newInput[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                //
                //
                //ADD
                if(flag)//set subshell's i/o and set the current signals buffer to null so we get rid of it later
                {
                    free((**input).buffer);
                    (**input).buffer = malloc(sizeof(char));
                    (**input).buffer = " "; //my attempt to set buffer to a space
                    free((**temp).input);  //need to give it i/o, instantiated to have 0 i/o
                    //free((**temp).output);
                    (**temp).input = malloc(strlen(newInput));
                    //(**temp).output= malloc(strlen(newOutput)); //OUTPUT COMMENTED OUT BECAUSE SHOULD BE INITIALIZED TO 0 ALREADY
                    //(**temp).output = 0;
                    strcpy((**temp).input, newInput);
                }
                //
                //
                //ADD
                //free word and malloc it
                else
                {
                    free((**input).buffer);
                    (**input).buffer = malloc(strlen(newOutput));
                    (**input).output = 0;
                    strcpy((**input).input, newInput);
                    strcpy((**input).buffer, newOutput);
                }
                flag=0;
                //
                //
                //ADD
            }
            //case 3
            else if(numRight)
            {
                //
                //
                //ADD
                if((**input).last!=NULL && !strcmp((*((**input).last)).buffer,")")) //the thing before it is this ) need to set the signal to empty and give ) the proper input/output
                {
                    flag=1;
                }
                //
                //
                //ADD
                whereInBuffer = 0;
                whereInNew = 0;
                //creating a string to put into the token's input (need to malloc space for token's input)
                char* newInput = (char*)checked_malloc(strlen((**input).buffer));
                (**input).input = (char*) checked_malloc(strlen((**input).buffer));
                //creating a string to put into the token's output (need to malloc space for token's output)
                char* newOutput = (char*)checked_malloc(strlen((**input).buffer));
                (**input).output = (char*) checked_malloc(strlen((**input).buffer));
                while((**input).buffer[whereInBuffer] != '>')
                {
                    newInput[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                whereInNew = 0;
                whereInBuffer++;
                while((**input).buffer[whereInBuffer] != '\0')
                {
                    newOutput[whereInNew] = (**input).buffer[whereInBuffer];
                    whereInNew++;
                    whereInBuffer++;
                }
                //
                //
                //ADD
                
                if(flag)//set subshell's i/o and set the current signals buffer to null so we get rid of it later
                {
                    free((**input).buffer);
                    (**input).buffer = malloc(sizeof(char));
                    (**input).buffer = " "; //my attempt to set buffer to a space
                   // free((**temp).input);  //need to give it i/o, instantiated to have 0 i/o
                    free((**temp).output);
                  //  (**temp).input = malloc(strlen(newInput)); //OUTPUT COMMENTED OUT BECAUSE SHOULD BE INITIALIZED TO 0 ALREADY
                    (**temp).output= malloc(strlen(newOutput));
                   // (**temp).input = 0;
                    strcpy((**temp).output, newOutput);
                }
                
                //
                //
                //ADD
                //free word and malloc it
                else
                {
                    free((**input).buffer);
                    (**input).buffer = malloc(strlen(newInput));
                    (**input).input = 0;
                    strcpy((**input).buffer, newInput);
                    strcpy((**input).output, newOutput);
                }
                flag=0;
                //
                //
                //ADD
                
            }
            else
            {
                (**input).input = (char*) checked_malloc(strlen((**input).buffer));
                (**input).output = (char*) checked_malloc(strlen((**input).buffer));
                (**input).input=0;
                (**input).output=0;
            }
        }
        input = &((**input).next); //move to next token
        //reset
        numRight=0;
        numLeft=0;
        spotInBuffer=0;
    }
}

struct commandLine* wildForrest(char *buffer)
{
    int numlines = 1;
    //char * stringToUse = checked_malloc(strlen(buffer));
    char * lookAhead = (buffer);
    int numCommands = 1;
    lookAhead++;
    
    struct commandLine* temp = checked_malloc(sizeof(struct commandLine));
    temp[numCommands-1].completeCommand = checked_malloc(strlen(buffer));
    //numCommands++;
    
    while (*buffer=='\n')
    {
        numlines++;
        buffer++; //advance both our pointers together
        lookAhead++;
    }
    //skip leading newlines
    
    //now we MUST be on a word, and we need to take in the entire command string here
    
    //errorChange
    if(!isWord(*buffer) && (*buffer)!= '(')
    {

        //error(1,0,"Error Report on Line %d\n", numlines);
        
        
    }
    
    
    int letter= 1;
    //cycle thru the string from beginning to end of entire input buffer, which is entire file
    while(*buffer!='\0')
    {
        
               temp[numCommands-1].completeCommand [letter-1]= *buffer;
        letter++; //add a char to the string we will add to command line
        
        
        //as long as the buffer is a newline, keepin skippin it BUT adding to the line number we on
        while (*lookAhead=='\n')
        {
            numlines++;
           //buffer++; only advance look ahead keep buff on the last char before newlines
            lookAhead++;
        }
        if(*(--lookAhead) == '\n')
        {
        
            lookAhead++;
            
        booj:
        if(  (isWord(*buffer) || *buffer == ')' )   && (isWord(*lookAhead)|| (*lookAhead) == '\0'||(*lookAhead)=='('|| (isToken(*lookAhead)) )) // a \n \n a so we are done with out command create our command line and package
        {
            //create and add command line
            
            //temp[numCommands-1].completeCommand = stringToUse;
            numCommands++;
            temp = checked_realloc(temp, numCommands *sizeof(struct commandLine));
            temp[numCommands-1].completeCommand = checked_malloc(strlen(buffer));
            //free(stringToUse);
            //stringToUse[0] = 'k';
            letter = 1;
           // char * stringToUse = checked_malloc(strlen(buffer));
            buffer = lookAhead;
             buffer--;
            
            
        }
        else if((isToken(*buffer) || *buffer == '<'|| *buffer == '>') && (isWord(*lookAhead) || (*lookAhead) == '(' ||  (*lookAhead) == ')' ))
                //special token \n a, so we must continue building command line
        {
            buffer = lookAhead; //catch up both to the right place
            buffer--; //retract buffer by one, look ahead needs to be ahead by 1
                    
        }
        else if(isWord(*buffer) && (isToken(*lookAhead)|| *buffer == '<'|| *buffer == '>'))
        {
            buffer = lookAhead; //catch up both to the right place
            buffer--; //retract buffer by one, look ahead needs to be ahead by 1
        }
        else //errorChange
        {
            
           // error(1,0,"Error Report on Line %d\n", numlines);
            
           // printf("error on %d\n", numlines); //erase THIS
        }
        }
        
    
        else if (*(++lookAhead) == '\0')
        {
            //lookAhead--;
            goto booj;
        }
     
        

        buffer++; //advance buffer, both our lookAhead and current
        lookAhead++;
  
        
        
    }
    
    return temp;
}


void linkUpStream(command_stream_t *streamToLink, int counterCommands)
{
    int counter = 0;
    struct command_stream * current = *streamToLink;
    command_stream_t *temp = streamToLink;
    
    *temp++;
    
    
    while (counter<(counterCommands-1)) {
        
        (*streamToLink)->next = *temp;
        
        temp++;
        streamToLink++;
        
        
    }
    
    
}



command_t constructSimpleCommand(struct signal signal)
{
    command_t newCommand = checked_malloc(sizeof(struct command));
    
    newCommand->input = malloc(sizeof(strlen(signal.input)));
    newCommand->output = malloc(sizeof(strlen(signal.output)));
    
    if(signal.input!=0)
    {	
        char *bufferOne = cleanLeadingWhiteSpace(signal.input);
        strcpy(newCommand->input,bufferOne);	
    }	
    else
    {
        newCommand->input = 0;
    }
    if(signal.output!=0)
    {
    
	char *buffer = cleanLeadingWhiteSpace(signal.output);
	strcpy(newCommand->output,buffer);
	
    }
    else
    {
	newCommand->output = 0;
    }
    
    newCommand->u.word = malloc(strlen(signal.buffer)*sizeof(char*));
    newCommand->u.word[0] = malloc(strlen(signal.buffer));
    
   // strcpy(newCommand->u.word[0],signal.buffer);
    
    
    char *temp = malloc(2*2*sizeof(char)*strlen(signal.buffer));
    
    int i = 0;
    
    int a = 0;
    
    while(signal.buffer[i]!='\0')
    {
        temp[a] =signal.buffer[i];
        a++;
        i++;
        
        if(signal.buffer[i]==' ')
        {
            temp[a] = signal.buffer[i];
            i++;
            a++;
            while (signal.buffer[i]==' ') {
                i++;
            }
            
        }
        
    }
    
    temp[a] = '\0';
    
    int whatWord= 0;
    int whatLetter = 0;
    int d = 0;
    
    if(temp[d] == ' ')
    {
        while (temp[d] == ' ') {
            d++;
        }
        
    }
    
    while(temp[d]!='\0')
    {
        newCommand->u.word[whatWord][whatLetter] = temp[d];
        d++;
        whatLetter++;
        if(temp[d] == ' ')
        {
     	

	//	strcpy(newCommand->u.word[whatWord], cleanLeadingWhiteSpace(newCommand->u.word[whatWord]));
	  	newCommand->u.word[whatWord][whatLetter] = '\0';     
            while (temp[d]==' ') {
                d++;
            }
            
            whatLetter = 0;
            whatWord++;
            //d++;
        
            newCommand->u.word[whatWord] = malloc(strlen(signal.buffer)*sizeof(char));
       		if(d == a)
		{
		newCommand->u.word[whatWord] = NULL;
		}

	 }

        
    }
    whatWord++;
	newCommand->u.word[whatWord] = malloc(strlen(signal.buffer)*sizeof(char));
    newCommand->u.word[whatWord] = NULL;
    newCommand->status = -1;
    newCommand->type = SIMPLE_COMMAND;
    
    return newCommand;

}
//
//
//ADD; added two parameters + changed logic for making subshell
command_t constructAllOtherCommands(command_t first, command_t second, enum command_type whatType, char* input, char* output)
{
    command_t returner = checked_malloc(sizeof(struct command));
    
    
    returner->type = whatType; // set the type
    if(second == NULL) //this is the subshell case
    {
        returner->input = malloc(1024);
        returner->output = malloc(1024);
        if(input!=0)
        {
            strcpy(returner->input, cleanLeadingWhiteSpace( input));
        }
        else
        {
            returner->input = 0;
        }
        if(output!=0)
        {
            strcpy(returner->output, cleanLeadingWhiteSpace( output));
        }
        else
        {
            returner->output = 0;
        }
        returner->u.subshell_command = first;
    }
    else //this is all other cases
    {
        returner->u.command[0] = first;
        returner->u.command[1] = second;
    }
    
    returner->status = -1;
    
    return returner;
    
}
//
//
//ADD

char * killComments(int (*get_next_byte) (void *),
                    void *get_next_byte_argument, int *numlines, char **allWords)
{
    
 //   char *allWords;
    
    
    *allWords = (char*) checked_malloc(sizeof(char));
    char symbol;
    int charspace=0;
    int numBytes = 0;
    getByte(get_next_byte, get_next_byte_argument, &symbol,numlines);
    
    if(symbol!='#')
    {
       (*allWords)[charspace] = symbol;
        charspace++;
       *allWords=(char*)checked_realloc(*allWords, charspace*sizeof(char));
    }
    numBytes++;
    while(symbol!=EOF)
    {
        
        if(symbol == '#')
        {
            
            if(numBytes == 1)
            {
                while (symbol!='\n' && symbol != EOF)  {
                    getByte(get_next_byte, get_next_byte_argument, &symbol,numlines);
                    numBytes++;
                    
                }
                (*allWords)[charspace] = symbol;
                charspace++;
                *allWords=(char*)checked_realloc(*allWords, (charspace+1)*sizeof(char));
                
                
                
                
                
            }
            else
            {
                if((*allWords)[charspace-1]!='\'' && (*allWords)[charspace-1] != '\"')
                {
                    while (symbol!='\n' && symbol != EOF)  {
                        getByte(get_next_byte, get_next_byte_argument, &symbol,numlines);
                        numBytes++;
                    }
                    (*allWords)[charspace] = symbol;
                    charspace++;
                    *allWords=(char*)checked_realloc(*allWords,(charspace+1)*sizeof(char));
                    
                }
                else
                {
                    (*allWords)[charspace] = symbol;
                    charspace++;
                    *allWords=(char*)checked_realloc(*allWords,(charspace+1)*sizeof(char));
                }
                
            }
            
            if(symbol==EOF)
            {
                // allWords = (char*) checked_realloc(allWords, (charspace+1)*sizeof(char));
                (*allWords)[charspace] = '\0';
                return *allWords;
                
            }
            
            
        }
        
        getByte(get_next_byte, get_next_byte_argument, &symbol,numlines);
        if(symbol!='#')
        {
            if(symbol == EOF)
            {
                // allWords = (char*) checked_realloc(allWords, (charspace+1)*sizeof(char));
                (*allWords)[charspace] = '\0';
                return *allWords;
            }
            (*allWords)[charspace] = symbol;
            charspace++;
            (*allWords)=(char*)checked_realloc
            (*allWords, (charspace+1)*sizeof(char));
        }
        numBytes++;
        
    }
    //allWords = (char*) checked_realloc(allWords, (charspace+1)*sizeof(char));
    (*allWords)[charspace] = '\0';
    return *allWords;
}

//
//
//ADD
command_t firstTreePrune(command_t candidateCommandOne,command_t candidateCommandTwo, enum command_type type)
{
    if(candidateCommandTwo == NULL)//is subhsell
    {
        return constructAllOtherCommands(candidateCommandOne, candidateCommandTwo, type, candidateCommandOne->input, candidateCommandOne->output);
    }
    
    if(candidateCommandTwo->type == 4 || candidateCommandTwo->type ==5)//i/o should be null unless explicitly creating subshell
    {
        return constructAllOtherCommands(candidateCommandOne, candidateCommandTwo, type, NULL, NULL);
    }
    
    else if(type == 3 || type==1)
    {
        return constructAllOtherCommands(candidateCommandOne, candidateCommandTwo, type, NULL, NULL);
    }
    else
        return NULL;
    
}
//
//
//ADD

//
//
//ADD
command_t secondTreePrune(command_t candidateCommandOne, command_t candidateCommandTwo,enum command_type type) {
    // in a sub shell command, the second argument 'b' is always set to NULL
    
 
    command_t tester = firstTreePrune(candidateCommandOne,candidateCommandTwo,type);
    
    if(tester==NULL) //subshell has highest precedence so this should never deal with a subshell case
    {
        tester = constructAllOtherCommands(secondTreePrune(candidateCommandOne,candidateCommandTwo->u.command[0],type), candidateCommandTwo->u.command[1], candidateCommandTwo->type, NULL, NULL);
    }
    return tester;

}
//
//
//ADD


command_t buildGoodTree(signalTree *treeToClean)
{

    command_t candidateCommandOne; // this is how we will compile the signal tree into a good tree
        //this is command one that we will work on
    
    if((*treeToClean) == NULL)  // the other base case when we are at the end of the signalTree
    {
        return NULL;
    }
    
    if((*treeToClean)->type == SIMPLE_COMMAND)
    {
        candidateCommandOne = constructSimpleCommand(**treeToClean); //base case for recursion we stop if we hit a simple command
            //cant break down further
        *treeToClean =  (*treeToClean)->next; //advance our token by 1
        if((*treeToClean) == NULL)
        {
            return candidateCommandOne; // this checks to see if we hit the end of our signal tree here, if so just return this guy
        }
    }
    else if((*treeToClean)->type == SUBSHELL_COMMAND)
    {
        //
        //
        //ADD; idea create temp i/o on this stack to add it to the subshell
        char* input = malloc(1024);
        char* output = malloc(1024);
        
	if((**treeToClean).input!=0)
{
	strcpy (input, (**treeToClean).input);
} 
	else
		input = 0;
	if((**treeToClean).output!=0)
{
       strcpy (output, (**treeToClean).output);
 }
	else
	output=0;

       //
        //
        //
        *treeToClean = (*treeToClean)->next; //advance by one to pass the (
        command_t subshell = buildGoodTree(treeToClean);
      	*treeToClean = (*treeToClean)->next;

	  //
        //
        //ADD: idea add an i/o parameters so that we can give the subshell command i/o
        candidateCommandOne = constructAllOtherCommands(subshell, NULL, SUBSHELL_COMMAND, input, output); // this is the subshell case
        if((*treeToClean) == NULL)
        {
            return candidateCommandOne; // this checks to see if we hit the end of our signal tree here, if so just return this guy
        }
        //
        //
        //ADD
  
        
    }
    if(!strcmp((*treeToClean)->buffer,")"))
    {
           return candidateCommandOne; //if we hit the end of the subshell then advance it
    }
       
    int type = (*treeToClean)->type;
    *treeToClean = (*treeToClean)->next;
 
    
    command_t candidateCommandTwo = buildGoodTree(treeToClean);
    
    //
    //
    //ADD
    if(candidateCommandTwo == NULL) //has to be a subshell
    {
        return constructAllOtherCommands(candidateCommandOne, candidateCommandTwo, type, candidateCommandOne->input, candidateCommandOne->output);
    }
    
    if(candidateCommandTwo->type == 4 || candidateCommandTwo->type ==5)//i/o should be null unless strictly creating a subshell
    {
        
         return constructAllOtherCommands(candidateCommandOne, candidateCommandTwo, type, NULL, NULL);

    }
        
    else if(type == 3 || type == 1)
    {
         return constructAllOtherCommands(candidateCommandOne, candidateCommandTwo, type, NULL, NULL);
    }
    else
        return secondTreePrune(candidateCommandOne, candidateCommandTwo, type);
    //
    //
    //ADD
}

void killEmptyTokens(signalTree * listToWork)
{
    while(*listToWork!=NULL)
    
    {
        
        int counter = 0;
        while ((**listToWork).buffer[counter]!='\0')
        {
            if(isspace((**listToWork).buffer[counter]))
            {
                counter++;
            }
            else
            {
                goto booj;
            }
        }
            
        
        if((**listToWork).buffer[0] !='\0')
        {
            signalTree bad = *listToWork;
           if((*bad).next == NULL)
  	  {
           (*(*bad).last).next = NULL; 
           } 
	else
	{
            (*(*bad).last).next = (*bad).next;
            (*(*bad).next).last = (*bad).last;
         }   
            free(bad);

        }
    
        else
        {
            booj:
            listToWork = &(**listToWork).next;
            
        }
    }
    
}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
                     void *get_next_byte_argument)
{

    /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

    int numlines = 0;
    
    char *print;
    
    print = (char*) checked_malloc(sizeof(char));
    //print =
    killComments(get_next_byte, get_next_byte_argument, &numlines, &print);
    

    
    struct commandLine * testForrest = wildForrest(print);
    

    
    signalTree * signalList = checked_malloc(sizeof(struct signal*));
    int counterSignalList = 1;
    int badTrees = 0;
    
    command_stream_t streamToReturn = malloc(sizeof(struct command_stream));
    
    
    command_stream_t firstCommand= NULL;
    command_stream_t currentCommand = NULL;
    
    while (testForrest[badTrees].completeCommand != NULL && strcmp(testForrest[badTrees].completeCommand, "")   ) {
        
	signalList[counterSignalList-1] = turnIntoSignals(testForrest[badTrees]);
	
        killEmptyTokens(&signalList[counterSignalList-1]);
        
        checkError(&signalList[counterSignalList-1]); //added error check
        
        checkIO(&signalList[counterSignalList-1]);
        
        
        killEmptyTokens(&signalList[counterSignalList-1]);
        
        checkError(&signalList[counterSignalList-1]);
 
        signalList = checked_realloc(signalList, (counterSignalList+1)*sizeof(struct signal*));
        
        command_t treeToAdd = buildGoodTree(&signalList[counterSignalList-1]);
        command_stream_t adder = checked_malloc(sizeof(struct command_stream));
        adder->next = NULL;
        adder->tree = treeToAdd;
        if (firstCommand == NULL)
	 {
            firstCommand = currentCommand = adder;
        } 
	else
	 {
            currentCommand->next = adder;
            currentCommand = currentCommand->next;
        }
        
        badTrees++;
        counterSignalList++;
    }
 
    return firstCommand;
   

}


command_t
read_command_stream (command_stream_t* s)
{
    /* FIXME: Replace this with your implementation too.  */
  
    if(*s == NULL)
    {
        return NULL;
    }
    else
    {
        command_stream_t temp = *s;
         *s = (**s).next;
        return (*temp).tree;
    }
   
}






