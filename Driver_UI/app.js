"use strict"

// dependencies
const express = require("express")
const http = require("http")
const { Server } = require("socket.io")
const path = require("path")
const fs = require("fs")
const { SerialPort } = require("serialport")
const xbee = require("xbee");
//const spi = require("spi-device")

// initalize web server
const app = express();
const server = http.createServer(app);
const io = new Server(server);
const PORT = 3000

app.use(express.static(path.join(__dirname, "public"))); 
//Middleware required to read request body
app.use(express.urlencoded({extended:true}));
app.set("view engine", "ejs");

// routes
app.get("/", (req, res) => {
    res.render("screen.ejs")
});

app.post('/api/postdata',(req,res)=>{
  //TODO: Still need to figure out how to properly parse information from POST for other purposes
  console.log(req.body.param1);
  res.sendStatus(200);
});

// TEST DATA
const data = {
    temperature: 26,
    voltage: 632,
    speed: 54,
    timerStart: true,
    timerStop: false,
    timerReset: false,
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

/*
const mcp3008 = spi.open(0, 0, err => {
    if (err) throw err;
  
    // An SPI message is an array of one or more read+write transfers
    const message = [{
      sendBuffer: Buffer.from([0x01, 0x80, 0x00]), // The message to send (channel 5)
      receiveBuffer: Buffer.alloc(3),              // Raw data read from channel 5
      byteLength: 3,
      speedHz: 20000 // Use a low bus speed to get a good reading from the TMP36
    }];
  
    mcp3008.transfer(message, (err, message) => {
      if (err) throw err;
  
      // Convert raw value from sensor to celcius and log to console
      const rawValue = message[0].receiveBuffer[2] & 0x03;
      const convertedValue = (rawValue * 3.3) / 1023; // Assuming 3.3V reference voltage
      console.log(convertedValue);
      
      mcp3008.close((err) => {
        if (err) throw err;
        console.log('SPI connection closed.');
      });
    });
  });
 */ 

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