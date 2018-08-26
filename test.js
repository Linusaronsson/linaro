function OUTER_MAIN() {
function weird_func_name(x) {
console.log(x);

    function ff() {
        console.log(x)
        x = 1337
    }

    function ff2() {
        var y = 9999;
        var xdddd3 = function() { var z = 3; }
        xdddd3()
        console.log(x)
    }

    x = 100;
    var p = {ff, ff2}
    return p
}

var t = weird_func_name(10)
t.ff();
t.ff2();


}

OUTER_MAIN();
