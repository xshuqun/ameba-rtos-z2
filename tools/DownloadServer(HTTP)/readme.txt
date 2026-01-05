The script file is used to send image to device based on http or https by OTA function.

Command : 
	python http_server.py

(1) The image to be sent should be placed in the same directory as the script.
(2) You can edit http_server.py to change ip, port, certificate and key of server. You can choose to use HTTP or HTTPS by setting value of 'is_https'.
	 If 'is_https' is set to 0, it means that HTTP will be used and server_port should be set to 8082.
	 If 'is_https' is set to 1, it means that HTTPS will be used and server_port should be set to 443.
(3) If necessary, you can generate the server certificate and key as follows:
		openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem
(4) Please check the client side do support the ciphersuite MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256.