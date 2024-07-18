console.log('db testing....');

var db = require('db');
var assert = require('assert');

function t_db(conn){
	conn.execute('create table test(t1 int, t2 varchar(128), t3 blob, t4 datetime);');
	conn.execute("insert into test values(?,?,?,?);", 1123, 'aaaaa', new Buffer('DDDDDDDDDD'), new Date('1998-04-14 12:12:12'));

	var rs = conn.execute('select * from test;');
	var r = rs[0];

	assert.equal(typeof r['t1'], 'number');
	assert.equal(typeof r['t2'], 'string');
	assert.equal(typeof r['t3'], 'object');
	assert.equal(typeof r['t4'], 'object');

	assert.strictEqual(r['t1'], 1123);
	assert.strictEqual(r['t2'], 'aaaaa');
	assert.strictEqual(r['t3'].toString(), 'DDDDDDDDDD');
	assert.strictEqual(r['t4'], new Date('1998-04-14 12:12:12'));
		
	conn.execute('drop table test;');

	conn.close();
}

assert.equal('123456\\r\\n\\\'\\\"\\x1acccds', db.escape('123456\r\n\'\"\x1acccds'));
assert.equal(db.format("test?, ?, ?, ?", 123, 'ds\r\na', new Date('1998-4-14 12:12:12')), "test'123', 'ds\\r\\na', '1998-04-14 12:12:12', ''");

t_db(db.open('sqlite:test.db'));
t_db(db.open('mysql://root@localhost/test'));

