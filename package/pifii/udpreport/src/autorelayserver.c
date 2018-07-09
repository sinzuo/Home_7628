#include <stdio.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/types.h>   
#include <sys/socket.h>  
#include <netpacket/packet.h>  
#include <net/if.h>  
#include <net/if_arp.h>  
#include <sys/ioctl.h>  
#include <getopt.h>
#include <stdlib.h>
//#include <libubox/uloop.h>
//#include <pthread.h>
#include "log.h"
#include "common.h"
#include "probesource.h"
#include "config.h"


typedef struct{
     char ssid[64];
     char enc[64];
     char key[64];
}wifi_info;

//send to server
#define ETHERNET_FRAME_TYPE_TO_SERVER 0x9886
//send to client
#define ETHERNET_FRAME_TYPE_TO_CLIENT 0x9887

//CMD
#define CMD_TYPE_PROBE 0x0A  //
#define CMD_TYPE_ACK_PROBE 0x0B  //
#define CMD_TYPE_WIFI_INFO 0x0C
#define CMD_TYPE_ACK_WIFI_INFO 0x0D
#define CMD_TYPE_CONF_REQUEST 0x0E
#define CMD_TYPE_CONF_RESPONSE 0x0F

#define CMD_TYPE_APROAM_SEND 0x03
#define CMD_TYPE_WIFICLIENT_SEND 0x10
#define CMD_TYPE_WIFICLIENT_RESP 0x11
//#define CMD_TYPE_APROAM_PRO 0x04

#define CMD_INFO_AP_SSID  0x81 
#define CMD_INFO_AP_ENC   0x82
#define CMD_INFO_AP_KEY   0x86 
#define CMD_INFO_APROAM_SEND  0xA6 
#define CMD_INFO_APROAM_RECV 0xA7
#define CMD_INFO_APROAM_CONF 0xA8
#define CMD_INFO_WIFICLIENT_SEND 0xA9

#define WIFI_CLIENT_SIZE   5*7 
  
#define LEN     60  
//global var
unsigned char ethernet_frame_buffer[4000];
unsigned char ethernet_frame_data[4000];
unsigned char self_mac_addr[6];
static int global_socket_handle;

unsigned char *global_remote_mac_p;
unsigned char global_conf_flag;

unsigned char global_wifi_client[WIFI_CLIENT_SIZE*2]={'\0'};
static int global_wifi_client_size=0;
static int global_wifi_client_flag=1;
//struct uloop_timeout g_send_data_timer;

/* 
void print_str16(unsigned char buf[], size_t len)  
{  
        int     i;  
        unsigned char   c;  
        if(buf == NULL || len <= 0)  
                return;  
        for(i=0; i<len; i++){  
                c = buf[i];  
                printf("%02x", c);  
        }  
        printf("\n");  
}  

void print_sockaddr_ll(struct sockaddr_ll *sa)  
{  
        if(sa == NULL)  
                return;  
        printf("sll_family:%d\n", sa->sll_family);  
        printf("sll_protocol:%#x\n", ntohs(sa->sll_protocol));  
        printf("sll_ifindex:%#x\n", sa->sll_ifindex);  
        printf("sll_hatype:%d\n", sa->sll_hatype);  
        printf("sll_pkttype:%d\n", sa->sll_pkttype);  
        printf("sll_halen:%d\n", sa->sll_halen);  
        printf("sll_addr:"); 
        print_str16(sa->sll_addr, sa->sll_halen);  
}  
*/
int get_wifi_client(unsigned char *wc,int th,int bufsize)
{
    char buf[64] = {'\0'};
    char cmd[256] = {'\0'};
    unsigned char mac[6] = {'\0'}; 
    unsigned char macstr[13] = {'\0'}; 
    unsigned char *rmp=global_remote_mac_p; 
    int dbm=0;
    int iloop = 0;
    
    sprintf(cmd,"iwinfo ra0 a | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\"");
    if(NULL != rmp)
    {
        sprintf(cmd,"%s | grep -v %02X:%02X:%02X:%02X:%02X:%02X",cmd,rmp[0],rmp[1],rmp[2],rmp[3],rmp[4],rmp[5]);
    }
    sprintf(cmd,"%s | awk \'{print $1$2}\' | tr -d :",cmd);
    //sprintf(cmd,"iwinfo ra0 a | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\"\
    //             | grep -v %02X:%02X:%02X:%02X:%02X:%02X | awk \'{print $1$2}\' | tr -d :"
    //             ,rmp[0],rmp[1],rmp[2],rmp[3],rmp[4],rmp[5]);
    //FILE *fpp = popen("iwinfo ra0 a | grep -E \".{2}:.{2}:.{2}:.{2}:.{2}:.{2}\" | awk \'{print $1$2}\' | tr -d :","r");
    FILE *fpp = popen(cmd,"r");
    while(fgets(buf,sizeof(buf),fpp) != NULL && iloop < bufsize)//WIFI_CLIENT_SIZE)
    {
       if( '\n' == buf[strlen(buf) - 1])
       {
           buf[strlen(buf) - 1] = '\0';
       }else
       {
           buf[strlen(buf)] = '\0';
       }
       //printf("buf_len:%d\n",strlen(buf));
       if(15 == strlen(buf)) 
       {
           dbm=atoi(buf+12);
           //printf("dbm:%d\n",dbm);
           if(dbm < th)
           {
               memcpy(macstr,buf,12); 
               macstr[12]='\0';
               //debug("%s",macstr);
               tomac(macstr,mac);
               memcpy(wc+iloop,mac,6);
               *(wc+iloop+6) = (unsigned char)dbm;//(unsigned char)atoi(buf+12);
               iloop = iloop + 7;
           }
       }
    }
    pclose(fpp);
    return iloop; //size
}

