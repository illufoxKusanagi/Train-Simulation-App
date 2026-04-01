const fs = require('fs');
const file = 'backend/http_server/api_handler.cpp';
let str = fs.readFileSync(file, 'utf8');

str = str.replace(/"accelLow"/g, '"accelMin"');
str = str.replace(/"accelHigh"/g, '"accelMax"');
str = str.replace(/"weakeningLow"/g, '"weakeningMin"');
str = str.replace(/"weakeningHigh"/g, '"weakeningMax"');

// also replace translation keys while we are here: 
// The file frontend/messages/en.json and id.json
fs.writeFileSync(file, str);
console.log("Updated api_handler.cpp");
