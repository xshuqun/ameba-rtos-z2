#include "platform_opts.h"

#if defined(CONFIG_EXAMPLE_WEBSOCKET_SERVER) && CONFIG_EXAMPLE_WEBSOCKET_SERVER
#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <websocket/wsserver_api.h>
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "websocket/ws_server_msg.h"
#include "mbedtls/version.h"

#define USE_WSS    0

#if USE_WSS
#if defined(MBEDTLS_VERSION_NUMBER) && (MBEDTLS_VERSION_NUMBER>=0x03010000)
// mbedtls-3.x removed cert modules, so please embed your own test certificates in your test code
static const unsigned char *mbedtls_test_srv_crt =
	"-----BEGIN CERTIFICATE-----\r\n"                                      \
	"MIIDwDCCAqigAwIBAgIBBDANBgkqhkiG9w0BAQsFADCBgDELMAkGA1UEBhMCQ04x\r\n" \
	"CzAJBgNVBAgMAkpTMQswCQYDVQQHDAJTWjEQMA4GA1UECgwHUmVhbHNpbDEQMA4G\r\n" \
	"A1UECwwHUmVhbHRlazEXMBUGA1UEAwwOMTkyLjE2OC42MS4xMTIxGjAYBgkqhkiG\r\n" \
	"9w0BCQEWC1JlYWxzaWwuY29tMB4XDTIzMDgyMzA4NTQ1NFoXDTI0MDgyMjA4NTQ1\r\n" \
	"NFowdzELMAkGA1UEBhMCQ04xCzAJBgNVBAgMAkpTMRAwDgYDVQQKDAdSZWFsc2ls\r\n" \
	"MRAwDgYDVQQLDAdSZWFsdGVrMRcwFQYDVQQDDA4xOTIuMTY4LjYxLjExMjEeMBwG\r\n" \
	"CSqGSIb3DQEJARYPUmVhbHNpbEBydGsuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOC\r\n" \
	"AQ8AMIIBCgKCAQEAwI06fb+rZSjDpNdNYpHfUfF1UHmH4HBYeVJBcD0GKyFZMqeQ\r\n" \
	"UI4Q+RNfZHSA8ROffFjE6ZeCy1fy6V2SrfN6DBuoJQaOtt5oJqUu5XWS0IY2yG8d\r\n" \
	"wVNYLK51UHBkhobRq0dnLJAILNSkhqi35DKIWo6XRjrbQgWW3jAnlY6aBBduxjvo\r\n" \
	"8eKroR5Ca/qutuLisNlJgtvJBssRTWfF/Ivxs2MfgVR+ciTfVtvbPHZwGvgr5Kly\r\n" \
	"gc3agWTmVov0V1e+qzOVNTaW8yitQUBON2ro1cKlXxy1AmnDjTN95gk/SWANAddg\r\n" \
	"A4aDxzDiLNQSRhkPUc4x6RvsS7nSlDIWPiUBQQIDAQABo00wSzAJBgNVHRMEAjAA\r\n" \
	"MB0GA1UdDgQWBBSPymxc0w7ahLK6f5dFp+vHNIRvuTAfBgNVHSMEGDAWgBTGcAYf\r\n" \
	"hy0OEBlX9jqKJbk1OA57cDANBgkqhkiG9w0BAQsFAAOCAQEAmK7eiVnYre9+1YXR\r\n" \
	"BuVFVPwSUoM8XCOXS3QvJ7xDYwg5IWA+wGweJMIz+X8iC1g7cZqQB+FrwdUomL3k\r\n" \
	"e7saBeOHuqJgA8vU0liUpPQRNHDiwGLWiq2YO+efr5WRLcBOq/5G45IeCg2ytGZS\r\n" \
	"Ah+xVMkS5+Y7pRCb2sg1mOj6S4E5xDRZN+x+W0Xa9eQ2uB6ICKvFicb5LMMkbFIH\r\n" \
	"xK7A4dh4KcU1qykvCuiwhE9iE5RI0r8enRu3mgsyz8OW03YljoUICOStW9EEwAOD\r\n" \
	"K4TpYUGb15em3CYP+Gu/9FpV4IJ0Y3KPU1M2kBoBD77eJp9tqXnS5tlHaOwNO92b\r\n" \
	"93/tHw==\r\n"                                                         \
	"-----END CERTIFICATE-----\r\n";

