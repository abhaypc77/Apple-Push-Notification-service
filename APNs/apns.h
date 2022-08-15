#ifndef APNS_H
#define APNS_H

#define APNS_SIMPLE_NF   0  //Simple Notification Format
#define APNS_ENHANCED_NF 1  //Enhanced Notification Format  
#define APNS_MODERN_NF   2  //Modern Notification Fromat
#define APNS_ERR_RES     8  //Error Response Format

#define APNS_DEVICE_TOKEN 1
#define APNS_PAYLOAD      2
#define APNS_IDENTIFIER   3
#define APNS_EXPIRATION   4
#define APNS_PRIORITY     5

#define APNS_DEVICE_TOKEN_LEN 32
#define APNS_IDENTIFIER_LEN   4
#define APNS_EXPIRATION_LEN   4
#define APNS_PRIORITY_LEN     1

#define APNS_NO_ERROR             0
#define APNS_PROCESSING_ERROR     1
#define APNS_MISSING_DEVICE_TOKEN 2
#define APNS_MISSING_TOPIC        3 
#define APNS_MISSING_PAYLOAD      4
#define APNS_INVALID_TOKEN_SIZE   5
#define APNS_INVALID_TOPIC_SIZE   6
#define APNS_INVALID_PAYLOAD_SIZE 7
#define APNS_INVALID_TOKEN        8
#define APNS_SHUTDOWN             10
#define APNS_UNKNOWN              255

typedef struct {
  u_int8_t id;
  u_int16_t data_len;
  char data[2048];
}APNS_item;


typedef struct {
  u_int8_t command;
  u_int32_t data_len;
  char *data;
}APNS_frame;


typedef struct {
  u_int8_t command;
  u_int8_t status;
  u_int32_t identifier;
}APNS_error;



#endif




