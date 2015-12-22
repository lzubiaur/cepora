Duktape.modSearch = function (id) {
    /* readFile() reads a file from disk, and returns a string or undefined.
     * 'id' is in resolved canonical form so it only contains terms and
     * slashes, and no '.' or '..' terms.
     */
    var res;

    print('loading module:', id);

    res = readfile('js/' + id + '.js');
    if (typeof res === 'string') {
        return res;
    }

    throw new Error('module not found: ' + id);
}

function xinspect(o,i){
    if(typeof i=='undefined') i = '';
    if(i.length>50)return '[MAX ITERATIONS]';
    var r=[];
    for(var p in o){
        var t=typeof o[p];
        r.push(i+'"'+p+'" ('+t+') => '+(t=='object' ? 'object:'+xinspect(o[p],i+'  ') : o[p]+''));
    }
    return r.join(i+'\n');
}

var coffee = require('lib/coffee-script').CoffeeScript;
alert(coffee.VERSION);

var script = readfile('js/test.coffee');
eval(coffee.compile(script))

var minimist = require('lib/minimist');

alert(xinspect(minimist('--help -a gldd'.split(' '))));

var livescript = require('lib/livescript');
alert(typeof livescript, xinspect(livescript))
// var livescript = require('lib/livescript');