int get_relay_wifi(wifi_info *w)
{
    if(NULL == w)
         return -1;
    char buf[64] = {'\0'};
    FILE *fpp = popen("/usr/bin/get_relay_wifi","r");
    while(fgets(buf,sizeof(buf),fpp) != NULL)
    {
       if( '\n' == buf[strlen(buf) - 1])
       {
           buf[strlen(buf) - 1] = '\0';
       }else
       {
           buf[strlen(buf)] = '\0';
       }
       if(0 == strncmp("ssid:",buf,5))
       {
          strcpy(w->ssid,buf+5); 
       }
       else if(0 == strncmp("enc:",buf,4))
       {
          strcpy(w->enc,buf+4); 
          if(0 == strcmp(w->enc,"psk+psk2"))
          {
              strcpy(w->enc,"WPA1PSKWPA2PSK/AES");
          }
          else if(0 == strcmp(w->enc,"psk2"))
          {
              strcpy(w->enc,"WPA2PSK/AES");
          }
          else if(0 == strcmp(w->enc,"psk"))
          {
              strcpy(w->enc,"WPAPSK/AES");
          }
          else if(0 == strcmp(w->enc,"none"))
          {
              strcpy(w->enc,"NONE");
          }
          else if(0 == strcmp(w->enc,"psk2+tkip+ccmp"))
          {
              strcpy(w->enc,"WPA2PSK/TKIPAES");
          }
          else if(0 == strcmp(w->enc,"psk2+tkip"))
          {
              strcpy(w->enc,"WPA2PSK/TKIP");
          }
          else if(0 == strcmp(w->enc,"psk2+ccmp"))
          {
              strcpy(w->enc,"WPA2PSK/AES");
          }
          else if(0 == strcmp(w->enc,"psk+ccmp"))
          {
              strcpy(w->enc,"WPAPSK/AES");
          }
          else if(0 == strcmp(w->enc,"psk+tkip"))
          {
              strcpy(w->enc,"WPAPSK/TKIP");
          }
          else if(0 == strcmp(w->enc,"psk+tkip+ccmp"))
          {
              strcpy(w->enc,"WPAPSK/TKIPAES");
          }
          else
          {
              strcpy(w->enc,"NONE");
          }
       }
       else if(0 == strncmp("key:",buf,4))
       {
          strcpy(w->key,buf+4);
       }
    }
    pclose(fpp);
    return 0;
}

