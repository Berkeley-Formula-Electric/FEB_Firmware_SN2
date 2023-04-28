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
    temperature: 26,
    voltage: 632,
    speed: 54,
    timerStart: true,
    timerStop: false,
    timerReset: false,
}

io.on('connection', (socket) => {
    // socket.emit("data", data)
    setInterval(() => {
        data.temperature += (Math.random() - 0.5) * 0.1
        data.voltage += (Math.random() - 0.5) * 0.1
        data.speed += (Math.random() - 0.5) * 0.1
        socket.emit("data", data)
    }, 100)
});

server.listen(PORT, () => {
	console.log(`Listening on port ${PORT}: http://localhost:${PORT}`);
});