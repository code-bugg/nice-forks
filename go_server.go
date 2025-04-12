package main

import (
	"fmt"
	"net/http"
)

// Define a handler function that will be called for requests to the root path.
func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hello, World!") // Send "Hello, World!" as the response.
}

func main() {
	// Register the handler function for the root path ("/").
	http.HandleFunc("/", handler)

	// Specify the port the server should listen on.
	port := ":8080"
	fmt.Printf("Server is listening on port %s...\n", port)

	// Start the HTTP server. ListenAndServe will block until the server is stopped.
	err := http.ListenAndServe(port, nil)
	if err != nil {
		fmt.Println("Error starting the server:", err)
	}
}
