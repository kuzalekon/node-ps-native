const ps = require('../build/Release/ps-native');

ps.list(['PID', 'name'], (error, pl) => {
    console.log(pl);
});
