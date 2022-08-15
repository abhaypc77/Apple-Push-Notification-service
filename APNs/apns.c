#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "apns.h"

#define FILL_ITEM(apns_item, ID, len, DATA) {\
      apns_item->id = ID;\
      apns_item->data_len = len;\
      memcpy(apns_item->data, DATA, len);\
  }

#define COPY_ITEM_TO_FRAME(frame, frame_len, item) {\
    memcpy(frame + frame_len, &item.id, 1);\
    frame_len += 1;\
    memcpy(frame + frame_len, &item.data_len, 2);\
    frame_len += 2;\
    memcpy(frame + frame_len, item.data, item.data_len);\
    frame_len += item.data_len;\
  }

#define COPY_BYTES(msg, len, data, data_len){\
     memcpy(msg, data + len, data_len);\
     len += data_len;\
  }


#define DUMP_APNS_DATA(command, data_len, data, file_name) {\
    FILE *fp = fopen(file_name, "a+");\
    if (fp == NULL) {\
      printf("Error in openning file [%s]", file_name);\
    }\
    else {\
      fwrite(&command, 1, 1, fp);\
      fwrite(&data_len, 1, 4, fp);\
      fwrite(data, 1, data_len, fp);\
    }\
    fclose(fp);\
  }

/*--------------------------------------------------------
Function to create APNS item. This Function fills the 
APNS_item structure and returns the variable of type
structure APNS_item.
---------------------------------------------------------*/

int make_apns_item(APNS_item *apns_item, int item_id, char *item_data)
{
  int item_data_len = 0;

  switch(item_id)
  {
    case APNS_DEVICE_TOKEN:
      FILL_ITEM(apns_item, item_id, APNS_DEVICE_TOKEN_LEN, item_data);
      break;

    case APNS_PAYLOAD:
      item_data_len = strlen(item_data);
      FILL_ITEM(apns_item, item_id, item_data_len, item_data);
      break;

    case APNS_IDENTIFIER:
      FILL_ITEM(apns_item, item_id, APNS_IDENTIFIER_LEN, item_data);
      break;

    case APNS_EXPIRATION:
      FILL_ITEM(apns_item, item_id, APNS_EXPIRATION_LEN, item_data);
      break;

    case APNS_PRIORITY:
      FILL_ITEM(apns_item, item_id, APNS_PRIORITY_LEN, item_data);
      break;
   
    default: 
      printf("Invalid Identifier\n"); 
  }

return 0;
}

/*---------------------------------------------------------------------------------------------------------------
Function to make APNS request by creating APNS_frame. This function first creates all apns_items by invoking
make_apns_item() and then dumps apns_items to APNS_frame.
---------------------------------------------------------------------------------------------------------------*/

