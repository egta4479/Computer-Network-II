/* A tftp client implementation.
   Author: Erik Nordström <erikn@it.uu.se>
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

#include "tftp.h"

extern int h_errno;

#define TFTP_TYPE_GET 0
#define TFTP_TYPE_PUT 1

/* Should cover most needs */
#define MSGBUF_SIZE (TFTP_DATA_HDR_LEN + BLOCK_SIZE)


/* 
 * NOTE:
 * In tftp.h you will find definitions for headers and constants. Make
 * sure these are used throughout your code.
 */


/* A connection handle */
struct tftp_conn {
	int type; /* Are we putting or getting? */
	FILE *fp; /* The file we are reading or writing */
	int sock; /* Socket to communicate with server */
	int blocknr; /* The current block number */
	char *fname; /* The file name of the file we are putting or getting */
	char *mode; /* TFTP mode */
	struct sockaddr_in peer_addr; /* Remote peer address */
	socklen_t addrlen; /* The remote address length */
	char msgbuf[MSGBUF_SIZE]; /* Buffer for messages being sent or received */
};

/* Close the connection handle, i.e., delete our local state. */
void tftp_close(struct tftp_conn *tc) 
{
	if (!tc)
		return;

	fclose(tc->fp);
	close(tc->sock);
	free(tc);
}

/* Connect to a remote TFTP server. */
struct tftp_conn *tftp_connect(int type, char *fname, char *mode, 
			       const char *hostname)
{
	struct hostent *hent;
	struct tftp_conn *tc;
	
	if (!fname || !mode || !hostname)
		return NULL;

	tc = malloc(sizeof(struct tftp_conn));
	
	if (!tc)
		return NULL;
	
	tc->sock = socket(PF_INET, SOCK_DGRAM, 0);

	if (tc->sock < 0) {
		fprintf(stderr, "Socket error!\n");
		free(tc);
		return NULL;
	}

	if (type == TFTP_TYPE_PUT)
		tc->fp = fopen(fname, "rb");
	else if (type == TFTP_TYPE_GET)
		tc->fp = fopen(fname, "wb");
	else {
		fprintf(stderr, "Invalid TFTP mode, must be put or get\n");
		return NULL;		
	}
		 
	if (tc->fp == NULL) {
		fprintf(stderr, "File I/O error!\n");
		close(tc->sock);
		free(tc);
		return NULL;
	}
		
	hent = gethostbyname(hostname);

	if (!hent) 
		goto err_out;

	/* We boldly assume that the first address in the hostent
	 * struct is the correct one */

	tc->peer_addr.sin_family = PF_INET;
	tc->peer_addr.sin_port = htons(TFTP_PORT);
	
	memcpy(&tc->peer_addr.sin_addr, 
	       hent->h_addr_list[0], sizeof(struct in_addr));
	
	tc->addrlen = sizeof(struct sockaddr_in);

	tc->type = type;
	tc->mode = mode;
	tc->fname = fname;
	tc->blocknr = 0;

	memset(tc->msgbuf, 0, MSGBUF_SIZE);
	
	return tc;

err_out:
	close(tc->sock);
	fclose(tc->fp);
	free (tc);
	return NULL;
}


/*
 struct line {
       int length;
       char contents[0];
     };
     
     struct line *thisline = (struct line *)
       malloc (sizeof (struct line) + this_length);
     thisline->length = this_length;
*/
/*
  Send a read request to the server.
  1. Format message.
  2. Send the request using the connection handle.
  3. Return the number of bytes sent, or negative on error.
 */
int tftp_send_rrq(struct tftp_conn *tc)
{


	printf("Read request\n");
	int size;
	struct tftp_rrq *rrq;
	rrq = malloc(TFTP_RRQ_HDR_LEN);
	
	size = sendto(tc->sock, &tc->msgbuf, TFTP_RRQ_LEN(tc->fname, tc->mode), 0, (struct sockaddr_in*)&tc->peer_addr, (int) &tc->addrlen);
	//recvfrom(tc->sock,&tc->msgbuf,MSGBUF_SIZE,0,(struct sockaddr *)&tc->peer_addr,&tc->addrlen)
   close(tc->sock);
   free(rrq);
   return size;
}
/*
  
  Send a write request to the server.
  1. Format message.
  2. Send the request using the connection handle.
  3. Return the number of bytes sent, or negative on error.
 */
int tftp_send_wrq(struct tftp_conn *tc)
{
	int size;
	struct tftp_wrq *wrq;
	wrq->opcode = TFTP_OPC_WRQ;
	//size = sendto(tc->sock, tc->msgbuf, TFTP_WRQ_LEN(tc->fname, tc->mode), 0, (struct sockaddr_in*)&tc->peer_addr, tc->addrlen);
   close(tc->sock);
   return size;
}


/*
  Acknowledge reception of a block.
  1. Format message.
  2. Send the acknowledgement using the connection handle.
  3. Return the number of bytes sent, or negative on error.
 */
int tftp_send_ack(struct tftp_conn *tc)
{
	//int size;
	//struct tftp_ack *ack;
	//ack->opcode = TFTP_OPC_ACK;
	//size = sendto(tc->sock, tc->msgbuf, TFTP_WRQ_LEN(tc->fname, tc->mode), 0, (struct sockaddr_in*)&tc->peer_addr, tc->addrlen);
   //close(tc->sock);
   //return size;
   return 0;
	/* struct tftp_ack *ack; */
        
}