static const unsigned char *mbedtls_test_srv_key =
	"-----BEGIN PRIVATE KEY-----\r\n"                                  \
	"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDAjTp9v6tlKMOk\r\n" \
	"101ikd9R8XVQeYfgcFh5UkFwPQYrIVkyp5BQjhD5E19kdIDxE598WMTpl4LLV/Lp\r\n" \
	"XZKt83oMG6glBo623mgmpS7ldZLQhjbIbx3BU1gsrnVQcGSGhtGrR2cskAgs1KSG\r\n" \
	"qLfkMohajpdGOttCBZbeMCeVjpoEF27GO+jx4quhHkJr+q624uKw2UmC28kGyxFN\r\n" \
	"Z8X8i/GzYx+BVH5yJN9W29s8dnAa+CvkqXKBzdqBZOZWi/RXV76rM5U1NpbzKK1B\r\n" \
	"QE43aujVwqVfHLUCacONM33mCT9JYA0B12ADhoPHMOIs1BJGGQ9RzjHpG+xLudKU\r\n" \
	"MhY+JQFBAgMBAAECggEABOPhUyhpwYSG7s9iq/5GlMBu3rfDbhK0Wy/DW9wfkBCw\r\n" \
	"AP5qnB0g4bkupZtkMJszZEyl2EU3Nr5Zpo3FkqI5xzJxngUcB+6/PZIEOPgi3dBh\r\n" \
	"dkwveNrgGlG7nCrKkXRoQUhriUvG0Aqo0LGwFT0no4h2X5Em7eGR9WZ1hDxuUsG7\r\n" \
	"+R8dmLyxRl3W9SMqcQWH3mitFjWhYhc5WgEIEXJBcYS7I7xVnGxxARe4cT9FwzPc\r\n" \
	"roQMQYQJbGh9Nn4R1awrKARURLTz4d0yKuF6itKqzrW2s6pUguQvtt2zWGRIsNq1\r\n" \
	"fTp5hETL29E4VYLldrB4eUfHVzKkAllpApSCIwqfkQKBgQDnrGxK27DaRrU/0uNO\r\n" \
	"Z5Vw8WfaC6ZSuXa60MsiNIRP70tq9UvNByfAJRs5TlwODqFE0plqQ4VnWpcKHPLv\r\n" \
	"83z5aSDIXnM8K2uTAtcyDAwTh+ph/e8qALtjYSv6WBaBmTYf8/LG9jp5nAghte53\r\n" \
	"pkywElMNa32zkZC5xnZ2+XKHeQKBgQDUxS1x8OCeSvKePfeJ6VujGo3tz4Mld8BE\r\n" \
	"tmkGtW7xiYQt7pcXWkrlQUCiMQCA3WVqsonU6bvGxG6VXGeOXplWwdnzMhCxfelr\r\n" \
	"fmKIpaqj7NPNtbC2t8wllr8aIXsrIGuZrK3k9P+hBgp+FiRfytyctPA8UlmHB5bm\r\n" \
	"EwHxZNquCQKBgQDip2DnN9nlIuouP+8chODCt5sHyv/0zPfou18Q3tpXreBWCxgW\r\n" \
	"xZpRNLxWq2AKbKvAPl3Tha2dORat0TaoJMou5HgrL6SDyGMdV9kbYIfsD7vyFNKI\r\n" \
	"gGDUUHwLF2Ulc7RtPGhX2QvKcCnQeUKNkohSdgsFBRXcuVwjNJQvv8oZ2QKBgFEN\r\n" \
	"xGAxiQAEQSSzKoepFDWwOf0sHiO52M7URnI9z8PHxUjSKVDGHt3y27JtxGlS7f2V\r\n" \
	"ghhP6i6wSggwKWr88qEwO9z/Z4iWFMPUY6j2o2jLIWyuGGGi5uwy3x4PPyKQuSzT\r\n" \
	"sXMVOgSXjou116l+WEiCTTpNjLl51KxU9VqGNyAxAoGAPrgNbucNQxBh3rCcU9DI\r\n" \
	"cj/6tytVnrQ3M6x0Qssj5pYPqXVAfMrWkMP++0+u23cUNHX+8eAoZc4c+a+0F83V\r\n" \
	"d28mzCHEr0RZTU4K7YMrq0LK64TRjFQnJkX7eUD+2hvfNqGDjmyG7LqDvVNRME34\r\n" \
	"Nv3QqMX12BAlTyBJ6DjO4oY=\r\n"                                         \
	"-----END PRIVATE KEY-----\r\n";

