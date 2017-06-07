const ps = require('../build/Release/ps-native');

try {
    //ps.list((error,data) => console.log(data));
ps.find(/{.*}\\.exe/, (error, proc) => {
    if (null !== error) console.log(error);
   else console.log(proc);
});
}
catch (e) {
    console.error(e);
}