/*
  Send a data block to the other side.
  1. Format message.
  2. Add data block to message according to length argument.
  3. Send the data block message using the connection handle.
  4. Return the number of bytes sent, or negative on error.

  TIP: You need to be able to resend data in case of a timeout. When
  resending, the old message should be sent again and therefore no
  new message should be created. This can, for example, be handled by
  passing a negative length indicating that the creation of a new
  message should be skipped.
 */
int tftp_send_data(struct tftp_conn *tc, int length)
{	
	//int size;
	//struct tftp_data *tdata;
	//tdata->opcode = TFTP_OPC_DATA;
	//tdata->blocknr = tc->blocknr;
	//size = sendto(tc->sock, tc->msgbuf, TFTP_WRQ_LEN(tc->fname, tc->mode), 0, (struct sockaddr_in*)&tc->peer_addr, tc->addrlen);
	//close(tc->sock);
	/* struct tftp_data *tdata; */
        return 0;
}

/*
  Transfer a file to or from the server.

 */
int tftp_transfer(struct tftp_conn *tc)
{
  int retval = 0;
  int len;
  int totlen = 0;
  struct timeval timeout;
  /* Define a timeout for resending data. */
  timeout.tv_sec = 2;
  timeout.tv_usec = 20;
    
 
        /* Sanity check */
  if (!tc)
    return -1;
 
        len = 0;
 
  /* After the connection request we should start receiving data
   * immediately */
  
 
        /* Check if we are putting a file or getting a file and send
* the corresponding request. */
    if(tc->type == TFTP_TYPE_GET){
      printf("*** Sending read request\n");
      tftp_send_rrq(tc);
    }
    else if(tc->type == TFTP_TYPE_PUT){
      printf("*** Sending write request\n");
      tftp_send_wrq(tc);
    }
    else{
      printf("*** Error: No type given(put or get)\n");
 
    }
 
        /* ... */
        /*
Put or get the file, block by block, in a loop.
*/
  do {
      printf("*** Enter do-loop\n");
 
    /* 1. Wait for something from the server (using
* 'select'). If a timeout occurs, resend last block
* or ack depending on whether we are in put or get
* mode.
      */        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(tc->sock, &readfds);
        
        select((tc->sock)+1, &readfds, NULL, NULL, &timeout);
        
        int nbytes = BLOCK_SIZE;
        char *buff[nbytes];
        int msgType = -1;
 
 
        if(FD_ISSET(tc->sock, &readfds)){
          printf("*** Kollar FD_ISSET, den är true\n");
          int numbytes;
          if((numbytes = recvfrom(tc->sock,&tc->msgbuf,MSGBUF_SIZE,0,(struct sockaddr *)&tc->peer_addr,&tc->addrlen))==-1)
          {
          /*Return value is the number of bytes received
          and put into buff, or -1 on error. */
          perror("recvfrom");
          exit(1);         
          }
          printf("*** Mottaget packet: \"%s\"\n", buff);
          msgType =*buff[0] + *buff[1];
          printf("*** Meddalande-typ: \"%d\"\n", msgType);
 
        }else{
        
          if(tc->type == TFTP_TYPE_GET){
              printf("*** Timeout, resend ack\n");
          }
          else if(tc->type == TFTP_TYPE_PUT){
              printf("*** Timeout, resend block\n");
              /* send block again */
          }
        }
        
        
 
                /* ... */
 
                /* 2. Check the message type and take the necessary
* action. */
    switch (msgType) {
    case TFTP_OPC_DATA:
                    printf("*** Sending ack\n");
            int blockNr = *buff[2]+*buff[3];        
                        /* Received data block, send ack */
            
      break;
    case TFTP_OPC_ACK:
                        printf("*** Sending next block\n");
                        /* Received ACK, send next block */
      break;
    case TFTP_OPC_ERR:
                        /* Handle error... */
            printf("error\n");
    default:
      fprintf(stderr, "\nUnknown message type\n");
      goto out;
 
    }
 
  } while ( 0 /* 3. Loop until file is finished */);
 
  printf("\nTotal get/put: %d bytes.\n", totlen);
out:  
  return retval;
}

int main (int argc, char **argv) 
{
	
	char *fname = NULL;
	char *hostname = NULL;
	char *progname = argv[0];
	int retval = -1;
	int type = -1;
	struct tftp_conn *tc;

        /* Check whether the user wants to put or get a file. */
	while (argc > 0) {
		
		if (strcmp("-g", argv[0]) == 0) {
			fname = argv[1];
			hostname = argv[2];
			type = TFTP_TYPE_GET;
			break;		
		} else if (strcmp("-p", argv[0]) == 0) {
			fname = argv[1];
			hostname = argv[2];
			
			type = TFTP_TYPE_PUT;
			break;
		}
		argc--;
		argv++;	
	}
	
        /* Print usage message */
	if (!fname || !hostname) {
		fprintf(stderr, "Usage: %s [-g|-p] FILE HOST\n",
			progname);
		return -1;
	}
	printf("***Connecting to remote server\n");
        /* Connect to the remote server */
	tc = tftp_connect(type, fname, MODE_OCTET, hostname);

	if (!tc) {
		fprintf(stderr, "Connection error!\n");
		return -1;
	}

        /* Transfer the file to or from the server */	
	retval = tftp_transfer(tc);
	
        if (retval < 0) {
                fprintf(stderr, "Transfer failed\n");
        }

        /* We are done. Cleanup our state. */
	tftp_close(tc);
	
	return retval;
}
