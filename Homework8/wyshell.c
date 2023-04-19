/*
* wyshell.c
* Author: Buck Harris
* Date: April 1, 2023
*
* COSC 3750, Homework 8
*
* This is the code for the wyshell
* program that emulates the shell
* command.
*/

#include "wyscanner.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node Node;
struct node
{
 struct node *next,*prev;
 char **argv;
 int argc;
 int input, output, error;
 char *in_file, *out_file, *err_file;
};


Node* createNode()
{
 Node *myNode = calloc(1,sizeof(Node));
 if(myNode == NULL)
 {
  perror("calloc()");
  return NULL;
 }
 myNode->argc = 0;
 myNode->argv = calloc(50, sizeof(char*));
 if(myNode->argv == NULL)
 {
  perror("calloc()");
  return NULL;
 }
 myNode->input = STDIN_FILENO;
 myNode->output = STDOUT_FILENO;
 myNode->error = STDERR_FILENO;
 myNode->in_file = "stdin";
 myNode->out_file = "stdout";
 myNode->err_file = "stderr";

 return myNode;
}

void deallocate(Node *head)
{
 Node *current;
 while(head != NULL)
 {
  current = head;
  head = head->next;
  for(int i = 0; i < current->argc; i++)
  {
   free(current->argv[i]);
  }
  free(current->argv);
  free(current);
 }
}

