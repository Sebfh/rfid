/**
 * Module dependencies.
 */
 
var express = require('express')
  , routes = require('./routes')
  , http = require('http');
 

var app = express();
var server = app.listen();
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
});
app.get('/client', function (req, res)
{
    res.render('client.html');
});

io.sockets.on('connection', function (socket) {
  socket.emit('news', { hello: 'world' });
  socket.on('my other event', function (data) {
    console.log(data);
  });
});
 
 
console.log("Express server listening on port %d", server.address().port);