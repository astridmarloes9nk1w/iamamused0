var os = require('os');
var coroutine = require('coroutine');

var ss = [];

exports.countObject = nm => {
    var cnt = 0;

    function count(l) {
        if (l['class'] == nm) {
            cnt += l.objects;
            return;
        }

        if (l.inherits)
            l.inherits.forEach((v) => {
                count(v);
            });
    }

    count(os.memoryUsage().nativeObjects);
    return cnt;
}

exports.makeid = length => {
    var result = '';
    var characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    var charactersLength = characters.length;
    for (var i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

exports.gc = () => {
    for (var i = 0; i < 20; i++) {
        coroutine.sleep(1);
        GC();
    }
}

exports.push = s => ss.push(s);

exports.cleanup = () => {
    ss.forEach(s => {
        s.close();
    });
}