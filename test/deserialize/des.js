
let startupFilepath = path.join(__dirname, './public/startup.log')
fs.appendFileSync(startupFilepath, `${new Date().toISOString()}\n`, 'utf-8')

let counterFilepath = path.join(__dirname, './public/counter.log')
if (!fs.existsSync(counterFilepath)) {
  fs.writeFileSync(counterFilepath, '1', 'utf-8')
} else {
  let cycles = Number(fs.readFileSync(counterFilepath))
  fs.writeFileSync(counterFilepath, "" + (cycles + 1))
}

