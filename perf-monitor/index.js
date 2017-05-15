var term = require( 'terminal-kit' ).terminal ;  
const readLastLines = require('read-last-lines');
const codes = require('./pmucodes.json');

setInterval(() => {    
    readLastLines.read('/tmp/perf.log', 154)
        .then((lines) => {
            term.clear();
            const values = {};
            lines.split('\n').forEach((line) => {
                if (/stat/.test(line))
                    return;

                var cols = line.split(':');
                const metric = {
                    value: cols[0],
                    id: cols[2]
                };

                if (metric.id === 'r400fa')
                    metric.id = 'r500fa'; // I think these are equivalent

                if (metric.id) {
                    const event = codes.find(x => x.EventCode == "0x" + metric.id.slice(1));
                    if (event) {
                        metric.title = event.EventName;
                    } else {
                        metric.title = metric.id;
                    }
                    values[metric.id] = metric;
                }
            });

            var currentRow = "";
            var currentColumn = 0;
            const columnWidth = 60;
            
            Object.keys(values).sort().forEach(key => {
                const metric = values[key];
                currentRow += `${metric.title}: ${metric.value}`;

                const desiredWidth = (currentColumn+1) * columnWidth;
                while (currentRow.length < desiredWidth) {
                    currentRow += ' '
                }

                if (currentColumn < 2) {
                    currentColumn++;
                } else {
                    term(`${currentRow}\n`);
                    currentColumn = 0;
                    currentRow = '';
                }
            });
            term(`${currentRow}\n`);
        });
}, 1000);

