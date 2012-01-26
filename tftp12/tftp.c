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
	char msgbuf[MSGBUF_SIZE+100]; /* Buffer for messages being sent or received */
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
	printf("started connecting...!\n");
	if (!fname || !mode || !hostname)
		return NULL;

	tc = malloc(sizeof(struct tftp_conn));
	
	if (!tc)
		return NULL;

	/* Create a socket. 
	 * Check return value. */
        tc->sock=socket(AF_INET,SOCK_DGRAM,0);
	/* ... */

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
	
	
	/* get address from host name.
	 * If error, gracefully clean up.*/

	hent = gethostbyname(hostname);
	if (hent == NULL) 
		{
			puts("error resolving hostname.\n");
    			exit(1);
		}

	/* Assign address to the connection handle.
	 * You can assume that the first address in the hostent
	 * struct is the correct one */

	tc->peer_addr.sin_family = AF_INET;
	tc->peer_addr.sin_port = htons(TFTP_PORT);
	
	memcpy(&tc->peer_addr.sin_addr, 
	       hent->h_addr_list[0], sizeof(struct in_addr));
	printf("Address resolved %s\n",(char *)(inet_ntoa(tc->peer_addr.sin_addr.s_addr)));
        //servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	tc->addrlen = sizeof(struct sockaddr_in);

	tc->type = type;
	tc->mode = mode;
	tc->fname = fname;
	tc->blocknr = 0;
	//memset(tc->msgbuf, 0, MSGBUF_SIZE);
	printf("TFTP connected!\n");
	return tc;
}

/*
  Send a read request to the server.
  1. Format message.
  2. Send the request using the connection handle.
  3. Return the number of bytes sent, or negative on error.
 */
