#include <iostream>
#include <cstring>       // Pour memset()
#include <sys/types.h>   // Pour les types de sockets
#include <sys/socket.h>  // Pour les sockets
#include <netinet/in.h>  // Pour les structures d'adresse (sockaddr_in)
#include <unistd.h>      // Pour close()
#include <cstdlib>

int main(int argc, char **argv) {
	if (argc != 2)
		return(1);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Création du socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Socket failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Attache le socket au port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt error" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    // Associe le socket à l'adresse et au port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Met le socket en écoute
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "En attente de connexion..." << std::endl;

    // Accepte une connexion
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        std::cerr << "Accept error" << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connecté: chat activé" << std::endl;

	std::string	input;
	while (true) {
		std::getline(std::cin, input);
		if (input == "exit")
			break;
		input = "server " + input + "\n";
		send(new_socket, input.c_str(), input.length(), 0);
		std::cout << "Message envoyé : " << input;
	}

    close(new_socket);
    close(server_fd);
    return 0;
}
