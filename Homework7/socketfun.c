/*
 * Course:COSC 3750.
 * File: socketfun.h
 * Current Date: March 19, 2021
 * Original Author: Dr. James Plank
 *
 * I got the first version from Dr. James Plank at the 
 * University of Tennessee, Knoxville. No telling how many versions
 * its been through before or since.
 *
 * Refer to the header for all the other includes and a basic howto.
 *
 * October 2014: Need to update the functions here. The gethostbyname() and
 * gethostbyaddr() functions are documented as obsolete and are replaced by
 * getaddrinfo() and getnameinfo().  This will take some work but should not be
 * too bad. 
 *
 * Well that's not right. It was UGLLLLLEEEEEEEE. but it seems to work now. 
 */

#include "socketfun.h"

/*
 * This creates the socket the 'server' will listen to and accept connections
 * on.
 *
 * Modified to use the getaddrinfo() function. The man page examples are miserable 
 * because they assume information not in evidence. Like you are supposed to know 
 * that the getaddressinfo second argument is actually a string representation of 
 * the port number that you want to connect to. The man page calls that a 
 * "service".  However, once you give it a service and a hostname, the ai_addr 
 * that is part of the result, or some other aspect, specifies the port. 
 * Unlike the old version where you had to hand-craft the correct address
 * structure. Good luck figuring out it is supposed to be. 
 */  

int serve_socket(char *hostname, int port)
{

  int rtval;
  int sockfd;
  struct addrinfo *result;
  char pstring[8];

  sprintf(pstring,"%d",port);

  // this is me and my port
  rtval=getaddrinfo(hostname,pstring, NULL, &result);
  if(rtval!= 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rtval));
    return -1;
  }

  /* following the example from man getaddrinfo()*/
  if ((sockfd = socket(result->ai_family, result->ai_socktype,
          result->ai_protocol)) == -1) {
    perror("socket()");
    return -1;
  }

  if (bind(sockfd, result->ai_addr, result->ai_addrlen) == -1) {
    perror("bind()");
    return -1;
  }

  freeaddrinfo(result);
  return sockfd;
}

/*
 * Once the connection is created the 'server' uses this to allow external
 * processes to connect.
 *
 * Did not really need to change anything here, all same-same from previous.
 *
 * Note that the sockaddr_in must be used for AF_INET connections.
 */

int accept_connection(int sockfd)
{
  unsigned int size;
  struct sockaddr_in sock_addr;
  int new_sock;

  sock_addr.sin_family = AF_INET;

  /*
   * Marks the socket as passive, used to accept connections.
   * Second arg is the number of pending requests that can be queued.
   */
  if (listen(sockfd, 1) == -1) {
    perror("listen()");
    return -1;
  }

  /*
   * This function fills the sock_addr with the info about the 
   * host the host at the other end, if you need it. We are ignoring it
   */

  // declared because otherwise next line is too long, waaah.
  size = sizeof(struct sockaddr);

  if ((new_sock = accept(sockfd, (struct sockaddr*)&sock_addr, &size)) == -1) {
    perror("accept()");
    return -1;
  }

  return new_sock;

}

/*
 * seriously modified!!!!!
 * This is the 'client' function, and all it needs to connect to some arbitrary
 * host. The caller must supply a valid hostname and port number.
 *
 * Modified to use the getaddrinfo() function. The examples are miserable because
 * they assume information not in evidence. Like you are supposed to know that
 * the getaddressinfo second argument is actually a string representation of the
 * port number that you want to connect to. The man page calls that a "service".
 * However, once you give it a service and a hostname, the ai_addr that is part
 * of the result, or some other aspect, specifies the port. 
 */
int request_connection(char *hostname, int port)
{
  int rtval;
  int sockfd;
  struct addrinfo *result;
  char rem_port[12];

  sprintf(rem_port,"%d",port);



  // now this is the OTHER machine
  // the NULL means no 'hints'
  rtval=getaddrinfo(hostname,rem_port, NULL, &result);
  if(rtval!= 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rtval));
    return 1;
  }

  /* 
   * This is the same (I hope) as specifying AF_INET and SOCK_STREAM and either
   * 0 or TCP for the protocol.
   *
   * Yup, seems to work correctly! 
   */
  if ((sockfd = socket(result->ai_family, result->ai_socktype,
          result->ai_protocol)) == -1) {
    perror("socket()");
    return -1;
  }

  /*
   * Here is where I get confused. Where the heck is the port in this?
   * Maybe if I cast something differently I'll see it.
   *
   * Yup. These "addresses" like result->ai_addr are there to keep the compiler
   * happy apparently and the functions cast things to what they like. Try
   * running this in gdb, then "print *result->ai_addr", looks really ugly.
   * Well if you "print (struct sockaddr_in)*result->ai_addr" you will see the
   * port number (have to use ntohs() to get the REAL number) along with the
   * type (TCP/IP) and the address (some big int) that is not in dotted decimal.
   */
  
  rtval = connect(sockfd, result->ai_addr, result->ai_addrlen) ;
  if(rtval < 0) {
    perror("Could not connect(): ");
    return -1;
  }
  else {
    return sockfd;
  }
}

