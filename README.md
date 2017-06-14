<p align="center">
    <img src="logo.png" width="350" height="100" alt="ps-native" />
</p>
<p align="center">
    <strong>Native Node.js binding for enumerate, find and kill processes by OS specific API</strong>
</p>

### Installation
**Note**: The module is uses the [`node-gyp`](https://github.com/nodejs/node-gyp) and `c++ compiler` with c++11 support to build the extension.

You can use [`npm`](https://github.com/isaacs/npm) to download and install it:
* The latest [`ps-native`](https://www.npmjs.com/package/ps-native) package: `npm install ps-native`
* GitHub's [`master`](https://github.com/kalexey89/node-ps-native/tree/master) branch: `npm install https://github.com/kalexey89/node-ps-native/tarball/master`

### Supported OS and platforms
**_OS_**:
* `Windows` *(desktop: Window 7+, server: Windows Server 2008+)*;
* `Linux` *(any Linux-based distributives)*;
* ~~MacOS~~ *(plans...)*.

**_Platforms_**:
* [`Node.js`](https://nodejs.org) *(greater then version 4)*;
* [`NW.js`](https://nwjs.io/) *(if NW.js contains a supported version of Node.js)*.

### Usage
**Note:** the module must be [installed](#installation) before use.

``` javascript
const ps = require('ps-native');

/*
 * Enumerate processes
 */

ps.enum(['pid', 'name'], (error, proclist) => {
    if (null === error) console.log(proclist);
    else throw error;
});


/*
 * Find processes
 */

// By PID
ps.find(4354, ['pid', 'name'], (error, proc) => {
    if (null === error) console.log(proc);
    else throw error;
});

// By name
ps.find('gedit', ['pid', 'name'], (error, proclist) => {
    if (null === error) console.log(proclist);
    else throw error;
});

// By regexp
ps.find(/ch.*/, ['pid', 'name'], (error, proclist) => {
    if (null === error) console.log(proclist);
    else throw error;
});


/*
 * Kill process
 */
 
ps.kill(4532, ps.SIGTERM, (error) => {
    if (null !== error) throw error;
});
```

### API
See the [API documentation](https://github.com/kalexey89/node-ps-native/wiki) in the wiki.

### Testing
[`mocha`](https://github.com/visionmedia/mocha) is required to run unit tests.

In `ps-native` directory (where its `package.json` resides) run the following:

	npm install mocha
	npm test
	
### License
[`MIT`](https://mit-license.org/), &copy; 2017 Kuznetsov Aleksey (kalexey89).
