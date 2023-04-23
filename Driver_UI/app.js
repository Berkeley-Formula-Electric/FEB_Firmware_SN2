"use strict"

// dependencies
const express = require("express")
const http = require("http");
const { Server } = require("socket.io")
const path = require("path");

// initalize web server
const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(express.static(path.join(__dirname, "public")));


const PORT = 3000

app.set("view engine", "ejs");

// routes
app.get("/", (req, res) => {
    res.render("screen.ejs")
})

// TEST DATA
const data = {
    temperature: 1000,
    voltage: 3.5,
    speed: 100
}

io.on('connection', (socket) => {
    socket.emit("data", data)
    setInterval(() => {
        data.temperature += 1
        data.voltage += 1
        data.speed += 1
        socket.emit("data", data)
    }, 10000)
});

server.listen(PORT, () => {
	console.log(`Listening on port ${PORT}: http://localhost:${PORT}`);
});