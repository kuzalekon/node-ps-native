const ps = require('../build/Release/ps-native');

ps.kill(4, 2, (error) => {
    console.log(error);
});

ps.list(['pid', 'name'], (error, pl) => {
    console.log(pl);
});
