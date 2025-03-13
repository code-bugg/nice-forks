package main

import (
	"fmt"
	"net"
	"io"
)

func handleClient(conn net.Conn) {
	defer conn.Close()
	buffer := make([]byte, 1024)
	
	for {
		n, err := conn.Read(buffer)
		if err != nil {
			if err == io.EOF {
				fmt.Println("Client disconnected.")
			}
			return
		}
		conn.Write(buffer[:n])
	}
}

func main(){
	listener, err := net.Listen("tcp", ":8080")
	if err != nil {
		fmt.Println("Error:", err)
		return
	}
	defer listener.Close()
	
	fmt.Println("Go Server listening on port 8080...")
	
	for{
		conn, err := listener.Accept()
		if err != nil{
			fmt.Println("Connection error: ", err)
			continue
		}
		go handleClient(conn)
	}
}