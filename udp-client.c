
/* Sample UDP client 130.238.8.107*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#define OPCODE_RRQ   1
#define MODE_NETASCII "netascii"
#define TFTP_RRQ_LEN(f,m) (sizeof(struct tftp_rrq) + strlen(f) + strlen(m) + 2)
#define TFTP_RRQ_HDR_LEN sizeof(struct tftp_rrq)
   struct tftp_rrq {
	u_int16_t opcode;
	char *fname;
        char endbyte;
        char *mode;
        char endbyte2;
   };
  struct tftp_error {
	u_int16_t opcode;
	u_int16_t errorCode;
	char *errMsg;
        char endbyte;
   };

int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   struct tftp_rrq *readreq;
   readreq = malloc(TFTP_RRQ_HDR_LEN);
   readreq->opcode=htons((u_short)OPCODE_RRQ);
   readreq->fname="dene.c";
   readreq->endbyte=0;
   readreq->mode=MODE_NETASCII;
   readreq->endbyte=0;

  
   printf("filename =%s and size= %d \n",readreq->fname,sizeof(readreq->fname));
   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   memset(&servaddr, 0, sizeof servaddr);
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(argv[1]);
   servaddr.sin_port=htons(6969);
   printf("I am about getting line =%d \n",sizeof(readreq));
   void *result;
   result = malloc (sizeof(struct tftp_rrq)-2);
   printf("%d size of result =%d and %d opcode:%d\n",sizeof(struct tftp_rrq),sizeof(result),TFTP_RRQ_LEN(readreq->fname, readreq->mode)-4,sizeof(readreq->opcode));
   mempcpy (result,&readreq->opcode, sizeof(readreq->opcode));
   mempcpy (result+2,readreq->fname, strlen(readreq->fname));
   mempcpy (result+2+strlen(readreq->fname),&readreq->endbyte, sizeof(readreq->endbyte));
   mempcpy (result+2+strlen(readreq->fname)+1,readreq->mode, strlen(readreq->mode));
   mempcpy (result+2+strlen(readreq->fname)+1+strlen(readreq->mode),&readreq->endbyte2, sizeof(readreq->endbyte2));

   sendto(sockfd,result,sizeof(struct tftp_rrq)-2,0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
   u_int16_t myopcode=0;
   mempcpy (&myopcode,(u_int16_t *)result, sizeof(readreq->opcode));
   printf("package sent and opcode %hd\n",ntohs(myopcode));
   free(result);
   result = NULL;
   void *kresult = malloc (512);
   n=recvfrom(sockfd,kresult,512,0,NULL,NULL);
   //n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
   u_int16_t *opcode;
   opcode=malloc(1);
   memcpy(opcode,(u_int16_t *)kresult,1);
   u_int16_t reop=*opcode;
   memcpy(opcode,(u_int16_t *)(kresult+1),1);
   u_int16_t reop2=*opcode;
 
   u_int16_t *errcode;
   errcode=malloc(1);
   memcpy(errcode,(u_int16_t *)(kresult+2),1);
   u_int16_t reerror=*errcode;
   memcpy(errcode,(u_int16_t *)(kresult+2+1),1);
   u_int16_t reerror2=*errcode; 

   u_int16_t *trial;
   trial=malloc(2);
   memcpy(trial,(u_int16_t *)kresult,2);
   printf("REAL RESULT=%d,\n",ntohs(*trial));
   int opcoderesult=reop<<8|reop2;
   int errorresult=reerror<<8|reerror2;   
   
   char errMsg[n-5];
   memcpy(errMsg,(char *)(kresult+4),n-5);

   char endbyte;
   //endbyte=malloc(1);
   memcpy(&endbyte,(char *)(kresult+n-1),1);
   printf("opcode=%d,errorcode=%d, errormsg=<%s>, endbyte=<%d> package go and result=%d\n",opcoderesult,errorresult,errMsg,endbyte,n);
}
//char txt[512];
//memcpy (txt, (char*)(0x02804C20), sizeof(txt));
