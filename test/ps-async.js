const ps = require('../build/Release/ps-native');

ps.list((error, processes) => {
    if (null === error) {
        console.log(processes);
    }
    else {
        throw error;
    }
});
