const ps = require('../build/Release/ps-native');

try {
    console.log(ps.list());
}
catch (error) {
    console.log(error);
}
