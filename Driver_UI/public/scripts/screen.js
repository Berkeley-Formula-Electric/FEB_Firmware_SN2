"use strict"

// Speed - Temperature - Voltage - Ready To Drive - Battery percentage - Timer/Lap
const carData = {
    temperature: 0, voltage: 0, speed: 0, rtd: true, time : 0
}
const carBattery = {
    lowerBound: 200, upperBound: 700
}
const carTimer = {
    start: false, stop: false, reset: false, isRunning: false, startTime: 0, timePaused: 0
}

// Socket connection
const socket = io();
socket.on("connect", (socket) => {
    console.log("connected!")
})

socket.on("data", (data) => {
    if ("temperature" in data) {
        carData.temperature = data.temperature
    }
    if ("voltage" in data) {
        carData.voltage = data.voltage
    }
    if ("speed" in data) {
        carData.speed = data.speed
    }
    if ("timerStart" in data) {
        carTimer.start = data.timerStart
    }
    if ("timerStop" in data) {
        carTimer.stop = data.timerStop
    }
    if ("timerReset" in data) {
        carTimer.reset = data.timerReset
    }
})

// Data formatting
function roundPrecision(value, degree) {
    return Math.round(value * Math.pow(10, degree)) / Math.pow(10, degree)
}

function formatTemperature(value) {
    return `${roundPrecision(value, 2).toFixed(2)}°C`
}

function formatVoltage(value) {
    return `${roundPrecision(value, 2)} V`
}

function formatSpeed(value) {
    return `${roundPrecision(value, 2).toFixed(2)} kph`
}

function formatBattery(value) {
    return `${roundPrecision(value, 2).toFixed(2)} %`
}

function formatTime(value) {
    
    const totalSeconds = Math.floor(value / 1000);
    const minutes = Math.floor(totalSeconds / 60);
    const seconds = totalSeconds % 60;
    const ms = Math.floor(value % 1000 / 10);
    
    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}:${ms.toString().padStart(2, '0')}`;  
}


// Math functions

function calculateBattery(voltage) {
    let calculated = ((voltage - carBattery.lowerBound) / (carBattery.upperBound - carBattery.lowerBound)) * 100
    return calculated
}


// Canvas interface

function setup() {
	createCanvas(1100, 620)
}

function draw() {
    background(220)
    fill(255)

    // speed
    fill(255)
    rect(0, 0, width/2, height/2)

    fill(0)
    textSize(120)
    textAlign(CENTER, CENTER);
    text(formatSpeed(carData.speed), width / 4, height / 4);

    // timer
    fill(255)
    rect(0, height/2, width/2, height/2)

    if (carTimer.reset) {
        carTimer.timePaused = 0;
        carTimer.startTime = 0;
        carData.time = 0;
        carTimer.reset = false;
    }
    if (carTimer.start && !carTimer.isRunning) {
        carTimer.startTime = Date.now() - carTimer.timePaused;
        carTimer.isRunning = true;
        carTimer.start = false;
    }
    if (carTimer.stop && carTimer.isRunning) {
        carTimer.timePaused = Date.now() - carTimer.startTime;
        carTimer.isRunning = false;
        carTimer.stop = false;
    }
    if (carTimer.isRunning) {
        carData.time = Date.now() - carTimer.startTime;
    }

    fill(0)
    textSize(120)
    textAlign(CENTER, CENTER);
    
    text(formatTime(carData.time), width / 4, height * 3/4);

    // battery percentage
    fill(255)
    rect(width / 2, 0, width / 2, height / 2)
    
    let percent = calculateBattery(carData.voltage)
    let batteryColor = "green"
    if (percent < 30) {
        batteryColor = "red"
    } else if (percent < 60) {
        batteryColor = "orange"
    } else if (percent < 80) {
        batteryColor = "yellow"
    }
    
    fill(batteryColor)
    textSize(120)
    textAlign(CENTER, CENTER);
    text(formatBattery(percent), width / 4 * 3, height / 4);

    // battery temperature
    fill(255)
    rect(width / 2, height / 2, width / 4, height / 2)

    let tempRed = Math.round(Math.min(255, (255 / 50) * (50 - carData.temperature)));
    let tempBlue = Math.round(Math.min(255, (255 / 50) * (50 - carData.temperature)));
    let tempColor = `rgb(${tempRed}, 0, ${tempBlue})`;

    fill(tempColor);
    textSize(64)
    textAlign(CENTER, CENTER);
    text(formatTemperature(carData.temperature), width / 8 * 5, height / 4 * 3);

    // ready to drive
    if (carData.rtd) {
        fill(64, 173, 62)
    } else {
        fill(235, 64, 52)
    }
    rect(width / 4 * 3, height / 2, width / 4, height / 2)
    
    fill(0)
    textSize(100)
    textAlign(CENTER, CENTER);
    text("RTD", width / 8 * 7, height / 4 * 3);
}
