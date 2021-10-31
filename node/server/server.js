var express = require("express");
var session = require("express-session");
var bodyParser = require("body-parser");
var path = require("path");
var fs = require("fs");
var { Board, Led, Switch } = require("johnny-five");

var app = express();
var server = require('http').createServer(app);
var io = require('socket.io').listen(server);

var port = 8000;
// authentication key generation
var psk = "";

var PSKLENGTH = 56;
var chars = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '$', '%']

// creates a unique psk for each login
function generatePSK() {
    var passcode;
    var index;
    for (var i = 0; i < PSKLENGTH; i++) {
        index = Math.floor(Math.random() * 40);
        passcode += chars[index];
    }
    return passcode;
}

// LOGIN INFORMATION CLASS
class Login_Information {
    constructor(username, password) {
        this.username = username
        this.password = password
    }
}

// instances of the class - sets up login information
var eric = new Login_Information("ericho", "ericho")

// Connecting to Arduino via Johnny-Five
/*
const board = new Board();

board.on("ready", function() {
    console.log("### Board ready!");
    const led = new five.Led(23);
    const mag_switch = new five.Switch(13);
    mag_switch.on("close", () => led.on());
    mag_switch.on("open", () => led.off());
});
*/


// Server declares
app.use(bodyParser.urlencoded({
    extended: false
}));
app.use(bodyParser.json());
app.use(session({
    secret: 'garagedoor',
    resave: true,
    saveUninitialized: true
}));
app.use(express.static('./../public'))

// connects the node server to the html
app.get('/', function(request, response) {
    response.sendFile(path.join(__dirname + './../public/login.html'));
});

app.get('/redirect', function(request, response) {
    response.sendFile(path.join(__dirname + './../public/redirect.html'));
});

// login to account on the server
var loggedIn = false;

app.post('/login', function(request, response) {
    var username = request.body.username;
    var password = request.body.password;

    var today = new Date();
    var json_today = JSON.stringify(today);

    // file = glob.sync("./../logs/log.json");

    if (username && password) {
        var outData;
        if (username == eric.username && password == eric.password) {
            request.session.loggedin = true;
            loggedIn = true;
            request.session.username = username;

            outData = json_today + ": User \"" + JSON.stringify(username) + "\" has successfully logged in.\n"
            fs.appendFileSync(__dirname + './../logs/log.json', outData);

            psk = generatePSK();
            fs.writeFileSync('connect.txt', psk)

            response.redirect('/auth');
        } else {
            outData = json_today + ": User \"" + JSON.stringify(username) + "\" attempted to log in.\n"
            fs.appendFileSync(__dirname + './../logs/log.json', outData);
            response.redirect("/redirect");
        }
    } else {
        response.send("Please enter a username and password");
    }
});

// Authentication with copying and pasting the psk on screen
app.get('/auth', function(request, response) {
    if (loggedIn) {
        response.sendFile(path.join(__dirname + "./../public/auth.html"));
    } else {
        response.redirect("/");
    }
});


// Authentication with copying and pasting the psk on screen
const auth = io.of('/auth');

auth.on('connection', function(client) {
    client.on('clicked', function(data) {
        auth.emit('buttonUpdate', psk);
    });
})


// Verification of the authentication key
var authLogged = false;

app.post('/auth', function(request, response) {
    var password = request.body.auth;

    if (password == psk) {
        authLogged = true;
        response.redirect('/control');
    } else {
        response.redirect('/redirect');
    }

});

// garage controller
app.get('/control', function(request, response) {
    if (loggedIn && authLogged) {
        response.sendFile(path.join(__dirname + "./../public/control.html"));
    } else {
        response.redirect("/");
    }
});

// connects via websockets
const ctrl = io.of('/control');

// closed is closed, open is open
var status = "closed";

ctrl.on('connection', function(client) {
    client.on('logoutClicked', function() {
        ctrl.emit('logoutUpdate');
        loggedIn = false;
        authLogged = false;
    });

    client.on('executeCommand', function(data) {
        ctrl.emit('commandUpdate', status);
        if (status == "closed") {
            status = "open";
        } else {
            status = "closed";
        }
    })
})

// starts the server
server.listen(port, function() {
    console.log("Started Server on PORT 8000");
    
});