int get_probe_data(unsigned char *probe_buf)
{
    if(NULL == probe_buf)
    {
        return 0;
    }
    unsigned char mactable[256*8];
    unsigned char *data=NULL;
    //unsigned char probe_buf[256*8];
    unsigned char *ppb=NULL;
    char dbm=-95;
    int i=0;
    int j=0;
    int size=0;
    probe_source_init();
    size=probe_source_read(mactable,256*8);
    probe_source_destroy();
    for(i=0;i<size;i=i+8)
    {
        data=mactable + i;
        dbm=data[6]-95;
        if((char)config->roam->route_pro_th <= dbm)
        {
            ppb=probe_buf + j;
            memcpy(ppb,data,6);
            ppb[6] = dbm;
            j = j + 7;
            //printf("%02X:%02X:%02X:%02X:%02X:%02X %d %d\n",data[0],data[1],data[2],data[3],data[4],data[5],dbm,data[7]);
        }
    }
    /*
    for(i=0;i<j;i=i+7)
    {
        ppb=probe_buf+i;
        printf("%02X:%02X:%02X:%02X:%02X:%02X %d\n",ppb[0],ppb[1],ppb[2],ppb[3],ppb[4],ppb[5],(char)ppb[6]);
    }
    */
    return j;
}

int listen_network_dev(char const *network_dev_name, unsigned short type,unsigned char *mac_addr)//,int *interface_index)
{
        int     result=0, fd, n, count=0;  
        char    buf[LEN];  
        struct  sockaddr_ll      sa;   
        struct ifreq    ifr;  
        //unsigned char    dst_mac[6]={'\0'};  
        struct timeval timeout = {10,0};
  
        memset(&sa, 0, sizeof(sa));  
        sa.sll_family = PF_PACKET;  
        sa.sll_protocol = htons(type);  
        //create socket  
        fd = socket(PF_PACKET, SOCK_RAW, htons(type));  
        if(fd < 0){  
                debug("socket error\n");  
                return -1;  
        }  
        //wait for 3s
	if( setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout) ) < 0 )
	{
		debug("can't set receive timeout!");
		return -1;
	}
        
	//wait for 3s
	if( setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout) ) < 0 )
	{
		debug("can't set send timeout!");
		return -1;
	}
        //printf("ifname:%s\n",network_dev_name);
        // get flags   
        strcpy(ifr.ifr_name,network_dev_name);  
        result = ioctl(fd, SIOCGIFFLAGS, &ifr);  
        if(result != 0){   
                debug("ioctl error, get flags\n");  
                return -1;  
        }         
       //need mac addr
	if( mac_addr )
	{
		//copy network card's name
		strncpy(ifr.ifr_name, network_dev_name, sizeof(ifr.ifr_name));
		//get mac address
		if( ioctl(fd, SIOCGIFHWADDR, &ifr) < 0 )
		{
			debug("can't send broadcast packets!");
			return -1;
		}

		memcpy(mac_addr, ifr.ifr_hwaddr.sa_data,6);
	}
        /*
        ifr.ifr_flags |= IFF_PROMISC;  
        // set promisc mode  
        result = ioctl(fd, SIOCSIFFLAGS, &ifr);  
        if(result != 0){  
                perror("ioctl error, set promisc\n");  
                return errno;  
        }  
        */ 

        //get index  
        result = ioctl(fd, SIOCGIFINDEX, &ifr);  
        if(result != 0){  
                debug("ioctl error, get index\n");  
                return -1;  
        }  
        sa.sll_ifindex = ifr.ifr_ifindex;  
  
        //bind fd  
        result = bind(fd, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll));  
        if(result != 0){  
                debug("bind error\n");  
                return -1;  
        }
        return fd; 
}

int util_receive_ethernet_frame(int socket_handle,unsigned char * ethernet_frame,int *recved_data_len)
{
	int data_len;
	unsigned char frame[ETH_FRAME_LEN];
	unsigned char frame_header_len=6+6+2+2;	// dst_mac[6]. src_mac[6]. eth[2]. len[2]

	*recved_data_len = 0;

	memset( frame,0,sizeof(frame) );

	//receive one frame
	data_len = recv(socket_handle,frame,ETH_FRAME_LEN,0);
        //data_len = recvfrom(socket_handle, frame, ETH_FRAME_LEN, 0,0,0); //(struct sockaddr *)&sa_recv, &sa_len);
        //printf("recv:%d\n",data_len);
	if( data_len == -1 )
	{
		return -1;
	}
	else if( data_len < frame_header_len )
	{
		return -1;
	}
	else
	{
                //print_str16(frame,data_len);
		//check if the checksum is ok
		int data_len=ntohs( *( unsigned short* )(frame+OFFSET_OF_ETHERNET_LEN));
		int checksum=ip_checksum(frame,data_len+OFFSET_OF_ETHERNET_DATA);
		//invalid checksum
		if( checksum )
		{
			debug("the checksum is not right!");
			return -1;
		}

		*recved_data_len = data_len;
		memcpy(ethernet_frame,frame,data_len+OFFSET_OF_ETHERNET_DATA);
	}

	return 0;
}

