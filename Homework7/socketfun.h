/*
 * Course:COSC 3750.
 * File: socketfun.h
 * Current Date: March 19, 2021
 * Author: Dr. James Plank
 * I go the first version from Dr. James Plank at UT Knoxville. No telling how
 * many versions before or since. 
 *
 * The first two functions are used by the server. Call "serve_socket()" to create
 * the socket that is bound to the specific port. 
 *
 * Then the server calls "accept_connection()", a blocking call, to wait for a
 * client connection request. 
 *
 * The client ONLY needs to use "request_connection()" with the hostname and
 * port number of the server. 
 *
 * The functions have been modified to use getaddrinfo() etc. because the older
 * functions were obsoleted.
 */

/*
 * Do man on socket.h for a good starting place. man 7 socket also gives really
 * heplful information. Well, for some value of helpful.
 */
#ifndef _SOCKET_FUN_H
#define _SOCKET_FUN_H

#include <sys/types.h>
#include <stdio.h>
#include <netdb.h>

extern int serve_socket(char *hostname, int port);
		/* This serves a socket on the given host name 
		 * and port number.  It returns a socket descriptor for that
		 * socket.  hostname should be the name of the 
		 * SERVER's machine. */

extern int accept_connection(int sockfd);
		/* This accepts a connection on the given 
		 * socket (i.e. it should only be called on by
		 * the server. It returns the fd for the 
		 * connection. This fd is of course r/w */

extern int request_connection(char *hostname, int port);
		/* This is what the client calls to connect to
		 * a server's port.  It returns the fd for the
		 * connection. */

#endif
