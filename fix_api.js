const fs = require('fs');
const file = 'frontend/src/services/api.ts';
let str = fs.readFileSync(file, 'utf8');

str = str.replace(/accelLow: number;\s*accelMedium: number;\s*accelHigh: number;\s*weakeningLow: number;\s*weakeningMedium: number;\s*weakeningHigh: number;/,
`accelMin: number;
    accelMax: number;
    weakeningMin: number;
    weakeningMax: number;`);

fs.writeFileSync(file, str);
console.log("Updated api.ts");
