var test = require("test");
test.setup();

var db = require('db');
var fs = require('fs');
var coroutine = require('coroutine');

var vmid = coroutine.vmid;

describe("db", function() {
    it("escape", function() {
        assert.equal('123456\\r\\n\'\'\\\"\\\x1acccds', db.escape(
            '123456\r\n\'\"\x1acccds', true));
    });

    it("formatMySQL", function() {
        assert.equal(db.formatMySQL("test?, ?, ?, ?", 123, 'ds\r\na',
                new Date('1998-4-14 12:12:12')),
            "test123, 'ds\\r\\na', '1998-04-14 12:12:12', ''");

        assert.equal(db.formatMySQL("test?", [1, 2, 3, 4]),
            "test(1,2,3,4)");
    });

    function _test(conn_str) {
        var conn;

        before(function() {
            conn = db.open(conn_str);
            try {
                conn.execute('drop table test;');
            } catch (e) {}
        });

        after(function() {
            try {
                conn.execute('drop table test;');
                conn.close();
            } catch (e) {}
        });

        it("empty sql", function() {
            assert.throws(function() {
                conn.execute("  ");
            })
        });

        it("escape", function() {
            var rs = conn.execute("select ? as t;", '123456\r\n\'\"\x1acccds');
            assert.equal(rs[0].t, '123456\r\n\'\"\x1acccds');
        });

        it("create table", function() {
            if (conn.type == 'mssql')
                conn.execute('create table test(t1 int, t2 varchar(128), t3 VARBINARY(100), t4 datetime);');
            else
                conn.execute('create table test(t1 int, t2 varchar(128), t3 BLOB, t4 datetime);');
        });

        it("insert", function() {
            conn.execute("insert into test values(?,?,?,?);", 1123,
                'aaaaa', new Buffer('DDDDDDDDDD'), new Date(
                    '1998-04-14 12:12:12'));
        });

        it("select", function() {
            var rs = conn.execute('select * from test;');
            var r = rs[0];

            if (conn.txBufferSize) {
                console.log(conn.rxBufferSize, conn.txBufferSize);
                conn.txBufferSize = 16777220;
                console.log(conn.rxBufferSize, conn.txBufferSize);
            }

            assert.equal(typeof r['t1'], 'number');
            assert.equal(typeof r['t2'], 'string');
            assert.equal(typeof r['t3'], 'object');
            assert.equal(typeof r['t4'], 'object');

            assert.strictEqual(r['t1'], 1123);
            assert.strictEqual(r['t2'], 'aaaaa');
            assert.strictEqual(r['t3'].toString(), 'DDDDDDDDDD');
            assert.deepEqual(r['t4'], new Date('1998-04-14 12:12:12'));

            assert.deepEqual(rs.fields, [
                "t1",
                "t2",
                "t3",
                "t4"
            ]);

            assert.deepEqual(Object.keys(r), [
                "t1",
                "t2",
                "t3",
                "t4"
            ]);
        });

        it("update/affected", function() {
            var rs = conn.execute("update test set t2='test101.1' where t1=1123");
            assert.equal(rs.affected, 1);
        });

        it("binary", function() {
            var b = new Buffer();
            b.resize(1);

            for (var i = 0; i < 256; i++) {
                b[0] = i;
                conn.execute("delete from test;");
                conn.execute("insert into test values(1,'aa', ?, ?);",
                    b, new Date());
                var rs = conn.execute("select * from test;");
                assert.equal(rs[0].t3.length, 1);
                assert.equal(rs[0].t3[0], i);
            }
        });

        describe("trans", function() {
            before(function() {
                try {
                    var b = new Buffer();
                    conn.execute("insert into test values(?,?,?,?);", 101, 'test101', b, new Date());
                } catch (e) {}
            });

            it("begin/commit", function() {
                conn.begin();
                conn.execute("update test set t2='test101.1' where t1=101");
                conn.commit();

                var rs = conn.execute("select * from test where t1=101");
                assert.equal(rs[0].t2, "test101.1");
            });

            it("begin/rollback", function() {
                conn.begin();
                conn.execute("update test set t2='test101.1' where t1=101");
                conn.rollback();

                var rs = conn.execute("select * from test where t1=101");
                assert.equal(rs[0].t2, "test101.1");
            });
        });

        it("execute bug", function() {
            var a = 0;
            coroutine.start(function() {
                a = 1;
            })
            for (var i = 0; i < 100 && a == 0; i++)
                conn.execute("select 100;", 100);
            assert.equal(a, 1);
        });
    }

    describe("sqlite", function() {
        var conn_str = 'sqlite:test.db' + vmid;
        after(function() {
            fs.unlink("test.db" + vmid);
        });
        _test(conn_str);

        it("check synchronous mode", function() {
            var conn = db.open(conn_str);
            var synchronous = conn.execute("PRAGMA synchronous;")[0].synchronous;
            conn.close();

            assert.equal(synchronous, 1);
        });

        it("check journal mode", function() {
            var conn = db.open(conn_str);
            var journal_mode = conn.execute("PRAGMA journal_mode;")[0].journal_mode;
            conn.close();
            assert.equal(journal_mode, "wal");
        });
    });

    xdescribe("mysql", function() {
        _test('mysql://root@localhost/test');
    });

    xdescribe("mssql", function() {
        _test('mssql://sa@localhost/test');
    });

    describe("leveldb", function() {
        after(clear_db);

        function clear_db() {
            try {
                fs.readdir("testdb" + vmid).forEach(function(s) {
                    if (s.name != "." && s.name != "..")
                        fs.unlink("testdb" + vmid + "/" + s.name);
                });

                fs.rmdir("testdb" + vmid);
            } catch (e) {};
        }

        it('open/close', function() {
            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.close();
            clear_db();
        });

        it('set/get', function() {
            var b = "bbbbb";
            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.set("test", b);
            assert.equal(ldb.get("test").toString(), "bbbbb");
            ldb.close();
            clear_db();
        });

        it('binary Key', function() {
            var b = "bbbbb1";
            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.set("test1", b);
            assert.equal(ldb.get("test1").toString(), "bbbbb1");
            ldb.close();
            clear_db();
        });

        it('multi set/get', function() {
            var data = {
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ccc": "ccc value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.mset(data);
            assert.equal(ldb.get("aaa").toString(), "aaa value");
            assert.equal(ldb.get("bbb").toString(), "bbb value");
            assert.equal(ldb.get("ccc").toString(), "ccc value");
            assert.equal(ldb.get("ddd").toString(), "ddd value");

            assert.deepEqual(ldb.mget(['aaa', 'ccc', 'eee']).toArray(), [
                new Buffer('aaa value'), new Buffer('ccc value'), null
            ]);

            ldb.close();
            clear_db();
        });

        it('remove/has', function() {
            var b = "bbbbb";
            var ldb = db.openLevelDB("testdb" + vmid);
            assert.isNull(ldb.get("not_exists"));
            assert.isFalse(ldb.has("not_exists"));
            ldb.set("not_exists", b);
            assert.isTrue(ldb.has("not_exists"));
            ldb.remove("not_exists");
            assert.isFalse(ldb.has("not_exists"));

            ldb.set("not_exists", b);
            assert.isTrue(ldb.has("not_exists"));
            ldb.remove("not_exists");
            assert.isFalse(ldb.has("not_exists"));
            ldb.close();
            clear_db();
        });

        it('batch remove', function() {
            var data = {
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ccc": "ccc value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.mset(data);

            ldb.mremove(["bbb", "ddd"]);

            assert.equal(ldb.get("aaa").toString(), "aaa value");
            assert.isNull(ldb.get("bbb"));
            assert.equal(ldb.get("ccc").toString(), "ccc value");
            assert.isNull(ldb.get("ddd"));

            ldb.close();
            clear_db();
        });

        it('begin/commit', function() {
            var b = "bbbbb";
            var c = "ccccc";
            var ldb = db.openLevelDB("testdb" + vmid);

            ldb.set("test", b);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            var tr = ldb.begin();

            tr.set("test", c);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            tr.commit();

            assert.equal(ldb.get("test").toString(), "ccccc");

            ldb.close();
            clear_db();
        });

        it('begin/close', function() {
            var b = "bbbbb";
            var c = "ccccc";
            var ldb = db.openLevelDB("testdb" + vmid);

            ldb.set("test", b);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            var tr = ldb.begin();

            tr.set("test", c);
            assert.equal(ldb.get("test").toString(), "bbbbb");

            tr.close();

            assert.equal(ldb.get("test").toString(), "bbbbb");

            ldb.close();
            clear_db();
        });

        it('forEach', function() {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value"
            };

            var ldb = db.openLevelDB("testdb" + vmid);

            var count = 0;
            ldb.forEach(function(v, k) {
                count++;
            });
            assert.equal(count, 0);

            ldb.mset(data);

            count = 0;
            ldb.forEach(function(v, k) {
                assert.equal(data[k], v.toString());
                delete data[k];
                count++;
            });
            assert.equal(count, 4);

            ldb.close();
            clear_db();
        });

        it('between', function() {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value"
            };

            var data1 = {
                "ccc": "ccc value",
                "bbb": "bbb value"
            };

            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.mset(data);

            var count = 0;
            ldb.between("bbb", "ddd", function(v, k) {
                assert.equal(data1[k], v.toString());
                delete data1[k];
                count++;
            });
            assert.equal(count, 2);

            ldb.close();
            clear_db();
        });

        it('break', function() {
            var data = {
                "ccc": "ccc value",
                "aaa": "aaa value",
                "bbb": "bbb value",
                "ddd": "ddd value"
            };

            var data1 = {
                "aaa": "aaa value",
                "bbb": "bbb value"
            };

            var ldb = db.openLevelDB("testdb" + vmid);
            ldb.mset(data);

            var count = 0;
            ldb.forEach(function(v, k) {
                assert.equal(data1[k], v.toString());
                delete data1[k];
                count++;
                if (count == 2)
                    return true;
            });
            assert.equal(count, 2);

            ldb.close();
            clear_db();
        });

    });
});

// test.run(console.DEBUG);