int util_server_send_ack_probe_frame( MacAddr* address)
{
	debug("util_server_send_ack_probe_frame");
        int real_frame_size=0;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_ACK_PROBE,NULL,0);

	//send ethernet frame
         //send ethernet frame
        util_send_ethernet_frame(
                                global_socket_handle
                                ,ETHERNET_FRAME_TYPE_TO_CLIENT
                                ,ethernet_frame_buffer
                                ,real_frame_size
                                ,self_mac_addr
                                ,address->value
                        );
        //save remote mac 
        if(NULL == global_remote_mac_p)
        {
            global_remote_mac_p = (unsigned char *)malloc(6);
        }
        if(NULL != global_remote_mac_p)
        {
            memcpy(global_remote_mac_p,address->value,6);
        }

	return 0;
}

int util_send_aproam_data()
{
        unsigned char wifi_client[WIFI_CLIENT_SIZE];
        int wifi_client_size=get_wifi_client(wifi_client,(char)config->roam->route_sta_th,WIFI_CLIENT_SIZE); //signal > -65 
        if(0 >= wifi_client_size)
        {
           return 0; 
        }
        debug("roam client:%d",wifi_client_size/7);
        unsigned char remote_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 
        if(NULL != global_remote_mac_p)
        {
            memcpy(remote_mac,global_remote_mac_p,6);
        }

	//resend search server message
	int real_frame_size=0;
        int saved_data_len;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_APROAM_SEND,NULL,0);
        saved_data_len=real_frame_size;

        util_add_field_to_frame(CMD_INFO_APROAM_SEND,wifi_client,wifi_client_size);

        //set the data length
	ethernet_frame_buffer[ LENGTH_OF_FIELD ]=real_frame_size-saved_data_len;
	if(real_frame_size-saved_data_len > 255)
	{
		ethernet_frame_buffer[ LENGTH_OF_FIELD ]=254;
	}
	//util_start_to_send_one_frame(real_frame_size,address,is_broadcast);
	//send ethernet frame
        util_send_ethernet_frame(
				global_socket_handle
				,ETHERNET_FRAME_TYPE_TO_CLIENT
				,ethernet_frame_buffer
				,real_frame_size
				,self_mac_addr
                                ,remote_mac
				//,address->value
			);
	return 0;
}
int util_send_wificlient()
{
        unsigned char wifi_client[WIFI_CLIENT_SIZE*2];
        unsigned char wifi_client_tmp[WIFI_CLIENT_SIZE*2];
        int wifi_client_size=get_wifi_client(wifi_client_tmp,(char)0,WIFI_CLIENT_SIZE*2); //signal > -65 
        if(0 >= wifi_client_size)
        {
           return 0; 
        }
        int i=0,j=0;
        for(i=0;i<wifi_client_size;i=i+7) //mac:6,dbm:1
        {
            memcpy(wifi_client+j,wifi_client_tmp+i,6);
            j=j+6;
        }
        wifi_client_size = j;
        if(global_wifi_client_size == wifi_client_size 
           && 0 == memcmp(global_wifi_client,wifi_client,wifi_client_size)
           && 0 == global_wifi_client_flag)
        {
            return 0; 
        }
        global_wifi_client_size = wifi_client_size; 
        memset(global_wifi_client,'\0',WIFI_CLIENT_SIZE*2);
        memcpy(global_wifi_client,wifi_client,wifi_client_size);
        global_wifi_client_flag = 1;
        debug("wifi client:%d",wifi_client_size/6); 
        unsigned char remote_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 
        if(NULL != global_remote_mac_p)
        {
            memcpy(remote_mac,global_remote_mac_p,6);
        }

	//resend search server message
	int real_frame_size=0;
        int saved_data_len;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_WIFICLIENT_SEND,NULL,0);
        saved_data_len=real_frame_size;

        util_add_field_to_frame(CMD_INFO_WIFICLIENT_SEND,wifi_client,wifi_client_size);

        //set the data length
	ethernet_frame_buffer[ LENGTH_OF_FIELD ]=real_frame_size-saved_data_len;
	if(real_frame_size-saved_data_len > 255)
	{
		ethernet_frame_buffer[ LENGTH_OF_FIELD ]=254;
	}
	//util_start_to_send_one_frame(real_frame_size,address,is_broadcast);
	//send ethernet frame
        util_send_ethernet_frame(
				global_socket_handle
				,ETHERNET_FRAME_TYPE_TO_CLIENT
				,ethernet_frame_buffer
				,real_frame_size
				,self_mac_addr
                                ,remote_mac
				//,address->value
			);
	return 0;
}


