#include <stdio.h>   
#include <stdlib.h>   
#include <errno.h>   
#include <string.h>   
#include <sys/un.h>   
#include <sys/time.h>   
#include <sys/times.h>   
#include <sys/ioctl.h>   
#include <unistd.h>   
#include <netinet/in.h>   
#include <netdb.h>  
#include <arpa/inet.h>

// Settings ;)

	// TIMEOUT

	#define TIMEOUT_Sec 10
	#define TIMEOUT_Msec 0

	// MAX_PACKET

	#define DEF_PACKET_SIZE 32
	#define MAX_PACKET 1024

	// MIN_PACKET

	#define ICMP_MIN 8    // Minimum 8 byte icmp packet (just header)   

	// MAX_HOPS
	
	#define Max_Hops 64


// Defines for ICMP message types   

#define ICMP_ECHOREPLY      0   
#define ICMP_DESTUNREACH    3   
#define ICMP_SRCQUENCH      4   
#define ICMP_REDIRECT       5   
#define ICMP_ECHO           8   
#define ICMP_TIMEOUT       11   
#define ICMP_PARMERR       12  



// Structs 

	/*

	Bits	0–7		8–15	16–23	24–31
	- 		Type	Code	  Checksum

	Frage ???
	wie groß ist Data-Bereich ?  

*/

struct icmph {         
    char icmph_type;                                // Type of ICMP         
    char icmph_code;                                // Code: = 0
    unsigned short int icmph_checksum;              // checksum ?
    unsigned short icmph_id;                        // Unique id - unuesed
    unsigned short icmph_seq;                       // Sequence number - unused 
};           

typedef struct icmph IcmpHeader;

	
struct iph {     
    unsigned int   iph_ihl:4;                       // Length of the header   
    unsigned int   iph_ver:4;                       // Version of IP      
    unsigned char  iph_tos;            				// Type of service   
    unsigned short iph_total_len;      				// Total length of the packet   
    unsigned short iph_ident;          				// Unique identifier   
    unsigned short iph_frag_and_flags; 				// Flags and Fragment   
    unsigned char  iph_ttl;            				// Time to live   
    unsigned char  iph_proto;          				// Protocol (TCP, UDP etc)   
    unsigned short iph_checksum;       				// IP checksum   
    unsigned int   iph_sourceIP;       				// Source IP   
    unsigned int   iph_destIP;         				// Destination IP              
};           

typedef struct iph IpHeader;


int myprinterror(char* errorMsg){
    perror(errorMsg);
    exit(-1);
    return 0;
}

int decode_resp(char *buf, int bytes, struct sockaddr_in *from, int ttl)   
{   
    IpHeader       *iphdr = NULL;   
    IcmpHeader     *icmphdr = NULL;   
    unsigned short  iphdrlen;   
    struct hostent *lpHostent = NULL;   
    struct in_addr  inaddr = from->sin_addr;   
   
    iphdr = (IpHeader *)buf; 
    // 4 byte = 32 bit  = word   
    iphdrlen = iphdr->iph_ihl * 4;    

    if (bytes < iphdrlen + ICMP_MIN)
    {
        printf("Too few bytes from %s\n", inet_ntoa(from->sin_addr));   
        return 0;
    }
    icmphdr = (IcmpHeader*)(buf + iphdrlen);   

    switch (icmphdr->icmph_type)   
    {   
        case ICMP_ECHOREPLY:     // Der Ziel erreicht wurde.   
            lpHostent = gethostbyaddr((const char *)&from->sin_addr, 4, AF_INET);   
            if (lpHostent != NULL)   
                printf("Hi, I am %s!\n What is your request? \n",lpHostent->h_name);   
            return 1;   
            break;   
        case ICMP_TIMEOUT:         
            	lpHostent = gethostbyaddr((const char *)&from->sin_addr, 4, AF_INET); 
                if (lpHostent != NULL)     
                	printf("%d %s %s\n", ttl ,lpHostent->h_name,inet_ntoa(from->sin_addr));   
            return 0;   
            break;   
        case ICMP_DESTUNREACH:    
            printf("%2d  %s  reports: Host is unreachable\n", ttl,    
                inet_ntoa(inaddr));   
            return 1;   
            break;   
        default:   
            printf("non-echo type %d recvd\n", icmphdr->icmph_type);   
            return 1;   
            break;   
    }  
    return 0;   
}   

unsigned short checksum(unsigned short *buffer, int size)    
{   
    unsigned long cksum=0;   
   
    while(size > 1)    
    {   
        cksum += *buffer++;   
        size -= sizeof(unsigned short);   
    }   
    if(size )   
        cksum += *(unsigned char*)buffer;   
    cksum = (cksum >> 16) + (cksum & 0xffff);   
    cksum += (cksum >> 16);   
   
    return (unsigned short)(~cksum);   
} 


