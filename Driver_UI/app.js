"use strict"

// dependencies
const express = require("express")
const http = require("http")
const { Server } = require("socket.io")
const path = require("path")
const fs = require("fs")
const { SerialPort } = require("serialport")
const xbee = require("xbee");

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


/* POST REQUEST BODY FORMAT 
{
  'data': <String of all data to be parsed here>
}
*/
app.post('/api/postdata',(req,res)=>{
  //String processing code since data is of format "time_stamp, node, msg_type, data"
  console.time("split");
  let received_string = String(req.body.data).replace(/\s+/g, ''); //ensures the received data is of a string datatype
  let comma_sep_data = received_string.split(',');
  console.timeEnd("split");
  if(comma_sep_data.length != 4){
    res.sendStatus(200);
    return;
  }
  let sender = comma_sep_data[1];
  let message_type = comma_sep_data[2];
  let received_data = comma_sep_data[3];

  //Parse data into proper field
  console.time("store");
  if (sender == 'BMS'){
    if (message_type == 'TEMPERATURE'){
      data.temperature = received_data;
    }else if (message_type == 'VOLTAGE'){
      data.voltage = received_data;
    }
  }else if (sender == 'RMS_INFO'){
    data.speed = received_data;
  }
  console.timeEnd("store");
  console.time("res");
  res.sendStatus(200);
  console.timeEnd("res");
});

const data = {
    temperature: 0,
    voltage: 0,
    speed: 0,
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
        socket.emit("data", data)
    }, 100)
});

// activate server
server.listen(PORT, () => {
	console.log(`Listening on port ${PORT}: http://localhost:${PORT}`);
});