int make_apns_request(char *device_token, char *payload, u_int32_t identifier, u_int32_t expiry, u_int8_t priority)
{
  APNS_frame apns_frame;
  APNS_item apns_item;
  memset((void*)&apns_frame, 0, sizeof(APNS_frame));
  apns_frame.data = (char*) malloc(1024 * 4);

  apns_frame.command  = APNS_MODERN_NF;
  apns_frame.data_len = 0;

  if (device_token) {
    make_apns_item(&apns_item, APNS_DEVICE_TOKEN, device_token);
    COPY_ITEM_TO_FRAME(apns_frame.data, apns_frame.data_len, apns_item);
    printf("ITEM ID:[%d], DATA LEN:[%d], DATA:[%s]\n", apns_item.id, apns_item.data_len, apns_item.data);
  }
  
  if (payload) {
    make_apns_item(&apns_item, APNS_PAYLOAD, payload);
    COPY_ITEM_TO_FRAME(apns_frame.data, apns_frame.data_len, apns_item);
    printf("ITEM ID:[%d], DATA LEN:[%d], DATA:[%s]\n", apns_item.id, apns_item.data_len, apns_item.data);
  }
 
  if (identifier) {
    make_apns_item(&apns_item, APNS_IDENTIFIER, &identifier);
    COPY_ITEM_TO_FRAME(apns_frame.data, apns_frame.data_len, apns_item);
    printf("ITEM ID:[%d], DATA LEN:[%d], DATA:[%s]\n", apns_item.id, apns_item.data_len, apns_item.data);
  }

  if (expiry) {
    make_apns_item(&apns_item, APNS_EXPIRATION, &expiry);
    COPY_ITEM_TO_FRAME(apns_frame.data, apns_frame.data_len, apns_item);
    printf("ITEM ID:[%d], DATA LEN:[%d], DATA:[%s]\n", apns_item.id, apns_item.data_len, apns_item.data);
  }

  if (priority) {
    make_apns_item(&apns_item, APNS_PRIORITY, &priority);
    COPY_ITEM_TO_FRAME(apns_frame.data, apns_frame.data_len, apns_item);
    printf("ITEM ID:[%d], DATA LEN:[%d], DATA:[%s]\n", apns_item.id, apns_item.data_len, apns_item.data);
  }

  printf("command:[%d], LEN:[%d], DATA:[%s]\n\n\n", apns_frame.command, apns_frame.data_len, apns_frame.data);
  DUMP_APNS_DATA(apns_frame.command, apns_frame.data_len, apns_frame.data, "apns_request.dat");
  
  free(apns_frame.data);

return 0;
}


int proc_apns_req(char *read_buf, int bytes_read)
{

  APNS_item apns_item;
  APNS_frame apns_frame;
  APNS_error apns_error;
  int frame_data_len = 0;

  printf("Buffer:[%s]\n", read_buf);
  memcpy(&apns_frame.command, read_buf, 1);
  if (apns_frame.command != 0 && apns_frame.command != 1 && apns_frame.command != 2) {
    apns_error.command = APNS_ERR_RES;
    apns_error.status = APNS_PROCESSING_ERROR;
    apns_error.identifier = 0; //TODO: check for valid identifier
    exit(-1);
  }

  memcpy(&apns_frame.data_len, read_buf + 1, 4);  
  
  apns_frame.data = (char *) malloc(apns_frame.data_len);
  memcpy(apns_frame.data, read_buf + 5, apns_frame.data_len);  
 
  printf("command:[%d], LEN:[%d], DATA:[%s]\n", apns_frame.command, apns_frame.data_len, apns_frame.data);
  while(frame_data_len < apns_frame.data_len)
  {
    COPY_BYTES(&apns_item.id, frame_data_len, apns_frame.data, 1);
    COPY_BYTES(&apns_item.data_len, frame_data_len, apns_frame.data, 2);
    COPY_BYTES(apns_item.data, frame_data_len, apns_frame.data, apns_item.data_len);
    
    printf("ITEM ID:[%d], DATA LEN:[%d], DATA:[%s]\n", apns_item.id, apns_item.data_len, apns_item.data);
  }  
  
  DUMP_APNS_DATA(apns_frame.command, apns_frame.data_len, apns_frame.data, "apns_response.dat");


return 0;
}


int main(int argc, char *argv[])
{
  char device_token[32] =    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";  // must be 32 bit long
  char *payload = "\"aps\":{\"alert\":\"you have a mail\"}}";     // must be <= 2k
  //char *payload = "m";
  u_int32_t identifier = 1;
  u_int32_t expiry = 1;
  u_int8_t priority = 5;
  FILE *fp; 
  int file_size, bytes_read;
  char read_buf[1024 * 5]; 
  struct stat fstat; 
 
  make_apns_request(device_token, payload, identifier, expiry, priority); 
  
  fp = fopen("apns_request.dat", "r");
  if(fp == NULL) {
    printf("Error in openning file");
    exit (-1);
  }
  else {
    stat("apns_request.dat", &fstat);
    file_size = fstat.st_size;
    bytes_read = fread(read_buf, 1, file_size, fp);
  }
  
  printf("Request file size : [%d], Buffer:[%s]\n\n\n", file_size, read_buf);
  proc_apns_req(read_buf, bytes_read);


return 0;
}
