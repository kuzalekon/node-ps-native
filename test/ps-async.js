const ps = require('../build/Release/ps-native');

ps.list(['pid', 'name'], (error, pl) => {
    console.log(pl);
});
