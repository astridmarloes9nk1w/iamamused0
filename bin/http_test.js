/**
 * 
 */

console.log('http testing....');

var assert = require('assert');
var io = require('io');
var http = require('http');
var encoding = require('encoding');

var d = new http.Request().headers;

d.add('a', '100');
d.add('b', '200');
d.add('a', '300');
d.add('c', '400');
d.add('d', '500');
d.add('c', '600');
d.add('d', '700');

assert.equal(d['a'], '100');

assert.equal(d.has('a'), true);
assert.equal(d.first('a'), '100');

a = d.all('a');
assert.equal(a.length, 2);
assert.equal(a[0], '100');
assert.equal(a[1], '300');

d.remove('a');
assert.equal(d.has('a'), false);
assert.strictEqual(d.first('a'), null);

assert.strictEqual(d['a'], undefined);

assert.equal(d.first('c'), '400');

a = d.all('c');
assert.equal(a.length, 2);
assert.equal(a[0], '400');
assert.equal(a[1], '600');

d.set('c', '800');

a = d.all('c');
assert.equal(a.length, 1);
assert.equal(a[0], '800');

assert.equal(d.first('c'), '800');

d.add({
	d : "900",
	b : "1000"
});

a = d.all('d');
assert.equal(a.length, 3);
assert.equal(a[0], '500');
assert.equal(a[1], '700');
assert.equal(a[2], '900');

a = d.all('b');
assert.equal(a.length, 2);
assert.equal(a[0], '200');
assert.equal(a[1], '1000');

d.set({
	d : "900",
	b : "1000"
});

a = d.all('d');
assert.equal(a.length, 1);
assert.equal(a[0], '900');

a = d.all('b');
assert.equal(a.length, 1);
assert.equal(a[0], '1000');

d.set('e', 2000);
a = d.all('e');
assert.equal(a.length, 1);
assert.equal(a[0], '2000');

d.set('f', 214748364700);
assert.equal(d['f'], '214748364700');

d.set('f', 214.123);
assert.equal(d['f'], '214.123');

t = new Date('2012-12-12 12:12:12');
d.set('f', t);
assert.equal(d['f'], 'Wed, 12 Dec 2012 04:12:12 GMT');

var bad_reqs = [ " GET / HTTP/1.0\r\nkeepalive: close\r\n\r\n",
		"GET ? HTTP/1.0\r\nkeepalive: close\r\n\r\n",
		"GET / \r\nkeepalive: close\r\n\r\n",
		"GET / HTTP/a.0\r\nkeepalive: close\r\n\r\n",
		"GET / HTT/1.0\r\nkeepalive: close\r\n\r\n",
		"GET / HTTP/1.\r\nkeepalive: close\r\n\r\n" ];

function readreq(u) {
	var ms = new io.MemoryStream();
	var bs = new io.BufferedStream(ms);
	bs.EOL = "\r\n";

	bs.writeText(u);
	ms.seek(0, io.SEEK_SET);

	var req = new http.Request();

	assert.throws(function() {
		req.readFrom(bs);
	});
}

bad_reqs.forEach(readreq);

var ms = new io.MemoryStream();
var bs = new io.BufferedStream(ms);
bs.EOL = "\r\n";

bs
		.writeText("GET / HTTP/1.0\r\nhead1: 100\r\nhead2: 200\r\nContent-type:test\r\nContent-length:10\r\n\r\n0123456789");
ms.seek(0, io.SEEK_SET);

var req = new http.Request();
req.readFrom(bs);
assert.equal('100', req.headers['head1']);
assert.equal('200', req.headers['head2']);
assert.equal(10, req.contentLength);
assert.equal('test', req.contentType);
assert.equal('0123456789', req.body.read());

var keep_reqs = {
	"GET / HTTP/1.0\r\n\r\n" : false,
	"GET / HTTP/1.0\r\nconnection: close\r\n\r\n" : false,
	"GET / HTTP/1.0\r\nconnection: keep-alive\r\n\r\n" : true,
	"GET / HTTP/1.1\r\n\r\n" : true,
	"GET / HTTP/1.1\r\nconnection: close\r\n\r\n" : false,
	"GET / HTTP/1.1\r\nconnection: keep-alive\r\n\r\n" : true,
};

for ( var n in keep_reqs) {
	var ms = new io.MemoryStream();
	var bs = new io.BufferedStream(ms);
	bs.EOL = "\r\n";

	bs.writeText(n);
	ms.seek(0, io.SEEK_SET);

	var req = new http.Request();
	req.readFrom(bs);
	assert.equal(keep_reqs[n], req.keepAlive);
}

function get_cookie(txt) {
	var ms = new io.MemoryStream();
	var bs = new io.BufferedStream(ms);
	bs.EOL = "\r\n";

	bs.writeText(txt);
	ms.seek(0, io.SEEK_SET);

	var req = new http.Request();
	req.readFrom(bs);
	return req.cookie;
}

var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new;\r\n\r\n");
assert.equal(c['$Version'], '1');
assert.equal(c['Skin'], 'new');

var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20\r\n\r\n");
assert.equal(c['Skin'], 'new ');

var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20cookie %sdf\r\n\r\n");
assert.equal(c['Skin'], 'new cookie %sdf');

function get_query(txt) {
	var ms = new io.MemoryStream();
	var bs = new io.BufferedStream(ms);
	bs.EOL = "\r\n";

	bs.writeText(txt);
	ms.seek(0, io.SEEK_SET);

	var req = new http.Request();
	req.readFrom(bs);
	return req.query;
}

var c = get_query("GET /test?a=100&b=200 HTTP/1.0\r\n\r\n");
assert.equal(c['a'], '100');
assert.equal(c['b'], '200');

var c = get_query("GET /test?a&b HTTP/1.0\r\n\r\n");
assert.equal(c['a'], '');
assert.equal(c['b'], '');

var c = get_query("GET /test?a&&&&&&&&&&&&&&&&&&&&&&&&&b HTTP/1.0\r\n\r\n");
assert.equal(c['a'], '');
assert.equal(c['b'], '');

function get_form(txt) {
	var ms = new io.MemoryStream();
	var bs = new io.BufferedStream(ms);
	bs.EOL = "\r\n";

	bs.writeText(txt);
	ms.seek(0, io.SEEK_SET);

	var req = new http.Request();
	req.readFrom(bs);
	return req.form;
}

var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:11\r\n\r\na=100&b=200");
assert.equal(c['a'], '100');
assert.equal(c['b'], '200');

var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:3\r\n\r\na&b");
assert.equal(c['a'], '');
assert.equal(c['b'], '');

var c = get_form("GET /test HTTP/1.0\r\nContent-type:application/x-www-form-urlencoded\r\nContent-length:27\r\n\r\na&&&&&&&&&&&&&&&&&&&&&&&&&b");
assert.equal(c['a'], '');
assert.equal(c['b'], '');

var ms = new io.MemoryStream();

var rep = new http.Response();
rep.body.write(new Buffer("0123456789"));

rep.sendTo(ms);
ms.rewind();
console.log(encoding.hexEncode(ms.read()));