int main(int argc, char** argv)
{
 while(1)
 {
  Node *head = NULL;
  Node *current = NULL;
  int rtn;
  char *rpt;
  char buf[1024];

  int break_flag = 0;

  //get line of input
  fprintf(stdout,"$> ");
  rpt=fgets(buf,256,stdin);
  if(rpt == NULL)
  {
   if(feof(stdin))
   {
    return 0;
   }
   else
   {
    perror("fgets from stdin");
    return 1;
   }
  }

  //create head node
  if(head == NULL)
  {
   head = createNode();
   if(head == NULL)
   {
    return 1;
   }
   current = head;
  }

  //begin parsing line
  rtn = parse_line(buf);
  while(1)
  {
   switch(rtn)
   {
    case QUOTE_ERROR:
     fprintf(stderr,"QUOTE_ERROR\n");
     break_flag++;
     break;
    case ERROR_CHAR:
     fprintf(stderr,"ERROR_CHAR: %c\n", error_char);
     break_flag++;
     break;
    case SYSTEM_ERROR:
     perror("system error");
     exit(1);
     break;
    case EOL:
     if(current->input == PIPE && current->argc == 0)
     {
      fprintf(stderr, "Error: cannot end command with pipe.\n");
      break_flag++;
      break;
     }
     fprintf(stdout," --: EOL\n");
     break_flag++;
     break;
    case REDIR_OUT:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before redirect.\n");
      break_flag++;
      break;
     }
     if(current->output != STDOUT_FILENO)
     {
      break_flag++;
      fprintf(stderr, "Error: too many output redirections.\n");
      break;
     }
     current->output = REDIR_OUT;
     fprintf(stdout," >\n");
     rtn = parse_line(NULL);
     if(rtn == WORD)
     {
      current->out_file = strdup(lexeme);
      printf(" --: %s\n", current->out_file);
     }
     else
     {
      fprintf(stderr, "Error: Invalid  arguments.\n");
      break_flag++;
     }
     break;
    case REDIR_IN:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before redirect.\n");
      break_flag++;
      break;
     }
     if(current->input == PIPE)
     {
      fprintf(stderr, "Error: Cannot redirect input after pipe.\n");
      break_flag++;
      break;
     }
     if(current->input != STDIN_FILENO)
     {
      break_flag++;
      fprintf(stderr, "Error: too many input redirections.\n");
      break;
     }
     current->input = REDIR_IN;
     fprintf(stdout," <\n");
     rtn = parse_line(NULL);
     if(rtn == WORD)
     {
      current->in_file = strdup(lexeme);
      printf(" --: %s\n", current->in_file);
     }
     else
     {
      fprintf(stderr, "Error: Invalid  arguments.\n");
      break_flag++;
     }
     break;
    case REDIR_ERR:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before redirect.\n");
      break_flag++;
      break;
     }
     if(current->error != STDERR_FILENO)
     {
      break_flag++;
      fprintf(stderr, "Error: too many error redirections.\n");
      break;
     }
     current->error = REDIR_ERR;
     fprintf(stdout," 2>\n");
     rtn = parse_line(NULL);
     if(rtn == WORD)
     {
      current->err_file = strdup(lexeme);
      printf(" --: %s\n", current->err_file);
     }
     else
     {
      fprintf(stderr, "Error: Invalid  arguments.\n");
      break_flag++;
     }
     break;
    case APPEND_OUT:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before redirect.\n");
      break_flag++;
      break;
     }
     if(current->output != STDOUT_FILENO)
     {
      break_flag++;
      fprintf(stderr, "Error: too many output redirections.\n");
      break;
     }
     current->output = APPEND_OUT;
     rtn = parse_line(NULL);
     if(rtn == WORD)
     {
      current->out_file = strdup(lexeme);
      printf(" --: %s\n", current->out_file);
     }
     else
     {
      fprintf(stderr, "Error: Invalid  arguments.\n");
      break_flag++;
     }
     fprintf(stdout," >>\n");
     break;
    case APPEND_ERR:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before redirect.\n");
      break_flag++;
      break;
     }
     if(current->error != STDERR_FILENO)
     {
      break_flag++;
      fprintf(stderr, "Error: too many error redirections.\n");
      break;
     }
     current->error = APPEND_ERR;
     fprintf(stdout," 2>>\n");
     rtn = parse_line(NULL);
     if(rtn == WORD)
     {
      current->err_file = strdup(lexeme);
      printf(" --: %s\n", current->err_file);
     }
     else
     {
      fprintf(stderr, "Error: Invalid  arguments.\n");
      break_flag++;
     }
     break;
    case REDIR_ERR_OUT:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before redirect.\n");
      break_flag++;
      break;
     }
     if(current->error != STDERR_FILENO)
     {
      break_flag++;
      fprintf(stderr, "Error: too many error redirections.\n");
      break;
     }
     current->error = REDIR_ERR_OUT;
     current->err_file = strdup(current->out_file);
     fprintf(stdout," 2>&1\n");
     break;
    case SEMICOLON:
     if(current->input == PIPE && current->argc == 0)
     {
      fprintf(stderr, "Error: cannot end command with pipe.\n");
      break_flag++;
      break;
     }
     fprintf(stdout, " ;\n");
     current->next = createNode();
     current = current->next;
     break_flag = 0;
     break;
    case PIPE:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before pipe.\n");
      break_flag++;
      break;
     }
     if(current->output == REDIR_OUT)
     {
      fprintf(stderr, "Error: Cannot redirect output before pipe.\n");
      break_flag++;
      break;
     }
     if(current->input == PIPE && current->argc == 0)
     {
      fprintf(stderr, "Error: too many pipes.\n");
      break_flag++;
      break;
     }
     fprintf(stdout, " |\n");
     current->output = PIPE;
     current->next = createNode();
     current = current->next;
     current->input = PIPE;
     break;
    case AMP:
     if(current->argc == 0)
     {
      fprintf(stderr, "Error: Must specify command before &.\n");
      break_flag++;
      break;
     }
     fprintf(stdout, " &\n");
     rtn = parse_line(NULL);
     if(rtn == EOL)
     {
      if(current->input == PIPE && current->argc == 0)
      {
       fprintf(stderr, "Error: cannot end command with pipe.\n");
       break_flag++;
       break;
      }
      fprintf(stdout," --: EOL\n");
      break_flag++;
      break;
     }
     else if(rtn == SEMICOLON)
     {
      if(current->input == PIPE && current->argc == 0)
      {
       fprintf(stderr, "Error: cannot end command with pipe.\n");
       break_flag++;
       break;
      }
      fprintf(stdout, " ;\n");
      current->next = createNode();
      current = current->next;
      break_flag = 0;
      break;
     }
     else
     {
      fprintf(stderr,"Ampersand must be at end of command.\n");
      break_flag++;
      break;
     }
     case WORD:
      if(current->argc == 0)
      {
       current->argv[argc] = strdup(lexeme);
       current->argc++;
       printf(":--: %s\n", current->argv[argc]);
      }
      else if(current->output != REDIR_OUT &&
                  current->input != REDIR_IN &&
                   current->error != REDIR_ERR)
      {
       current->argv[argc] = strdup(lexeme);
       current->argc++;
       printf(" --: %s\n", current->argv[argc]);
      }
      else
      {
       fprintf(stderr, "Error: Invalid  arguments.\n");
       break_flag++;
      }
      break;
     default:
      fprintf(stdout,"Unknown token: %d\n", rtn);
    }
    //check things that cause us to abandon line
    if(break_flag > 0)
    {
     break;
    }
    rtn = parse_line(NULL);
   }
   deallocate(head);
 }
}