int util_server_send_wifi_frame(MacAddr* address)
{
	//resend search server message
	debug("util_server_send_wifi_frame");
        wifi_info wi; 
        memset(&wi,'\0',sizeof(wifi_info));
        get_relay_wifi(&wi);
        if(0 == strlen(wi.ssid) || 0 == strlen(wi.enc))
        {
            return -1;
        }
	int real_frame_size=0;
        int saved_data_len;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_ACK_WIFI_INFO,NULL,0);
        saved_data_len=real_frame_size;

        if (0 < strlen(wi.ssid) && 0 < strlen(wi.enc))
        {
	    util_add_field_to_frame(CMD_INFO_AP_SSID,wi.ssid,strlen(wi.ssid)+1);
	    util_add_field_to_frame(CMD_INFO_AP_ENC,wi.enc,strlen(wi.enc)+1);
        }
        if (0 != strcmp("NONE",wi.enc) && 0 < strlen(wi.key))
        {
	    util_add_field_to_frame(CMD_INFO_AP_KEY,wi.key,strlen(wi.key)+1);
        }

        //set the data length
	ethernet_frame_buffer[ LENGTH_OF_FIELD ]=real_frame_size-saved_data_len;
	if(real_frame_size-saved_data_len > 255)
	{
		ethernet_frame_buffer[ LENGTH_OF_FIELD ]=254;
	}
	//util_start_to_send_one_frame(real_frame_size,address,is_broadcast);
	//send ethernet frame
        util_send_ethernet_frame(
				global_socket_handle
				,ETHERNET_FRAME_TYPE_TO_CLIENT
				,ethernet_frame_buffer
				,real_frame_size
				,self_mac_addr
				,address->value
			);
	return 0;
}

int roam_config_request(MacAddr* address)
{
	int real_frame_size=0;
        int saved_data_len;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_CONF_RESPONSE,NULL,0);
        saved_data_len=real_frame_size;

        unsigned char th[2] = {0};
        th[0] = config->roam->relay_sta_th; 
        th[1] = config->roam->relay_pro_th; 
        debug("roam_config_request,relayStaTh:%d,relayProTh:%d",(char)th[0],(char)th[1]);

	util_add_field_to_frame(CMD_INFO_APROAM_CONF,th,2);
        //set the data length
	ethernet_frame_buffer[ LENGTH_OF_FIELD ]=real_frame_size-saved_data_len;
	if(real_frame_size-saved_data_len > 255)
	{
		ethernet_frame_buffer[ LENGTH_OF_FIELD ]=254;
	}
	//remote mac;
        unsigned char remote_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 
        if(NULL == address)
        {
            if(NULL != global_remote_mac_p)
            {
                memcpy(remote_mac,global_remote_mac_p,6);
            }
        }
        else
        {
            memcpy(remote_mac,address->value,6);
        }

	//send ethernet frame
        util_send_ethernet_frame(
				global_socket_handle
				,ETHERNET_FRAME_TYPE_TO_CLIENT
				,ethernet_frame_buffer
				,real_frame_size
				,self_mac_addr
				,remote_mac
			);
	return 0;

}

