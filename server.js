/**
 * Module dependencies.
 */
 
var express = require('express')
  , routes = require('./routes')
  , http = require('http');
 
var port = process.env.port || 1337; 

var app = express();
var server = app.listen(port);
var io = require('socket.io').listen(server);

io.configure(function () { 
  io.set("transports", ["xhr-polling"]); 
  io.set("polling duration", 10); 
});
 
app.configure(function(){
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.engine('html', require('ejs').renderFile);
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.static(__dirname + '/public'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(app.router);
});
 
app.configure('development', function(){
  app.use(express.errorHandler());
});
 
app.get('/', routes.index);

app.get('/about', function (req, res)
{
    res.render('about.html');
    io.sockets.emit('news', { message: 'somebody is at about'});
});
app.get('/client', function (req, res)
{
    res.render('client.html');
});

// app.get('/scan', function (req, res)
// {
//     res.send('id: ' + req.query.id);
//    	io.sockets.emit('news', { message: 'id:' + req.query.id});
// });

app.post('/scan', function(req, res) {
    var card = req.body.card;
    io.sockets.emit('news', { message: 'card:' + card});
});

io.sockets.on('connection', function (socket) {
  socket.emit('news', { message: "Welcome"});
  socket.on('news', function (data) {
  	console.log("port %d", server.address().port);
    io.sockets.emit('news', data);
  });
  
});
 
 
console.log("Express server listening on port %d", server.address().port);