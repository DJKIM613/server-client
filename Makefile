all : echo_client echo_server

echo_client:
	g++  -o echo_client echo_client.cpp -lpthread

echo_server:
	g++  -o echo_server echo_server.cpp -lpthread

clean:
	rm -f echo_client
	rm -f echo_server
