const net = require('net');
const server = net.createServer((socket) => {
	console.log('Client connected');
	
	socket.on('data', (data) => {
		socket.write(data);
	});
	
	socket.on('end', () => {
		console.log('Client disconnected');
	});
});

server.listen(8080, () => {
	console.log('Node.js Server listening on port 8080...');
});