int send_aproam_probe(unsigned char *pm,unsigned char len,MacAddr* address)
{
        if(NULL == pm || 0 >= len || NULL == address)
        {
            return 0;
        }
        //process aproam probe
        unsigned char probe_buf[256*8];
        unsigned char *ppb=NULL;
        //unsigned char ml[14]={0x68,0x17,0x29,0x2D,0xB4,0xA5,0xB5,0x48,0x3C,0x0C,0x74,0x69,0x3F,0xB8};
        unsigned char *pml=NULL;
        //int ml_size=14;
        int psize = get_probe_data(probe_buf);
        if(0 >= psize )
        {
            debug("no probe data");
            return 0;
        }
        unsigned char *pms=(unsigned char*)malloc(len);
        unsigned char *ppms=NULL;
        if(NULL == pms)
        {
            return 0;
        } 

        int i = 0;
        int j = 0;
        int fsize = 0; //frame size
        for(j=0;j<len;j=j+7)
        {
            pml=pm+j;
            for(i=0;i<psize;i=i+7)
            {
                ppb=probe_buf+i;
                if(0 == memcmp(pml,ppb,6)) //&& (char)ppb[6] == (char)pml[6])
                {
                    ppms=pms + fsize;
                    memcpy(ppms,pml,7);
                    fsize = fsize + 7;
                    debug("%02X:%02X:%02X:%02X:%02X:%02X %d\n",ppb[0],ppb[1],ppb[2],ppb[3],ppb[4],ppb[5],(char)ppb[6]);
                    break;
                }
            }
        } 
        //end 

        if(0 >= fsize || NULL == pms)
        {
           return 0; 
        }
        debug("response probe data to relay device");
	//resend search server message
	int real_frame_size=0;
        int saved_data_len;
	unsigned char *p_ethernet_frame=ethernet_frame_buffer;
	//add search server cmd
	util_add_field_to_frame(CMD_TYPE_APROAM_SEND,NULL,0);
        saved_data_len=real_frame_size;

        util_add_field_to_frame(CMD_INFO_APROAM_RECV,pms,fsize);

        //set the data length
	ethernet_frame_buffer[ LENGTH_OF_FIELD ]=real_frame_size-saved_data_len;
	if(real_frame_size-saved_data_len > 255)
	{
		ethernet_frame_buffer[ LENGTH_OF_FIELD ]=254;
	}
	//util_start_to_send_one_frame(real_frame_size,address,is_broadcast);
	//send ethernet frame
        util_send_ethernet_frame(
				global_socket_handle
				,ETHERNET_FRAME_TYPE_TO_CLIENT
				,ethernet_frame_buffer
				,real_frame_size
				,self_mac_addr
				,address->value
			);
        free(pms);
	return 0;

}

int aproam_process(unsigned char *pm,unsigned char len)
{
    if(NULL == pm || 7 > len)
    {
        return 0;
    }
    char cmd[304]={'\0'}; //max kickmac 5 numbs
    char *iw="iwpriv ra0 set DisConnectSta=";
    unsigned char *ppb=NULL;
    int j = 0;
    sprintf(cmd,"echo \'");
    //sprintf(cmd,"%s#!/bin/sh\n",cmd);
    for(j=0;j<len;j=j+7)
    {
         ppb=pm+j;
         sprintf(cmd,"%s%s%02X:%02X:%02X:%02X:%02X:%02X\n",cmd,iw,ppb[0],ppb[1],ppb[2],ppb[3],ppb[4],ppb[5]);
         if(5 < j/7) //max 5 nums
         {
             break;
         }
         //printf("%02X:%02X:%02X:%02X:%02X:%02X %d\n",ppb[0],ppb[1],ppb[2],ppb[3],ppb[4],ppb[5],(char)ppb[6]);
    }
    sprintf(cmd,"%s \' >/tmp/iwp.sh;sh /tmp/iwp.sh 2>/dev/null",cmd);
    debug("kickmac cmd:%s",cmd);
    FILE  *fp=popen(cmd,"r");
    pclose(fp);
    return 0; 
}