int tftp_send_rrq(struct tftp_conn *tc)
{
	/* struct tftp_rrq *rrq; */

	struct tftp_rrq *rrq;
	char endbyte=0;
	printf("sending read request\n");
	rrq = malloc(TFTP_RRQ_LEN(tc->fname, tc->mode));
	rrq->opcode =htons((u_short)OPCODE_RRQ);
	mempcpy (tc->msgbuf,&rrq->opcode, sizeof(rrq->opcode));
        mempcpy (tc->msgbuf+2,tc->fname, strlen(tc->fname));
        mempcpy (tc->msgbuf+2+strlen(tc->fname),&endbyte, sizeof(endbyte));
        mempcpy (tc->msgbuf+2+strlen(tc->fname)+1,tc->mode, strlen(tc->mode));
        mempcpy (tc->msgbuf+2+strlen(tc->fname)+1+strlen(tc->mode),&endbyte, sizeof(endbyte));
	int size = sendto(tc->sock, tc->msgbuf, TFTP_RRQ_LEN(tc->fname, tc->mode), 0, (struct sockaddr*)&tc->peer_addr,sizeof(tc->peer_addr) );
        
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
	/* struct tftp_wrq *wrq; */
	struct tftp_wrq *wrq;
	char endbyte=0;
	printf("sending write request\n");
	wrq = malloc(TFTP_RRQ_LEN(tc->fname, tc->mode));
	wrq->opcode =htons((u_short)OPCODE_WRQ);
	mempcpy (tc->msgbuf,&wrq->opcode, sizeof(wrq->opcode));
        mempcpy (tc->msgbuf+2,tc->fname, strlen(tc->fname));
        mempcpy (tc->msgbuf+2+strlen(tc->fname),&endbyte, sizeof(endbyte));
        mempcpy (tc->msgbuf+2+strlen(tc->fname)+1,tc->mode, strlen(tc->mode));
        mempcpy (tc->msgbuf+2+strlen(tc->fname)+1+strlen(tc->mode),&endbyte, sizeof(endbyte));
	int size = sendto(tc->sock, tc->msgbuf, TFTP_RRQ_LEN(tc->fname, tc->mode), 0, (struct sockaddr*)&tc->peer_addr,sizeof(tc->peer_addr) );
	free(wrq);
        wrq = NULL;
        memset(tc->msgbuf,0,MSGBUF_SIZE);
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
	/* struct tftp_ack *ack; */
        
        return 0;
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
	/* struct tftp_data *tdata; */
        printf("I am sending data! and length=%d",length);
	int size = sendto(tc->sock, tc->msgbuf, length+4, 0, (struct sockaddr*)&tc->peer_addr,sizeof(tc->peer_addr) );
        printf("\nsent data=%d",size);
        return size;
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

        /* Sanity check */
	if (!tc)
		return -1;

        len = 0;

	/* After the connection request we should start receiving data
	 * immediately */
	
        /* Set a timeout for resending data. */

        timeout.tv_sec = TFTP_TIMEOUT;
        timeout.tv_usec = 0;

        /* Check if we are putting a file or getting a file and send
         * the corresponding request. */
        if(tc->type == TFTP_TYPE_GET)
	{
      		printf("RRQ-Read request\n");
      		tftp_send_rrq(tc);
    	}
    	else if(tc->type == TFTP_TYPE_PUT)
	{
      		printf("WR-Write request\n");
      		tftp_send_wrq(tc);
    	}
    	else
	{
      		printf("Error: type is neither put nor get\n");
    	}

        /* ... */

        /*
          Put or get the file, block by block, in a loop.
         */
         u_int16_t counter=0;
         u_int16_t server_counter;
         int data_length;
	 //memset(tc->msgbuf, 0, MSGBUF_SIZE);
	do {
		/* 1. Wait for something from the server (using
                 * 'select'). If a timeout occurs, resend last block
                 * or ack depending on whether we are in put or get
                 * mode. */
		totlen=recvfrom(tc->sock,tc->msgbuf,MSGBUF_SIZE,0,NULL,NULL);

                /* ... */
		u_int16_t msg_type;
   		memcpy(&msg_type,(u_int16_t *)tc->msgbuf,2);
                msg_type=ntohs(msg_type);
		
                u_int16_t block_number;
                memcpy(&block_number,(u_int16_t *)(tc->msgbuf+2),2);
                block_number=ntohs(block_number);
                
                /* 2. Check the message type and take the necessary
                 * action. */
		switch ( msg_type ) {
		case OPCODE_DATA:
                        /* Received data block, send ack */
			printf("Received data block, send ack!\n");
			break;
		case OPCODE_ACK:
			printf("Received ACK, send block! %d\n",block_number);
			counter++;
			server_counter=htons(counter);
			printf("server counter:%d %d",server_counter,ntohs(server_counter));
                        /* Received ACK, send next block */
			u_int16_t opcode=htons((u_int16_t)OPCODE_DATA);
			                        
                        mempcpy(tc->msgbuf, &opcode, sizeof(opcode));
                        mempcpy((tc->msgbuf)+sizeof(opcode), &server_counter, sizeof(server_counter));
			printf("start reading data!");
                        data_length=fread((tc->msgbuf)+(2*sizeof(opcode)), 1, BLOCK_SIZE , tc->fp);
			printf("\nread data length %d\n",data_length);
			//tftp_send_data(tc,data_length);
                        
			break;
		case OPCODE_ERR:
                        /* Handle error... */
			printf(" Handle error...!\n");
                        exit(1);
		default:
			fprintf(stderr, "\nUnknown message type\n");
			goto out;

		}

	} while ( data_length>0 /* 3. Loop until file is finished */);

	printf("\nTotal data bytes sent/received: %d.\n", totlen);
	out:
        fclose(tc->fp);
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

        /* Connect to the remote server */
	tc = tftp_connect(type, fname, MODE_NETASCII, hostname);

	if (!tc) {
		fprintf(stderr, "Failed to connect!\n");
		return -1;
	}

        /* Transfer the file to or from the server */	
	
        retval = tftp_transfer(tc);
	
        if (retval < 0) {
                fprintf(stderr, "File transfer failed!\n");
        }
        
        /* We are done. Cleanup our state. */
	//tftp_send_rrq(tc);
	//tftp_send_wrq(tc);
	tftp_close(tc);
	
	return retval=0;
}
