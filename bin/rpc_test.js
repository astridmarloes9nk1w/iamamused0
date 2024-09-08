console.log('rpc testing....');

var assert = require('assert');

var rpc = require('rpc');
var http = require('http');
var mq = require('mq');
var encoding = require('encoding');

var jr = rpc.json(function(m, p1, p2) {
	m.value = '';
	return p1 + ',' + p2;
});

var m = new http.Request();

m.value = 'test/tttt/tttt/';
m.setHeader("Content-Type", "application/json, charset=utf-8;");
m.body.write(new Buffer(encoding.jsonEncode({
	method : 'aaaa',
	params : [ 100, 200 ],
	id : 1234
})));

jr.invoke(m);

m.response.body.rewind();
assert.equal(m.response.body.read().toString(),
		'{"id":1234,"result":"100,200"}');

m.value = 'test';
jr = rpc.json({
	test : {
		aaaa : function(m, p1, p2) {
			m.value = '';
			return p1 + ',' + p2;
		}
	}
});

jr.invoke(m);

m.response.body.rewind();
assert.equal(m.response.body.read().toString(),
		'{"id":1234,"result":"100,200"}');

jr = rpc.json(mq.routing({
	'^(/.*)$' : http.fileHandler('httpd/www/'),
	'^/xhr(/.*)$' : {
		test : {
			fun : function(v, a, b) {
				return a + b;
			}
		}
	}
}));

m = new http.Request();

m.value = '/xhr/test';
m.setHeader("Content-Type", "application/json");
m.body.write(new Buffer(encoding.jsonEncode({
	method : 'fun',
	params : [ 100, 200 ],
	id : 1234
})));

jr.invoke(m);

m.response.body.rewind();
assert.equal(m.response.body.read().toString(), '{"id":1234,"result":300}');

m = new http.Request();

m.value = '/xhr/test';
m.setHeader("Content-Type", "application/x-www-form-urlencoded");
m.body.write(new Buffer("jsonrpc="
		+ encoding.encodeURIComponent(encoding.jsonEncode({
			method : 'fun',
			params : [ 100, 200 ],
			id : 1234
		}))));

jr.invoke(m);

m.response.body.rewind();
assert
		.equal(
				m.response.body.read().toString(),
				'<html><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><script>window.name=\"{\\\"id\\\":1234,\\\"result\\\":300}\";</script></html>');