static const unsigned char *mbedtls_test_ca_crt =
	"-----BEGIN CERTIFICATE-----\r\n"                                      \
	"MIID4zCCAsugAwIBAgIUHY1ubFUc6bSfUuid5WvqqrgYYEowDQYJKoZIhvcNAQEL\r\n" \
	"BQAwgYAxCzAJBgNVBAYTAkNOMQswCQYDVQQIDAJKUzELMAkGA1UEBwwCU1oxEDAO\r\n" \
	"BgNVBAoMB1JlYWxzaWwxEDAOBgNVBAsMB1JlYWx0ZWsxFzAVBgNVBAMMDjE5Mi4x\r\n" \
	"NjguNjEuMTEyMRowGAYJKoZIhvcNAQkBFgtSZWFsc2lsLmNvbTAeFw0yMzA4MjMw\r\n" \
	"ODMzMjJaFw0zMzA4MjAwODMzMjJaMIGAMQswCQYDVQQGEwJDTjELMAkGA1UECAwC\r\n" \
	"SlMxCzAJBgNVBAcMAlNaMRAwDgYDVQQKDAdSZWFsc2lsMRAwDgYDVQQLDAdSZWFs\r\n" \
	"dGVrMRcwFQYDVQQDDA4xOTIuMTY4LjYxLjExMjEaMBgGCSqGSIb3DQEJARYLUmVh\r\n" \
	"bHNpbC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDO6ae/cN9Q\r\n" \
	"J4SnCucLxRokS5Jel3T5tBu+2mTmzP7g73Ka/O0eWRNhsAEoZIErG67S3w33F7Yy\r\n" \
	"ZPepyUz/lToLrduAoVxsAlvsz59trDQdp/C/CjZ7q6GlupYj0jMxI9DH9uNthmpH\r\n" \
	"bbmbrYaX4CtM4MQyxPVRmK2YWnXOBuNi1p11B3yFnNtQ8pUnGCfnPqxBxOf0oeuE\r\n" \
	"GUEzBgPksUDpM2ViCj3BtyPoCmuj3gxmD9tBUe76ypUI+XlOuz33wC7jnANPaLVd\r\n" \
	"mtLobxzN3xl52CJKky9YPqtUC7RgrDU9YeUjtjmvXvzsmkfFosRovFs1FGpuNTda\r\n" \
	"R0oKtJSgj+wnAgMBAAGjUzBRMB0GA1UdDgQWBBTGcAYfhy0OEBlX9jqKJbk1OA57\r\n" \
	"cDAfBgNVHSMEGDAWgBTGcAYfhy0OEBlX9jqKJbk1OA57cDAPBgNVHRMBAf8EBTAD\r\n" \
	"AQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCwQY3gX4IZyocrqF2EYeErapriuGwZFXcW\r\n" \
	"/EnVfkd4pgSSj7VHmv8UAh4Vivy9FszLtSOMUlbaiJ6ZFPHBhGGpookcRfFFwrXT\r\n" \
	"UErw7Ff0TLfeQhn45heL4mWJDJCK/RfvxAbAtbEK7Sp+a2D5wp5rbbygmgT8exOy\r\n" \
	"jePK0m0bIRfrIDIbfPclO/4yo0I5CJbQ1skkCqdG3YTBQ0+5NRzSn85ecW12ztex\r\n" \
	"Cf8pooXr6f8aD4qQAOAZ0vT8kA705gw4bwBL/xM92YecAcVxrZtY+GLH1Yfyigzj\r\n" \
	"0cuvZGpHTzlUbpndV2xSFOR6SNMUi9knFeQlQgpVBzOhNmhuw51a\r\n"             \
	"-----END CERTIFICATE-----\r\n";
#else
// use test_srv_crt, test_srv_key, test_ca_list in certs.c, "#define MBEDTLS_CERTS_C" in config_rsa.h
#include <mbedtls/certs.h>
#endif
#endif

void echo_data(ws_conn *conn, int data_len, enum opcode_type opcode)
{
	printf("\r\n>>>>>> Receiving: %s with length: %d from %d\n", conn->receivedData, data_len, conn->sock);
	if (opcode == TEXT_FRAME) {
		ws_server_sendText((char *)conn->receivedData, data_len, 1, conn);
	} else if (opcode == BINARY_FRAME) {
		ws_server_sendBinary(conn->receivedData, data_len, 1, conn);
	}
}

static void example_wsserver_thread(void *param)
{
	printf("\r\n\r\n\r\n>>>>>>>>>>>>>>>websocket server example<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
	vTaskDelay(10000);
	while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {
		printf("\r\n\r\n\r\n>>>>>>>>>>>>>>Wifi is disconnected!!Please connect!!<<<<<<<<<<<<<<<<<\r\n\r\n\r\n");
		vTaskDelay(10000);
	}
#if USE_WSS
	if (ws_server_setup_cert(mbedtls_test_srv_crt, mbedtls_test_srv_key, mbedtls_test_ca_crt) != 0) {
		printf("\nERROR: ws_server_setup_cert\n");
		goto exit;
	}
#endif
	ws_server_dispatch(echo_data);

#if USE_WSS
	if (ws_server_start(443, 5, 4096, WS_SERVER_SECURE_TLS) != 0) {
#else
	if (ws_server_start(80, 5, 4096, WS_SERVER_SECURE_NONE) != 0) {
#endif
		printf("ERROR: ws_server_start");
	}
#if USE_WSS
exit:
#endif
	vTaskDelete(NULL);
}

void example_wsserver(void)
{
	if (xTaskCreate(example_wsserver_thread, ((const char *)"example_wsserver_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate(example_wsserver_thread) failed", __FUNCTION__);
	}
}

#endif //#if defined(CONFIG_EXAMPLE_WEBSOCKET_SERVER) && CONFIG_EXAMPLE_WEBSOCKET_SERVER
