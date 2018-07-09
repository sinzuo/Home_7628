#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct
{
	unsigned char value[6];
//用于字节对齐的两个字节,保证真个结构体占用8个字节
	unsigned char align_bytes[2];
}MacAddr;

#define ETH_ALEN 6
#define LENGTH_OF_ETHERNET_DST_MAC	6
#define LENGTH_OF_ETHERNET_SRC_MAC	6
#define LENGTH_OF_ETHERNET_TYPE		2
#define LENGTH_OF_ETHERNET_LEN		2
#define LENGTH_OF_ETHERNET_CHECKSUM	2
#define LENGTH_OF_FIELD	1
#define LENGTH_OF_FIELD_LENGTH	1

#define ETH_FRAME_LEN 1514
#define OFFSET_OF_ETHERNET_DST_MAC 0
#define OFFSET_OF_ETHERNET_SRC_MAC ( OFFSET_OF_ETHERNET_DST_MAC+LENGTH_OF_ETHERNET_DST_MAC )
#define OFFSET_OF_ETHERNET_TYPE ( OFFSET_OF_ETHERNET_SRC_MAC+LENGTH_OF_ETHERNET_SRC_MAC )
#define OFFSET_OF_ETHERNET_LEN ( OFFSET_OF_ETHERNET_TYPE+LENGTH_OF_ETHERNET_TYPE )
#define OFFSET_OF_ETHERNET_DATA ( OFFSET_OF_ETHERNET_LEN+LENGTH_OF_ETHERNET_LEN )

#define util_add_data_to_frame(data_ptr,size)	do\
			{\
				if( real_frame_size + size > ETH_FRAME_LEN-20 )\
				{\
					return -1;\
				}\
				memcpy(p_ethernet_frame,data_ptr,size);\
				real_frame_size += size;	\
				p_ethernet_frame += size;	\
			}while(0)

//data_len&0x01 to make sure the data length is even
#define util_add_field_to_frame(cmd,data,data_len) 	do\
			{\
				unsigned char temp_value=cmd;\
				util_add_data_to_frame(&temp_value,LENGTH_OF_FIELD);\
				temp_value=data_len;\
				util_add_data_to_frame(&temp_value,LENGTH_OF_FIELD_LENGTH);\
				if( ( data ) != NULL )\
				{\
					util_add_data_to_frame(data,data_len);\
				}\
				if( ( data_len )&0x01 )\
				{\
					temp_value=0;\
					util_add_data_to_frame(&temp_value,1);\
				}\
			}while(0)


// below is marco and functions for recv
#define util_get_common_data_from_frame(data_ptr,len)	do\
			{\
				if(current_len + len > frame_len)\
				{\
					return -1;\
				}\
				if( ( data_ptr ) && ( len ) )\
				{\
					memcpy(data_ptr,p_ethernet_frame, len);\
				}\
				p_ethernet_frame += len;\
				current_len += len;\
			}while(0)

unsigned short ip_checksum(const void *data, int len);
int util_send_ethernet_frame(
		int socket_handle
		,unsigned short ethernet_frame_type
		,unsigned char *ethernet_data
		,int ethernet_data_len
		,unsigned char* src_mac
		,unsigned char* dst_mac
		);
char *tomac(const char *pszStr,char *pmac);
#endif

