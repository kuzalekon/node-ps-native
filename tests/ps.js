const ps = require('../index');

try {
    ps.find(3944, (error, process) => console.log(process));
}
catch (e) {
    console.error(e);
}
