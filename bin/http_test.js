var test = require("test");
test.setup();

var io = require('io');
var fs = require('fs');
var http = require('http');
var encoding = require('encoding');
var zlib = require('zlib');

describe(
		"http",
		function() {
			describe("headers", function() {
				var d = new http.Request().headers;

				it("add", function() {
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
				});

				it("all", function() {
					a = d.all('a');
					assert.equal(a.length, 2);
					assert.equal(a[0], '100');
					assert.equal(a[1], '300');
				});

				it("remove", function() {
					d.remove('a');
					assert.equal(d.has('a'), false);
					assert.strictEqual(d.first('a'), null);

					assert.strictEqual(d['a'], undefined);

					assert.equal(d.first('c'), '400');

					a = d.all('c');
					assert.equal(a.length, 2);
					assert.equal(a[0], '400');
					assert.equal(a[1], '600');
				});

				it("set", function() {
					d.set('c', '800');

					a = d.all('c');
					assert.equal(a.length, 1);
					assert.equal(a[0], '800');

					assert.equal(d.first('c'), '800');
				});

				it("add({})", function() {
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
				});

				it("set({})", function() {
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
				});

				it("set other type", function() {
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

					assert.deepEqual(new Date(d['f']), t);
				});
			});

			describe(
					"parse",
					function() {
						function get_request(u) {
							var ms = new io.MemoryStream();
							var bs = new io.BufferedStream(ms);
							bs.EOL = "\r\n";

							bs.writeText(u);
							ms.seek(0, fs.SEEK_SET);

							var r = new http.Request();

							r.readFrom(bs);

							return r;
						}

						function get_response(txt) {
							var ms = new io.MemoryStream();
							var bs = new io.BufferedStream(ms);
							bs.EOL = "\r\n";

							bs.writeText(txt);
							ms.seek(0, fs.SEEK_SET);

							var req = new http.Response();
							req.readFrom(bs);
							return req;
						}

						it(
								"bad request",
								function() {
									var bad_reqs = [
											" GET / HTTP/1.0\r\nkeepalive: close\r\n\r\n",
											"GET ? HTTP/1.0\r\nkeepalive: close\r\n\r\n",
											"GET / \r\nkeepalive: close\r\n\r\n",
											"GET / HTTP/a.0\r\nkeepalive: close\r\n\r\n",
											"GET / HTT/1.0\r\nkeepalive: close\r\n\r\n",
											"GET / HTTP/1.\r\nkeepalive: close\r\n\r\n" ];

									function readreq(u) {
										assert.throws(function() {
											get_request(u);
										});
									}

									bad_reqs.forEach(readreq);
								});

						it("bad response", function() {
							assert.throws(function() {
								get_response("HTTP/1.0\r\n\r\n");
							});

							assert.throws(function() {
								get_response("HTTP/1.0 \r\n\r\n");
							});

							assert.throws(function() {
								get_response("HTTP/1.0,200\r\n\r\n");
							});

							assert.throws(function() {
								get_response("HTTP/1.0 1\r\n\r\n");
							});

							assert.throws(function() {
								get_response("HTTP/1.0 1111\r\n\r\n");
							});

							assert.throws(function() {
								get_response("HTTP/1.0 asd\r\n\r\n");
							});
						});

						it(
								"basic format",
								function() {
									var req = get_request("GET / HTTP/1.0\r\nhead1: 100\r\nhead2: 200\r\nContent-type:test\r\nContent-length:    10\r\n\r\n0123456789");

									assert.equal('100', req.headers['head1']);
									assert.equal('200', req.headers['head2']);
									assert.equal(10, req.length);
									assert.equal('test',
											req.headers['content-type']);
									assert.equal('0123456789', req.body.read());

									assert.equal(req.stream.size(), 94);
									req.clear();
									assert.equal(req.stream, null);

									var r = get_response("HTTP/1.0 200\r\n\r\n");
									assert.equal(r.status, 200);
									assert.equal(r.protocol, 'HTTP/1.0');
								});

						it(
								"keep-alive",
								function() {
									var keep_reqs = {
										"GET / HTTP/1.0\r\n\r\n" : false,
										"GET / HTTP/1.0\r\nconnection: close\r\n\r\n" : false,
										"GET / HTTP/1.0\r\nconnection: keep-alive\r\n\r\n" : true,
										"GET / HTTP/1.1\r\n\r\n" : true,
										"GET / HTTP/1.1\r\nconnection: close\r\n\r\n" : false,
										"GET / HTTP/1.1\r\nconnection: keep-alive\r\n\r\n" : true,
									};

									for ( var n in keep_reqs) {
										var req = get_request(n);
										assert.equal(keep_reqs[n],
												req.keepAlive);
									}
								});

						it(
								"cookie",
								function() {
									function get_cookie(txt) {
										return get_request(txt).cookie;
									}

									var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new;\r\n\r\n");
									assert.equal(c['$Version'], '1');
									assert.equal(c['Skin'], 'new');

									var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20\r\n\r\n");
									assert.equal(c['Skin'], 'new ');

									var c = get_cookie("GET / HTTP/1.0\r\ncookie: $Version=1; Skin=new%20cookie %sdf\r\n\r\n");
									assert.equal(c['Skin'], 'new cookie %sdf');
								});

						it(
								"query",
								function() {
									function get_query(txt) {
										return get_request(txt).query;
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
								});

						it(
								"form",
								function() {
									function get_form(txt) {
										return get_request(txt).form;
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

									var c = get_form('GET /test HTTP/1.0\r\nContent-type:multipart/form-data;boundary=7d33a816d302b6\r\nContent-length:150\r\n\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="a"\r\n\r\n100\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="b"\r\n\r\n200\r\n--7d33a816d302b6\r\n');
									assert.equal(c['a'], '100');
									assert.equal(c['b'], '200');

									var c = get_form('GET /test HTTP/1.0\r\nContent-type:multipart/form-data;boundary=7d33a816d302b6\r\nContent-length:201\r\n\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="a"\r\n\r\n100\r\n--7d33a816d302b6\r\nContent-Disposition: form-data;name="b";filename="test"\r\nContent-Transfer-Encoding: base64\r\n\r\n200\r\n--7d33a816d302b6\r\n');
									assert.equal(c['a'], '100');
									assert.equal(c['b'].fileName, 'test');
									assert.equal(
											c['b'].contentTransferEncoding,
											'base64');
									assert.equal(c['b'].body.read().toString(),
											'200');
								});
					});

			describe(
					"encode",
					function() {
						it(
								"response",
								function() {
									var rep = new http.Request();
									rep.body.write(new Buffer("0123456789"));

									var ms = new io.MemoryStream();

									rep.sendTo(ms);
									ms.rewind();
									assert
											.equal(
													ms.read(),
													'GET / HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
								});
						it(
								"response",
								function() {
									var ms = new io.MemoryStream();

									var rep = new http.Response();
									rep.body.write(new Buffer("0123456789"));

									rep.sendTo(ms);
									ms.rewind();
									assert
											.equal(
													ms.read(),
													'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
								});

						it(
								"address",
								function() {
									var rep = new http.Request();
									rep.body.write(new Buffer("0123456789"));
									rep.address = "/docs/";
									rep.value = "/docs/";

									var ms = new io.MemoryStream();

									rep.sendTo(ms);
									ms.rewind();
									assert
											.equal(
													ms.read(),
													'GET /docs/ HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
								});

						it(
								"query",
								function() {
									var rep = new http.Request();
									rep.body.write(new Buffer("0123456789"));
									rep.address = "/docs";
									rep.value = "/docs";
									rep.queryString = "page=100&style=wap";

									var ms = new io.MemoryStream();

									rep.sendTo(ms);
									ms.rewind();
									assert
											.equal(
													ms.read(),
													'GET /docs?page=100&style=wap HTTP/1.1\r\nConnection: keep-alive\r\nContent-Length: 10\r\n\r\n0123456789');
								});
					});

			describe("file handler", function() {
				var hfHandler = new http.fileHandler('./');
				var url = 'test.html';
				var rep;

				function hfh_test(url, headers) {
					var req = new http.Request();
					req.value = url;
					if (headers)
						req.addHeader(headers);
					hfHandler.invoke(req);
					return req.response;
				}

				function clean() {
					try {
						fs.unlink('test.html');
					} catch (e) {
					}
					;
					try {
						fs.unlink('test.html.gz');
					} catch (e) {
					}
					;
				}

				before(clean);
				after(clean);

				it("file not found", function() {
					rep = hfh_test(url);
					assert.equal(404, rep.status);
					rep.clear();
				});

				it("normal", function() {
					fs.writeFile('test.html', 'test html file');

					rep = hfh_test(url);
					assert.equal(200, rep.status);
					assert.equal(14, rep.length);

					assert.deepEqual(
							new Date(rep.firstHeader('Last-Modified')), fs
									.stat('test.html').mtime);
				});

				it("not modified", function() {
					var rep1 = hfh_test(url, {
						'If-Modified-Since' : rep.firstHeader('Last-Modified')
					});
					assert.equal(304, rep1.status);
					rep1.clear();
					rep.clear();
				});

				it("changed", function() {
					var rep = hfh_test(url, {
						'If-Modified-Since' : new Date('1998-04-14 12:12:12')
					});
					assert.equal(200, rep.status);
					assert.equal(14, rep.length);
					assert.equal('text/html', rep.firstHeader('Content-Type'));
					rep.clear();
				});

				it("pre-gzip", function() {
					var sgz = 'gz test file';
					var gz = fs.open('test.html.gz', 'w');
					gz.write(zlib.gzip(new Buffer(sgz)));
					gz.close();

					var rep = hfh_test(url, {
						'Accept-Encoding' : 'deflate,gzip'
					});
					assert.equal(200, rep.status);
					assert.equal('gzip', rep.firstHeader('Content-Encoding'));
					assert.equal('text/html', rep.firstHeader('Content-Type'));
					rep.body.rewind();
					assert.equal(sgz, zlib.gunzip(rep.body.readAll())
							.toString());
					rep.body.close();
					rep.clear();
				});

				it("don't gzip small file", function() {
					fs.unlink('test.html.gz');

					var rep = hfh_test(url, {
						'Accept-Encoding' : 'deflate,gzip'
					});
					assert.equal(200, rep.status);
					assert.equal(null, rep.firstHeader('Content-Encoding'));
					rep.clear();
				});
			});
		});

//test.run();