int util_aproam_decode_field_info(unsigned char* p_ethernet_frame,int frame_len,MacAddr* address)
{
	int current_len=0;
	unsigned char field;
	unsigned char field_length;
	unsigned int current_len_before;
	unsigned char* p_ethernet_frame_before;

	while( current_len < frame_len )
	{
		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}

		//get field value
		util_get_common_data_from_frame(&field,LENGTH_OF_FIELD);
		//get field length
		util_get_common_data_from_frame(&field_length,LENGTH_OF_FIELD_LENGTH);
		current_len_before=current_len;
		p_ethernet_frame_before=p_ethernet_frame;

                unsigned char *pm=NULL;//(unsigned char*)malloc(field_length);
		switch( field )
		{
			case CMD_INFO_APROAM_SEND:
                                pm=(unsigned char*)malloc(field_length);
                                if(NULL != pm)
                                {
                                    debug("process request for roam");
				    memcpy(pm,(const char* )p_ethernet_frame,field_length);
                                    send_aproam_probe(pm,field_length,address);
                                }
                                if(NULL == pm)
                                { 
                                    free(pm);
                                }
				break;
			case CMD_INFO_APROAM_RECV:
                                pm=(unsigned char*)malloc(field_length);
                                if(NULL != pm)
                                {
                                    debug("len:%d,roam execute kickmac",field_length);
				    memcpy(pm,(const char* )p_ethernet_frame,field_length);
                                    aproam_process(pm,field_length);
                                }
                                if(NULL == pm)
                                { 
                                    free(pm);
                                }
				break;
			
			default:
				util_get_common_data_from_frame(NULL,field_length);
				break;
		}

		//avoid error
		if( current_len-current_len_before < field_length )
		{
			current_len=current_len_before+field_length;
			p_ethernet_frame=p_ethernet_frame_before+field_length;
		}

		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}

	}
	return 0;
}

int util_decode_frame_info(unsigned char* ethernet_frame)
{
	MacAddr remote_mac_addr;
	unsigned char* p_ethernet_frame=ethernet_frame;
	int current_len=0;
	int frame_len=ntohs( *( unsigned short* )(ethernet_frame+OFFSET_OF_ETHERNET_LEN) )+OFFSET_OF_ETHERNET_DATA;
	unsigned char field;
	unsigned int field_length;
	unsigned char fake_field_length;
	unsigned int current_len_before;
	unsigned char* p_ethernet_frame_before;

	//get dest mac address
	util_get_common_data_from_frame(NULL,LENGTH_OF_ETHERNET_DST_MAC);
	//get source mac address
	util_get_common_data_from_frame(remote_mac_addr.value,LENGTH_OF_ETHERNET_SRC_MAC);
	//util_put_mac_to_buffer(&remote_mac_addr);
	//get ethernet frame type
	util_get_common_data_from_frame(NULL,LENGTH_OF_ETHERNET_TYPE);
	//get ethernet frame len
	util_get_common_data_from_frame(NULL,LENGTH_OF_ETHERNET_LEN);

	while( current_len < frame_len-LENGTH_OF_ETHERNET_CHECKSUM )
	{
		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}
		//get field value
		util_get_common_data_from_frame(&field,LENGTH_OF_FIELD);
		//get field length
		util_get_common_data_from_frame(&fake_field_length,LENGTH_OF_FIELD_LENGTH);
		current_len_before=current_len;
		p_ethernet_frame_before=p_ethernet_frame;

		field_length=frame_len-current_len-LENGTH_OF_ETHERNET_CHECKSUM;

		switch( field )
		{
                        case CMD_TYPE_PROBE:
                            util_server_send_ack_probe_frame(&remote_mac_addr); 
                            break; 
                        case CMD_TYPE_WIFI_INFO:
                            util_server_send_wifi_frame(&remote_mac_addr); 
                            //global_conf_flag = 1; //need send threadshold to relay device
                            break;
                        case CMD_TYPE_APROAM_SEND:
                            //debug("frame_len:%d",frame_len);   
                            util_aproam_decode_field_info(p_ethernet_frame,field_length,&remote_mac_addr);
                            break;
                        case CMD_TYPE_CONF_REQUEST:
                            //debug("frame_len:%d",frame_len);   
                            roam_config_request(&remote_mac_addr);
			    global_wifi_client_flag = 0;	
                            break;
                        case CMD_TYPE_WIFICLIENT_RESP:
			    global_wifi_client_flag = 0;	
		            break;
			//not recognized cmd
			default:
				util_get_common_data_from_frame(NULL,field_length);
				break;
		}

		//avoid error
		if( current_len-current_len_before < field_length )
		{
			current_len=current_len_before+field_length;
			p_ethernet_frame=p_ethernet_frame_before+field_length;
		}

		//always begin with even byte
		if( current_len&0x01 )
		{
			p_ethernet_frame++;
			current_len++;
		}
	}

	return 0;
}

