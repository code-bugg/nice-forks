#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void handle_client(int client_socket) {
	char buffer[BUFFER_SIZE];
	ssize_t bytes_recieved;
	
	while((bytes_recieved = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0){
		send(client_socket, buffer, bytes_recieved, 0);
	}
	close(client_socket);
	std::cout << "Client disconnected.\n";
}

int main(){
	int server_socket, client_socket;
	sockaddr_in server_addr{}, client_addr{};
	socklen_t addr_size = sizeof(client_addr);
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	
	bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	listen(server_socket, 5);
	std::cout << "C++ Server listening on port " << PORT << " ...\n";
	while((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) >= 0){
		std::thread(handle_client, client_socket).detach();
	}

	close(server_socket);
	return 0;
}