"use strict"

// dependencies
const express = require("express")
const http = require("http")
const { Server } = require("socket.io")
const path = require("path")
const fs = require("fs")
const { SerialPort } = require("serialport")
const xbee = require("xbee");
const spi = require("spi-device")

// initalize web server
const app = express();
const server = http.createServer(app);
const io = new Server(server);
const PORT = 3000

app.use(express.static(path.join(__dirname, "public")));
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
    timerReset: true,
}

// timer
let time = 0
const dt = 10;
setInterval(() => {
    time += dt * 0.001
}, dt)
let counter = 1;

function roundPrecision(value, degree) {
    return Math.round(value * Math.pow(10, degree)) / Math.pow(10, degree)
}

let version = 0;
fs.readFile('data.txt', 'utf8', (err, data) => {
    if (err) throw err;
    version = parseInt(data);
    console.log(version)

    fs.open(`data/data${parseInt(data)}.csv`, 'w', (err) => {})
    fs.writeFile('data.txt', `${parseInt(data) + 1}`, (err) => {
        if (err) throw err;
        console.log('The file has been saved!');
      }); 

    fs.appendFile(`data/data${version}.csv`, `\n`, (err) => {});
    setInterval(() => {
        fs.appendFile(`data/data${version}.csv`, `${roundPrecision(time, 1)}, ${counter}\n`, (err) => {});    
        counter++
    }, 100);
});

// const mcp3008 = spi.open(0, 0, err => {
//     // An SPI message is an array of one or more read+write transfers
//     const message = [{
//       receiveBuffer: Buffer.alloc(1),              // Raw data read from channel 5
//       byteLength: 3,
//       speedHz: 20000 // Use a low bus speed to get a good reading from the TMP36
//     }];
  
//     if (err) throw err;
  
//     mcp3008.transfer(message, (err, message) => {
//       if (err) throw err;
  
//       // Convert raw value from sensor to celcius and log to console
//       const rawValue = message[0].receiveBuffer[0]
//       console.log(rawValue);
//     });
// });

// const serial_xbee = new SerialPort({
//     path: "/dev/cu.usbserial-D309NYWG",
//     baudRate: 57600,
// });

// serial_xbee.on("data", (data) => {
//     console.log(data)
//     // console.log(++counter)
//     // console.log('xbee data received:', data.type);    
// });

// socket connection
io.on('connection', (socket) => {
    setInterval(() => {
        data.temperature += (Math.random() - 0.5) * 0.1
        data.voltage += (Math.random() - 0.5) * 0.1
        data.speed += (Math.random() - 0.5) * 0.1
        socket.emit("data", data)
    }, 100)
});

// activate server
server.listen(PORT, () => {
	console.log(`Listening on port ${PORT}: http://localhost:${PORT}`);
});