'use strict'

const os = require('os');
const { spawn } = require('child_process');

const expect = require('expect.js');

const ps = require('../index.js');

describe('Kill', () => {

    const exe = 'linux' === os.platform() ? 'sh' : 'notepad.exe';
    let pid = NaN;

    beforeEach(() => {
        pid = spawn(exe, []).pid;
    });

    it('by pid (default signal)', (done) => {

        ps.kill(pid, (error) => {
            expect(error).to.be(null);

            done();
        });

    });

    it('by pid (custom signal)', (done) => {

        const signal = 'linux' === os.platform() ? ps.SIGSTOP : 1;

        ps.kill(pid, signal, (error) => {
            expect(error).to.be(null);

            done();
        });

    });

});
