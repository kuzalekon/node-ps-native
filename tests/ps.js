const ps = require('../build/Release/ps-native');

try {
    ps.enum((error,data) => console.log(data));
}
catch (e) {
    console.error(e);
}