void deamon(void)
{
        pid_t pid, sid;
        pid = fork();
        if (pid < 0)
                exit(-1);
        if (pid > 0)
                exit(0);

        sid = setsid();
        if (sid < 0) {
                debug("setsid() returned error\n");
                exit(-1);
        }

        char *directory = ".";
        if ((chdir(directory)) < 0) {
            debug("chdir() returned error\n");
            exit(-1);
        }
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        //close(STDIN_FILENO);
        //close(STDOUT_FILENO);
        //close(STDERR_FILENO);
}
/*
static void aproam_data_sender(struct uloop_timeout *timeout)
{
     unsigned char wc[WIFI_CLIENT_SIZE];
     unsigned char *pwc=NULL;
     memset(wc,'\0',sizeof(wc));
     int c=get_wifi_client(wc,-65);
     debug("aproam data send:%d",c/7);
     int i=0;
     while(i < c)
     {
        pwc=wc + i; 
        debug("%02X%02X%02X%02X%02X%02X,%d\n",pwc[0],pwc[1],pwc[2],pwc[3],pwc[4],pwc[5],(char)pwc[6]);
        i = i + 7;
     }
     uloop_timeout_set(timeout,10 * 1000);
}

void wifi_aproam_process(void *args)
{
      g_send_data_timer.cb=aproam_data_sender;
      uloop_init();
      uloop_timeout_set(&g_send_data_timer,10 * 1000);
      uloop_run();
      uloop_done(); 
      return ;
}
*/
int main(int argc,char* argv[])  
{  
#if 1 
        int ch;
        char network_dev_name[16] = {'\0'};
	const char *optstr = "i:";

	if( argc == 1 )
	{
		fprintf(stderr,"\nautorelayserver -i interface\n");
		return 0;
	}

	while( ( ch=getopt(argc,argv,optstr) ) != -1 )
	{
		switch( ch )
		{
			case 'i':
				strcpy(network_dev_name,optarg);
				break;
		}
	}

	if( !strlen( network_dev_name ) )
	{
		debug("error:please specify the network device!");
		return -1;
	}
        //deamon 
        config_load(); //if load config failed ,autorelayserver will exit
        if(!config->roam->enable)
        {
            config_exit();
            return 0;
        }
        deamon();


        int recved_data_len;
        socklen_t   sa_len=0;  
        global_socket_handle = listen_network_dev(network_dev_name,0x9886,self_mac_addr);
        if (0 > global_socket_handle)
        {
            debug("error:create socket failed");
            return -1;
        }

        //aproam process
        /*
        pthread_t tid;
        if(0 != pthread_create(&tid,NULL,(void*)wifi_aproam_process,NULL))
        {
            debug("pthread_create failed!");
        }
        pthread_detach(tid); 
        */

        //recvfrom  
        global_remote_mac_p=NULL; //remote mac point init
        global_conf_flag=1;
        debug("autorelayserver start");
        while(1){  
                memset(ethernet_frame_data, 0, sizeof(ethernet_frame_data));  
                if( !util_receive_ethernet_frame(global_socket_handle,ethernet_frame_data,&recved_data_len ) )
		{
			//process frame info
                        //debug("decode frame");
			util_decode_frame_info(ethernet_frame_data);
		}
                util_send_aproam_data();
                if(global_conf_flag)
                {
                    roam_config_request(NULL);
                    global_conf_flag = 0; 
                }
                util_send_wificlient();
        }  
        config_exit();
#endif
#if 0 
    config_load();
    printf("en:%d",config->roam->enable); 
    printf("routeStaTh:%d",(char)config->roam->route_sta_th); 
    config_exit();
#endif
        return 0;  
}  
