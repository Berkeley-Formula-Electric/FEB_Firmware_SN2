"use strict"

const socket = io();

socket.on("connect", (socket) => {
    console.log("connected!")
})

socket.on("data", (data) => {
    if ("temperature" in data) {
        document.getElementById("temperature-txt").innerHTML = data.temperature
    }
    if ("voltage" in data) {
        document.getElementById("voltage-txt").innerHTML = data.voltage
    }
    if ("speed" in data) {
        document.getElementById("speed-txt").innerHTML = data.speed
    }
})