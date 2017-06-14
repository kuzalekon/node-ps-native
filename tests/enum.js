'use strict'

const expect = require('expect.js');
const ps = require('../index.js');

describe('Enum', () => {

    it('with default fields', (done) => {

        ps.enum((error, proclist) => {
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


    it('with one field', (done) => {

        ps.enum(['pid'], (error, proclist) => {
            expect(error).to.be(null);
            expect(proclist).to.be.an('array');
            expect(proclist).to.not.have.length(0);
            expect(proclist[0]).to.only.have.key('pid');

            done();
        });

    });

    it('with multiply fields', (done) => {

        ps.enum(['pid', 'parent', 'name'], (error, proclist) => {
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
