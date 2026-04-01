const fs = require('fs');

function updateFile(file, en) {
    let str = fs.readFileSync(file, 'utf8');
    if (en) {
        str = str.replace(/"fuzzyRangesTitle": "[^"]+",/, '"searchSpaceTitle": "Parameter Search Space",');
        str = str.replace(/"fuzzyRangesDescription": "[^"]+",/, '"searchSpaceDescription": "Define the minimum and maximum sweep ranges for optimization.",');
    } else {
        str = str.replace(/"fuzzyRangesTitle": "[^"]+",/, '"searchSpaceTitle": "Ruang Pencarian Parameter",');
        str = str.replace(/"fuzzyRangesDescription": "[^"]+",/, '"searchSpaceDescription": "Tentukan batas minimum dan maksimum untuk ruang pencarian sweep optimasi.",');
    }
    fs.writeFileSync(file, str);
}

updateFile('frontend/messages/en.json', true);
updateFile('frontend/messages/id.json', false);
console.log("Updated translations");
