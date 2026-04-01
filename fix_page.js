const fs = require('fs');
const file = 'frontend/src/app/optimization/page.tsx';
let str = fs.readFileSync(file, 'utf8');

// Replacements
str = str.replace(/accelLow: 0\.6,[\s\S]*?weakeningHigh: 70,/m, 
`accelMin: 0.6,
      accelMax: 1.2,
      weakeningMin: 35,
      weakeningMax: 70,`);

str = str.replace(/const nAcc = Math\.round\(\(vals\.accelHigh - vals\.accelLow\) \/ 0\.05\) \+ 1;/, 
`const nAcc = Math.round((vals.accelMax - vals.accelMin) / 0.05) + 1;`);

str = str.replace(/const nVp1 = Math\.round\(\(vals\.weakeningHigh - vals\.weakeningLow\) \/ 5\) \+ 1;/, 
`const nVp1 = Math.round((vals.weakeningMax - vals.weakeningMin) / 5) + 1;`);

// The grids: <div className="grid grid-cols-3 gap-4"> -> grid-cols-2
// We expect exactly two of these "grid-cols-3 gap-4" arrays inside the Form loop
str = str.replaceAll('grid-cols-3', 'grid-cols-2');

// Text strings
str = str.replace(/fuzzyRangesTitle"\)}<\/p>\s*<p className="text-muted-foreground">\s*\{t\("fuzzyRangesDescription"\)\}\s*<\/p>/m, 
`searchSpaceTitle")}</p>
                  <p className="text-muted-foreground">
                    {t("searchSpaceDescription")}
                  </p>`);

fs.writeFileSync(file, str);
console.log("Updated page.tsx");
