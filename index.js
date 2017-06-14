
const os = require('os');
let ps = require('bindings')('ps-native');

if ('linux' === os.platform()) {
    ps.SIGHUP       = -1;
    ps.SIGINT       = -2;
    ps.SIGQUIT      = -3;
    ps.SIGKILL      = -9;
    ps.SIGUSR1      = -10;
    ps.SIGUSR2      = -12;
    ps.SIGTERM      = -15;
}

module.exports = exports = ps;
