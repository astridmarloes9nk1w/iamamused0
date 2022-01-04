/**
 * 
 */

var assert = require('assert');
var os = require('os');

var path = require('path');

var isWindows = os.type() === 'Windows';

// assert.throwAssert();

// --------------- basename

assert.equal(path.basename('./test.js'), 'test.js');
assert.equal(path.basename('./test.js', 'js'), 'test.');
assert.equal(path.basename('./test.js', '.js'), 'test');
assert.equal(path.basename('./test.js', 't.js'), 'tes');
assert.equal(path.basename('./.js', '.js'), '');

// --------------- extname

assert.equal(path.extname('./test.js'), '.js');

assert.equal(path.extname(''), '');
assert.equal(path.extname('/path/to/file'), '');
assert.equal(path.extname('/path/to/file.ext'), '.ext');
assert.equal(path.extname('/path.to/file.ext'), '.ext');
assert.equal(path.extname('/path.to/file'), '');
assert.equal(path.extname('/path.to/.file'), '');
assert.equal(path.extname('/path.to/.file.ext'), '.ext');
assert.equal(path.extname('/path/to/f.ext'), '.ext');
assert.equal(path.extname('/path/to/..ext'), '.ext');
assert.equal(path.extname('file'), '');
assert.equal(path.extname('file.ext'), '.ext');
assert.equal(path.extname('.file'), '');
assert.equal(path.extname('.file.ext'), '.ext');
assert.equal(path.extname('/file'), '');
assert.equal(path.extname('/file.ext'), '.ext');
assert.equal(path.extname('/.file'), '');
assert.equal(path.extname('/.file.ext'), '.ext');
assert.equal(path.extname('.path/file.ext'), '.ext');
assert.equal(path.extname('file.ext.ext'), '.ext');
assert.equal(path.extname('file.'), '.');
assert.equal(path.extname('.'), '');
assert.equal(path.extname('./'), '');
assert.equal(path.extname('.file.ext'), '.ext');
assert.equal(path.extname('.file'), '');
assert.equal(path.extname('.file.'), '.');
assert.equal(path.extname('.file..'), '.');
// assert.equal(path.extname('..'), '');
assert.equal(path.extname('../'), '');
assert.equal(path.extname('..file.ext'), '.ext');
assert.equal(path.extname('..file'), '.file');
assert.equal(path.extname('..file.'), '.');
assert.equal(path.extname('..file..'), '.');
assert.equal(path.extname('...'), '.');
assert.equal(path.extname('...ext'), '.ext');
assert.equal(path.extname('....'), '.');
assert.equal(path.extname('file.ext/'), '');

if (isWindows) {
	// On windows, backspace is a path separator.
	assert.equal(path.extname('.\\'), '');
	assert.equal(path.extname('..\\'), '');
	assert.equal(path.extname('file.ext\\'), '');
} else {
	// On unix, backspace is a valid name component like any other character.
	assert.equal(path.extname('.\\'), '');
	assert.equal(path.extname('..\\'), '.\\');
	assert.equal(path.extname('file.ext\\'), '.ext\\');
}

// --------------- dirname

assert.equal(path.dirname('/a/b/'), '/a');
assert.equal(path.dirname('/a/b'), '/a');
assert.equal(path.dirname('/a'), '/');
assert.equal(path.dirname('/'), '/');

if (isWindows) {
	assert.equal(path.dirname('c:\\'), 'c:\\');
	assert.equal(path.dirname('c:\\foo'), 'c:\\');
	assert.equal(path.dirname('c:\\foo\\'), 'c:\\');
	assert.equal(path.dirname('c:\\foo\\bar'), 'c:\\foo');
	assert.equal(path.dirname('c:\\foo\\bar\\'), 'c:\\foo');
	assert.equal(path.dirname('c:\\foo\\bar\\baz'), 'c:\\foo\\bar');
	assert.equal(path.dirname('\\'), '\\');
	assert.equal(path.dirname('\\foo'), '\\');
	assert.equal(path.dirname('\\foo\\'), '\\');
	assert.equal(path.dirname('\\foo\\bar'), '\\foo');
	assert.equal(path.dirname('\\foo\\bar\\'), '\\foo');
	assert.equal(path.dirname('\\foo\\bar\\baz'), '\\foo\\bar');
	assert.equal(path.dirname('c:'), 'c:');
	assert.equal(path.dirname('c:foo'), 'c:');
	assert.equal(path.dirname('c:foo\\'), 'c:');
	assert.equal(path.dirname('c:foo\\bar'), 'c:foo');
	assert.equal(path.dirname('c:foo\\bar\\'), 'c:foo');
	assert.equal(path.dirname('c:foo\\bar\\baz'), 'c:foo\\bar');
	assert.equal(path.dirname('\\\\unc\\share'), '\\\\unc\\share');
	assert.equal(path.dirname('\\\\unc\\share\\foo'), '\\\\unc\\share\\');
	assert.equal(path.dirname('\\\\unc\\share\\foo\\'), '\\\\unc\\share\\');
	assert.equal(path.dirname('\\\\unc\\share\\foo\\bar'),
			'\\\\unc\\share\\foo');
	assert.equal(path.dirname('\\\\unc\\share\\foo\\bar\\'),
			'\\\\unc\\share\\foo');
	assert.equal(path.dirname('\\\\unc\\share\\foo\\bar\\baz'),
			'\\\\unc\\share\\foo\\bar');
}

// path normalize tests
if (isWindows) {
	assert.equal(path.normalize('c:/foo/../../../bar'), 'c:\\bar');
	assert.equal(path.normalize('./fixtures///b/../b/c.js'),
			'fixtures\\b\\c.js');
	assert.equal(path.normalize('/foo/../../../bar'), '\\bar');
	assert.equal(path.normalize('a//b//../b'), 'a\\b');
	assert.equal(path.normalize('a//b//./c'), 'a\\b\\c');
	assert.equal(path.normalize('a//b//.'), 'a\\b');
} else {
	assert.equal(path.normalize('./fixtures///b/../b/c.js'), 'fixtures/b/c.js');
	assert.equal(path.normalize('/foo/../../../bar'), '/bar');
	assert.equal(path.normalize('a//b//../b'), 'a/b');
	assert.equal(path.normalize('a//b//./c'), 'a/b/c');
	assert.equal(path.normalize('a//b//.'), 'a/b');
}
