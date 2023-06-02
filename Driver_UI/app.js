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
  let received_string = String(req.body.data).replace(/\s+/g, ''); //ensures the received data is of a string datatype
  let comma_sep_data = received_string.split(',');
  if(comma_sep_data.length != 5){
    res.sendStatus(100);
    console.log('Incorrect length of data / # of arguments. Data may have been truncated.')
    return;
  }
  let sender = comma_sep_data[2];
  let message_type = comma_sep_data[3];
  let received_data = comma_sep_data[4];

  //Parse data into proper field
  if (sender == 'BMS'){
    if (message_type == 'TEMPERATURE'){
      actual_data.temperature = received_data;
    }else if (message_type == 'VOLTAGE'){
      actual_data.voltage = received_data;
    }
  }else if (sender == 'RMS'){
    actual_data.speed = received_data;
  }
  res.sendStatus(200);
});

//Used to receive from POST request. Change to 'data' once testing is done
//Fields initialized to 0 on startup
const actual_data = {
  temperature: 0,
  voltage: 0,
  speed:0
}

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

//~ let version = 0;
//~ fs.readFile('data.txt', 'utf8', (err, data) => {
    //~ if (err) throw err;
    //~ version = parseInt(data);
    //~ console.log(version)

    //~ fs.open(`data/data${parseInt(data)}.csv`, 'w', (err) => {})
    //~ fs.writeFile('data.txt', `${parseInt(data) + 1}`, (err) => {
        //~ if (err) throw err;
        //~ console.log('The file has been saved!');
      //~ }); 

    //~ fs.appendFile(`data/data${version}.csv`, `\n`, (err) => {});
    //~ setInterval(() => {
        //~ fs.appendFile(`data/data${version}.csv`, `${roundPrecision(time, 1)}, ${counter}\n`, (err) => {});    
        //~ counter++
    //~ }, 100);
//~ });

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
