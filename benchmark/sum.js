const start = Date.now();
let sum = 0;
for(let i=0;i<10000000;i++){
    sum += i;
}
const duration = (Date.now() - start) / 1000;
console.log(sum);
console.log(`time: ${duration.toFixed(3)}`);
