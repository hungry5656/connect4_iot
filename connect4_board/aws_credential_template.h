/*
 * This is a template file for all the necessary information
 * User is required to rename this file to aws_credential.h after setting up the necessary  
 */

#ifndef AWS_CREDENTIAL_H_
#define AWS_CREDENTIAL_H_

// connection info
#define APPLICATION_NAME        "SSL"
#define APPLICATION_VERSION     "1.1.1"
#define SERVER_NAME             "example.us-east-1.amazonaws.com" // server address of your AWS cloud shadow
#define GOOGLE_DST_PORT         8443

#define SL_SSL_CA_CERT "/cert/rootCA.der" // ca cert address
#define SL_SSL_PRIVATE "/cert/private.der" // private key address
#define SL_SSL_CLIENT  "/cert/client.der" // public key address
#define SHADOW_NAME     "Client1" // name of your shadow

#define DATE                20    /* Current Date */
#define MONTH               2     /* Month 1-12 */
#define YEAR                2024  /* Current year */
#define HOUR                12    /* Time - hours */
#define MINUTE              00    /* Time - minutes */
#define SECOND              0     /* Time - seconds */

// http packet info
#define POSTHEADER "POST /things/Client1/shadow HTTP/1.1\r\n" // change the POST header by the shadow address accordingly
#define GETHEADER "GET /things/Client1/shadow HTTP/1.1\r\n" // change the GET header by the shadow address accordingly
#define HOSTHEADER "Host: example.us-east-1.amazonaws.com\r\n"
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: application/json; charset=utf-8\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"

#endif /* AWS_CREDENTIAL_H_ */
