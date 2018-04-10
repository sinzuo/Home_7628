#include "common.h"
#include "log.h"

unsigned short ip_checksum(const void *data, int len)
{
	unsigned short checksum = 0;
	int left = len;
	const unsigned char *data_8 = data;

	while(left > 1)
	{
		checksum += ( ( *data_8 )<<8 )+( *(data_8+1) );
		data_8 += 2;
		left -= 2;
	}

	if(left)
	{
		checksum += ( ( *( unsigned char* )data_8 )<<8 );
	}

	while(checksum > 0xffff)
	{
		checksum = (checksum >> 16) + (checksum & 0xFFFF);
	}

	checksum = (~checksum) & 0xffff;

	return htons(checksum);
}
//send ethernet frame
int util_send_ethernet_frame(
		int socket_handle
		,unsigned short ethernet_frame_type
		,unsigned char *ethernet_data
		,int ethernet_data_len
		,unsigned char* src_mac
		,unsigned char* dst_mac
		)
{
	unsigned short b16;
	int n;
	int real_frame_size;
	unsigned char *p_ethernet_frame;
	unsigned char ethernet_frame[ETH_FRAME_LEN];

	real_frame_size = 0;
	p_ethernet_frame = ethernet_frame;

	//destination mac
	util_add_data_to_frame(dst_mac,ETH_ALEN);
	//source mac
	util_add_data_to_frame(src_mac,ETH_ALEN);
	//data type in the ethernet frame
	b16 = htons( ethernet_frame_type );
	util_add_data_to_frame(&b16,sizeof(b16));

	p_ethernet_frame += LENGTH_OF_ETHERNET_LEN;// skip len

	//copy ethernet data
	memcpy(p_ethernet_frame,ethernet_data,ethernet_data_len);
	if( ethernet_data_len&0x01 )
	{
		p_ethernet_frame[ ethernet_data_len ]=0;
		ethernet_data_len++;
	}

	// dst_mac[6]. src_mac[6]. eth[2]
	p_ethernet_frame = ethernet_frame+OFFSET_OF_ETHERNET_LEN;
	//2 bytes checksum
	b16 = htons( ethernet_data_len+LENGTH_OF_ETHERNET_CHECKSUM );
	memcpy(p_ethernet_frame,&b16,sizeof(b16));

	real_frame_size=OFFSET_OF_ETHERNET_DATA+ethernet_data_len+LENGTH_OF_ETHERNET_CHECKSUM;

	//calculate the checksum
	b16=ip_checksum(ethernet_frame,real_frame_size-LENGTH_OF_ETHERNET_CHECKSUM);
	memcpy(ethernet_frame+real_frame_size-LENGTH_OF_ETHERNET_CHECKSUM,&b16,sizeof(b16));

	n = send(socket_handle,ethernet_frame,real_frame_size,0);

	if(n == -1)
	{
		return -1;
	}
	else if(n < real_frame_size)
	{
		return -1;
	}

	return 0;
}

char *tomac(const char *pszStr,char *pmac)
{
    const int MAC_MIN_STR=12; 
    const int MAC_MAX_STR=17; 
    const int MAC_ADDR_LEN=6; 
    char * pszTmpStr = NULL;    
    if(NULL == pszStr || NULL == pmac) 
    {
        return NULL; 
    }
    if( MAC_MIN_STR != strlen(pszStr) && MAC_MAX_STR != strlen(pszStr))
    {
        printf("error:mac string size is not 12 or 17\n");
        return NULL; 
    }
    else if(MAC_MAX_STR == strlen(pszStr))
    {
        pszTmpStr = (char*)malloc(MAC_MIN_STR); 
        int i = 0,j = 0;
        while(i < MAC_MIN_STR && j < MAC_MAX_STR)
        {
           if(':' != pszStr[j])
           {
              pszTmpStr[i] =  pszStr[j];
              i++;
           }
           j++;
        }
        
    }
    else 
    {
        pszTmpStr = (char*)malloc(MAC_MIN_STR); 
        strncpy(pszTmpStr,pszStr,MAC_MIN_STR);
    }
    int iLoop = 0; 
    unsigned char *pszTmp = (unsigned char*)malloc(MAC_ADDR_LEN); 
    bzero(pszTmp,6);
    char *pLoop = pszTmpStr;
    char *pLoopTwo = pLoop + 1;
    while(iLoop < MAC_MIN_STR)
    {
        pLoop = pszTmpStr + iLoop;
        pLoopTwo = pLoop + 1;
        if(48 <= *pLoop && 57 >= *pLoop) //0~9 
        {
            pszTmp[iLoop/2] = (*pLoop - 48) << 4; 
        }
        else if(65 <= *pLoop && 70 >= *pLoop) //A~F
        {
            pszTmp[iLoop/2] = (*pLoop - 55) << 4; 
        }
        else if(97 <= *pLoop && 102 >= *pLoop) //a~f
        { 
            pszTmp[iLoop/2] = (*pLoop -87) << 4; 
        }
        else
        {
            printf("error:\'%c\' is not hex\n",*pLoop);
            goto error;
        }

        if(48 <= *pLoopTwo && 57 >= *pLoopTwo) //0~9
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 48); 
        }
        else if( 65 <= *pLoopTwo && 70 >= *pLoopTwo) //A~F
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 55);  
        }
        else if( 97 <= *pLoopTwo && 102 >= *pLoopTwo) //a~f 
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 87);   
        }
        else
        {
            printf("error:\'%c\' is not hex\n",*pLoopTwo);
            goto error;

        }
        
        iLoop = iLoop + 2; 
    }
    if( MAC_MIN_STR == iLoop )
    {
        memcpy(pmac,pszTmp,MAC_ADDR_LEN); 
        goto done;
    }
error:
    free(pszTmp);
    free(pszTmpStr);
    //printf("iLoop:%d",iLoop);
    return NULL;
done:
    //printf("iLoop:%d",iLoop);
    free(pszTmp);
    free(pszTmpStr);
    return pmac; 
}