int fill_icmp_data(char *icmp_data,int datasize){
    memset(icmp_data, 0, MAX_PACKET);
    IcmpHeader *icmpH = NULL;
    icmpH = (IcmpHeader*)icmp_data;
    // type 0
    icmpH->icmph_type = ICMP_ECHO;
    memset(icmp_data+sizeof(IcmpHeader),1, datasize - sizeof(IcmpHeader));  
    icmpH->icmph_checksum = checksum((unsigned short*)icmp_data,datasize); 
            

    return 0;
}



int initialization(struct sockaddr_in * from , struct sockaddr_in * dest, const char* destIP){

    // mithilfe von diesem Struct kann man viele Informationen ueber eine Webseite erhalten.
    // z.b die IP-addresse.
    // Anwendung: gethostbyname(..), gethostbyaddr(..)

    struct hostent *hp = NULL;   
    int socketRaw;
    struct timeval timeout;

    // IPPROTO_ICMP ? weil ich ICMP Packte empfangen und senden werde.
    socketRaw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); 
    if (socketRaw < 0) 
        myprinterror("Erorr: Open Socket");   

    timeout.tv_sec  = TIMEOUT_Sec;   
    timeout.tv_usec = TIMEOUT_Msec;  

    // Wie lange soll sockRaw warten, bis die Input/Output Opertion fertig sein wird.

    // Empfang Timeout
    if (setsockopt(socketRaw, SOL_SOCKET, SO_RCVTIMEO,    
        &timeout, sizeof(struct timeval)) < 0)
        myprinterror("Erorr: Set Timeout");

    // Senden Timeout
    if (setsockopt(socketRaw, SOL_SOCKET, SO_SNDTIMEO,    
            &timeout, sizeof(struct timeval)) < 0)
        myprinterror("Erorr: Set Timeout");

    memset(dest, 0, sizeof(struct sockaddr_in));   
    dest->sin_family = AF_INET;  

    // Ip-adress zu IPv4  
    if ((dest->sin_addr.s_addr = inet_addr(destIP)) == INADDR_NONE)   
    {   
        // falls arg[1] ist ein Domain.
        // ich meine, dass arg[1] kein IP ist,
        // dann kann man mithilfe von gethostbyname IP von Domain erhalten.

        hp = gethostbyname(destIP);   
        if (hp)   
            memcpy(&dest->sin_addr, hp->h_addr, hp->h_length);   
        else   
        {   
            // Normaleweise das ist ein Server-side Problem.
            printf("Unable to resolve %s\n",destIP);   
            exit(-1);   
        }   
    } 


    return socketRaw;
}
int main(int argc, char const *argv[])
{
	int socketRaw,datasize,done = 0,checker; 
    // die Grosse von from (sockaddr_in)
    unsigned int fromlen;

    // in sockaddr_in wird die wichtige information ueber den Server oder den Client gespeichert.
    /*
	    struct sockaddr_in {
	    short int          sin_family;  // Address family, AF_INET 
	    unsigned short int sin_port;    // Port number
	    struct in_addr     sin_addr;    // Internet address
	    unsigned char      sin_zero[8]; // Same size as struct sockaddr
		};
	*/

    struct sockaddr_in from,dest;   

    // fuer malloc

    char *recvbuf,*icmp_data;  

    if (argc < 2)
        myprinterror("How To Use: ./tracerout IP-adress or Domain");

    // Die Initialization von socketRaw, dest und from.

    socketRaw = initialization(&from,&dest,argv[1]);

    datasize = DEF_PACKET_SIZE;   
           
    datasize += sizeof(IcmpHeader);     

    icmp_data = malloc(MAX_PACKET*sizeof(char));   
    recvbuf = malloc(MAX_PACKET*sizeof(char));     
   
    if ((icmp_data == NULL) || (recvbuf == NULL))   
        myprinterror("Erorr: malloc"); 

    fill_icmp_data(icmp_data, datasize);  
    fromlen = sizeof(from);

    // Wichtig: Raw socket with ICMP does not let us write IP header values
	for(int ttl = 1; ((ttl < Max_Hops) && (!done)); ttl++)   
    {      
	    if ((setsockopt(socketRaw, IPPROTO_IP, IP_TTL, (int *)&ttl, sizeof(int))) < 0)   
	        myprinterror("Error: Setsockopt");   

        if ((sendto(socketRaw, icmp_data, datasize, 0, (struct sockaddr *)&dest, sizeof(dest))) < 0)
            myprinterror("Erorr: Sendto , Please Try Again ;)");

        if ((checker = recvfrom(socketRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&from, &fromlen)) < 0)   
            myprinterror("Erorr: Sendto");

        done = decode_resp(recvbuf, checker, &from, ttl);
    }   

    free(icmp_data);
    free(recvbuf);
	
	return 0;
}