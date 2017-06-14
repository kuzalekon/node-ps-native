'use strict'

const os = require('os');
const { exec } = require('child_process');

const expect = require('expect.js');

const ps = require('../index.js');

const WIN32_DEFAULT_APP = 'notepad.exe';
const LINUX_DEFAULT_APP = 'sh';

describe('Find', () => {

    const exe = 'linux' === os.platform() ? LINUX_DEFAULT_APP
                                          : WIN32_DEFAULT_APP;
    let child = null;

    before(() => {
        child = exec(exe);
    });

    after(() => {
        child.kill('SIGTERM');
    });

    it('by pid (default fields)', (done) => {

        ps.find(child.pid, (error, proc) => {
            expect(error).to.be(null);
            expect(proc).to.be.an('object');
            expect(proc).to.only.have.keys([
                'pid',
                'parent',
                'name',
                'priority',
                'threads'
            ]);

            done();
        });

    });

    it('by pid (one field)', (done) => {

        ps.find(child.pid, ['pid'], (error, proc) => {
            expect(error).to.be(null);
            expect(proc).to.be.an('object');
            expect(proc).to.only.have.keys('pid');

            done();
        });

    });

    it('by pid (multiply fields)', (done) => {

        ps.find(child.pid, ['pid', 'parent', 'name'], (error, proc) => {
            expect(error).to.be(null);
            expect(proc).to.be.an('object');
            expect(proc).to.only.have.keys([
                'pid',
                'parent',
                'name'
            ]);

            done();
        });

    });


    it('by name (default fields)', (done) => {

        ps.find(exe, (error, proclist) => {
        console.log(proclist);
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.keys([
                'pid',
                'parent',
                'name',
                'priority',
                'threads'
            ]);

            done();
        });

    });

    it('by name (one field)', (done) => {

        ps.find(exe, ['pid'], (error, proclist) => {
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.keys('pid');

            done();
        });

    });

    it('by name (multiply fields)', (done) => {

        ps.find(exe, ['pid', 'parent', 'name'], (error, proclist) => {
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.keys([
                'pid',
                'parent',
                'name'
            ]);

            done();
        });

    });


    /*
     * Find by regular expression
     */

    it('by regexp (default fields)', (done) => {

        ps.find(PROCESS_REGEXP, (error, proclist) => {
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.keys([
                'pid',
                'parent',
                'name',
                'priority',
                'threads'
            ]);

            done();
        });

    });

    it('by regexp (one field)', (done) => {

        ps.find(PROCESS_REGEXP, ['pid'], (error, proclist) => {
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.keys('pid');

            done();
        });

    });

    it('by regexp (multiply fields)', (done) => {

        ps.find(PROCESS_REGEXP, ['pid', 'parent', 'name'], (error, proclist) => {
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.keys([
                'pid',
                'parent',
                'name'
            ]);

            done();
        });

    });

});
