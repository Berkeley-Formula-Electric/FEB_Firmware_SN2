"use strict"

// Global variables
const socket = io();
const carData = {
    temperature: 0, voltage: 0, speed: 0, rtd: true
}

// Socket connection

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
})

// Data formatting
function roundPrecision(value, degree) {
    return Math.round(value * Math.pow(10, degree)) / Math.pow(10, degree)
}

function formatTemperature(value) {
    return `${roundPrecision(value, 2)}°C`
}

function formatVoltage(value) {
    return `${roundPrecision(value, 2)} V`
}

function formatSpeed(value) {
    return `${roundPrecision(value, 2)} kph`
}

// Canvas interface

function setup() {
	createCanvas(1200, 600)
}

function draw() {
    background(220)
    fill(255)

    // speed
    fill(255)
    rect(0, 0, width/2, height)

    fill(0)
    textSize(128)
    textAlign(CENTER, CENTER);
    text(formatSpeed(carData.speed), width / 4, height / 2);

    // battery voltage
    fill(255)
    rect(width / 2, 0, width / 2, height / 2)
    
    fill(0)
    textSize(128)
    textAlign(CENTER, CENTER);
    text(formatVoltage(carData.voltage), width / 4 * 3, height / 4);

    // battery temperature
    fill(255)
    rect(width / 2, height / 2, width / 4, height / 2)
    
    fill(0)
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
